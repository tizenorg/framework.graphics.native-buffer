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

#include "native-buffer-test-common.h"

#define SHADER_ATTRIBUTE_POSITION   0
#define SHADER_ATTRIBUTE_TEXCOORD   1

static char *shader_str[2] =
{
    "uniform mat4 u_mvp_matrix; attribute vec4 a_position; attribute vec2 a_texcoord; varying vec2 v_texcoord; void main() { gl_Position = u_mvp_matrix * a_position; v_texcoord = a_texcoord; }",
    "#extension GL_OES_EGL_image_external : require\n precision mediump float; varying vec2 v_texcoord; uniform samplerExternalOES s_texture; void main() { gl_FragColor = texture2D(s_texture, v_texcoord); }"
};

void fill_YV12_buffer(unsigned char* buf, int w, int h, int stride, int size) {
    FILE *file = fopen("./data/YV12_512_512.yuv", "r");
    if (file == NULL) {
        printf("Failed to load raw data!\n");
        return;
    }

    unsigned char *data = (unsigned char*)malloc (size);
    fread (data, sizeof (unsigned char), size, file);
    memcpy (buf, data, size);
    free (data);
    fclose (file);
}

void create_common(AppDataCommon *app_data)
{
    app_data->provider = native_buffer_provider_create (NATIVE_BUFFER_PROVIDER_CORE);

    return;
}

void destroy_common(AppDataCommon *app_data)
{
    native_buffer_provider_destroy (app_data->provider);

    return;
}

void create_window(AppDataCommon *app_data)
{
    app_data->display = XOpenDisplay (0);
    app_data->window = XCreateSimpleWindow (app_data->display, DefaultRootWindow (app_data->display),
        0, 0, SURFACE_WIDTH, SURFACE_HEIGHT, 0, WhitePixel (app_data->display, 0), BlackPixel (app_data->display, 0));
    XMapWindow (app_data->display, app_data->window);
}

void destroy_window(AppDataCommon *app_data)
{
    XFlush(app_data->display);
    XDestroyWindow (app_data->display, app_data->window);
    XCloseDisplay (app_data->display);
}

void egl_init(AppDataCommon *app_data)
{
    EGLint major, minor, num;
    EGLConfig egl_config;
    EGLint attr[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_STENCIL_SIZE, 0,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 0,
        EGL_NONE
    };
    EGLint ctxattr[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    app_data->egl_display = eglGetDisplay (app_data->display);
    if (app_data->egl_display == EGL_NO_DISPLAY)
    {
        printf("[TEST] eglGetDisplay ERROR\n");
        goto error;
    }

    if (!eglInitialize (app_data->egl_display, &major, &minor))
    {
        printf("[TEST] eglInitialize ERROR\n");
        goto error;
    }

    if (!eglBindAPI (EGL_OPENGL_ES_API))
    {
        printf("[TEST] eglBindAPI ERROR\n");
        goto error;
    }

    if (!eglChooseConfig (app_data->egl_display, attr, &egl_config, 1, &num))
    {
        printf("[TEST] eglChooseConfig ERROR\n");
        goto error;
    }

    if (num != 1)
    {
        printf("[TEST] eglChooseConfig did not get exactly one config: %d\n", num);
        goto error;
    }

    app_data->egl_surface = eglCreateWindowSurface (app_data->egl_display, egl_config, (NativeWindowType)app_data->window, NULL);
    if (app_data->egl_surface == EGL_NO_SURFACE)
    {
        printf("[TEST] eglCreateWindowSurface ERROR\n");
        goto error;
    }

    app_data->egl_context = eglCreateContext (app_data->egl_display, egl_config, EGL_NO_CONTEXT, ctxattr);
    if (app_data->egl_context == EGL_NO_CONTEXT)
    {
        printf("[TEST] eglCreateContext ERROR\n");
        goto error;
    }

    printf("eglDisplay: %x, eglSurface: %x, eglContext: %x\n", (int)app_data->egl_display, (int)app_data->egl_surface,
        (int)app_data->egl_context);
    eglMakeCurrent (app_data->egl_display, app_data->egl_surface, app_data->egl_surface, app_data->egl_context);

    fneglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC) eglGetProcAddress("eglCreateImageKHR");
    fnglEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress("glEGLImageTargetTexture2DOES");
    fneglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC) eglGetProcAddress("eglDestroyImageKHR");

    return;
error:
    exit(1);
}

void egl_fini(AppDataCommon *app_data)
{
    eglDestroySurface (app_data->egl_display, app_data->egl_surface);
    eglDestroyContext (app_data->egl_display, app_data->egl_context);
    eglTerminate (app_data->egl_display);
}

void gles_init(AppDataCommon *app_data)
{
    int shader_status;

    static GLfloat vertices[] =
    {
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    static GLfloat texcoords[] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    app_data->vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    app_data->fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
    app_data->program = glCreateProgram ();

    glShaderSource (app_data->vertex_shader, 1, (const char **)&shader_str[0], NULL);
    glCompileShader (app_data->vertex_shader);
    glGetShaderiv (app_data->vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if (shader_status != GL_TRUE)
    {
        printf("[TEST] Vertex shader compile ERROR\n");
        goto error;
    }

    glShaderSource (app_data->fragment_shader, 1, (const char **)&shader_str[1], NULL);
    glCompileShader (app_data->fragment_shader);
    glGetShaderiv (app_data->fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if (shader_status != GL_TRUE)
    {
        printf("[TEST] Fragment shader compile ERROR\n");
        goto error;
    }

    glAttachShader (app_data->program, app_data->vertex_shader);
    glAttachShader (app_data->program, app_data->fragment_shader);

    glBindAttribLocation (app_data->program, SHADER_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation (app_data->program, SHADER_ATTRIBUTE_TEXCOORD, "a_texcoord");

    glLinkProgram (app_data->program);
    glGetProgramiv (app_data->program, GL_LINK_STATUS, &shader_status);
    if (shader_status != GL_TRUE)
    {
        printf("[TEST] Shader program link ERROR\n");
        goto error;
    }

    glValidateProgram (app_data->program);
    glGetProgramiv (app_data->program, GL_VALIDATE_STATUS, &shader_status);
    if (shader_status != GL_TRUE)
    {
        printf("[TEST] Shader program validation ERROR\n");
        goto error;
    }

    app_data->mvp_loc = glGetUniformLocation (app_data->program, "u_mvp_matrix");
    app_data->sampler_loc = glGetUniformLocation (app_data->program, "s_texture");

    glUseProgram (app_data->program);
    glUniform1i (app_data->sampler_loc, 0);

    glClearColor (0.5f, 0.5f, 0.5f, 1.0f);

    glVertexAttribPointer (SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray (SHADER_ATTRIBUTE_POSITION);

    glVertexAttribPointer (SHADER_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
    glEnableVertexAttribArray (SHADER_ATTRIBUTE_TEXCOORD);

    return;
error:
    exit(1);
}

void gles_fini(AppDataCommon *app_data)
{
    eglMakeCurrent (app_data->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    glDeleteShader (app_data->vertex_shader);
    glDeleteShader (app_data->fragment_shader);
    glDeleteProgram (app_data->program);
}



