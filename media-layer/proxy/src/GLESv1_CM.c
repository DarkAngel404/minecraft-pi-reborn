#include <stdint.h>

#include <GLES/gl.h>

#include <libreborn/libreborn.h>

#include "common/common.h"

static int get_glFogfv_params_length(GLenum pname) {
    return pname == GL_FOG_COLOR ? 4 : 1;
}
CALL(11, glFogfv, void, (GLenum pname, const GLfloat *params)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) pname);
    int length = get_glFogfv_params_length(pname);
    for (int i = 0; i < length; i++) {
        write_float(params[i]);
    }

    // Release Proxy
    end_proxy_call();
#else
    GLenum pname = (GLenum) read_int();
    int length = get_glFogfv_params_length(pname);
    GLfloat params[length];
    for (int i = 0; i < length; i++) {
        params[i] = read_float();
    }
    // Run
    glFogfv(pname, params);
#endif
}

// Track Bindings
#if defined(MEDIA_LAYER_PROXY_SERVER)
static GLuint bound_buffer = 0;
static GLuint bound_texture = 0;
static unsigned char vertex_array_enabled = 0;
static unsigned char color_array_enabled = 0;
static unsigned char tex_coord_array_enabled = 0;
static unsigned char *get_array_enabled_pointer(GLenum array) {
    switch (array) {
        case GL_VERTEX_ARRAY: {
            return &vertex_array_enabled;
        }
        case GL_COLOR_ARRAY: {
            return &color_array_enabled;
        }
        case GL_TEXTURE_COORD_ARRAY: {
            return &tex_coord_array_enabled;
        }
        default: {
            ERR("Unsupported Array Pointer: %i", array);
        }
    }
}
#endif

// 'pointer' Is Only Supported As An Integer, Not As An Actual Pointer
#if defined(MEDIA_LAYER_PROXY_SERVER)
#define CALL_GL_POINTER(unique_id, name) \
    static int is_set_##name = 0; \
    CALL(unique_id, name, void, (GLint size, GLenum type, GLsizei stride, const void *pointer)) { \
        /* Check */ \
        static GLint last_size; \
        static GLenum last_type; \
        static GLsizei last_stride; \
        static const void *last_pointer; \
        if (is_set_##name && last_size == size && last_type == type && last_stride == stride && last_pointer == pointer) { \
            return; \
        } else { \
            is_set_##name = 1; \
            last_size = size; \
            last_type = type; \
            last_stride = stride; \
            last_pointer = pointer; \
        } \
        /* Lock Proxy */ \
        start_proxy_call(); \
        \
        /* Arguments */ \
        write_int(bound_buffer); \
        write_int((uint32_t) size); \
        write_int((uint32_t) type); \
        write_int((uint32_t) stride); \
        write_int((uint32_t) pointer); \
        \
        /* Release Proxy */ \
        end_proxy_call(); \
    }
#else
#define CALL_GL_POINTER(unique_id, name) \
    CALL(unique_id, name, unused, unused) { \
        /* Setup Buffer Binding */ \
        GLuint bound_buffer = (GLuint) read_int(); \
        glBindBuffer(GL_ARRAY_BUFFER, bound_buffer); \
        /* Check State */ \
        if (bound_buffer == 0) { \
            PROXY_ERR("gl*Pointer() Functions Are Only Supported When A Buffer Is Bound To GL_ARRAY_BUFFER"); \
        } \
        GLint size = (GLint) read_int(); \
        GLenum type = (GLenum) read_int(); \
        GLsizei stride = (GLsizei) read_int(); \
        const void *pointer = (const void *) (uintptr_t) read_int(); \
        /* Run */ \
        name(size, type, stride, pointer); \
    }
#endif

CALL_GL_POINTER(12, glVertexPointer)

CALL(13, glLineWidth, void, (GLfloat width)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(width);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat width = read_float();
    // Run
    glLineWidth(width);
#endif
}

CALL(14, glBlendFunc, void, (GLenum sfactor, GLenum dfactor)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) sfactor);
    write_int((uint32_t) dfactor);

    // Release Proxy
    end_proxy_call();
#else
    GLenum sfactor = (GLenum) read_int();
    GLenum dfactor = (GLenum) read_int();
    // Run
    glBlendFunc(sfactor, dfactor);
#endif
}

CALL(15, glDrawArrays, void, (GLenum mode, GLint first, GLsizei count)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) mode);
    write_int((uint32_t) first);
    write_int((uint32_t) count);
    write_int(bound_buffer);
    write_int(bound_texture);
    if (!vertex_array_enabled) {
        IMPOSSIBLE();
    }
    write_byte(color_array_enabled);
    write_byte(tex_coord_array_enabled);

    // Release Proxy
    end_proxy_call();
    flush_write_cache();
#else
    GLenum mode = (GLenum) read_int();
    GLint first = (GLint) read_int();
    GLsizei count = (GLsizei) read_int();
    GLuint bound_buffer = (GLuint) read_int();
    GLuint bound_texture = (GLuint) read_int();
    unsigned char color_array_enabled = read_byte();
    unsigned char tex_coord_array_enabled = read_byte();
    // Run
    glBindBuffer(GL_ARRAY_BUFFER, bound_buffer);
    glBindTexture(GL_TEXTURE_2D, bound_texture);
    glEnableClientState(GL_VERTEX_ARRAY);
#define set_array_enabled(condition, enum) condition ? glEnableClientState(enum) : glDisableClientState(enum);
    set_array_enabled(color_array_enabled, GL_COLOR_ARRAY);
    set_array_enabled(tex_coord_array_enabled, GL_TEXTURE_COORD_ARRAY);
    glDrawArrays(mode, first, count);
#endif
}

CALL(16, glColor4f, void, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(red);
    write_float(green);
    write_float(blue);
    write_float(alpha);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat red = read_float();
    GLfloat green = read_float();
    GLfloat blue = read_float();
    GLfloat alpha = read_float();
    // Run
    glColor4f(red, green, blue, alpha);
#endif
}

CALL(17, glClear, void, (GLbitfield mask)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) mask);

    // Release Proxy
    end_proxy_call();
#else
    GLbitfield mask = (GLbitfield) read_int();
    // Run
    glClear(mask);
#endif
}

#if defined(MEDIA_LAYER_PROXY_CLIENT)
// Preserve Buffer For Performance
static size_t _glBufferData_data_size = 0;
static void *_glBufferData_data = NULL;
__attribute__((destructor)) static void _free_glBufferData_data() {
    if (_glBufferData_data != NULL) {
        free(_glBufferData_data);
    }
}
#endif
CALL(18, glBufferData, void, (__attribute__((unused)) GLenum target, GLsizeiptr size, const void *data, GLenum usage)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) size);
    write_int((uint32_t) usage);
    write_int(bound_buffer);
    // Write Data
    unsigned char is_null = data == NULL;
    write_byte(is_null);
    if (!is_null) {
        safe_write((void *) data, (size_t) size);
    }

    // Release Proxy
    end_proxy_call();
#else
    GLsizeiptr size = (GLsizeiptr) read_int();
    GLenum usage = (GLenum) read_int();
    GLuint bound_buffer = (GLuint) read_int();
    // Load Data
    void *data = NULL;
    unsigned char is_null = read_byte();
    if (!is_null) {
        // Allocate
        int size_changed = 0;
        if (_glBufferData_data == NULL) {
            _glBufferData_data = malloc((size_t) size);
            size_changed = 1;
        } else if (((size_t) size) > _glBufferData_data_size) {
            _glBufferData_data = realloc(_glBufferData_data, (size_t) size);
            size_changed = 1;
        }
        // Verify
        ALLOC_CHECK(_glBufferData_data);
        if (size_changed) {
            _glBufferData_data_size = size;
        }
        data = _glBufferData_data;
        // Read
        safe_read(data, (size_t) size);
    }
    // Run
    glBindBuffer(GL_ARRAY_BUFFER, bound_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
#endif
}

CALL(19, glFogx, void, (GLenum pname, GLfixed param)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) pname);
    write_int((uint32_t) param);

    // Release Proxy
    end_proxy_call();
#else
    GLenum pname = (GLenum) read_int();
    GLfixed param = (GLfixed) read_int();
    // Run
    glFogx(pname, param);
#endif
}

CALL(20, glFogf, void, (GLenum pname, GLfloat param)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) pname);
    write_float(param);

    // Release Proxy
    end_proxy_call();
#else
    GLenum pname = (GLenum) read_int();
    GLfloat param = read_float();
    // Run
    glFogf(pname, param);
#endif
}

CALL(21, glMatrixMode, void, (GLenum mode)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) mode);

    // Release Proxy
    end_proxy_call();
#else
    GLenum mode = (GLenum) read_int();
    // Run
    glMatrixMode(mode);
#endif
}

CALL_GL_POINTER(22, glColorPointer)

CALL(23, glScissor, void, (GLint x, GLint y, GLsizei width, GLsizei height)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) x);
    write_int((uint32_t) y);
    write_int((uint32_t) width);
    write_int((uint32_t) height);

    // Release Proxy
    end_proxy_call();
#else
    GLint x = (GLint) read_int();
    GLint y = (GLint) read_int();
    GLsizei width = (GLsizei) read_int();
    GLsizei height = (GLsizei) read_int();
    // Run
    glScissor(x, y, width, height);
#endif
}

CALL(24, glTexParameteri, void, (__attribute__((unused)) GLenum target, GLenum pname, GLint param)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) pname);
    write_int((uint32_t) param);
    write_int(bound_texture);

    // Release Proxy
    end_proxy_call();
#else
    GLenum pname = (GLenum) read_int();
    GLint param = (GLint) read_int();
    GLuint bound_texture = (GLuint) read_int();
    // Run
    glBindTexture(GL_TEXTURE_2D, bound_texture);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
#endif
}

// Get Size (In Memory) Of Specified Texture
static int get_texture_size(GLsizei width, GLsizei height, GLenum format, GLenum type) {
    int multiplier;
    if (type == GL_UNSIGNED_BYTE) {
        switch (format) {
            case GL_RGB: {
                multiplier = 3;
                break;
            }
            case GL_RGBA: {
                multiplier = 4;
                break;
            }
            default: {
                PROXY_ERR("Unsupported Texture Format: %u", (unsigned int) format);
            }
        }
    } else {
        multiplier = sizeof (unsigned short);
    }
    int line_size = width * multiplier;
    {
        // Handle Alignment
        int alignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        // Round
        int diff = line_size % alignment;
        if (diff > 0) {
            line_size = line_size + (alignment - diff);
        }
    }
    return line_size * height; // This Should Have The Same Behavior On 32-Bit And 64-Bit Systems
}

CALL(25, glTexImage2D, void, (__attribute__((unused)) GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Get Texture Size
    unsigned char is_null = pixels == NULL;
    int size;
    if (!is_null) {
        size = get_texture_size(width, height, format, type);
    }

    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) level);
    write_int((uint32_t) internalformat);
    write_int((uint32_t) width);
    write_int((uint32_t) height);
    write_int((uint32_t) border);
    write_int((uint32_t) format);
    write_int((uint32_t) type);
    write_int(bound_texture);
    write_byte(is_null);
    if (!is_null) {
        safe_write((void *) pixels, (size_t) size);
    }

    // Release Proxy
    end_proxy_call();
#else
    GLint level = (GLint) read_int();
    GLint internalformat = (GLint) read_int();
    GLsizei width = (GLsizei) read_int();
    GLsizei height = (GLsizei) read_int();
    GLint border = (GLint) read_int();
    GLenum format = (GLenum) read_int();
    GLenum type = (GLenum) read_int();
    GLuint bound_texture = (GLuint) read_int();
    unsigned char is_null = read_byte();
    void *pixels = NULL;
    if (!is_null) {
        int size = get_texture_size(width, height, format, type);
        pixels = malloc(size);
        ALLOC_CHECK(pixels);
        safe_read(pixels, (size_t) size);
    }
    // Run
    glBindTexture(GL_TEXTURE_2D, bound_texture);
    glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, pixels);
    // Free
    if (!is_null) {
        free(pixels);
    }
#endif
}

CALL(26, glEnable, void, (GLenum cap)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) cap);

    // Release Proxy
    end_proxy_call();
#else
    GLenum cap = (GLenum) read_int();
    // Run
    glEnable(cap);
#endif
}

#if defined(MEDIA_LAYER_PROXY_SERVER)
void glEnableClientState(GLenum array) {
    // Set
    unsigned char *enabled = get_array_enabled_pointer(array);
    if (*enabled) {
        return;
    } else {
        *enabled = 1;
    }
}
#endif

CALL(28, glPolygonOffset, void, (GLfloat factor, GLfloat units)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(factor);
    write_float(units);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat factor = read_float();
    GLfloat units = read_float();
    // Run
    glPolygonOffset(factor, units);
#endif
}

CALL_GL_POINTER(41, glTexCoordPointer)

#if defined(MEDIA_LAYER_PROXY_SERVER)
void glDisableClientState(GLenum array) {
    // Set
    unsigned char *enabled = get_array_enabled_pointer(array);
    if (!*enabled) {
        return;
    } else {
        *enabled = 0;
        // Not needed when using compatibility layer
#ifndef MCPI_USE_GLES1_COMPATIBILITY_LAYER
        switch (array) {
            case GL_VERTEX_ARRAY: {
                is_set_glVertexPointer = 0;
                break;
            }
            case GL_COLOR_ARRAY: {
                is_set_glColorPointer = 0;
                break;
            }
            case GL_TEXTURE_COORD_ARRAY: {
                is_set_glTexCoordPointer = 0;
                break;
            }
        }
#endif
    }
}
#endif

CALL(30, glDepthRangef, void, (GLclampf near, GLclampf far)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(near);
    write_float(far);

    // Release Proxy
    end_proxy_call();
#else
    GLclampf near = read_float();
    GLclampf far = read_float();
    // Run
    glDepthRangef(near, far);
#endif
}

CALL(31, glDepthFunc, void, (GLenum func)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) func);

    // Release Proxy
    end_proxy_call();
#else
    GLenum func = (GLenum) read_int();
    // Run
    glDepthFunc(func);
#endif
}

#if defined(MEDIA_LAYER_PROXY_SERVER)
void glBindBuffer(GLenum target, GLuint buffer) {
    // Set
    if (target == GL_ARRAY_BUFFER) {
        bound_buffer = buffer;
    } else {
        PROXY_ERR("Unsupported Buffer Binding: %u", target);
    }
    // Not needed when using compatibility layer
#ifndef MCPI_USE_GLES1_COMPATIBILITY_LAYER
    is_set_glVertexPointer = 0;
    is_set_glColorPointer = 0;
    is_set_glTexCoordPointer = 0;
#endif
}
#endif

CALL(33, glClearColor, void, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(red);
    write_float(green);
    write_float(blue);
    write_float(alpha);

    // Release Proxy
    end_proxy_call();
#else
    GLclampf red = read_float();
    GLclampf green = read_float();
    GLclampf blue = read_float();
    GLclampf alpha = read_float();
    // Run
    glClearColor(red, green, blue, alpha);
#endif
}

CALL(34, glPopMatrix, void, ()) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();
    // Release Proxy
    end_proxy_call();
#else
    // Run
    glPopMatrix();
#endif
}

CALL(35, glLoadIdentity, void, ()) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();
    // Release Proxy
    end_proxy_call();
#else
    // Run
    glLoadIdentity();
#endif
}

CALL(36, glScalef, void, (GLfloat x, GLfloat y, GLfloat z)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(x);
    write_float(y);
    write_float(z);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat x = read_float();
    GLfloat y = read_float();
    GLfloat z = read_float();
    // Run
    glScalef(x, y, z);
#endif
}

CALL(37, glPushMatrix, void, ()) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();
    // Release Proxy
    end_proxy_call();
#else
    // Run
    glPushMatrix();
#endif
}

CALL(38, glDepthMask, void, (GLboolean flag)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) flag);

    // Release Proxy
    end_proxy_call();
#else
    GLboolean flag = (GLboolean) read_int();
    // Run
    glDepthMask(flag);
#endif
}

CALL(39, glHint, void, (GLenum target, GLenum mode)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) target);
    write_int((uint32_t) mode);

    // Release Proxy
    end_proxy_call();
#else
    GLenum target = (GLenum) read_int();
    GLenum mode = (GLenum) read_int();
    // Run
    glHint(target, mode);
#endif
}

static int get_glMultMatrixf_size() {
    return 16;
}
CALL(40, glMultMatrixf, void, (const GLfloat *m)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    safe_write((void *) m, sizeof (float) * get_glMultMatrixf_size());

    // Release Proxy
    end_proxy_call();
#else
    GLfloat m[get_glMultMatrixf_size()];
    safe_read((void *) m, sizeof (float) * get_glMultMatrixf_size());
    // Run
    glMultMatrixf(m);
#endif
}

CALL(42, glDeleteBuffers, void, (GLsizei n, const GLuint *buffers)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) n);
    for (int i = 0; i < n; i++) {
        write_int((uint32_t) buffers[i]);
    }

    // Release Proxy
    end_proxy_call();
#else
    GLsizei n = (GLsizei) read_int();
    GLuint buffers[n];
    for (int i = 0; i < n; i++) {
        buffers[i] = (GLuint) read_int();
    }
    // Run
    glDeleteBuffers(n, buffers);
#endif
}

CALL(43, glColorMask, void, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) red);
    write_int((uint32_t) green);
    write_int((uint32_t) blue);
    write_int((uint32_t) alpha);

    // Release Proxy
    end_proxy_call();
#else
    GLboolean red = (GLboolean) read_int();
    GLboolean green = (GLboolean) read_int();
    GLboolean blue = (GLboolean) read_int();
    GLboolean alpha = (GLboolean) read_int();
    // Run
    glColorMask(red, green, blue, alpha);
#endif
}

CALL(44, glTexSubImage2D, void, (__attribute__((unused)) GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Get Texture Size
    unsigned char is_null = pixels == NULL;
    int size;
    if (!is_null) {
        size = get_texture_size(width, height, format, type);
    }

    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) level);
    write_int((uint32_t) xoffset);
    write_int((uint32_t) yoffset);
    write_int((uint32_t) width);
    write_int((uint32_t) height);
    write_int((uint32_t) format);
    write_int((uint32_t) type);
    write_int(bound_texture);
    write_byte(is_null);
    if (!is_null) {
        safe_write((void *) pixels, (size_t) size);
    }

    // Release Proxy
    end_proxy_call();
#else
    GLint level = (GLint) read_int();
    GLint xoffset = (GLint) read_int();
    GLint yoffset = (GLint) read_int();
    GLsizei width = (GLsizei) read_int();
    GLsizei height = (GLsizei) read_int();
    GLenum format = (GLenum) read_int();
    GLenum type = (GLenum) read_int();
    GLuint bound_texture = (GLuint) read_int();
    unsigned char is_null = read_byte();
    void *pixels = NULL;
    if (!is_null) {
        int size = get_texture_size(width, height, format, type);
        pixels = malloc(size);
        ALLOC_CHECK(pixels);
        safe_read(pixels, (size_t) size);
    }
    // Run
    glBindTexture(GL_TEXTURE_2D, bound_texture);
    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, pixels);
    // Free
    if (!is_null) {
        free(pixels);
    }
#endif
}

CALL(45, glGenTextures, void, (GLsizei n, GLuint *textures)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) n);

    // Get Return Value
    for (GLsizei i = 0; i < n; i++) {
        textures[i] = (GLuint) read_int();
    }

    // Release Proxy
    end_proxy_call();
#else
    GLsizei n = (GLsizei) read_int();
    GLuint textures[n];
    // Run
    glGenTextures(n, textures);
    // Return Value
    for (GLsizei i = 0; i < n; i++) {
        write_int((uint32_t) textures[i]);
    }
#endif
}

CALL(46, glDeleteTextures, void, (GLsizei n, const GLuint *textures)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) n);
    for (GLsizei i = 0; i < n; i++) {
        write_int((uint32_t) textures[i]);
    }

    // Release Proxy
    end_proxy_call();
#else
    GLsizei n = (GLsizei) read_int();
    GLuint textures[n];
    for (GLsizei i = 0; i < n; i++) {
        textures[i] = (GLuint) read_int();
    }
    // Run
    glDeleteTextures(n, textures);
#endif
}

CALL(47, glAlphaFunc, void, (GLenum func, GLclampf ref)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) func);
    write_float(ref);

    // Release Proxy
    end_proxy_call();
#else
    GLenum func = (GLenum) read_int();
    GLclampf ref = read_float();
    // Run
    glAlphaFunc(func, ref);
#endif
}

static int get_glGetFloatv_params_size(GLenum pname) {
    switch (pname) {
        case GL_MODELVIEW_MATRIX:
        case GL_PROJECTION_MATRIX: {
            return 16;
        }
        default: {
            PROXY_ERR("Unsupported glGetFloatv Property: %u", pname);
        }
    }
}
CALL(48, glGetFloatv, void, (GLenum pname, GLfloat *params)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) pname);

    // Get Return Value
    int size = get_glGetFloatv_params_size(pname);
    safe_read((void *) params, sizeof (GLfloat) * size);

    // Release Proxy
    end_proxy_call();
#else
    GLenum pname = (GLenum) read_int();
    int size = get_glGetFloatv_params_size(pname);
    GLfloat params[size];
    // Run
    glGetFloatv(pname, params);
    // Return Value
    safe_write((void *) params, sizeof (GLfloat) * size);
#endif
}

#if defined(MEDIA_LAYER_PROXY_SERVER)
void glBindTexture(GLenum target, GLuint texture) {
    // Set
    if (target == GL_TEXTURE_2D) {
        bound_texture = texture;
    } else {
        PROXY_ERR("Unsupported Texture Binding: %u", target);
    }
}
#endif

CALL(50, glTranslatef, void, (GLfloat x, GLfloat y, GLfloat z)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(x);
    write_float(y);
    write_float(z);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat x = read_float();
    GLfloat y = read_float();
    GLfloat z = read_float();
    // Run
    glTranslatef(x, y, z);
#endif
}

CALL(51, glShadeModel, void, (GLenum mode)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) mode);

    // Release Proxy
    end_proxy_call();
#else
    GLenum mode = (GLenum) read_int();
    // Run
    glShadeModel(mode);
#endif
}

CALL(52, glOrthof, void, (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(left);
    write_float(right);
    write_float(bottom);
    write_float(top);
    write_float(near);
    write_float(far);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat left = read_float();
    GLfloat right = read_float();
    GLfloat bottom = read_float();
    GLfloat top = read_float();
    GLfloat near = read_float();
    GLfloat far = read_float();
    // Run
    glOrthof(left, right, bottom, top, near, far);
#endif
}

CALL(53, glDisable, void, (GLenum cap)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) cap);

    // Release Proxy
    end_proxy_call();
#else
    GLenum cap = (GLenum) read_int();
    // Run
    glDisable(cap);
#endif
}

CALL(54, glCullFace, void, (GLenum mode)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) mode);

    // Release Proxy
    end_proxy_call();
#else
    GLenum mode = (GLenum) read_int();
    // Run
    glCullFace(mode);
#endif
}

CALL(55, glRotatef, void, (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(angle);
    write_float(x);
    write_float(y);
    write_float(z);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat angle = read_float();
    GLfloat x = read_float();
    GLfloat y = read_float();
    GLfloat z = read_float();
    // Run
    glRotatef(angle, x, y, z);
#endif
}

CALL(56, glViewport, void, (GLint x, GLint y, GLsizei width, GLsizei height)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) x);
    write_int((uint32_t) y);
    write_int((uint32_t) width);
    write_int((uint32_t) height);

    // Release Proxy
    end_proxy_call();
#else
    GLint x = (GLint) read_int();
    GLint y = (GLint) read_int();
    GLsizei width = (GLsizei) read_int();
    GLsizei height = (GLsizei) read_int();
    // Run
    glViewport(x, y, width, height);
#endif
}

CALL(57, glNormal3f, void, (GLfloat nx, GLfloat ny, GLfloat nz)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_float(nx);
    write_float(ny);
    write_float(nz);

    // Release Proxy
    end_proxy_call();
#else
    GLfloat nx = read_float();
    GLfloat ny = read_float();
    GLfloat nz = read_float();
    // Run
    glNormal3f(nx, ny, nz);
#endif
}

CALL(58, glIsEnabled, GLboolean, (GLenum cap)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) cap);

    // Get Return Value
    GLboolean ret = (GLboolean) read_int();

    // Release Proxy
    end_proxy_call();

    // Return
    return ret;
#else
    GLenum cap = (GLenum) read_int();
    // Run
    GLboolean ret = glIsEnabled(cap);
    // Return Value
    write_int((uint32_t) ret);
#endif
}

static int get_glGetIntegerv_params_size(GLenum pname) {
    switch (pname) {
        case GL_TEXTURE_BINDING_2D:
        case GL_PACK_ALIGNMENT:
        case GL_UNPACK_ALIGNMENT: {
            return 1;
        }
        case GL_VIEWPORT: {
            return 4;
        }
        default: {
            PROXY_ERR("Unsupported glGetIntegerv Property: %u", pname);
        }
    }
}
CALL(61, glGetIntegerv, void, (GLenum pname, GLint *params)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Stop Redudnant Proxy Call
    if (pname == GL_TEXTURE_BINDING_2D) {
        params[0] = bound_texture;
        return;
    }

    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) pname);

    // Get Return Value
    int size = get_glGetIntegerv_params_size(pname);
    safe_read((void *) params, sizeof (GLint) * size);

    // Release Proxy
    end_proxy_call();
#else
    GLenum pname = (GLenum) read_int();
    int size = get_glGetIntegerv_params_size(pname);
    GLint params[size];
    // Run
    glGetIntegerv(pname, params);
    // Return Value
    safe_write((void *) params, sizeof (GLint) * size);
#endif
}

CALL(65, glReadPixels, void, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Get Texture Size
    int size = get_texture_size(width, height, format, type);

    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) x);
    write_int((uint32_t) y);
    write_int((uint32_t) width);
    write_int((uint32_t) height);
    write_int((uint32_t) format);
    write_int((uint32_t) type);

    // Get Return Value
    safe_read((void *) data, size);

    // Release Proxy
    end_proxy_call();
#else
    GLint x = (GLint) read_int();
    GLint y = (GLint) read_int();
    GLsizei width = (GLsizei) read_int();
    GLsizei height = (GLsizei) read_int();
    GLenum format = (GLenum) read_int();
    GLenum type = (GLenum) read_int();
    int size = get_texture_size(width, height, format, type);
    void *pixels = malloc(size);
    ALLOC_CHECK(pixels);
    // Run
    glReadPixels(x, y, width, height, format, type, pixels);
    // Return Value
    safe_write((void *) pixels, (size_t) size);
    // Free
    free(pixels);
#endif
}

CALL(67, glGenBuffers, void, (GLsizei n, GLuint *buffers)) {
#if defined(MEDIA_LAYER_PROXY_SERVER)
    // Lock Proxy
    start_proxy_call();

    // Arguments
    write_int((uint32_t) n);

    // Get Return Value
    for (GLsizei i = 0; i < n; i++) {
        buffers[i] = (GLuint) read_int();
    }

    // Release Proxy
    end_proxy_call();
#else
    GLsizei n = (GLsizei) read_int();
    GLuint buffers[n];
    // Run
    glGenBuffers(n, buffers);
    // Return Value
    for (GLsizei i = 0; i < n; i++) {
        write_int((uint32_t) buffers[i]);
    }
#endif
}
