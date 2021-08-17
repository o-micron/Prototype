/// Copyright 2021 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#pragma once

#ifdef NDEBUG
#define PROTOTYPE_TARGET_NAME "RELEASE"
#define PROTOTYPE_TARGET_RELEASE
#else
#define PROTOTYPE_TARGET_NAME "DEBUG"
#define PROTOTYPE_TARGET_DEBUG
#endif

#if defined(__EMSCRIPTEN__)
#define PROTOTYPE_PLATFORM_NAME "WEBKIT"
#define PROTOTYPE_PLATFORM_WEBKIT
#define PROTOTYPE_FORCE_INLINE inline
#define PROTOTYPE_FORCE_NO_INLINE
#define PROTOTYPE_DYNAMIC_FN_CALL
#define PROTOTYPE_DLL_HANDLE_TYPE            void*
#define PROTOTYPE_DLL_OPEN(A, B)             dlopen(A, B)
#define PROTOTYPE_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
#define PROTOTYPE_DLL_CLOSE(A)               dlclose(A)
#define PROTOTYPE_FUNCTION_NAME              __func__
#elif defined(_WIN32) || defined(_WIN64)
#define PROTOTYPE_PLATFORM_NAME "WINDOWS"
#define PROTOTYPE_PLATFORM_WINDOWS
#define PROTOTYPE_FORCE_INLINE               __forceinline
#define PROTOTYPE_FORCE_NO_INLINE            _declspec(noinline)
#define PROTOTYPE_DYNAMIC_FN_CALL            __stdcall
#define PROTOTYPE_DLL_HANDLE_TYPE            HINSTANCE
#define PROTOTYPE_DLL_OPEN(A, B)             LoadLibrary(A)
#define PROTOTYPE_DLL_GET_PROC_ADDRESS(A, B) GetProcAddress(A, B)
#define PROTOTYPE_DLL_CLOSE(A)               FreeLibrary(A)
#define PROTOTYPE_FUNCTION_NAME              __FUNCTION__
#elif defined(__linux__)
#define PROTOTYPE_PLATFORM_NAME "LINUX"
#define PROTOTYPE_PLATFORM_LINUX
#define PROTOTYPE_FORCE_INLINE inline
#define PROTOTYPE_FORCE_NO_INLINE
#define PROTOTYPE_DYNAMIC_FN_CALL
#define PROTOTYPE_DLL_HANDLE_TYPE            void*
#define PROTOTYPE_DLL_OPEN(A, B)             dlopen(A, B)
#define PROTOTYPE_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
#define PROTOTYPE_DLL_CLOSE(A)               dlclose(A)
#define PROTOTYPE_FUNCTION_NAME              __func__
#elif defined(__APPLE__)
#define PROTOTYPE_PLATFORM_NAME "DARWIN"
#define PROTOTYPE_PLATFORM_DARWIN
#define PROTOTYPE_FORCE_INLINE inline
#define PROTOTYPE_FORCE_NO_INLINE
#define PROTOTYPE_DYNAMIC_FN_CALL
#define PROTOTYPE_DLL_HANDLE_TYPE            void*
#define PROTOTYPE_DLL_OPEN(A, B)             dlopen(A, B)
#define PROTOTYPE_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
#define PROTOTYPE_DLL_CLOSE(A)               dlclose(A)
#define PROTOTYPE_FUNCTION_NAME              __func__
#else
#error "Platform not supported."
#endif

#if defined(__cplusplus)
#define PROTOTYPE_EXTERN extern "C"
#else
#define PROTOTYPE_EXTERN extern
#endif

#if defined(__clang__)
#define Attachable(NAME) __attribute__((annotate(#NAME)))
#else
#define Attachable(NAME)
#endif

#if defined(PROTOTYPE_PLATFORM_WINDOWS)
#define PROTOTYPE_PURE_ABSTRACT __declspec(novtable)
#else
#define PROTOTYPE_PURE_ABSTRACT
#endif

// clang-format off

#if defined(PROTOTYPE_ASSERTIONS_ENABLED)
    #include <iostream>
    #if defined(_MSC_VER)
        #include <intrin.h>
        #define prototypeDebugBreak() __debugbreak();
    #else
        #define prototypeDebugBreak()
    #endif
    #define PROTOTYPE_ASSERT(expr)                                                                                     \
        {                                                                                                              \
            if (expr) {                                                                                                \
            } else {                                                                                                   \
                prototypeReportAssertionFailure(#expr, "", __FILE__, __LINE__);                                        \
                prototypeDebugBreak();                                                                                 \
            }                                                                                                          \
        }

    #define PROTOTYPE_ASSERT_MSG(expr, message)                                                                        \
        {                                                                                                              \
            if (expr) {                                                                                                \
            } else {                                                                                                   \
                prototypeReportAssertionFailure(#expr, message, __FILE__, __LINE__);                                   \
                prototypeDebugBreak();                                                                                 \
            }                                                                                                          \
        }

    #if defined(PROTOTYPE_TARGET_DEBUG)
        #define PROTOTYPE_ASSERT_DEBUG(expr)                                                                           \
            {                                                                                                          \
                if (expr) {                                                                                            \
                } else {                                                                                               \
                    prototypeReportAssertionFailure(#expr, "", __FILE__, __LINE__);                                    \
                    prototypeDebugBreak();                                                                             \
                }                                                                                                      \
            }
    #else
        #define PROTOTYPE_ASSERT_DEBUG(expr)
    #endif
    PROTOTYPE_FORCE_INLINE void
    prototypeReportAssertionFailure(const char* expression, const char* message, const char* file, int line)
    {
        std::cerr << "[PROTOTYPE ASSERTION FAILURE]: {\n\t" << expression << "\n\tMESSAGE: '" << message
                << "'\n\tFILE: " << file << ":" << line << "\n}\n";
    }
#else
    #define PROTOTYPE_ASSERT(expr)
    #define PROTOTYPE_ASSERT_MSG(expr, message)
    #define PROTOTYPE_ASSERT_DEBUG(expr)
#endif

#if defined(PROTOTYPE_TARGET_DEBUG) && defined(PROTOTYPE_PLATFORM_WINDOWS)
    #define PROTOTYPE_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
    #define PROTOTYPE_NEW new
#endif

#if defined(PROTOTYPE_ASSETS_PATH)
    #define PROTOTYPE_ROOT_BUNDLE_PATH          "bundles/"
    #define PROTOTYPE_ROOT_LOG_PATH             "logs/"
    #define PROTOTYPE_ROOT_SCENE_PATH           "scenes/"
    #define PROTOTYPE_ROOT_FONT_PATH            "fonts/"
    #define PROTOTYPE_ROOT_MESH_PATH            "meshes/"
    #define PROTOTYPE_ROOT_SHADER_PATH          "shaders/"
#if defined(__EMSCRIPTEN__)
    #define PROTOTYPE_ROOT_OPENGL_SHADER_PATH   "shaders/opengles/"
#else
    #define PROTOTYPE_ROOT_OPENGL_SHADER_PATH   "shaders/opengl/"
#endif
    #define PROTOTYPE_ROOT_VULKAN_SHADER_PATH   "shaders/vulkan/"
    #define PROTOTYPE_ROOT_TEXTURE_PATH         "textures/"
    #define PROTOTYPE_ROOT_VIDEO_PATH           "videos/"
    #define PROTOTYPE_ROOT_PLUGIN_PATH          "plugins/"
    
    #define PROTOTYPE_BUNDLE_PATH(A)        PROTOTYPE_ASSETS_PATH "bundles/" A
    #define PROTOTYPE_LOG_PATH(A)           PROTOTYPE_ASSETS_PATH "logs/" A
    #define PROTOTYPE_SCENE_PATH(A)         PROTOTYPE_ASSETS_PATH "scenes/" A
    #define PROTOTYPE_FONT_PATH(A)          PROTOTYPE_ASSETS_PATH "fonts/" A
    #define PROTOTYPE_MESH_PATH(A)          PROTOTYPE_ASSETS_PATH "meshes/" A
    #define PROTOTYPE_SHADER_PATH(A)        PROTOTYPE_ASSETS_PATH "shaders/" A
#if defined(__EMSCRIPTEN__)
    #define PROTOTYPE_OPENGL_SHADER_PATH(A) PROTOTYPE_ASSETS_PATH "shaders/opengles/" A
#else
    #define PROTOTYPE_OPENGL_SHADER_PATH(A) PROTOTYPE_ASSETS_PATH "shaders/opengl/" A
#endif
    #define PROTOTYPE_VULKAN_SHADER_PATH(A) PROTOTYPE_ASSETS_PATH "shaders/vulkan/" A
    #define PROTOTYPE_TEXTURE_PATH(A)       PROTOTYPE_ASSETS_PATH "textures/" A
    #define PROTOTYPE_VIDEO_PATH(A)         PROTOTYPE_ASSETS_PATH "videos/" A
    #define PROTOTYPE_PLUGIN_PATH(A)        PROTOTYPE_ASSETS_PATH "plugins/" A
#else
    #error "You need to define assets path."
#endif

#if defined(PROTOTYPE_PLUGINS_PATH)
    #define PROTOTYPE_RUST_PLUGINS_PATH(A)  PROTOTYPE_PLUGINS_PATH "Rust/" A
    #define PROTOTYPE_CPP_PLUGINS_PATH(A)   PROTOTYPE_PLUGINS_PATH "Cpp/" A
#else
    #error "You need to define plugins path."
#endif

// #define PROTOTYPE_DEFAULT_SCENE_LAYER       "default"
#define PROTOTYPE_DEFAULT_MESH              "CUBE"
#define PROTOTYPE_DEFAULT_SHADER            "default"
#define PROTOTYPE_DEFAULT_TEXTURE           "default.jpg"
#define PROTOTYPE_DEFAULT_MATERIAL          "default"
#define PROTOTYPE_DEFAULT_COLLIDER_SHAPE    "default"
#define PROTOTYPE_DEFAULT_FRAMEBUFFER       "default"
#define PROTOTYPE_DEFAULT_SKYBOX            "graffiti.hdr"

#define PROTOTYPE_STRINGIFY(A) #A

#define PROTOTYPE_STRINGIFY_ENUM(A) static const std::string A##_Str = #A;
#define PROTOTYPE_STRINGIFY_ENUM_EXTENDED(A) static const std::string A##_Str = #A;

// this is here because VA_ARGS is not in the standard yet ? wtf ?! yep, this is the truth !
// MSVC will expand the VA_ARGS incorrectly leaving the whole thing down below a mess !
// A quick fix is here
#define PROTOTYPE_EXPAND(X) X

#define PROTOTYPE_CONCATENATE(arg1, arg2)  PROTOTYPE_CONCATENATE1(arg1, arg2)
#define PROTOTYPE_CONCATENATE1(arg1, arg2) PROTOTYPE_CONCATENATE2(arg1, arg2)
#define PROTOTYPE_CONCATENATE2(arg1, arg2) arg1##arg2

// ===-------------------------------------------------------------------------------------------------------------------------===
// X (1 .. 30)
// ===-------------------------------------------------------------------------------------------------------------------------===
#define PROTOTYPE_FOR_EACH_X_1(what, what2, x, ...) what2(x)
#define PROTOTYPE_FOR_EACH_X_2(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_1(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_3(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_2(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_4(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_3(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_5(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_4(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_6(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_5(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_7(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_6(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_8(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_7(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_9(what, what2, x, ...)                                                                              \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_8(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_10(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_9(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_11(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_10(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_12(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_11(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_13(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_12(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_14(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_13(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_15(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_14(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_16(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_15(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_17(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_16(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_18(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_17(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_19(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_18(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_20(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_19(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_21(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_20(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_22(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_21(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_23(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_22(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_24(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_23(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_25(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_24(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_26(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_25(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_27(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_26(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_28(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_27(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_29(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_28(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_30(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_29(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_31(what, what2, x, ...)                                                                             \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_30(what, what2, __VA_ARGS__))

#define PROTOTYPE_FOR_EACH_X_RSEQ_N() 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define PROTOTYPE_FOR_EACH_X_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...) N
#define PROTOTYPE_FOR_EACH_X_NARG_(...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ARG_N(__VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_NARG(...)  PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_NARG_(__VA_ARGS__, PROTOTYPE_FOR_EACH_X_RSEQ_N()))
#define PROTOTYPE_FOR_EACH_X_(N, what, what2, x, ...) PROTOTYPE_EXPAND(PROTOTYPE_CONCATENATE(PROTOTYPE_FOR_EACH_X_, N)(what, what2, x, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X(what, what2, x, ...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_(PROTOTYPE_FOR_EACH_X_NARG(x, __VA_ARGS__), what, what2, x, __VA_ARGS__))
// ===-------------------------------------------------------------------------------------------------------------------------===

// ===-------------------------------------------------------------------------------------------------------------------------===
// X (ODD) (1 .. 30)
// ===-------------------------------------------------------------------------------------------------------------------------===
#define PROTOTYPE_FOR_EACH_X_ODD_1(what, what2, x, ...) what2(x)
#define PROTOTYPE_FOR_EACH_X_ODD_2(what, what2, x, ...)                                                                          \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_1(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_3(what, what2, x, ...)                                                                          \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_2(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_4(what, what2, x, ...)                                                                          \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_3(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_5(what, what2, x, ...)                                                                          \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_4(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_6(what, what2, x, ...)                                                                          \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_5(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_7(what, what2, x, ...)                                                                          \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_6(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_8(what, what2, x, ...)                                                                          \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_7(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_9(what, what2, x, ...)                                                                          \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_8(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_10(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_9(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_11(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_10(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_12(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_11(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_13(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_12(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_14(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_13(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_15(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_14(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_16(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_15(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_17(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_16(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_18(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_17(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_19(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_18(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_20(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_19(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_21(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_20(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_22(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_21(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_23(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_22(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_24(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_23(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_25(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_24(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_26(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_25(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_27(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_26(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_28(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_27(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_29(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_28(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_30(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_29(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_31(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_30(what, what2, __VA_ARGS__))

#define PROTOTYPE_FOR_EACH_X_ODD_RSEQ_N() 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define PROTOTYPE_FOR_EACH_X_ODD_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...) N
#define PROTOTYPE_FOR_EACH_X_ODD_NARG_(...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_ARG_N(__VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD_NARG(...)  PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_NARG_(__VA_ARGS__, PROTOTYPE_FOR_EACH_X_ODD_RSEQ_N()))
#define PROTOTYPE_FOR_EACH_X_ODD_(N, what, what2, x, ...) PROTOTYPE_EXPAND(PROTOTYPE_CONCATENATE(PROTOTYPE_FOR_EACH_X_ODD_, N)(what, what2, x, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_ODD(what, what2, x, ...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_ODD_(PROTOTYPE_FOR_EACH_X_ODD_NARG(x, __VA_ARGS__), what, what2, x, __VA_ARGS__))
// ===-------------------------------------------------------------------------------------------------------------------------===

// ===-------------------------------------------------------------------------------------------------------------------------===
// X (EVEN) (1 .. 30)
// ===-------------------------------------------------------------------------------------------------------------------------===
#define PROTOTYPE_FOR_EACH_X_EVEN_1(what, what2, x, ...) 
#define PROTOTYPE_FOR_EACH_X_EVEN_2(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_1(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_3(what, what2, x, ...)                                                                         \                                                                                                                     \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_2(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_4(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_3(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_5(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_4(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_6(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_5(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_7(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_6(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_8(what, what2, x, ...)                                                                         \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_7(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_9(what, what2, x, ...)                                                                         \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_8(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_10(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_9(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_11(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_10(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_12(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_11(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_13(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_12(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_14(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_13(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_15(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_14(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_16(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_15(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_17(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_16(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_18(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_17(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_19(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_18(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_20(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_19(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_21(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_20(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_22(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_21(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_23(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_22(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_24(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_23(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_25(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_24(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_26(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_25(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_27(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_26(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_28(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_27(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_29(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_28(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_30(what, what2, x, ...)                                                                        \
    what(x)                                                                                                                      \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_29(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_31(what, what2, x, ...)                                                                        \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_30(what, what2, __VA_ARGS__))
                                                                                                                     
#define PROTOTYPE_FOR_EACH_X_EVEN_NARG(...)  PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_NARG_(__VA_ARGS__, PROTOTYPE_FOR_EACH_X_EVEN_RSEQ_N()))
#define PROTOTYPE_FOR_EACH_X_EVEN_NARG_(...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_ARG_N(__VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...) N
#define PROTOTYPE_FOR_EACH_X_EVEN_RSEQ_N() 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define PROTOTYPE_FOR_EACH_X_EVEN_(N, what, what2, x, ...) PROTOTYPE_EXPAND(PROTOTYPE_CONCATENATE(PROTOTYPE_FOR_EACH_X_EVEN_, N)(what, what2, x, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_EVEN(what, what2, x, ...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_EVEN_(PROTOTYPE_FOR_EACH_X_EVEN_NARG(x, __VA_ARGS__), what, what2, x, __VA_ARGS__))
// ===-------------------------------------------------------------------------------------------------------------------------===

// ===-------------------------------------------------------------------------------------------------------------------------===
// X, Y (1 .. 30)
// ===-------------------------------------------------------------------------------------------------------------------------===
#define PROTOTYPE_FOR_EACH_X_Y_1(what, what2, x, y, ...) what2(x, y)
#define PROTOTYPE_FOR_EACH_X_Y_2(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_1(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_3(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_2(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_4(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_3(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_5(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_4(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_6(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_5(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_7(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_6(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_8(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_7(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_9(what, what2, x, y, ...)                                                                         \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_8(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_10(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_9(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_11(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_10(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_12(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_11(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_13(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_12(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_14(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_13(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_15(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_14(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_16(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_15(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_17(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_16(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_18(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_17(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_19(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_18(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_20(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_19(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_21(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_20(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_22(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_21(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_23(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_22(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_24(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_23(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_25(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_24(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_26(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_25(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_27(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_26(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_28(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_27(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_29(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_28(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_30(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_29(what, what2, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_31(what, what2, x, y, ...)                                                                        \
    what(x, y)                                                                                                                   \
    PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_30(what, what2, __VA_ARGS__))

#define PROTOTYPE_FOR_EACH_X_Y_NARG(...)  PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_NARG_(__VA_ARGS__, PROTOTYPE_FOR_EACH_X_Y_RSEQ_N()))
#define PROTOTYPE_FOR_EACH_X_Y_NARG_(...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_ARG_N(__VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...) N
#define PROTOTYPE_FOR_EACH_X_Y_RSEQ_N() 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define PROTOTYPE_FOR_EACH_X_Y_(N, what, what2, x, y, ...) PROTOTYPE_EXPAND(PROTOTYPE_CONCATENATE(PROTOTYPE_FOR_EACH_X_Y_, N)(what, what2, x, y, __VA_ARGS__))
#define PROTOTYPE_FOR_EACH_X_Y(what, what2, x, y, ...) PROTOTYPE_EXPAND(PROTOTYPE_FOR_EACH_X_Y_(PROTOTYPE_FOR_EACH_X_Y_NARG(x, __VA_ARGS__), what, what2, x, y, __VA_ARGS__))
// ===-------------------------------------------------------------------------------------------------------------------------===

// clang-format on