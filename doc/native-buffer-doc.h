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

#ifndef __NATIVE_BUFFER_DOC_H__
#define __NATIVE_BUFFER_DOC_H__

/**
 * @internal
 * @ingroup CAPI_UI_FRAMEWORK
 * @defgroup NATIVE_BUFFER Native Buffer
 * @brief Provides buffer that can be shared between different types of hardware.
 *
 * @section NATIVE_BUFFER_HEADER Required Header
 *   \#include <native-buffer.h>
 *
 * @section NATIVE_BUFFER_OVERVIEW Overview
 * Provides buffer that can be written into or read from using CPU, GPU or video
 * without extra memory copy overhead.
 * Native buffer can be created with specified format and usage options.
 * Supported formats include commonly used
 * RGB colorspace formats (RGBA8888, RGBX8888, RGB888, RGB565, BGRA8888, A8),
 * and YUV colorspace formats (YV12, I420, NV12, NV21, NV12T).
 * The buffer can be locked for one or more specific uses or purposes.
 * It may be locked for CPU to gain read/write access, or GPU, video to use.
 * Native buffer can also be bound to OpenGL ES texture object using EGL image.
 * The texture object should use the GL_TEXTURE_EXTERNAL_OES texture target,
 * which is defined by GL_OES_EGL_image_external OpenGL ES extension.
 * Also, EGL_TIZEN_image_native_buffer EGL extension should be supported to
 * create an EGL image from native buffer.
 *
 */

#endif /* __NATIVE_BUFFER_DOC_H__ */

