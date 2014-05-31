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

static void utc_native_buffer_lock_test_p(void);
static void utc_native_buffer_lock_test_n(void);
static void utc_native_buffer_get_lock_count_test(void);


struct tet_testlist tet_testlist[] = {
	{ utc_native_buffer_lock_test_p, POSITIVE_TC_IDX },
	{ utc_native_buffer_lock_test_n, NEGATIVE_TC_IDX },
	{ utc_native_buffer_get_lock_count_test, POSITIVE_TC_IDX },
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

static void utc_native_buffer_lock_test_p(void)
{
    status_t status;

    void *addr;
    status = native_buffer_lock (buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &addr);
    if (status) {
        dts_fail ("native_buffer_lock", "native buffer lock failed");
    }

    status = native_buffer_unlock (buffer);
    if (status) {
        dts_fail ("native_buffer_unlock", "native buffer unlock failed");
    }

    dts_pass ("native_buffer_lock", "native buffer lock succeeded");
}

static void utc_native_buffer_lock_test_n(void)
{
    status_t status;

    void *addr;
    status = native_buffer_lock (buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &addr);
    if (status) {
        dts_fail ("native_buffer_lock", "native buffer lock failed");
    }

    status = native_buffer_unlock (buffer);
    if (status) {
        dts_fail ("native_buffer_unlock", "native buffer unlock failed");
    }

    dts_pass ("native_buffer_lock", "native buffer lock succeeded");
}

static void utc_native_buffer_get_lock_count_test(void)
{
    status_t status;
    void *addr;
    int i;

    for (i=0; i<10; i++) {
        if (native_buffer_get_lock_count (buffer) != i)
            dts_fail ("native_buffer_get_lock_count", "native buffer get lock count failed");

        status = native_buffer_lock (buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &addr);
        if (status) {
            dts_fail ("native_buffer_lock", "native buffer lock failed");
        }
    }

    for (i=10; i>0; i--) {
        if (native_buffer_get_lock_count (buffer) != i)
            dts_fail ("native_buffer_get_lock_count", "native buffer get lock count failed");

        status = native_buffer_unlock (buffer);
        if (status) {
            dts_fail ("native_buffer_unlock", "native buffer unlock failed");
        }
    }

    dts_pass ("native_buffer_get_lock_count", "native buffer get lock count test succeeded");
}



