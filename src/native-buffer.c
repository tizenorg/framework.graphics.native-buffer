/**************************************************************************

Native Buffer

Copyright 2013 Samsung Electronics co., Ltd. All Rights Reserved.

Contact: Dongyeon Kim <dy5.kim@samsung.com>
              Inpyo Kang <mantiger@samsung.com>
              Carsten Haitzler <raster@rasterman.com>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

#include "native-buffer-private.h"

#define NATIVE_BUFFER_VERSION_ENCODE(major, minor, micro) ( \
      ((major) * 10000)                                     \
    + ((minor) *   100)                                     \
    + ((micro) *     1))

#define NATIVE_BUFFER_VERSION NATIVE_BUFFER_VERSION_ENCODE( \
    NATIVE_BUFFER_VERSION_MAJOR,            \
    NATIVE_BUFFER_VERSION_MINOR,            \
    NATIVE_BUFFER_VERSION_MICRO)

#define NATIVE_BUFFER_VERSION_STRINGIZE(major, minor, micro)    \
    #major"."#minor"."#micro

#define NATIVE_BUFFER_VERSION_STRING NATIVE_BUFFER_VERSION_STRINGIZE(   \
        NATIVE_BUFFER_VERSION_MAJOR,                                    \
        NATIVE_BUFFER_VERSION_MINOR,                                    \
        NATIVE_BUFFER_VERSION_MICRO)

static int native_buffer_version_major; /**< major library version of native buffer at runtime */
static int native_buffer_version_minor; /**< minor library version of native buffer at runtime */
static int native_buffer_version_micro; /**< micro library version of native buffer at runtime */

#define NATIVE_BUFFER_IS_VALID(buffer) \
    ((buffer) && ((native_buffer_internal_t *)buffer)->ref_count)

#define NATIVE_BUFFER_RETURN_IF_FAIL(cond, val) {\
    if (!(cond)) {\
        NBP_ERR("%s is invalid\n", #cond);\
        return (val);\
    }\
}

#define NATIVE_BUFFER_RETURN_IF_NULL(cond, val) {\
    if (unlikely ((cond) == NULL)) {\
        NBP_ERR("%s is invalid\n", #cond);\
        return (val);\
    }\
}

#define SIZE_ALIGN( value, base ) (((value) + ((base) - 1)) & ~((base) - 1))

static void _native_buffer_version_check (void)
{
    native_buffer_version_major = NATIVE_BUFFER_VERSION_MAJOR;
    native_buffer_version_minor = NATIVE_BUFFER_VERSION_MINOR;
    native_buffer_version_micro = NATIVE_BUFFER_VERSION_MICRO;
}

NBP_PUBLIC int native_buffer_version (void)
{
    return NATIVE_BUFFER_VERSION;
}

NBP_PUBLIC const char *native_buffer_version_string (void)
{
    return NATIVE_BUFFER_VERSION_STRING;
}

static status_t _native_buffer_provider_core_init_fd (native_buffer_provider_t *provider)
{
    int dri2_ev_base, dri2_err_base, dri2_major, dri2_minor;
    char *drv_name, *dev_name;
    drm_magic_t magic;

    if (provider->type != NATIVE_BUFFER_PROVIDER_CORE)
        return STATUS_ERROR;

    if(!DRI2QueryExtension(provider->data.core.display, &dri2_ev_base, &dri2_err_base))
    {
        NBP_ERR ("Failed to get dri2 extension\n");
        return STATUS_ERROR;
    }

    if(!DRI2QueryVersion(provider->data.core.display, &dri2_major, &dri2_minor ))
    {
        NBP_ERR ("Failed to get dri2 version\n");
        return STATUS_ERROR;
    }

    if(!DRI2Connect(provider->data.core.display, provider->data.core.window, &drv_name, &dev_name ))
    {
        NBP_ERR ("Failed to get dri2 version\n");
        return STATUS_ERROR;
    }

    provider->data.core.fd = open(dev_name, O_RDWR);
    if (provider->data.core.fd < 0)
    {
        NBP_ERR ("Cannot open fd\n");
        free (drv_name);
        free (dev_name);
        return STATUS_ERROR;
    }

    free (drv_name);
    free (dev_name);

    if(drmGetMagic(provider->data.core.fd, &magic))
    {
        NBP_ERR ("Cannot get magic in drmGetMagic\n");
        return STATUS_ERROR;
    }

    if (!DRI2Authenticate(provider->data.core.display, provider->data.core.window, (unsigned int)magic))
    {
        NBP_ERR ("DRI2 authentication failed\n");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

static status_t _native_buffer_provider_core_init_bufmgr (native_buffer_provider_t *provider)
{
    if (provider->type != NATIVE_BUFFER_PROVIDER_CORE)
        return STATUS_ERROR;

    provider->data.core.bufmgr = tbm_bufmgr_init (provider->data.core.fd);
    NATIVE_BUFFER_RETURN_IF_NULL (provider->data.core.bufmgr, STATUS_ERROR);

    return STATUS_SUCCESS;
}

NBP_PUBLIC native_buffer_provider_t *native_buffer_provider_create (native_buffer_provider_type_t type)
{
    status_t status = STATUS_SUCCESS;

    if (type != NATIVE_BUFFER_PROVIDER_CORE)
    {
        NBP_ERR ("Invalid type\n");
        return NULL;
    }

    native_buffer_provider_t *provider = (native_buffer_provider_t *)calloc (1, sizeof (native_buffer_provider_t));
    NATIVE_BUFFER_RETURN_IF_NULL (provider, NULL);

    provider->type = type;
    switch (provider->type) {
        case NATIVE_BUFFER_PROVIDER_CORE:
        default:
        {
            provider->data.core.display = XOpenDisplay(0);
            if (unlikely (provider->data.core.display == NULL))
            {
                free (provider);
                return NULL;
            }

            provider->data.core.window = DefaultRootWindow (provider->data.core.display);

            status = _native_buffer_provider_core_init_fd (provider);
            if (unlikely (status))
            {
                native_buffer_provider_destroy (provider);
                return NULL;
            }

            status = _native_buffer_provider_core_init_bufmgr (provider);
            if (unlikely (status))
            {
                native_buffer_provider_destroy (provider);
                return NULL;
            }
        }
            break;
    }

    provider->buffer_count = 0;
    provider->mark_destroy = FALSE;

    pthread_mutex_init (&provider->lock, NULL);
    _native_buffer_version_check ();

    return provider;
}

NBP_PUBLIC status_t native_buffer_provider_destroy (native_buffer_provider_t *provider)
{
    NATIVE_BUFFER_RETURN_IF_NULL (provider, STATUS_NULL_POINTER);

    if (provider->buffer_count > 0)
    {
        // Defer destruction of provider until all buffers have been destroyed
        provider->mark_destroy = TRUE;
        return STATUS_SUCCESS;
    }

    switch (provider->type) {
        case NATIVE_BUFFER_PROVIDER_CORE:
        default:
        {
            if (provider->data.core.bufmgr)
            {
                tbm_bufmgr_deinit (provider->data.core.bufmgr);
            }

            if (provider->data.core.fd)
            {
                close (provider->data.core.fd);
            }

            if( provider->data.core.display )
            {
                XCloseDisplay(provider->data.core.display);
                provider->data.core.display = NULL;
            }
        }
            break;
    }

    pthread_mutex_destroy (&provider->lock);

    free (provider);

    return STATUS_SUCCESS;
}

static void _native_buffer_internal_init (native_buffer_internal_t *internal, native_buffer_provider_t *provider)
{
    internal->provider = provider;
    internal->display = NULL;
    internal->pixmap = 0;
    internal->is_external = FALSE;
    internal->egl_image = 0;
    internal->egl_surface = 0;
    internal->egl_display = 0;
    internal->egl_sync = 0;
    internal->egl_sync_surface = 0;
    internal->ref_count = 1;
    internal->lock_count = 0;
    internal->size = 0;
    internal->destroy_cb.func = NULL;
    internal->destroy_cb.data = NULL;
}

NBP_PUBLIC status_t native_buffer_internal_destroy_callback_set (native_buffer_internal_t *internal, native_buffer_internal_destroy_cb func, void *data)
{
    NATIVE_BUFFER_RETURN_IF_NULL (internal, STATUS_NULL_POINTER);

    internal->destroy_cb.func = func;
    internal->destroy_cb.data = data;

    return STATUS_SUCCESS;
}

NBP_PUBLIC status_t native_buffer_internal_destroy_callback_unset (native_buffer_internal_t *internal)
{
    NATIVE_BUFFER_RETURN_IF_NULL (internal, STATUS_NULL_POINTER);

    internal->destroy_cb.func = NULL;
    internal->destroy_cb.data = NULL;

    return STATUS_SUCCESS;
}

static int _calculate_stride (int width, native_buffer_format_t format)
{
    int stride;
    if (format == NATIVE_BUFFER_FORMAT_YV12 ||
        format == NATIVE_BUFFER_FORMAT_I420)
    {
#if USE_MALI
        stride = SIZE_ALIGN (width, 16);
#else
        stride = SIZE_ALIGN (width, 32);
#endif
    }
    else if (format == NATIVE_BUFFER_FORMAT_NV12 ||
        format == NATIVE_BUFFER_FORMAT_NV12T ||
        format == NATIVE_BUFFER_FORMAT_NV21)
    {
#if USE_MALI
        stride = SIZE_ALIGN (width, 16);
#else
        stride = SIZE_ALIGN (width, 32);
#endif
    }
    else
    {
        int bpp = 0;
        switch (format)
        {
            case NATIVE_BUFFER_FORMAT_RGBA_8888:
            case NATIVE_BUFFER_FORMAT_RGBX_8888:
            case NATIVE_BUFFER_FORMAT_BGRA_8888:
                bpp = 4;
                break;
            case NATIVE_BUFFER_FORMAT_RGB_888:
                bpp = 3;
                break;
            case NATIVE_BUFFER_FORMAT_RGB_565:
                bpp = 2;
                break;
            case NATIVE_BUFFER_FORMAT_A_8:
                bpp = 1;
                break;
            default:
                bpp = 4;
                break;
        }
#if USE_MALI
        stride = SIZE_ALIGN (width, 16) * bpp;
#else
        stride = SIZE_ALIGN (width, 32) * bpp;
#endif
    }
    return stride;
}

static int _calculate_size (int height, int stride, native_buffer_format_t format, int aligned)
{
    int size;
    if (format == NATIVE_BUFFER_FORMAT_YV12 ||
        format == NATIVE_BUFFER_FORMAT_I420)
    {
#if USE_MALI
        if (aligned)
            size = height * (stride + SIZE_ALIGN (stride/2, 16));
        else
            size = height * (stride + stride/2);
#else
        size = height * (stride + stride/2);
#endif
    }
    else if (format == NATIVE_BUFFER_FORMAT_NV12 ||
        format == NATIVE_BUFFER_FORMAT_NV12T ||
        format == NATIVE_BUFFER_FORMAT_NV21)
    {
#if USE_MALI
        size = height * (stride + SIZE_ALIGN (stride/2, 16));
#else
        size = height * (stride + stride/2);
#endif
    }
    else
    {
        size = stride * height;
    }
    return size;
}

#if USE_X_ERROR_HANDLER
static XErrorHandler x_error_handler_old = (XErrorHandler)0;
static int x_error_handler (Display *display, XErrorEvent *event )
{
    char error_text[256];

    XGetErrorText( display, event->error_code, error_text, 256 );
    NBP_ERR("X Error handler --> msg is  %s\n", error_text);

    return 0;
}
#endif

static status_t _native_buffer_init_from_pixmap (native_buffer_provider_t *provider, native_buffer_internal_t *buffer)
{
    DRI2Buffer *buf = NULL;
    unsigned int attach[1] = { DRI2BufferFrontLeft };
    int num, width, height;
    Drawable drawable;

    if (provider->type != NATIVE_BUFFER_PROVIDER_CORE)
    {
        return STATUS_ERROR;
    }

    // To ensure that XPixmap has been successfully created before calling DRI2GetBuffers
    // Otherwise, invalid drawable error will occur
    XSync (provider->data.core.display, FALSE);

    drawable = (Drawable)buffer->pixmap;
    DRI2CreateDrawable (provider->data.core.display, drawable);

#if USE_X_ERROR_HANDLER
    x_error_handler_old = XSetErrorHandler(x_error_handler);
#endif
    buf = DRI2GetBuffers (provider->data.core.display, drawable, &width, &height, attach, 1, &num);
#if USE_X_ERROR_HANDLER
    (void)XSetErrorHandler (x_error_handler_old);
#endif
    if (!buf || !buf->name)
    {
        NBP_ERR ("DRI2GetBuffers failed! buf: %d, width: %d, height: %d, num: %d\n", (int)buf, width, height, num);
        if (buf)
            XFree (buf);
        DRI2DestroyDrawable (provider->data.core.display, drawable);
        return STATUS_ERROR;
    }

    buffer->base.width = width;
    buffer->base.height = height;
    buffer->base.stride = buf->pitch;
    buffer->base.bo = tbm_bo_import (provider->data.core.bufmgr, buf->name);
    if (!buffer->base.bo)
    {
        NBP_ERR ("Failed to import bo: %d\n", buf->name);
        XFree (buf);
        DRI2DestroyDrawable (provider->data.core.display, drawable);
        return STATUS_ERROR;
    }
    buffer->size = tbm_bo_size (buffer->base.bo);
    NBP_DBG ("tbm bo %x imported from pixmap %x\n", (int)buffer->base.bo, (int)buffer->pixmap);

    XFree (buf);
    DRI2DestroyDrawable (provider->data.core.display, drawable);

    return STATUS_SUCCESS;
}

NBP_PUBLIC native_buffer_t *native_buffer_create (native_buffer_provider_t *provider,
            int width, int height, int requested_stride, native_buffer_format_t format, int usage)
{
    native_buffer_internal_t *buffer;

    NATIVE_BUFFER_RETURN_IF_NULL (provider, NULL);

    if (width < 1 || height < 1)
    {
        NBP_ERR ("Incorrect input (width: %d, height: %d)\n", width, height);
        return NULL;
    }

    buffer = (native_buffer_internal_t *)calloc (1, sizeof (native_buffer_internal_t));
    NATIVE_BUFFER_RETURN_IF_NULL (buffer, NULL);

    _native_buffer_internal_init (buffer, provider);

    buffer->base.version = NATIVE_BUFFER_VERSION;

    if (provider->type == NATIVE_BUFFER_PROVIDER_CORE)
    {
        if (usage & NATIVE_BUFFER_USAGE_3D_RENDER)
        {
            if (format != NATIVE_BUFFER_FORMAT_RGBA_8888 &&
                format != NATIVE_BUFFER_FORMAT_BGRA_8888 &&
                format != NATIVE_BUFFER_FORMAT_RGB_888 &&
                format != NATIVE_BUFFER_FORMAT_RGBX_8888)
            {
                NBP_ERR ("Invalid format for specified usage. Format should be either NATIVE_BUFFER_FORMAT_RGBA_8888 or NATIVE_BUFFER_FORMAT_BGRA_8888");
                free (buffer);
                return NULL;
            }

            GC gc = NULL;
            XGCValues gcv;
            buffer->display = provider->data.core.display;
            buffer->pixmap = XCreatePixmap (provider->data.core.display, provider->data.core.window, width, height,
                        DefaultDepth (provider->data.core.display, XDefaultScreen(provider->data.core.display)));
            if (unlikely (buffer->pixmap == NULL))
            {
                NBP_ERR ("XCreatePixmap failed\n");
                free (buffer);
                return NULL;
            }

            // Let's clear the buffer
            gcv.foreground = 0;
            gc = XCreateGC (provider->data.core.display, buffer->pixmap, GCForeground, &gcv);
            if (!gc)
            {
                NBP_ERR ("XCreateGC failed\n");
                free (buffer);
                return NULL;
            }
            XFillRectangle(provider->data.core.display, buffer->pixmap, gc, 0, 0, width, height);
            _native_buffer_init_from_pixmap (provider, buffer);
            buffer->base.format = format;
            XFreeGC(provider->data.core.display, gc);
        }
        else
        {
            tbm_bo_handle handle;
            buffer->base.width = width;
            buffer->base.height = height;
            buffer->base.format = format;
            buffer->base.stride = requested_stride > 0 ? requested_stride : _calculate_stride (width, format);
            buffer->size = _calculate_size (height, buffer->base.stride, format, (requested_stride == 0));
            buffer->base.bo = tbm_bo_alloc (provider->data.core.bufmgr, buffer->size, TBM_BO_DEFAULT);
            if (unlikely (buffer->base.bo == NULL))
            {
                NBP_ERR ("tbm_bo_alloc failed\n");
                free (buffer);
                return NULL;
            }

            // Let's clear the buffer
            handle = tbm_bo_map (buffer->base.bo, TBM_DEVICE_CPU, TBM_OPTION_READ|TBM_OPTION_WRITE);
            if (unlikely (handle.ptr == NULL))
            {
                NBP_ERR ("tbm_bo_map failed\n");
                free (buffer);
                return NULL;
            }

            memset (handle.ptr, 0, buffer->size);
            tbm_bo_unmap (buffer->base.bo);
        }

    }

    pthread_mutex_lock (&provider->lock);
    provider->buffer_count++;
    pthread_mutex_unlock (&provider->lock);

    return &buffer->base;
}

NBP_PUBLIC native_buffer_t *native_buffer_create_for_tbm (native_buffer_provider_t *provider, void *bo,
            int width, int height, int requested_stride, native_buffer_format_t format)
{
    native_buffer_internal_t *buffer;

    NATIVE_BUFFER_RETURN_IF_NULL (provider, NULL);

    if (provider->type != NATIVE_BUFFER_PROVIDER_CORE)
    {
        NBP_ERR ("Provider type should be CORE type\n");
        return NULL;
    }

    NATIVE_BUFFER_RETURN_IF_NULL (bo, NULL);

    buffer = (native_buffer_internal_t *)calloc (1, sizeof (native_buffer_internal_t));
    NATIVE_BUFFER_RETURN_IF_NULL (buffer, NULL);

    _native_buffer_internal_init (buffer, provider);

    unsigned int key = tbm_bo_export (bo);

    buffer->base.version = NATIVE_BUFFER_VERSION;
    buffer->base.width = width;
    buffer->base.height = height;
    buffer->base.format = format;
    buffer->base.stride = requested_stride > 0 ? requested_stride : _calculate_stride (width, format);
    buffer->size = _calculate_size (height, buffer->base.stride, format, (requested_stride == 0));
    buffer->base.bo = tbm_bo_import (provider->data.core.bufmgr, key);
    buffer->is_external = TRUE;

    pthread_mutex_lock (&provider->lock);
    provider->buffer_count++;
    pthread_mutex_unlock (&provider->lock);

    return &buffer->base;
}

NBP_PUBLIC status_t native_buffer_destroy (native_buffer_t *buffer)
{
    native_buffer_internal_t *internal = (native_buffer_internal_t *)buffer;
    NATIVE_BUFFER_RETURN_IF_NULL (internal, STATUS_NULL_POINTER);

    if (internal->ref_count <= 0)
    {
        return STATUS_ERROR;
    }
    internal->ref_count--;

    if (internal->destroy_cb.func)
        internal->destroy_cb.func (internal, internal->destroy_cb.data);

    if (internal->base.bo)
    {
        tbm_bo_unref (internal->base.bo);
        internal->base.bo = NULL;
    }

    if (!internal->is_external && internal->pixmap)
    {
        XFreePixmap (internal->display, internal->pixmap);
        internal->pixmap = 0;
    }

    pthread_mutex_lock (&internal->provider->lock);
    internal->provider->buffer_count--;
    pthread_mutex_unlock (&internal->provider->lock);
    if (internal->provider->buffer_count <= 0 &&
        internal->provider->mark_destroy)
    {
        native_buffer_provider_destroy (internal->provider);
    }

    free (internal);

    return STATUS_SUCCESS;
}

NBP_PUBLIC status_t native_buffer_lock (native_buffer_t *buffer, int usage, int option, void** addr)
{
    native_buffer_internal_t *internal;
    if (unlikely (buffer == NULL))
    {
        NBP_ERR ("Invalid native buffer object\n");
        *addr = NULL;
        return STATUS_NULL_POINTER;
    }

    internal = (native_buffer_internal_t *)buffer;

    if (internal->provider && internal->provider->type == NATIVE_BUFFER_PROVIDER_CORE)
    {
        int tbm_usage = 0, tbm_option = 0;
        switch (usage)
        {
            case NATIVE_BUFFER_USAGE_CPU:
                tbm_usage = TBM_DEVICE_CPU;
                break;
            case NATIVE_BUFFER_USAGE_2D:
                tbm_usage = TBM_DEVICE_2D;
                break;
            case NATIVE_BUFFER_USAGE_3D_TEXTURE:
            case NATIVE_BUFFER_USAGE_3D_RENDER:
                tbm_usage = TBM_DEVICE_3D;
                break;
            case NATIVE_BUFFER_USAGE_MM:
                tbm_usage = TBM_DEVICE_MM;
                break;
            case NATIVE_BUFFER_USAGE_DEFAULT:
            default:
                tbm_usage = TBM_DEVICE_DEFAULT;
                break;
        }

        if (option & NATIVE_BUFFER_ACCESS_OPTION_READ)
            tbm_option |= TBM_OPTION_READ;
        else if (option & NATIVE_BUFFER_ACCESS_OPTION_WRITE)
            tbm_option |= TBM_OPTION_WRITE;

        tbm_bo_handle handle = tbm_bo_map (buffer->bo, tbm_usage, tbm_option);
        *addr = handle.ptr;
        internal->lock_count++;
    }
    else
    {
        *addr = NULL;
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

NBP_PUBLIC status_t native_buffer_unlock (native_buffer_t *buffer)
{
    native_buffer_internal_t *internal;
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), STATUS_NULL_POINTER);

    internal = (native_buffer_internal_t *)buffer;
    if (!internal->lock_count)
    {
        return STATUS_ERROR;
    }

    if (internal->provider && internal->provider->type == NATIVE_BUFFER_PROVIDER_CORE)
    {
        tbm_bo_unmap (buffer->bo);
        internal->lock_count--;
    }
    else
    {
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

NBP_PUBLIC int native_buffer_get_lock_count (const native_buffer_t *buffer)
{
    native_buffer_internal_t *internal;
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), -1);

    internal = (native_buffer_internal_t *)buffer;
    return internal->lock_count;
}

NBP_PUBLIC int native_buffer_get_width (const native_buffer_t *buffer)
{
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), -1);

    return buffer->width;
}

NBP_PUBLIC int native_buffer_get_height (const native_buffer_t *buffer)
{
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), -1);

    return buffer->height;
}

NBP_PUBLIC int native_buffer_get_stride (const native_buffer_t *buffer)
{
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), -1);

    return buffer->stride;
}

NBP_PUBLIC int native_buffer_get_size (const native_buffer_t *buffer)
{
    native_buffer_internal_t *internal;
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), -1);

    internal = (native_buffer_internal_t *)buffer;
    return internal->size;
}

NBP_PUBLIC native_buffer_format_t native_buffer_get_format (const native_buffer_t *buffer)
{
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), -1);

    return buffer->format;
}

NBP_PUBLIC native_buffer_provider_t *native_buffer_get_provider (const native_buffer_t *buffer)
{
    native_buffer_internal_t *internal;
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), NULL);

    internal = (native_buffer_internal_t *)buffer;
    return internal->provider;
}

NBP_PUBLIC void *native_buffer_get_bo (const native_buffer_t *buffer)
{
    native_buffer_internal_t *internal;
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), NULL);

    internal = (native_buffer_internal_t *)buffer;

    if (internal->provider && internal->provider->type == NATIVE_BUFFER_PROVIDER_CORE)
    {
        return buffer->bo;
    }
    return NULL;
}

NBP_PUBLIC void *native_buffer_get_pixmap (const native_buffer_t *buffer)
{
    native_buffer_internal_t *internal;
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), NULL);

    internal = (native_buffer_internal_t *)buffer;

    if (internal->provider && internal->provider->type == NATIVE_BUFFER_PROVIDER_CORE)
    {
        return (void *)internal->pixmap;
    }
    return NULL;
}

NBP_PUBLIC status_t native_buffer_handle_socket_send (const native_buffer_t *buffer, int socket_fd)
{
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), STATUS_NULL_POINTER);

    NBP_ERR ("This function is not supported yet");

    return STATUS_ERROR;
}

NBP_PUBLIC native_buffer_t *native_buffer_handle_socket_receive (native_buffer_provider_t *provider, int socket_fd)
{
    NATIVE_BUFFER_RETURN_IF_NULL (provider, NULL);

    NBP_ERR ("This function is not supported yet");

    return NULL;
}

NBP_PUBLIC status_t native_buffer_madvise (native_buffer_t *buffer, int offset, int length, native_buffer_advise_t advice)
{
    NATIVE_BUFFER_RETURN_IF_FAIL (NATIVE_BUFFER_IS_VALID (buffer), STATUS_NULL_POINTER);

    NBP_ERR ("This function is not supported yet");

    return STATUS_ERROR;
}

