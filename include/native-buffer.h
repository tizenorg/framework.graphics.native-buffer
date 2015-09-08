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


#ifndef __NATIVE_BUFFER_H__
#define __NATIVE_BUFFER_H__

#ifdef  __cplusplus
# define NBP_BEGIN_DECLS  extern "C" {
# define NBP_END_DECLS    }
#else
# define NBP_BEGIN_DECLS
# define NBP_END_DECLS
#endif

NBP_BEGIN_DECLS

/**
 * @file native-buffer.h
 * @brief This file contains native-buffer API
 */
/**
 * @internal // Exclude native-buffer module and API set from Public API reference
 * @addtogroup NATIVE_BUFFER
 * @{
 */

/* native buffer version at compile time 1.1.1 */
/**
 * @brief Definition for native buffer major version.
 *
 * @since_tizen 2.3
 */
#define NATIVE_BUFFER_VERSION_MAJOR 1
/**
 * @brief Definition for native buffer minor version.
 *
 * @since_tizen 2.3
 */
#define NATIVE_BUFFER_VERSION_MINOR 1
/**
 * @brief Definition for native buffer micro version.
 *
 * @since_tizen 2.3
 */
#define NATIVE_BUFFER_VERSION_MICRO 1

/**
 * @brief Gets the native buffer version at runtime.
 * @details This can be used to compare native buffer version at runtime with compiled version.
 *
 * @return The version in format JJNNCC (JJ: Major, NN: Minor, CC: Micro)
 *
 * @since_tizen 2.3
 */
int native_buffer_version (void);

/**
 * @brief Gets the native buffer version at runtime in strings.
 * @details This can be used to compare native buffer version at runtime with compiled version.
 *
 * @return The version in string format
 *
 * @since_tizen 2.3
 */
const char *native_buffer_version_string (void);

/**
 * @brief The structure type representing the buffer provider object for creating native buffer objects handle.
 *
 * @since_tizen 2.3
 */
typedef struct native_buffer_provider native_buffer_provider_t;

/**
 * @brief The structure type representing the buffer object that manages buffer data handle.
 *
 * @since_tizen 2.3
 */
typedef struct native_buffer native_buffer_t;

/**
 * @brief Enumeration for return status of native buffer functions.
 *
 * @since_tizen 2.3
 */
typedef enum {
    STATUS_SUCCESS = 0,             /**< Success */
    STATUS_ERROR,                   /**< Error */
    STATUS_NO_MEMORY,               /**< Error: no memory */
    STATUS_NULL_POINTER,            /**< Error: NULL pointer */
    STATUS_EMPTY_POOL,              /**< Error: empty pool */
    STATUS_EGL_ERROR,               /**< Error: EGL error */
    STATUS_INVALID_BUFFER_STATE,    /**< Error: invalid buffer state */
} status_t;

/**
 * @brief Enumeration for provider types.
 * @details Currently only one type exists - the core type.
 *
 * @since_tizen 2.3
 */
typedef enum {
    NATIVE_BUFFER_PROVIDER_CORE     /**< Core type */
    /* ... provider types will be extended from this point on */
} native_buffer_provider_type_t;

/**
 * @brief Enumeration for native buffer formats.
 * @details Buffers will have specific pixel formats and layouts. This lists all
 *          the known/supported formats for buffers.
 *
 * @since_tizen 2.3
 */
typedef enum {
    NATIVE_BUFFER_FORMAT_INVALID,   /**< Invalid buffer format */
    NATIVE_BUFFER_FORMAT_RGBA_8888, /**< RGBA8888 */
    NATIVE_BUFFER_FORMAT_RGBX_8888, /**< RGBX8888 */
    NATIVE_BUFFER_FORMAT_RGB_888,   /**< RGB888 */
    NATIVE_BUFFER_FORMAT_RGB_565,   /**< RGB565 */
    NATIVE_BUFFER_FORMAT_BGRA_8888, /**< BGRA8888 */
    NATIVE_BUFFER_FORMAT_A_8,       /**< A8 */
    NATIVE_BUFFER_FORMAT_YV12,      /**< YV12 - 8bit Y plane followed by 8bit 2x2 subsampled V, U planes */
    NATIVE_BUFFER_FORMAT_I420,      /**< I420 - 8bit Y plane followed by 8bit 2x2 subsampled U, V planes */
    NATIVE_BUFFER_FORMAT_NV12,      /**< NV12 - 8bit Y plane followed by an interleaved U/V plane with 2x2 subsampling */
    NATIVE_BUFFER_FORMAT_NV21,      /**< NV21 - 8bit Y plane followed by an interleaved V/U plane with 2x2 subsampling */
    NATIVE_BUFFER_FORMAT_NV12T,     /**< NV12T - NV12, but use specified tile size */
    /* ... these formats will be extended over time from this point on */
} native_buffer_format_t;

/**
 * @brief Enumeration for native buffer usage.
 * @details When a buffer is locked, it is locked for one or more specific uses
 *          or purposes. It may be locked by CPU to read/write to it,
 *          for 2D accelerator to use it, or a 3D unit, video codec decoder or encoder
 *          to use or for an image capture device to write to (camera) etc.
 *
 * @since_tizen 2.3
 */
typedef enum {
    NATIVE_BUFFER_USAGE_DEFAULT     = 0x00000000, /**< To get default handle */
    NATIVE_BUFFER_USAGE_CPU         = 0x00000001, /**< Can be read from to or written by the CPU */
    NATIVE_BUFFER_USAGE_2D          = 0x00000002, /**< Can be accessed by the 2D accelerator as either a source or a destination */
    NATIVE_BUFFER_USAGE_3D_TEXTURE  = 0x00000004, /**< Can be accessed by the 3D accelerator as a source texture buffer */
    NATIVE_BUFFER_USAGE_3D_RENDER   = 0x00000008, /**< Can be accessed by the 3D accelerator as a destination buffer */
    NATIVE_BUFFER_USAGE_MM          = 0x00000010, /**< Can be accessed by video codec decode or encode hardware */
    NATIVE_BUFFER_USAGE_DISPLAY     = 0x00000020, /**< Can be scanned out by the display */
    /* ... these usages will be extended over time from this point on */
} native_buffer_usage_t;

/**
 * @brief Enumeration for native buffer access option.
 * @details When a buffer is locked, it is locked with specified access option,
 *          it may be locked for reading from the buffer only,
 *          or for writing into the buffer only,
 *          or for both reading from/writing into the buffer.
 *
 * @since_tizen 2.3
 */
typedef enum {
    NATIVE_BUFFER_ACCESS_OPTION_READ  = (1 << 0),   /**< Buffer can be locked for reading */
    NATIVE_BUFFER_ACCESS_OPTION_WRITE = (1 << 1)    /**< Buffer can be locked for writing */
} native_buffer_access_option_t;

/**
 * @brief Enumeration for native buffer advise option.
 * @details These are used to advise of usage scenarios of buffer memory.
 *          These are not explicit commands, but to be taken as hints. The system
 *          may or may not choose to implement them if possible or sensible.
 *
 * @since_tizen 2.3
 */
typedef enum {
    NATIVE_BUFFER_ADVISE_NORMAL,   /**< The default state of all buffer memory - standard memory */
    NATIVE_BUFFER_ADVISE_WILLNEED, /**< Indicates that the given memory will be needed soon, so if compressed, locked or swapped out, bring the memory online as soon as possible */
    NATIVE_BUFFER_ADVISE_DONTNEED, /**< Indicates that the memory given is not needed anymore and the OS can take that memory back and invalidate the memory contents as it sees fit\n If the memory is accessed again after this it may have undefined content */
    NATIVE_BUFFER_ADVISE_RESIDENT  /**< Hint that the memory should avoid to the paged out if possible as it is very important to keep it resident for performance purposes */
} native_buffer_advise_t;


/**
 * @brief Creates a buffer provider.
 * @details A buffer provider is a context or source of buffers.
 *          Currently, provider supports buffers from TBM.
 *          Buffers created by separate provider types may or
 *          may not be able to work with each other, but buffers from
 *          a single provider can work together. A provider may be initialized
 *          with options for the provider. The provider may also have a source
 *          pointer that acts as a handle indicating the source of buffers.
 *          A provider can be destroyed at any time, but may remain allocated/active
 *          in memory as long as any buffers created by that provider remain
 *          allocated/active. When the last buffer allocated by that provider is
 *          destroyed, AND the provider has been destroyed, then the provider
 *          destruction actually takes place at this time.
 *
 * @since_tizen 2.3
 *
 * @param[in] type The provider type
 *
 * @return The provider handle on success,
            otherwise @c NULL on failure
 *
 * @see native_buffer_provider_type_t
 * @see native_buffer_provider_destroy()
 *
 * @par Example
 * @code
 * native_buffer_provider_t *provider;
 * native_buffer_t *buffer;
 *
 * provider = native_buffer_provider_create(NATIVE_BUFFER_PROVIDER_CORE);
 * buffer = native_buffer_create(provider, NATIVE_BUFFER_FORMAT_BGRA_8888, 480, 800, 0, NATIVE_BUFFER_USAGE_3D_TEXTURE);
 * native_buffer_destroy(buffer);
 * native_buffer_provider_destroy(provider);
 * @endcode
 */
native_buffer_provider_t *native_buffer_provider_create (native_buffer_provider_type_t type);

/**
 * @brief Destroys a buffer provider.
 * @details This destroys a given native buffer provider when called.
 *          All memory and resources related to the provider are released when destroyed.
 *          Destruction will be deferred if one or more native buffers still exist
 *          in an undestroyed state and these buffers were created from the given
 *          native buffer provider. On destruction of the last native buffer
 *          created by the given provider, AND the provider has been destroyed by
 *          this call, then the actual destruction will take place.
 *
 * @since_tizen 2.3
 *
 * @param[in] provider The native buffer provider object
 *
 * @return #STATUS_SUCCESS on success,
 *          otherwise an error status value
 *
 * @retval #STATUS_SUCCESS      Successful
 * @retval #STATUS_NULL_POINTER Native buffer provider object is @c NULL
 *
 * @see native_buffer_provider_create()
 *
 * @par Example
 * @code
 * native_buffer_provider_t *provider;
 * native_buffer_t *buffer;
 *
 * provider = native_buffer_provider_create(NATIVE_BUFFER_PROVIDER_CORE);
 * buffer = native_buffer_create(provider, NATIVE_BUFFER_FORMAT_BGRA_8888, 480, 800, 0, NATIVE_BUFFER_USAGE_3D_TEXTURE);
 * native_buffer_destroy(buffer);
 * native_buffer_provider_destroy(provider);
 * @endcode
 */
status_t native_buffer_provider_destroy (native_buffer_provider_t *provider);

/**
 * @brief Creates a new native buffer.
 * @details This creates a new native buffer handle given the native buffer provider.
 *          and parameters provided. The native buffer @a provider must NOT be NULL
 *          and MUST be a valid provider that has not been destroyed yet.
 *          The buffer will be large enough to contain pixel data given the exact
 *          format and size dimensions requested by @a format, @a width and @a height.
 *          If the format is not supported by the provider, NULL will be returned.
 *          If the buffer width or height are unsupported by the provider, NULL will
 *          be returned. If there is not enough memory available for the buffer
 *          then @c NULL will be returned.
 *          The @a width and @a height must both be greater than @c 0, or creation
 *          may fail, as native buffers are defined to never allow zero sized buffers.
 *          The @a requested_stride is a request (or hint) and may, or may not be
 *          supported based on the provider limitations (e.g. hardware may require
 *          all buffers be a multiple of 32 pixels in width). Do not assume the
 *          stride will be what you requested, and use native_buffer_get_stride()
 *          to get the stride later on if creation is successful. Stride is defined
 *          as the number of bytes between one row of pixels and the next.
 *          A valid stride is always greater than @c 0.
 *          The @a usage indicates the intended/planned use of the buffer.
 *          If a buffer is unable to meet the requested option list, the buffer will
 *          fail to be created and NULL will be returned. A buffer may be capable
 *          of much more than just the options requested, but at a minimum will
 *          support the requested options. The implementation will attempt to
 *          allocate the closest match to the options given, if it can only create
 *          buffers with more option capabilities than requested.
 *
 * @since_tizen 2.3
 *
 * @param[in] provider          The native buffer provider object
 * @param[in] width             The width for the buffer
 * @param[in] height            The height for the buffer
 * @param[in] requested_stride  The requested stride for the buffer
 * @param[in] format            The format for the buffer
 * @param[in] usage             The usage for the buffer
 *
 * @return The native buffer handle on success,
 *          otherwise @c NULL on failure
 *
 * @see native_buffer_provider_t
 * @see native_buffer_format_t
 * @see native_buffer_usage_t
 * @see native_buffer_create_for_tbm()
 * @see native_buffer_destroy()
 *
 * @par Example
 * @code
 * native_buffer_t *buffer;
 *
 * buffer = native_buffer_create(provider, 480, 800, 0, NATIVE_BUFFER_FORMAT_BGRA_8888, NATIVE_BUFFER_USAGE_DISPLAY|NATIVE_BUFFER_USAGE_3D_TEXTURE);
 * native_buffer_destroy(buffer);
 * @endcode
 */
native_buffer_t *native_buffer_create (native_buffer_provider_t *provider,
            int width, int height, int requested_stride, native_buffer_format_t format, int usage);


/**
 * @brief Creates a new native buffer from the existing tbm bo handle.
 * @details This creates a new native buffer handle from the tbm bo handle
 *          given the native buffer provider and parameters provided.
 *          The native buffer @a provider must NOT be @c NULL
 *          and MUST be a valid provider that has not been destroyed yet.
 *          The buffer will be large enough to contain pixel data given the exact
 *          format and size dimensions requested by @a format, @a width and @a height.
 *          If the format is not supported by the provider, @c NULL will be returned.
 *          If the buffer width or height are unsupported by the provider, @c NULL will
 *          be returned. If there is not enough memory available for the buffer
 *          then @c NULL will be returned.
 *          The @a width and @a height must both be greater than @c 0, or creation
 *          may fail, as native buffers are defined to never allow zero sized buffers.
 *          The @a requested_stride is a request (or hint) and may, or may not be
 *          supported based on the provider limitations (e.g. hardware may require
 *          all buffers be a multiple of 32 pixels in width). Do not assume the
 *          stride will be what you requested, and use native_buffer_get_stride()
 *          to get the stride later on if creation is successful. Stride is defined
 *          as the number of bytes between one row of pixels and the next.
 *          A valid stride is always greater than @c 0.
 *          The @a usage indicates the intended/planned use of the buffer.
 *          If a buffer is unable to meet the requested option list, the buffer will
 *          fail to be created and @c NULL will be returned. A buffer may be capable
 *          of much more than just the options requested, but at a minimum will
 *          support the requested options. The implementation will attempt to
 *          allocate the closest match to the options given, if it can only create
 *          buffers with more optional capabilities than requested.
 *
 * @since_tizen 2.3
 *
 * @param[in] provider          The native buffer provider object
 * @param[in] bo                The tbm_bo handle
 * @param[in] width             The width for the buffer
 * @param[in] height            The height for the buffer
 * @param[in] requested_stride  The requested stride for the buffer
 * @param[in] format            The format for the buffer
 *
 * @return The native buffer handle on success,
            otherwise @c NULL on failure
 *
 * @see native_buffer_provider_t
 * @see native_buffer_format_t
 * @see native_buffer_create()
 * @see native_buffer_destroy()
 *
 * @par Example
 * @code
 * tbm_bufmgr bufmgr;
 * tbm_bo bo;
 * native_buffer_t *buffer;
 *
 * bo = tbm_bo_alloc (bufmgr, 480*800*4, TBM_BO_DEFAULT);
 * buffer = native_buffer_create_for_tbm(provider, bo, 480, 800, 0, NATIVE_BUFFER_FORMAT_BGRA_8888);
 * native_buffer_destroy(buffer);
 * tbm_bo_unref(bo);
 * @endcode
 */
native_buffer_t *native_buffer_create_for_tbm (native_buffer_provider_t *provider, void *bo,
            int width, int height, int requested_stride, native_buffer_format_t format);

/**
 * @brief Destroys a given native buffer.
 * @details This will destroy the given @a buffer, if the buffer is a valid/active
 *          buffer and is not @c NULL. If a buffer is still locked when this is called,
 *          then destruction is delayed until the buffer is unlocked to a fully unlocked state.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return #STATUS_SUCCESS on success,
 *          otherwise an error status value
 *
 * @retval #STATUS_SUCCESS      Successful
 * @retval #STATUS_NULL_POINTER Native buffer provider object is @c NULL
 * @retval #STATUS_ERROR        Native buffer has already been destroyed
 *
 * @see native_buffer_create()
 * @see native_buffer_create_for_tbm()
 *
 * @par Example
 * @code
 * native_buffer_t *buffer;
 *
 * buffer = native_buffer_create(provider, 480, 800, 0, NATIVE_BUFFER_FORMAT_BGRA_8888, NATIVE_BUFFER_USAGE_3D_TEXTURE);
 * native_buffer_destroy(buffer);
 * @endcode
 */
status_t native_buffer_destroy (native_buffer_t *buffer);

/**
 * @brief Locks a native buffer into place.
 * @details This locks the given @a buffer into place in memory so it can be used
 *          for a specific task, such as access by CPU or GPU, 2D accelerator,
 *          screen scanout unit, or anything else.
 *          The @a usage options indicate for what use the buffer is intended while locked.
 *          If locking is not possible given the usage options provided, then locking will fail.
 *          The buffer will remain locked until unlocked by native_buffer_unlock().
 *          If the buffer has native_buffer_lock() called more than once on it, then
 *          it must be unlocked the same number of times before it is fully unlocked.
 *          Any calls to native_buffer_lock() after the first, before a final unlock
 *          must use the same usage arguments, (or a subset of them)
 *          or the lock will fail.
 *          Locking and unlocking may not be fast or cost-free. They may or may not
 *          involve copies, flushing of caches or other heavy operations.
 *
 * @since_tizen 2.3
 *
 * @param[in]  buffer  The native buffer object
 * @param[in]  usage   The lock usage
 * @param[in]  option  The lock option
 * @param[out] addr    The base address in memory of the buffer defined as
 *                     the top-left of the buffer memory
 *
 * @return #STATUS_SUCCESS on success,
 *          otherwise an error status value
 *
 * @retval #STATUS_SUCCESS      Successful
 * @retval #STATUS_NULL_POINTER Native buffer provider object is @c NULL
 * @retval #STATUS_ERROR        Native buffer provider is an unsupported type
 *
 * @see native_buffer_unlock()
 *
 * @par Example
 * @code
 * native_buffer_t *buffer;
 * int stride, x, y;
 * unsigned int *pixels, *p;
 *
 * buffer = native_buffer_create(provider, 480, 800, 0, NATIVE_BUFFER_FORMAT_BGRA_8888, NATIVE_BUFFER_USAGE_CPU);
 * stride = native_buffer_get_stride(buffer);
 * if (native_buffer_lock(buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &pixels, ) == STATUS_SUCCESS) {
 *   for (y = 0; y < 800; y++) {
 *     p = ((unsigned char *)pixels) + (y * stride);
 *     for (x = 0; x < 480; x++ {
 *       *p = 0xff8844ff;
 *     }
 *   }
 *   native_buffer_unlock(buffer);
 * }
 * native_buffer_destroy(buffer);
 * @endcode
 */
status_t native_buffer_lock (native_buffer_t *buffer, int usage, int option, void** addr);

/**
 * @brief Unlocks a buffer once already locked.
 * @details This unlocks the @a buffer if already locked. If locked multiple times
 *          then unlocking will succeed as many times as the buffer was locked until
 *          as many unlocks have been called as locks. If the buffer was never
 *          locked before, then it will not succeed. The buffer must be a valid
 *          buffer and not @c NULL.
 *          If this is the final unlock of a destroyed buffer, the unlock may also
 *          complete the actual destruction (de-allocation) of the buffer at this point.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return #STATUS_SUCCESS on success,
 *          otherwise an error status value
 *
 * @retval #STATUS_SUCCESS      Successful
 * @retval #STATUS_NULL_POINTER Native buffer provider object is @c NULL
 * @retval #STATUS_ERROR        Native buffer provider is an unsupported type, or buffer was never locked before
 *
 * @see native_buffer_lock()
 *
 * @par Example
 * @code
 * native_buffer_t *buffer;
 * int stride, x, y;
 * unsigned int *pixels, *p;
 *
 * buffer = native_buffer_create(provider, 480, 800, 0, NATIVE_BUFFER_FORMAT_BGRA_8888, NATIVE_BUFFER_USAGE_CPU);
 * stride = native_buffer_get_stride(buffer);
 * if (native_buffer_lock(buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &pixels, ) == STATUS_SUCCESS) {
 *   for (y = 0; y < 800; y++) {
 *     p = ((unsigned char *)pixels) + (y * stride);
 *     for (x = 0; x < 480; x++) {
 *       *p++ = 0xff8844ff;
 *     }
 *   }
 *   native_buffer_unlock(buffer);
 * }
 * native_buffer_destroy(buffer);
 * @endcode
 */
status_t native_buffer_unlock (native_buffer_t *buffer);

/**
 * @brief Gets the number of active locks on the buffer.
 * @details This returns the number of times native_buffer_lock() has
 *          been called on a buffer without being unlocked by native_buffer_unlock().
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return @c 0 is returned if no locks are currently active,
 *          otherwise @c -1 on error
 *
 * @see native_buffer_lock()
 * @see native_buffer_unlock()
 *
 * @par Example
 * @code
 * if (native_buffer_get_lock_count(buffer) > 3)
 *   printf("too many locks active on buffer\n");
 * @endcode
 */
int native_buffer_get_lock_count (const native_buffer_t *buffer);

/**
 * @brief Gets the width in pixels of the buffer.
 * @details This returns the number of pixels in a horizontal row in
 *          the given buffer. Note that the width is in pixels, not bytes,
 *          so format determines the number of bytes per pixel and other layout characteristics.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return The width in pixels if native buffer object is valid,
 *          otherwise a negative value on error
 *
 * @see native_buffer_get_height()
 * @see native_buffer_get_stride()
 * @see native_buffer_get_size()
 * @see native_buffer_get_format()
 *
 * @par Example
 * @code
 * int stride, x, y, w, h;
 * unsigned int *pixels, *p;
 *
 * stride = native_buffer_get_stride(buffer);
 * w = native_buffer_get_width(buffer);
 * h = native_buffer_get_height(buffer);
 * if (native_buffer_lock(buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &pixels, ) == STATUS_SUCCESS) {
 *   for (y = 0; y < h; y++) {
 *     p = ((unsigned char *)pixels) + (y * stride);
 *     for (x = 0; x < w; x++) {
 *       *p++ = 0xff8844ff;
 *     }
 *   }
 *   native_buffer_unlock(buffer);
 * }
 * native_buffer_destroy(buffer);
 * @endcode
 */
int native_buffer_get_width (const native_buffer_t *buffer);

/**
 * @brief Gets the height in pixels of the buffer.
 * @details This returns the number of rows in the given buffer.
 *          Note that the height may be less than the actual buffer size allocated
 *          due to format and padding reasons.
 *          There is another call to get the total memory size of the buffer
 *          in bytes: native_buffer_size_get().
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return The height in pixels if native buffer object is valid,
 *          otherwise a negative value on error
 *
 * @see native_buffer_get_width()
 * @see native_buffer_get_stride()
 * @see native_buffer_get_size()
 * @see native_buffer_get_format()
 *
 * @par Example
 * @code
 * int stride, x, y, w, h;
 * unsigned int *pixels, *p;
 *
 * stride = native_buffer_get_stride(buffer);
 * w = native_buffer_get_width(buffer);
 * h = native_buffer_get_height(buffer);
 * if (native_buffer_lock(buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &pixels, ) == STATUS_SUCCESS) {
 *   for (y = 0; y < h; y++) {
 *     p = ((unsigned char *)pixels) + (y * stride);
 *     for (x = 0; x < w; x++) {
 *       *p++ = 0xff8844ff;
 *     }
 *   }
 *   native_buffer_unlock(buffer);
 * }
 * native_buffer_destroy(buffer);
 * @endcode
 */
int native_buffer_get_height (const native_buffer_t *buffer);

/**
 * @brief Gets the stride in bytes of the buffer.
 * @details This returns the number of bytes per row in the buffer.
 *          This is simply the gap in bytes between the start of one row
 *          and the start of the next row.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer  The native buffer object
 *
 * @return The stride in bytes if native buffer object is valid,
 *          otherwise a negative value on error
 *
 * @see native_buffer_get_width()
 * @see native_buffer_get_height()
 * @see native_buffer_get_size()
 * @see native_buffer_get_format()
 *
 * @par Example
 * @code
 * int stride, x, y, w, h;
 * unsigned int *pixels, *p;
 *
 * stride = native_buffer_get_stride(buffer);
 * w = native_buffer_get_width(buffer);
 * h = native_buffer_get_height(buffer);
 * if (native_buffer_lock(buffer, NATIVE_BUFFER_USAGE_CPU, NATIVE_BUFFER_ACCESS_OPTION_READ|NATIVE_BUFFER_ACCESS_OPTION_WRITE, &pixels, ) == STATUS_SUCCESS) {
 *   for (y = 0; y < h; y++) {
 *     p = ((unsigned char *)pixels) + (y * stride);
 *     for (x = 0; x < w; x++) {
 *       *p++ = 0xff8844ff;
 *     }
 *   }
 *   native_buffer_unlock(buffer);
 * }
 * native_buffer_destroy(buffer);
 * @endcode
 */
int native_buffer_get_stride (const native_buffer_t *buffer);

/**
 * @brief Gets the size of the buffer memory in bytes.
 * @details This returns the number of bytes consumed by the buffer memory.
 *          Buffers by definition cannot be zero sized and must contain at least @c 1 pixel of data.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return The buffer size in bytes if native buffer object is valid,
 *          otherwise a negative value on error
 *
 * @see native_buffer_get_width()
 * @see native_buffer_get_height()
 * @see native_buffer_get_stride()
 * @see native_buffer_get_format()
 *
 * @par Example
 * @code
 * printf("this buffer uses %ulKb of memory\n", native_buffer_get_size(buffer) / 1024);
 * @endcode
 */
int native_buffer_get_size (const native_buffer_t *buffer);

/**
 * @brief Gets the pixel format of the buffer.
 * @details This returns the format of the pixels in the given native buffer.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return The buffer format
 *
 * @see native_buffer_format_t
 * @see native_buffer_get_width()
 * @see native_buffer_get_height()
 * @see native_buffer_get_stride()
 * @see native_buffer_get_size()
 *
 * @par Example
 * @code
 * if (native_buffer_get_format(buffer) != NATIVE_BUFFER_FORMAT_BGRA_8888)
 *   printf("ERROR: cannot handle buffers of formats other than BGRA_8888\n");
 * @endcode
 */
native_buffer_format_t native_buffer_get_format (const native_buffer_t *buffer);

/**
 * @brief Gets the buffer provider that owns/created the buffer.
 * @details This returns the provider that created/owns the buffer.
 *          The provider handle is only valid until the provider is destroyed.
 *          The provider may be destroyed at many different points.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer The native buffer object
 *
 * @return The buffer provider object
 *
 * @see native_buffer_provider_t
 *
 * example:
 * @code
 * printf("provider is %p\n", native_buffer_get_provider(buffer));
 * @endcode
 */
native_buffer_provider_t *native_buffer_get_provider (const native_buffer_t *buffer);

/**
 * @brief Gets the internal bo that is owned by the native buffer.
 * @details This returns the bo that is owned by the buffer.
 *
 * @since_tizen 2.3
 *
 * @remarks Since the bo is owned by the buffer, it MUST NOT be unrefed outside.
 *          native buffer will take care of the destruction of the bo.
 *
 * @param[in] buffer The native buffer object
 *
 * @return @a tbm_bo handle on success,
            otherwise @c NULL on failure
 *
 * @par Example
 * @code
 * tbm_bo bo;
 * bo = native_buffer_get_bo(buffer);
 * tbm_bo_map(bo, TBM_DEVICE_CPU, TBM_OPTION_READ|TBM_OPTION_WRITE);
 * tbm_bo_unmap(bo);
 * @endcode
 */
void *native_buffer_get_bo (const native_buffer_t *buffer);

/**
 * @brief Gets the xpixmap ID that is owned by the native buffer.
 * @details When the native buffer is created with NATIVE_BUFFER_USAGE_3D_RENDER usage flag set,
 *          this will return the xpixmap ID.
 *
 * @since_tizen 2.3
 *
 * @remarks Since xpixmap is owned by the buffer, it MUST NOT be freed outside.
 *          Native buffer will take care of the destruction of the xpixmap.
 *
 * @param[in] buffer  The native buffer object
 *
 * @return @a xpixmap ID on success,
 *          otherwise @c NULL on failure
 *
 * @par Example
 * @code
 * native_buffer *buffer;
 * Pixmap pixmap;
 * buffer = native_buffer_create (provider, WIDTH, HEIGHT, 0, NATIVE_BUFFER_FORMAT_BGRA_8888,
 *                                               NATIVE_BUFFER_USAGE_3D_RENDER|NATIVE_BUFFER_USAGE_3D_TEXTURE);
 * pixmap = native_buffer_get_pixmap(buffer);
 * native_buffer_destroy (buffer);
 * @endcode
 *
 */
void *native_buffer_get_pixmap (const native_buffer_t *buffer);

/**
 * @brief Sends an opaque handle for a buffer down a socket connection.
 * @details The @a socket_fd must be a valid file descriptor
 *          for a socket connection to the process which the buffer is to be sent.
 *          This must be a standard local socket that allows to pass file
 *          descriptors if necessary. The actual format of the handle sent down the
 *          socket FD is an internal implementation detail (it may be a file
 *          descriptor itself or some global ID, filename or anything necessary
 *          defined by the system implementation), and is intended to ONLY be read
 *          on the other end of the socket by native_buffer_handle_socket_receive().
 *          Any protocol along the socket can send any preamble before this call
 *          that is deemed necessary to put the other end of the socket in a
 *          state to be read to receive the socket with native_buffer_handle_socket_receive().
 *          Sending a buffer to another process via socket does not destroy or
 *          release the buffer from the current process. Sending a buffer
 *          does not also require that the buffer is unlocked BUT it is highly recommended
 *          that the buffer is unlocked when it is sent.
 *          This function will block until the handle has been successfully passed
 *          to the kernel. The act of calling this function creates another handle
 *          to the buffer, and only once all processes have exited (or crashed) OR
 *          they have explicitly destroyed a native buffer sent via this mechanism,
 *          the @a buffer will actually be freed from the system.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer     The native buffer object
 * @param[in] socket_fd  The file descriptor for a socket connection
 *
 * @return #STATUS_SUCCESS on success,
 *         otherwise an error status value
 *
 * @retval #STATUS_SUCCESS Successful
 * @retval #STATUS_ERROR   Failed
 *
 * @see native_buffer_handle_socket_receive()
 *
 * @par Example
 * @code
 * native_buffer_handle_socket_send(buffer, socket);
 * @endcode
 */
status_t native_buffer_handle_socket_send (const native_buffer_t *buffer, int socket_fd);

/**
 * @brief Receives a buffer from a socket connection.
 * @details This function will block and wait until it has all the data necessary
 *          to create the native buffer. Once the native buffer has been created
 *          and returned by this function it may be acted upon like any other buffer
 *          and may even be sent on to other processes.
 *          The buffer begins its life after calling this function as if it had been created
 *          with native_buffer_create() passing the same parameters as the
 *          original creation that allocated the buffer before it was sent.
 *
 * @since_tizen 2.3
 *
 * @remarks The provider must be a valid provider handle that is compatible
 *          (or the same type) as the provider on the other end of the socket
 *          that sent the buffer with native_buffer_handle_socket_send().
 *
 * @param[in] provider    The native buffer provider object
 * @param[in] socket_fd  The file descriptor for a socket connection
 *
 * @return The native buffer handle on success,
 *          otherwise @c NULL on failure
 *
 * @see native_buffer_handle_socket_send()
 *
 * @par Example
 * @code
 * native_buffer_t *buffer;
 *
 * buffer = native_buffer_handle_socket_receive(provider, socket);
 * if (!buffer) printf("buffer receive failed\n");
 * @endcode
 *
 * @since_tizen 2.3
 */
native_buffer_t *native_buffer_handle_socket_receive (native_buffer_provider_t *provider, int socket_fd);

/**
 * @brief Provides hints on the memory in a buffer.
 * @details This provides hints or advice for the system as to what to do with
 *          the given buffer. The hints do not need to be supported, but may be
 *          if the system can support it.
 *          The @a offset is in bytes from the start address of the buffer and
 *          @a length is in bytes from the offset address. The @a advice covers
 *          the hints/rules to apply to the given memory address range indicated.
 *          Note that this may only apply to a subset of the memory if the address
 *          range is not page aligned or aligned to a memory unit that the system
 *          supports for the given buffer. Also note that you do not need to
 *          lock the buffer to call native_buffer_madvise() on it.
 *
 * @since_tizen 2.3
 *
 * @param[in] buffer  The native buffer object
 * @param[in] offset  The bytes from the start address of the buffer
 * @param[in] length  The bytes from the offset address
 * @param[in] advice  The hints to apply to the given memory address range
 *
 * @return #STATUS_SUCCESS on success,
 *         otherwise an error status value
 *
 * @retval #STATUS_SUCCESS Successful
 * @retval #STATUS_ERROR   Failed
 *
 * @see native_buffer_advise_t
 *
 * @par Example
 * @code
 * size_t size;
 *
 * size = native_buffer_get_size(buffer);
 * native_buffer_madvise(buffer, 0, size, NATIVE_BUFFER_ADVISE_DONTNEED);
 * @endcode
 */
status_t native_buffer_madvise (native_buffer_t *buffer, int offset, int length, native_buffer_advise_t advice);

/**
 * @}
 * End of a file
 */

NBP_END_DECLS

#endif /* __NATIVE_BUFFER__ */
