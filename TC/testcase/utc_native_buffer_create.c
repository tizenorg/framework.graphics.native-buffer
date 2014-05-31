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
#include <stdlib.h>

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_native_buffer_create_test_p(void);
static void utc_native_buffer_create_test_n(void);
static void utc_native_buffer_create_for_tbm_test_p(void);
static void utc_native_buffer_create_for_tbm_test_n(void);
static void utc_native_buffer_create_format_test(void);
static void utc_native_buffer_create_usage_test(void);

struct tet_testlist tet_testlist[] = {
	{ utc_native_buffer_create_test_p, NEGATIVE_TC_IDX },
	{ utc_native_buffer_create_test_n, POSITIVE_TC_IDX },
	{ utc_native_buffer_create_for_tbm_test_p, NEGATIVE_TC_IDX },
	{ utc_native_buffer_create_for_tbm_test_n, POSITIVE_TC_IDX },
    { utc_native_buffer_create_format_test, POSITIVE_TC_IDX },
    { utc_native_buffer_create_usage_test, POSITIVE_TC_IDX },
	{ NULL, 0 },
};

native_buffer_provider_t *provider;

static void startup(void)
{
	provider = native_buffer_provider_create (NATIVE_BUFFER_PROVIDER_CORE);
    if (provider == NULL) {
        dts_fail ("native_buffer_provider_create", "native buffer provider create failed");
        return;
    }

    /* start of TC */
    tet_printf("\n TC start");
}


static void cleanup(void)
{
    native_buffer_provider_destroy (provider);

	/* end of TC */
	tet_printf("\n TC end");
}

static void utc_native_buffer_create_test_p(void)
{
    native_buffer_t *buffer;

    buffer = native_buffer_create (provider, 512, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888, NATIVE_BUFFER_USAGE_CPU|NATIVE_BUFFER_USAGE_3D_TEXTURE);
    if (buffer == NULL)
        dts_fail ("native_buffer_create", "native buffer create failed");
    else
        dts_pass ("native_buffer_create", "native buffer create succeeded");

    native_buffer_destroy (buffer);
}

static void utc_native_buffer_create_test_n(void)
{
    native_buffer_t *buffer;

    buffer = native_buffer_create (provider, 0, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888, NATIVE_BUFFER_USAGE_CPU|NATIVE_BUFFER_USAGE_3D_TEXTURE);
    if (buffer == NULL)
        dts_pass ("native_buffer_create", "native buffer create failed with invalid parameter");
    else
        dts_fail ("native_buffer_create", "native buffer create succeeded with invalid parameter");

    native_buffer_destroy (buffer);
}

#include <tbm_bufmgr.h>
#include <drm.h>
#include <fcntl.h>
#include <X11/Xmd.h>
#include <dri2.h>
#include <xf86drm.h>
#include <unistd.h>
static tbm_bufmgr _utc_native_buffer_tbm_bufmgr_init(void)
{
    Display *display;
    Window window;
    int dri2_ev_base, dri2_err_base, dri2_major, dri2_minor;
    char *drv_name, *dev_name;
    drm_magic_t magic;
    int fd;
    tbm_bufmgr bufmgr;

    display = XOpenDisplay (0);
    if (display == NULL) {
        dts_fail ("XOpenDisplay", "XOpenDisplay failed");
        return NULL;
    }

    window = DefaultRootWindow (display);

    if (!DRI2QueryExtension(display, &dri2_ev_base, &dri2_err_base))
    {
        dts_fail ("DRI2QueryExtension", "DRI2QueryExtension failed");
        return NULL;
    }

    if (!DRI2QueryVersion(display, &dri2_major, &dri2_minor ))
    {
        dts_fail ("DRI2QueryVersion", "DRI2QueryVersion failed");
        return NULL;
    }

    if (!DRI2Connect(display, window, &drv_name, &dev_name ))
    {
        dts_fail ("DRI2Connect", "DRI2Connect failed");
        return NULL;
    }

    fd = open(dev_name, O_RDWR);
    if (fd < 0)
    {
        dts_fail ("open", "fd open failed");
        free (drv_name);
        free (dev_name);
        return NULL;
    }

    free (drv_name);
    free (dev_name);

    if (drmGetMagic(fd, &magic))
    {
        dts_fail ("drmGetMagic", "drmGetMagic failed");
        return NULL;
    }

    if (!DRI2Authenticate(display, window, (unsigned int)magic))
    {
        dts_fail ("DRI2Authenticate", "DRI2Authenticate failed");
        return NULL;
    }

    bufmgr = tbm_bufmgr_init (fd);
    if (bufmgr == NULL) {
        dts_fail ("tbm_bufmgr_init", "tbm_bufmgr_init failed");
        return NULL;
    }

    return bufmgr;
}

static void utc_native_buffer_create_for_tbm_test_p(void)
{
    native_buffer_t *buffer;
    tbm_bufmgr bufmgr;
    tbm_bo bo;

    bufmgr = _utc_native_buffer_tbm_bufmgr_init ();
    bo = tbm_bo_alloc (bufmgr, 512*512*4, TBM_BO_DEFAULT);
    if (bo == NULL) {
        dts_fail ("tbm_bo_alloc", "tbm_bo_alloc failed");
        return;
    }

    buffer = native_buffer_create_for_tbm (provider, bo, 512, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888);
    if (buffer == NULL)
        dts_fail ("native_buffer_create_for_tbm", "native buffer create for tbm failed");
    else
        dts_pass ("native_buffer_create_for_tbm", "native buffer create for tbm succeeded");

    native_buffer_destroy (buffer);
    tbm_bo_unref (bo);
    tbm_bufmgr_deinit (bufmgr);
}

static void utc_native_buffer_create_for_tbm_test_n(void)
{
    native_buffer_t *buffer;
    tbm_bufmgr bufmgr;
    tbm_bo bo;

    bufmgr = _utc_native_buffer_tbm_bufmgr_init ();
    bo = tbm_bo_alloc (bufmgr, 512*512*2, TBM_BO_DEFAULT); // Size mismatch between bo and native buffer
    if (bo == NULL) {
        dts_fail ("tbm_bo_alloc", "tbm_bo_alloc failed");
        return;
    }

    buffer = native_buffer_create_for_tbm (provider, bo, 512, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888);
    if (buffer == NULL)
        dts_pass ("native_buffer_create_for_tbm", "native buffer create for tbm failed with size mismatch");
    else
        dts_fail ("native_buffer_create_for_tbm", "native buffer create for tbm succeeded with size mismatch");

    native_buffer_destroy (buffer);
    tbm_bo_unref (bo);
    tbm_bufmgr_deinit (bufmgr);
}

static void utc_native_buffer_create_format_test(void)
{
    native_buffer_t *buffer;
    int format[11] = {
        NATIVE_BUFFER_FORMAT_RGBA_8888,
        NATIVE_BUFFER_FORMAT_RGBX_8888,
        NATIVE_BUFFER_FORMAT_RGB_888,
        NATIVE_BUFFER_FORMAT_RGB_565,
        NATIVE_BUFFER_FORMAT_BGRA_8888,
        NATIVE_BUFFER_FORMAT_A_8,
        NATIVE_BUFFER_FORMAT_YV12,
        NATIVE_BUFFER_FORMAT_I420,
        NATIVE_BUFFER_FORMAT_NV12,
        NATIVE_BUFFER_FORMAT_NV21,
        NATIVE_BUFFER_FORMAT_NV12T,
    };
    int i;

    for (i=0; i<11; i++) {
        buffer = native_buffer_create (provider, 512, 512, 0, format[i], NATIVE_BUFFER_USAGE_CPU|NATIVE_BUFFER_USAGE_3D_TEXTURE);
        if (buffer == NULL)
            dts_fail ("native_buffer_create", "native buffer create failed");

        native_buffer_destroy (buffer);
    }

    dts_pass ("native_buffer_create", "native buffer create with format succeeded");
}

static void utc_native_buffer_create_usage_test(void)
{
    native_buffer_t *buffer;
    int usage[9] = {
        NATIVE_BUFFER_USAGE_DEFAULT,
        NATIVE_BUFFER_USAGE_CPU,
        NATIVE_BUFFER_USAGE_2D,
        NATIVE_BUFFER_USAGE_3D_TEXTURE,
        NATIVE_BUFFER_USAGE_3D_RENDER,
        NATIVE_BUFFER_USAGE_MM,
        NATIVE_BUFFER_USAGE_DISPLAY,
        NATIVE_BUFFER_USAGE_CPU | NATIVE_BUFFER_USAGE_3D_TEXTURE,
        NATIVE_BUFFER_USAGE_CPU | NATIVE_BUFFER_USAGE_3D_RENDER,
    };
    int i;

    for (i=0; i<9; i++) {
        buffer = native_buffer_create (provider, 512, 512, 0, NATIVE_BUFFER_FORMAT_RGBA_8888, usage[i]);
        if (buffer == NULL)
            dts_fail ("native_buffer_create", "native buffer create failed");

        native_buffer_destroy (buffer);
    }

    dts_pass ("native_buffer_create", "native buffer create with usage succeeded");
}

