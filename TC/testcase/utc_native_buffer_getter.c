/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <tet_api.h>
#include <native-buffer.h>
#include <tbm_bufmgr.h>
#include <stdlib.h>

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_native_buffer_get_width_test(void);
static void utc_native_buffer_get_height_test(void);
static void utc_native_buffer_get_stride_test(void);
static void utc_native_buffer_get_size_test(void);
static void utc_native_buffer_get_format_test(void);
static void utc_native_buffer_get_provider_test(void);
static void utc_native_buffer_get_bo_test(void);
static void utc_native_buffer_get_pixmap_test(void);


struct tet_testlist tet_testlist[] = {
	{ utc_native_buffer_get_width_test, POSITIVE_TC_IDX },
	{ utc_native_buffer_get_height_test, POSITIVE_TC_IDX },
	{ utc_native_buffer_get_stride_test, POSITIVE_TC_IDX },
    { utc_native_buffer_get_size_test, POSITIVE_TC_IDX },
    { utc_native_buffer_get_format_test, POSITIVE_TC_IDX },
    { utc_native_buffer_get_provider_test, POSITIVE_TC_IDX },
    { utc_native_buffer_get_bo_test, POSITIVE_TC_IDX },
    { utc_native_buffer_get_pixmap_test, POSITIVE_TC_IDX },
	{ NULL, 0 },
};


native_buffer_provider_t *provider;
native_buffer_t *buffer;

static void startup(void)
{
    provider = native_buffer_provider_create (NATIVE_BUFFER_PROVIDER_CORE);
    if (provider == NULL) {
        dts_fail ("native_buffer_provider_create", "native buffer provider create failed");
        return;
    }

    buffer = native_buffer_create (provider, 512, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888, NATIVE_BUFFER_USAGE_CPU|NATIVE_BUFFER_USAGE_3D_TEXTURE);
    if (buffer == NULL)
        dts_fail ("native_buffer_create", "native buffer create failed");

    /* start of TC */
    tet_printf("\n TC start");
}


static void cleanup(void)
{
    native_buffer_destroy (buffer);
    native_buffer_provider_destroy (provider);

    /* end of TC */
    tet_printf("\n TC end");
}

static void utc_native_buffer_get_width_test(void)
{
    int width;

    width = native_buffer_get_width (buffer);
    if (width == 512)
        dts_pass ("native_buffer_get_width", "native buffer get width succeeded");
    else
        dts_fail ("native_buffer_get_width", "native buffer get width failed");
}

static void utc_native_buffer_get_height_test(void)
{
    int height;

    height = native_buffer_get_height (buffer);
    if (height == 512)
        dts_pass ("native_buffer_get_height", "native buffer get height succeeded");
    else
        dts_fail ("native_buffer_get_height", "native buffer get height failed");
}

static void utc_native_buffer_get_stride_test(void)
{
    int stride;

    stride = native_buffer_get_stride (buffer);
    if (stride > 0)
        dts_pass ("native_buffer_get_stride", "native buffer get stride succeeded");
    else
        dts_fail ("native_buffer_get_stride", "native buffer get stride failed");
}

static void utc_native_buffer_get_size_test(void)
{
    int size;

    size = native_buffer_get_size (buffer);
    if (size > 0)
        dts_pass ("native_buffer_get_size", "native buffer get size succeeded");
    else
        dts_fail ("native_buffer_get_size", "native buffer get size failed");
}

static void utc_native_buffer_get_format_test(void)
{
    native_buffer_format_t format;

    format = native_buffer_get_format (buffer);
    if (format == NATIVE_BUFFER_FORMAT_RGBA_8888)
        dts_pass ("native_buffer_get_format", "native buffer get format succeeded");
    else
        dts_fail ("native_buffer_get_format", "native buffer get format failed");
}

static void utc_native_buffer_get_provider_test(void)
{
    native_buffer_provider_t *provider_ret;

    provider_ret = native_buffer_get_provider (buffer);
    if (provider_ret == provider)
        dts_pass ("native_buffer_get_provider", "native buffer get provider succeeded");
    else
        dts_fail ("native_buffer_get_provider", "native buffer get provider failed");
}

static void utc_native_buffer_get_bo_test(void)
{
    tbm_bo bo;

    bo = native_buffer_get_bo (buffer);
    if (bo == NULL)
        dts_fail ("native_buffer_get_bo", "native buffer get bo failed");

    if (tbm_bo_size (bo) != native_buffer_get_size (buffer))
        dts_fail ("native_buffer_get_bo", "native buffer bo size test failed");

    dts_pass ("native_buffer_get_bo", "native buffer get bo succeeded");
}

static void utc_native_buffer_get_pixmap_test(void)
{
    void *pixmap;

    pixmap = native_buffer_get_pixmap (buffer);
    if (pixmap != NULL)
        dts_fail ("native_buffer_get_pixmap", "native buffer get pixmap succeeded even when buffer does not have NATIVE_BUFFER_USAGE_3D_RENDER usage set");

    native_buffer_t *buffer_test;
    buffer = native_buffer_create (provider, 512, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888, NATIVE_BUFFER_USAGE_CPU|NATIVE_BUFFER_USAGE_3D_RENDER);
    if (buffer == NULL)
        dts_fail ("native_buffer_create", "native buffer get pixmap failed");

    dts_pass ("native_buffer_get_pixmap", "native buffer get pixmap succeeded");
}


