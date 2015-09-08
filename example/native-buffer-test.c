/**************************************************************************

Native Buffer Test Case

Copyright 2013 Samsung Electronics co., Ltd. All Rights Reserved.

Contact: Dongyeon Kim <dy5.kim@samsung.com>
              Inpyo Kang <mantiger@samsung.com>

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


#include "native-buffer-test-common.h"

#define WIDTH   512
#define HEIGHT  512


typedef struct _AppData {
    AppDataCommon base;

    native_buffer_t *buffer;
    EGLImageKHR egl_image;
    GLuint texture;
} AppData;

void render_pre (AppData *app_data)
{
    EGLint attrs[] =
    {
        EGL_IMAGE_PRESERVED_KHR,    EGL_TRUE,
        EGL_NONE,
    };

    eglMakeCurrent (app_data->base.egl_display, app_data->base.egl_surface, app_data->base.egl_surface, app_data->base.egl_context);

    glGenTextures (1, &app_data->texture);
    glBindTexture (GL_TEXTURE_EXTERNAL_OES, app_data->texture);
    glTexParameteri (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    app_data->egl_image = fneglCreateImageKHR (app_data->base.egl_display, EGL_NO_CONTEXT, 0x32A0/*EGL_NATIVE_BUFFER_TIZEN*/,
        app_data->buffer, attrs);
    printf("egl_image: %d (err: %x)\n", app_data->egl_image, eglGetError());
    fnglEGLImageTargetTexture2DOES (GL_TEXTURE_EXTERNAL_OES, app_data->egl_image);
}

void render_post (AppData *app_data)
{
    glBindTexture (GL_TEXTURE_EXTERNAL_OES, 0);
    fneglDestroyImageKHR (app_data->base.egl_display, app_data->egl_image);
    glDeleteTextures (1, &app_data->texture);

    char *pixels = (char *)malloc (WIDTH*HEIGHT*4*sizeof(char));
    glReadPixels (0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    free (pixels);
}

void render(AppData *app_data)
{
    int i;

    render_pre (app_data);

    for (i=0; i<100; i++) {
        GLfloat mvp[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

        eglMakeCurrent (app_data->base.egl_display, app_data->base.egl_surface, app_data->base.egl_surface, app_data->base.egl_context);
        glClear (GL_COLOR_BUFFER_BIT);

        glBindTexture (GL_TEXTURE_EXTERNAL_OES, app_data->texture);

        glUniformMatrix4fv (app_data->base.mvp_loc, 1, GL_FALSE, &mvp[0][0]);
        glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

        eglSwapBuffers (app_data->base.egl_display, app_data->base.egl_surface);
    }

    render_post (app_data);
}

void test_format_rgba8888 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_RGBA_8888, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d) - Red should be shown on screen\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    {
        int i, j;
        for (j=0; j<HEIGHT; j++) {
            for (i=0; i<WIDTH; i++) {
                ptr[(j*WIDTH+i)*4+0] = 255; //R
                ptr[(j*WIDTH+i)*4+1] = 0; //G
                ptr[(j*WIDTH+i)*4+2] = 0; //B
                ptr[(j*WIDTH+i)*4+3] = 255; //A
            }
        }
    }
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_rgbx8888 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_RGBX_8888, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d) - Green should be shown on screen\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    {
        int i, j;
        for (j=0; j<HEIGHT; j++) {
            for (i=0; i<WIDTH; i++) {
                ptr[(j*WIDTH+i)*4+0] = 0; //R
                ptr[(j*WIDTH+i)*4+1] = 255; //G
                ptr[(j*WIDTH+i)*4+2] = 0; //B
            }
        }
    }
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_rgb888 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_RGB_888, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d) - Blue should be shown on screen\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    {
        int i, j;
        for (j=0; j<HEIGHT; j++) {
            for (i=0; i<WIDTH; i++) {
                ptr[(j*WIDTH+i)*3+0] = 0; //R
                ptr[(j*WIDTH+i)*3+1] = 0; //G
                ptr[(j*WIDTH+i)*3+2] = 255; //B
            }
        }
    }
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_rgb565 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_RGB_565, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d) - Blue should be shown on screen\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    {
        int i, j;
        for (j=0; j<HEIGHT; j++) {
            for (i=0; i<WIDTH; i++) {
                // Blue
                ptr[(j*WIDTH+i)*2+0] = 0;
                ptr[(j*WIDTH+i)*2+1] = 0x1F;
            }
        }
    }
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_bgra8888 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_BGRA_8888, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d) - Blue should be shown on screen\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    {
        int i, j;
        for (j=0; j<HEIGHT; j++) {
            for (i=0; i<WIDTH; i++) {
                ptr[(j*WIDTH+i)*4+0] = 255; // B
                ptr[(j*WIDTH+i)*4+1] = 0; // G
                ptr[(j*WIDTH+i)*4+2] = 0; // R
                ptr[(j*WIDTH+i)*4+3] = 255; //A
            }
        }
    }
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_nv12 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_NV12, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d)\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    // fill buffer
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_nv21 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_NV21, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d)\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    // fill buffer
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_i420 (AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_I420, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d)\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    fill_YV12_buffer (ptr, WIDTH, HEIGHT, native_buffer_get_stride(app_data->buffer), native_buffer_get_size(app_data->buffer));
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

void test_format_yv12(AppData *app_data)
{
    app_data->buffer = native_buffer_create (app_data->base.provider, WIDTH, HEIGHT, 0,
        NATIVE_BUFFER_FORMAT_YV12, NATIVE_BUFFER_USAGE_CPU);
    printf("%s(stride %d)\n", __func__, native_buffer_get_stride (app_data->buffer));

    char *ptr;
    native_buffer_lock (app_data->buffer, NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, (void**)&ptr);
    fill_YV12_buffer (ptr, WIDTH, HEIGHT, native_buffer_get_stride(app_data->buffer), native_buffer_get_size(app_data->buffer));
    native_buffer_unlock (app_data->buffer);

    render (app_data);
    native_buffer_destroy (app_data->buffer);
}

int main (int argc, char **argv)
{
    int i;
    AppData *app_data = (AppData *)malloc (sizeof (AppData));
    if (!app_data)
        return 0;

    create_window ((AppDataCommon *)app_data);
    egl_init ((AppDataCommon *)app_data);
    create_common ((AppDataCommon *)app_data);
    gles_init ((AppDataCommon *)app_data);

    test_format_rgba8888 (app_data);
    test_format_rgbx8888 (app_data);
    //test_format_rgb888 (app_data);
    //test_format_rgb565 (app_data);
    test_format_bgra8888 (app_data);
    test_format_nv12 (app_data);
    test_format_nv21 (app_data);
    test_format_i420 (app_data);
    test_format_yv12 (app_data);

    gles_fini ((AppDataCommon *)app_data);
    destroy_common ((AppDataCommon *)app_data);
    egl_fini ((AppDataCommon *)app_data);
    destroy_window ((AppDataCommon *)app_data);

    if (app_data)
        free (app_data);
    return 0;
}


