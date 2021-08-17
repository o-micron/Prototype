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

// clang-format off
#include "PrototypeOpenglRenderer.h"
// clang-format on
#include "../core/PrototypePhysics.h"
#include "../core/PrototypeEngine.h"
#include "PrototypeOpenglWindow.h"
#include "../core/PrototypePluginInstance.h"
#include "../core/PrototypeDatabase.h"
#include "../core/PrototypeScene.h"

#include <PrototypeCommon/Logger.h>

// --------------------------------------------------------
// FILE INTERNAL FUNCTIONS DECLARATIONS
// --------------------------------------------------------
static i32
prototypeWindowMin(i32 A, i32 B);
static i32
prototypeWindowMax(i32 A, i32 B);
static bool
prototypeWindowGlfwSetWindowCenter(GLFWwindow* window);
static void
prototypeWindowGlfwErrorCallback(i32 error, const char* description);
static void
prototypeWindowGlfwWindowSizeCallback(GLFWwindow* handle, i32 width, i32 height);
static void
prototypeWindowGlfwMouseBtnCallback(GLFWwindow* handle, i32 button, i32 action, i32 mods);
static void
prototypeWindowGlfwCursorPosCallback(GLFWwindow* handle, f64 x, f64 y);
static void
prototypeWindowGlfwScrollCallback(GLFWwindow* handle, f64 x, f64 y);
static void
prototypeWindowGlfwKeyCallback(GLFWwindow* handle, i32 key, i32 scancode, i32 action, i32 mods);
static void
prototypeWindowGlfwCharCallback(GLFWwindow* handle, unsigned char c);
static void
prototypeWindowGlfwDropCallback(GLFWwindow* handle, i32 numFiles, const char** names);
static void
prototypeWindowGlfwIconifyCallback(GLFWwindow* handle, int status);
static void
prototypeWindowGlfwMaximizedCallback(GLFWwindow* handle, int status);
// --------------------------------------------------------

PrototypeOpenglWindow::PrototypeOpenglWindow()
  : _handle(nullptr)
  , _deltaTime(1.0f / 60.0f)
  , _resolution{ 600, 400 }
  , _mouseLocation{ 1, 1 }
  , _physicalDeviceStr("UNKNOWN GPU VENDOR")
  , _mouseDown({ false, false, false })
  , _needsReload(false)
  , _needsInspector(false)
  , _isCtrlDown(false)
  , _isShiftDown(false)
  , _isAltDown(false)
  , _isIconified(false)
  , _isMaximized(false)
{}

bool
PrototypeOpenglWindow::init(i32 width, i32 height)
{
    _resolution = { (f32)width, (f32)height };

    glfwSetErrorCallback(prototypeWindowGlfwErrorCallback);
    if (!glfwInit()) { PrototypeLogger::fatal("Failed to initialize glfw %s:%i", __FILE__, __LINE__); }

    GLFWmonitor*       monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode    = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 0);

    _handle = glfwCreateWindow(
      (i32)_resolution.x, (i32)_resolution.y, "Prototype [" PROTOTYPE_TARGET_NAME "] [OPENGL]", nullptr, nullptr);
    if (!_handle) {
        PrototypeLogger::fatal("Failed to create a window %s:%i", __FILE__, __LINE__);
        return false;
    }
    prototypeWindowGlfwSetWindowCenter(_handle);
    glfwSetFramebufferSizeCallback(_handle, prototypeWindowGlfwWindowSizeCallback);
    glfwSetMouseButtonCallback(_handle, prototypeWindowGlfwMouseBtnCallback);
    glfwSetCursorPosCallback(_handle, prototypeWindowGlfwCursorPosCallback);
    glfwSetScrollCallback(_handle, prototypeWindowGlfwScrollCallback);
    glfwSetKeyCallback(_handle, prototypeWindowGlfwKeyCallback);
    glfwSetDropCallback(_handle, prototypeWindowGlfwDropCallback);
    glfwSetWindowIconifyCallback(_handle, prototypeWindowGlfwIconifyCallback);
    glfwSetWindowMaximizeCallback(_handle, prototypeWindowGlfwMaximizedCallback);
    glfwSetWindowUserPointer(_handle, this);
    glfwMakeContextCurrent(_handle);
    glfwSwapInterval(1);
    PrototypeEngineInternalApplication::renderer = PROTOTYPE_NEW PrototypeOpenglRenderer(this);

    _deltaTime = 1.0f / 60.0f;
    _time      = glfwGetTime();

    return true;
}

void
PrototypeOpenglWindow::deInit()
{
    delete PrototypeEngineInternalApplication::renderer;
    if (_handle) { glfwDestroyWindow(_handle); }
    glfwTerminate();
}

void
PrototypeOpenglWindow::setSize(i32 width, i32 height)
{
    prototypeWindowGlfwWindowSizeCallback(_handle, width, height);
}

bool
PrototypeOpenglWindow::update()
{
    _deltaTime = glfwGetTime() - _time;
    _time      = glfwGetTime();

    glfwPollEvents();
    glfwSwapBuffers(_handle);
    return glfwWindowShouldClose(_handle) || PrototypeEngineInternalApplication::shouldQuit;
}

void*
PrototypeOpenglWindow::handle()
{
    return (void*)_handle;
}

f32
PrototypeOpenglWindow::refreshRate()
{
    static f32 refreshRate = 0;
    if (refreshRate == 0) {
        GLFWmonitor*       primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);
        refreshRate                       = (f32)mode->refreshRate;
    }
    return refreshRate;
}

f64
PrototypeOpenglWindow::time()
{
    return _time;
}

f64
PrototypeOpenglWindow::deltaTime()
{
    return _deltaTime;
}

glm::vec2
PrototypeOpenglWindow::resolution()
{
    return _resolution;
}

bool
PrototypeOpenglWindow::needsReload()
{
    return _needsReload;
}

bool
PrototypeOpenglWindow::needsInspector()
{
    return _needsInspector;
}

bool
PrototypeOpenglWindow::isIconified()
{
    return _isIconified;
}

bool
PrototypeOpenglWindow::isMaximized()
{
    return _isMaximized;
}

bool
PrototypeOpenglWindow::isCtrlDown()
{
    return _isCtrlDown;
}

bool
PrototypeOpenglWindow::isShiftDown()
{
    return _isShiftDown;
}

bool
PrototypeOpenglWindow::isAltDown()
{
    return _isAltDown;
}

glm::bvec3
PrototypeOpenglWindow::isMouseDown()
{
    return _mouseDown;
}

glm::vec2
PrototypeOpenglWindow::mouseLocation()
{
    return _mouseLocation;
}

void
PrototypeOpenglWindow::resetDeltaTime()
{
    _deltaTime = 1.0f / 60.0f;
    _time      = glfwGetTime();
}

void
PrototypeOpenglWindow::setResolution(i32 width, i32 height)
{
    _resolution = { (f32)width, (f32)height };
}

void
PrototypeOpenglWindow::setNeedsInspector()
{
    _needsInspector = true;
}

void
PrototypeOpenglWindow::consumeNeedsInspector()
{
    _needsInspector = false;
}

void
PrototypeOpenglWindow::onMouseFn(i32 button, i32 action, i32 mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            _mouseDown[0] = true;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            _mouseDown[2] = true;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            _mouseDown[1] = true;
        }

    } else if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            _mouseDown[0] = false;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            _mouseDown[2] = false;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            _mouseDown[1] = false;
        }
    }
    PrototypeEngineInternalApplication::renderer->onMouse(button, action, mods);
    PrototypeEngineInternalApplication::physics->onMouse(button, action, mods);
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnMouse(&codeLinkPair.second, scriptableObject, button, action, mods);
        }
    }
}

void
PrototypeOpenglWindow::onMouseMoveFn(f64 x, f64 y)
{
    if (_mouseDown[0] || _mouseDown[1] || _mouseDown[2]) {
        i32 btn;
        if (_mouseDown[0]) {
            btn = GLFW_MOUSE_BUTTON_LEFT;
        } else if (_mouseDown[2]) {
            btn = GLFW_MOUSE_BUTTON_RIGHT;
        } else if (_mouseDown[1]) {
            btn = GLFW_MOUSE_BUTTON_MIDDLE;
        }
        f64 diffx = x - _mouseLocation[0];
        f64 diffy = y - _mouseLocation[1];
        PrototypeEngineInternalApplication::renderer->onMouseDrag(btn, diffx, diffy);
        PrototypeEngineInternalApplication::physics->onMouseDrag(btn, diffx, diffy);
        auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
        for (PrototypeObject* scriptableObject : scriptableObjects) {
            Script* script = scriptableObject->getScriptTrait();
            for (const auto& codeLinkPair : script->codeLinks) {
                PrototypePluginInstance::safeCallOnMouseDrag(&codeLinkPair.second, scriptableObject, btn, diffx, diffy);
            }
        }
    } else {
        PrototypeEngineInternalApplication::renderer->onMouseMove(x, y);
        PrototypeEngineInternalApplication::physics->onMouseMove(x, y);
        auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
        for (PrototypeObject* scriptableObject : scriptableObjects) {
            Script* script = scriptableObject->getScriptTrait();
            for (const auto& codeLinkPair : script->codeLinks) {
                PrototypePluginInstance::safeCallOnMouseMove(&codeLinkPair.second, scriptableObject, x, y);
            }
        }
    }
    _mouseLocation = { x, y };
}

void
PrototypeOpenglWindow::onMouseScrollFn(f64 x, f64 y)
{
    PrototypeEngineInternalApplication::renderer->onMouseScroll(x, y);
    PrototypeEngineInternalApplication::physics->onMouseScroll(x, y);
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnMouseScroll(&codeLinkPair.second, scriptableObject, x, y);
        }
    }
}

void
PrototypeOpenglWindow::onKeyboardFn(i32 key, i32 scancode, i32 action, i32 mods)
{
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
            _isCtrlDown = false;
        } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
            _isShiftDown = false;
        } else if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
            _isAltDown = false;
        } else if (key == GLFW_KEY_F5) {
            _needsReload = true;
        } else if (key == GLFW_KEY_F7) {
            _needsInspector = true;
        }
    } else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
            _isCtrlDown = true;
        } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
            _isShiftDown = true;
        } else if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
            _isAltDown = true;
        } else if (key == GLFW_KEY_ESCAPE) {
        }
    } else if (action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {}
    }
    PrototypeEngineInternalApplication::renderer->onKeyboard(key, scancode, action, mods);
    PrototypeEngineInternalApplication::physics->onKeyboard(key, scancode, action, mods);
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnKeyboard(&codeLinkPair.second, scriptableObject, key, scancode, action, mods);
        }
    }
}

void
PrototypeOpenglWindow::onWindowResizeFn(i32 width, i32 height)
{
    _resolution.x = (f32)width;
    _resolution.y = (f32)height;
    PrototypeEngineInternalApplication::renderer->onWindowResize(width, height);
    PrototypeEngineInternalApplication::physics->onWindowResize(width, height);
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnWindowResize(&codeLinkPair.second, scriptableObject, width, height);
        }
    }
}

void
PrototypeOpenglWindow::onWindowDragDropFn(i32 numFiles, const char** names)
{
    PrototypeEngineInternalApplication::renderer->onWindowDragDrop(numFiles, names);
    PrototypeEngineInternalApplication::physics->onWindowDragDrop(numFiles, names);
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnWindowDragDrop(&codeLinkPair.second, scriptableObject, numFiles, names);
        }
    }
}

void
PrototypeOpenglWindow::onWindowIconifyFn()
{
    _isIconified           = true;
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnWindowIconify(&codeLinkPair.second, scriptableObject);
        }
    }
}

void
PrototypeOpenglWindow::onWindowIconifyRestoreFn()
{
    _isIconified           = false;
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnWindowIconifyRestore(&codeLinkPair.second, scriptableObject);
        }
    }
}

void
PrototypeOpenglWindow::onWindowMaximizeFn()
{
    _isMaximized           = true;
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnWindowMaximize(&codeLinkPair.second, scriptableObject);
        }
    }
}

void
PrototypeOpenglWindow::onWindowMaximizeRestoreFn()
{
    _isMaximized           = false;
    auto scriptableObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskScript);
    for (PrototypeObject* scriptableObject : scriptableObjects) {
        Script* script = scriptableObject->getScriptTrait();
        for (const auto& codeLinkPair : script->codeLinks) {
            PrototypePluginInstance::safeCallOnWindowMaximizeRestore(&codeLinkPair.second, scriptableObject);
        }
    }
}

// --------------------------------------------------------
// FILE INTERNAL FUNCTIONS DEFINITIONS
// --------------------------------------------------------
static i32
prototypeWindowMin(i32 A, i32 B)
{
    return (A <= B ? A : B);
}

static i32
prototypeWindowMax(i32 A, i32 B)
{
    return (A >= B ? A : B);
}

static bool
prototypeWindowGlfwSetWindowCenter(GLFWwindow* window)
{
    if (!window) { return false; }
    int sx = 0, sy = 0;
    int px = 0, py = 0;
    int mx = 0, my = 0;
    int monitor_count = 0;
    int best_area     = 0;
    int final_x = 0, final_y = 0;
    glfwGetWindowSize(window, &sx, &sy);
    glfwGetWindowPos(window, &px, &py);
    GLFWmonitor** m = glfwGetMonitors(&monitor_count);
    if (!m) { return false; }
    for (int j = 0; j < monitor_count; ++j) {
        glfwGetMonitorPos(m[j], &mx, &my);
        const GLFWvidmode* mode = glfwGetVideoMode(m[j]);
        if (!mode) { continue; }
        int minX = prototypeWindowMax(mx, px);
        int minY = prototypeWindowMax(my, py);
        int maxX = prototypeWindowMin(mx + mode->width, px + sx);
        int maxY = prototypeWindowMin(my + mode->height, py + sy);
        int area = prototypeWindowMax(maxX - minX, 0) * prototypeWindowMax(maxY - minY, 0);
        if (area > best_area) {
            final_x   = mx + (mode->width - sx) / 2;
            final_y   = my + (mode->height - sy) / 2;
            best_area = area;
        }
    }
    if (best_area) {
        glfwSetWindowPos(window, final_x, final_y);
    } else {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        if (primary) {
            const GLFWvidmode* desktop = glfwGetVideoMode(primary);
            if (desktop) {
                glfwSetWindowPos(window, (desktop->width - sx) / 2, (desktop->height - sy) / 2);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

static void
prototypeWindowGlfwErrorCallback(i32 error, const char* description)
{
    PrototypeLogger::fatal("window error: %s\nFile: at %s:%i", description, __FILE__, __LINE__);
}

static void
prototypeWindowGlfwWindowSizeCallback(GLFWwindow* handle, i32 width, i32 height)
{
    if (width < 1 || height < 1) { return; }
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    window->onWindowResizeFn(width, height);
}

static void
prototypeWindowGlfwMouseBtnCallback(GLFWwindow* handle, i32 button, i32 action, i32 mods)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    window->onMouseFn(button, action, mods);
}

static void
prototypeWindowGlfwCursorPosCallback(GLFWwindow* handle, f64 x, f64 y)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    window->onMouseMoveFn(x, y);
}

static void
prototypeWindowGlfwScrollCallback(GLFWwindow* handle, f64 x, f64 y)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    window->onMouseScrollFn(x, y);
}

static void
prototypeWindowGlfwKeyCallback(GLFWwindow* handle, i32 key, i32 scancode, i32 action, i32 mods)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    window->onKeyboardFn(key, scancode, action, mods);
}

static void
prototypeWindowGlfwCharCallback(GLFWwindow* handle, unsigned char c)
{}

static void
prototypeWindowGlfwDropCallback(GLFWwindow* handle, i32 numFiles, const char** names)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    window->onWindowDragDropFn(numFiles, names);
}

static void
prototypeWindowGlfwIconifyCallback(GLFWwindow* handle, int status)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    if (status == GLFW_TRUE) {
        window->onWindowIconifyFn();
    } else if (status == GLFW_FALSE) {
        window->onWindowIconifyRestoreFn();
    }
}

static void
prototypeWindowGlfwMaximizedCallback(GLFWwindow* handle, int status)
{
    PrototypeOpenglWindow* window = static_cast<PrototypeOpenglWindow*>(glfwGetWindowUserPointer(handle));
    if (status == GLFW_TRUE) {
        window->onWindowMaximizeFn();
    } else if (status == GLFW_FALSE) {
        window->onWindowMaximizeRestoreFn();
    }
}
// --------------------------------------------------------