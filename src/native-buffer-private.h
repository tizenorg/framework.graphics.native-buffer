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

#ifndef __NATIVE_BUFFER_PRIVATE_H__
#define __NATIVE_BUFFER_PRIVATE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <X11/Xlib.h>

#include <drm.h>
#include <tbm_bufmgr.h>

#include <fcntl.h>
#include <X11/Xmd.h>
#include <dri2.h>
#include <xf86drm.h>
#include <unistd.h>

#include <dlfcn.h>

#include <assert.h>

#include "native-buffer.h"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifndef EGL_NATIVE_BUFFER_TIZEN
#define EGL_NATIVE_BUFFER_TIZEN 0x32A0
#endif

#define USE_FENCE_SYNC              1
#define USE_X_ERROR_HANDLER         0
#define USE_DEBUG_LOG               1

typedef void native_display;
typedef unsigned int native_pixmap;

#define NATIVE_BUFFER_VERSION       1

struct native_buffer_provider {
    native_buffer_provider_type_t type;

    union {
        struct {
            Display *display;
            Window window;
            int fd;
            tbm_bufmgr bufmgr;
        } core;
    } data;

    int buffer_count;
    int mark_destroy;

    pthread_mutex_t lock;
};

struct native_buffer
{
    int version;

    int width;
    int height;
    int stride;
    int format;

    void *bo;
};

typedef struct native_buffer_internal native_buffer_internal_t;

typedef void (*native_buffer_internal_destroy_cb) (native_buffer_internal_t *internal, void *data);
status_t native_buffer_internal_destroy_callback_set (native_buffer_internal_t *internal, native_buffer_internal_destroy_cb func, void *data);
status_t native_buffer_internal_destroy_callback_unset (native_buffer_internal_t *internal);


struct native_buffer_internal
{
    struct native_buffer base;

    struct native_buffer_provider *provider;
    native_display *display;
    native_pixmap pixmap;
    int is_external;

    void *egl_image;
    void *egl_surface;
    void *egl_display;
    void *egl_sync;
    void *egl_sync_surface;

    int ref_count;
    int lock_count;

    int size;

    struct {
        native_buffer_internal_destroy_cb func;
        void *data;
    } destroy_cb;
};

#define LOG_TAG "NBP"

#include <dlog.h>

#define NBP_ERR(fmt, arg...) LOGE(" "fmt, ##arg)
#define NBP_WARN(fmt, arg...) LOGW(" "fmt, ##arg)
#if USE_DEBUG_LOG
#define NBP_DBG(fmt, arg...) LOGD(" "fmt, ##arg)
#else
#define NBP_DBG(fmt, arg...)
#endif

#if __GNUC__ >= 4
#define NBP_PUBLIC  __attribute__ ((visibility("default")))
#define NBP_PRIVATE __attribute__ ((visibility("hidden")))
#else
#define NBP_PUBLIC
#define NBP_PRIVATE
#endif

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define likely(expr) (__builtin_expect (!!(expr), 1))
#define unlikely(expr) (__builtin_expect (!!(expr), 0))
#else
#define likely(expr) (expr)
#define unlikely(expr) (expr)
#endif

#endif /* __NATIVE_BUFFER_PRIVATE__ */
