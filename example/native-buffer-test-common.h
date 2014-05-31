/**************************************************************************

Native Buffer Test Case

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

#include <native-buffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <X11/Xlib.h>

#define SURFACE_WIDTH   512
#define SURFACE_HEIGHT  512


PFNEGLCREATEIMAGEKHRPROC fneglCreateImageKHR;
PFNGLEGLIMAGETARGETTEXTURE2DOESPROC fnglEGLImageTargetTexture2DOES;
PFNEGLDESTROYIMAGEKHRPROC fneglDestroyImageKHR;

typedef struct _AppDataCommon {
    native_buffer_provider_t *provider;

    Display *display;
    Window window;

    EGLDisplay egl_display;
    EGLContext egl_context;
    EGLSurface egl_surface;

    unsigned int vertex_shader;
    unsigned int fragment_shader;
    unsigned int program;

    int mvp_loc;
    int sampler_loc;

} AppDataCommon;

void fill_YV12_buffer(unsigned char* buf, int w, int h, int stride, int size);

void create_common(AppDataCommon *app_data);

void destroy_common(AppDataCommon *app_data);

void create_window(AppDataCommon *app_data);

void destroy_window(AppDataCommon *app_data);

void egl_init(AppDataCommon *app_data);

void egl_fini(AppDataCommon *app_data);

void gles_init(AppDataCommon *app_data);

void gles_fini(AppDataCommon *app_data);


