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

static void utc_native_buffer_provider_create_test_p(void);
static void utc_native_buffer_provider_create_test_n(void);
static void utc_native_buffer_provider_destroy_test_p(void);
static void utc_native_buffer_provider_destroy_test_n(void);

struct tet_testlist tet_testlist[] = {
	{ utc_native_buffer_provider_create_test_p, NEGATIVE_TC_IDX },
	{ utc_native_buffer_provider_create_test_n, POSITIVE_TC_IDX },
    { utc_native_buffer_provider_destroy_test_p, NEGATIVE_TC_IDX },
    { utc_native_buffer_provider_destroy_test_n, POSITIVE_TC_IDX },
	{ NULL, 0 },
};


static void startup(void)
{
	/* start of TC */
	tet_printf("\n TC start");
}


static void cleanup(void)
{
	/* end of TC */
	tet_printf("\n TC end");
}

static void utc_native_buffer_provider_create_test_p(void)
{
    native_buffer_provider_t *provider;

    provider = native_buffer_provider_create (NATIVE_BUFFER_PROVIDER_CORE);
    if (provider == NULL)
        dts_fail ("native_buffer_provider_create", "native buffer provider create failed");
    else
        dts_pass ("native_buffer_provider_create", "native buffer provider create succeeded");

    native_buffer_provider_destroy (provider);
}

static void utc_native_buffer_provider_create_test_n(void)
{
    native_buffer_provider_t *provider;

    provider = native_buffer_provider_create (-1);
    if (provider == NULL)
        dts_pass ("native_buffer_provider_create", "native buffer provider create failed with invalid parameter");
    else
        dts_fail ("native_buffer_provider_create", "native buffer provider create succeeded with invalid parameter");

    native_buffer_provider_destroy (provider);
}

static void utc_native_buffer_provider_destroy_test_p(void)
{
    native_buffer_provider_t *provider;
    status_t status;

    provider = native_buffer_provider_create (NATIVE_BUFFER_PROVIDER_CORE);
    if (provider == NULL) {
        dts_fail ("native_buffer_provider_create", "native buffer provider create failed");
        return;
    }

    status = native_buffer_provider_destroy (provider);
    if (status == STATUS_SUCCESS)
        dts_pass ("native_buffer_provider_destroy", "native buffer provider destroy succeeded");
    else
        dts_fail ("native_buffer_provider_destroy", "native buffer provider destroy failed");
}

static void utc_native_buffer_provider_destroy_test_n(void)
{
    native_buffer_provider_t *provider;
    status_t status;

    provider = native_buffer_provider_create (NATIVE_BUFFER_PROVIDER_CORE);
    if (provider == NULL) {
        dts_fail ("native_buffer_provider_create", "native buffer provider create failed");
        return;
    }

    status = native_buffer_provider_destroy (NULL);
    if (status == STATUS_SUCCESS)
        dts_fail ("native_buffer_provider_destroy", "native buffer provider destroy succeeded with invalid parameter");
    else
        dts_pass ("native_buffer_provider_destroy", "native buffer provider destroy failed with invalid parameter");

    native_buffer_provider_destroy (provider);
}


