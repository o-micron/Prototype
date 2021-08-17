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

#include "PrototypeOpenglUiSceneView.h"

#include "../core/PrototypeEngine.h"

#include "../core/PrototypeCameraSystem.h"
#include "../core/PrototypeEngine.h"
#include "../core/PrototypeInput.h"
#include "../core/PrototypePhysics.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"
#include "../core/PrototypeShortcuts.h"
#include "PrototypeOpenglRenderer.h"
#include "PrototypeOpenglWindow.h"

#define GLSL_VERSION std::string("#version 410 core")

PrototypeOpenglUiSceneView::PrototypeOpenglUiSceneView()
  : _guizmoOperation(ImGuizmo::OPERATION::TRANSLATE)
  , _guizmoMode(ImGuizmo::MODE::WORLD)
  , _isUsingGuizmo(false)
{
    _input = PROTOTYPE_NEW PrototypeInput();

    _input->subscribeKeyForStreams(GLFW_KEY_W);
    _input->subscribeKeyForStreams(GLFW_KEY_S);
    _input->subscribeKeyForStreams(GLFW_KEY_D);
    _input->subscribeKeyForStreams(GLFW_KEY_A);
    _input->subscribeKeyForStreams(GLFW_KEY_I);
    _input->subscribeKeyForStreams(GLFW_KEY_K);

    _input->subscribeKeyForStreams(GLFW_KEY_UP);
    _input->subscribeKeyForStreams(GLFW_KEY_DOWN);
    _input->subscribeKeyForStreams(GLFW_KEY_RIGHT);
    _input->subscribeKeyForStreams(GLFW_KEY_LEFT);
}

PrototypeOpenglUiSceneView::~PrototypeOpenglUiSceneView() { delete _input; }

void
PrototypeOpenglUiSceneView::onInit()
{
    snprintf(_renderingSettings,
             sizeof(_renderingSettings),
             "%s\nOpenGL: %i.%i - GLSL: %s",
             glGetString(GL_RENDERER),
             GL_MAJOR,
             GL_MINOR,
             GLSL_VERSION.substr(9, GLSL_VERSION.length() - 9).c_str());
}

void
PrototypeOpenglUiSceneView::onUpdate()
{
    _input->update();

    f32 vehicleAcceleration = _input->fetchKeyNormalizedValue(GLFW_KEY_UP);
    f32 vehicleBrake        = _input->fetchKeyNormalizedValue(GLFW_KEY_DOWN);
    f32 vehicleRight        = _input->fetchKeyNormalizedValue(GLFW_KEY_RIGHT);
    f32 vehicleLeft         = _input->fetchKeyNormalizedValue(GLFW_KEY_LEFT);
    f32 cameraForward       = _input->fetchKeyNormalizedValue(GLFW_KEY_W);
    f32 cameraBackward      = _input->fetchKeyNormalizedValue(GLFW_KEY_S);
    f32 cameraRight         = _input->fetchKeyNormalizedValue(GLFW_KEY_D);
    f32 cameraLeft          = _input->fetchKeyNormalizedValue(GLFW_KEY_A);
    f32 cameraUp            = _input->fetchKeyNormalizedValue(GLFW_KEY_I);
    f32 cameraDown          = _input->fetchKeyNormalizedValue(GLFW_KEY_K);

    auto vehicleObjets = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskTransform |
                                                                                         PrototypeTraitTypeMaskVehicleChasis);
    for (auto& vehicleObject : vehicleObjets) {
        VehicleChasis* chasis = vehicleObject->getVehicleChasisTrait();
        PrototypeEngineInternalApplication::physics->updateVehicleController(
          vehicleObject, vehicleAcceleration, vehicleBrake, vehicleRight - vehicleLeft);
    }

    Camera* cam = _camera->object->getCameraTrait();
    CameraSystemUpdateViewMatrix(cam, cameraLeft - cameraRight, cameraDown - cameraUp, cameraForward - cameraBackward);
    CameraSystemUpdateProjectionMatrix(cam);

    static glm::vec4 LightPosition;
    LightPosition = { 0.0f, 100.0f, 0.0f, PrototypeEngineInternalApplication::window->time() };
    static glm::vec4 LightColor;
    LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    glBindBuffer(GL_UNIFORM_BUFFER, _camera->ubo->id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &cam->viewMatrix()[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &cam->projectionMatrix()[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &cam->znear());
    glBufferSubData(GL_UNIFORM_BUFFER, 132, 4, &cam->zfar());
    glBufferSubData(GL_UNIFORM_BUFFER, 136, 8, &_cursorCoordinates[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &LightPosition[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &LightColor[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

PrototypeUIState_
PrototypeOpenglUiSceneView::onDrawFrame(PrototypeUIState_ state, void* awesomeFontTitle)
{
    ImVec2 availableContentRegion = ImGui::GetContentRegionAvail();
    if ((i32)availableContentRegion.x != _camera->finalFramebuffer->colorAttachments[0].width ||
        (i32)availableContentRegion.y != _camera->finalFramebuffer->colorAttachments[0].height) {
        state |= PrototypeUIState_SceneViewResized;
    }
    _dimensions = { availableContentRegion.x, availableContentRegion.y };
    float posx  = ImGui::GetCursorPosX();
    float posy  = ImGui::GetCursorPosY();

    const u32  TranslateButtonMask      = 1 << 0;
    const u32  RotateButtonMask         = 1 << 1;
    const u32  ScaleButtonMask          = 1 << 2;
    const u32  StatsButtonMask          = 1 << 3;
    static u32 guizmoToolbarButtonsMask = 0;

    ImGui::Image(
      (void*)(intptr_t)_camera->finalFramebuffer->colorAttachments[0].id, availableContentRegion, ImVec2(0, 1), ImVec2(1, 0));
    ImGuiIO& io           = ImGui::GetIO();
    ImVec2   pos          = ImGui::GetCursorScreenPos();
    float    mouseRegionX = io.MousePos.x - pos.x;
    float    mouseRegionY = io.MousePos.y - pos.y;
    // ImVec2   sp                                = ImGui::GetCursorPos();
    _cursorCoordinates.x = mouseRegionX;
    _cursorCoordinates.y = availableContentRegion.y - (-mouseRegionY) + 4; // hard coded due to some weird shit
    bool isReceivingDrop = false;
    if (ImGui::BeginDragDropTarget()) {
        isReceivingDrop = true;
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL")) {
            IM_ASSERT(payload->DataSize == sizeof(PglMaterial*));
            PrototypeSceneNode* droppedOnNode =
              onSceneViewClick({ _cursorCoordinates.x, _cursorCoordinates.y }, { _dimensions.x, _dimensions.y });
            if (droppedOnNode) {
                auto optObj = droppedOnNode->object();
                if (optObj.has_value()) {
                    auto obj = optObj.value();
                    if (obj->hasMeshRendererTrait()) {
                        PglMaterial*  material = *(PglMaterial**)payload->Data;
                        MeshRenderer* mr       = obj->getMeshRendererTrait();
                        for (size_t d = 0; d < mr->data().size(); ++d) { mr->data()[d].material = material->name; }
                        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                    }
                }
            }
        } else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MESH")) {
            IM_ASSERT(payload->DataSize == sizeof(PglGeometry*));
            PrototypeSceneNode* droppedOnNode =
              onSceneViewClick({ _cursorCoordinates.x, _cursorCoordinates.y }, { _dimensions.x, _dimensions.y });
            if (droppedOnNode) {
                auto optObj = droppedOnNode->object();
                if (optObj.has_value()) {
                    auto obj = optObj.value();
                    if (obj->hasMeshRendererTrait()) {
                        PglGeometry*  mesh = *(PglGeometry**)payload->Data;
                        MeshRenderer* mr   = obj->getMeshRendererTrait();
                        for (size_t d = 0; d < mr->data().size(); ++d) { mr->data()[d].mesh = mesh->name; }
                        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                    }
                }
            }
        } else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COLLIDER")) {
            IM_ASSERT(payload->DataSize == sizeof(PrototypeClipboard));
            PrototypeClipboard  clipboard = *(PrototypeClipboard*)payload->Data;
            PrototypeSceneNode* droppedOnNode =
              onSceneViewClick({ _cursorCoordinates.x, _cursorCoordinates.y }, { _dimensions.x, _dimensions.y });
            if (droppedOnNode) {
                auto optObj = droppedOnNode->object();
                if (optObj.has_value()) {
                    auto obj = optObj.value();
                    if (obj->hasColliderTrait()) {
                        PrototypeEngineInternalApplication::physics->updateCollider(obj, clipboard.data[0]);
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
        if (!isReceivingDrop && !_isUsingGuizmo && _isHovered && guizmoToolbarButtonsMask == 0) {
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                PrototypeSceneNode* node =
                  onSceneViewClick({ _cursorCoordinates.x, _cursorCoordinates.y }, { _dimensions.x, _dimensions.y });
                PrototypeEngineInternalApplication::scene->clearSelectedNodes();
                if (node) { node->select(); }
            }
        }
        _isHovered = true;
    } else {
        _isHovered = false;
    }

    _isUsingGuizmo            = false;
    const auto& selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
    if (!selectedNodes.empty()) {
        PrototypeSceneNode* selectedNode = *selectedNodes.begin();
        auto                optObject    = selectedNode->object();
        if (optObject.has_value()) {
            PrototypeObject* selectedObject = optObject.value();
            if (selectedObject->hasTransformTrait()) {
                Transform* tr = selectedObject->getTransformTrait();
                if (ImGuizmo::IsEnabled()) {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();
                    ImVec2 pos = ImGui::GetWindowPos();
                    ImVec2 siz = ImGui::GetWindowSize();
                    ImGuizmo::SetRect(pos.x, pos.y, siz.x, siz.y);

                    static glm::mat4 SelectedModelMatrix;
                    SelectedModelMatrix = tr->modelScaled();
                    ImGuizmo::Manipulate(&_camera->object->getCameraTrait()->viewMatrix()[0][0],
                                         &_camera->object->getCameraTrait()->projectionMatrix()[0][0],
                                         _guizmoOperation,
                                         _guizmoMode,
                                         &SelectedModelMatrix[0][0]);
                    if (ImGuizmo::IsOver()) { _isUsingGuizmo = true; }
                    if (ImGuizmo::IsUsing()) {
                        state |= PrototypeUIState_GuizmoUsed;
                        _isUsingGuizmo = true;
                        tr->setModelScaled(&SelectedModelMatrix[0][0]);
                        tr->updateComponentsFromMatrix();
                        if (_guizmoOperation == ImGuizmo::OPERATION::SCALE) {
                            PrototypeEngineInternalApplication::physics->scaleCollider(selectedObject, tr->scale());
                        } else {
                            tr->setNeedsPhysicsSync(true);
                        }
                    }
                }
            }
        }
    }

    ImGui::SetCursorPos(ImVec2(posx + 5.0f, posy + 5.0f));
    ImGui::Text("%s\n%.1f fps", _renderingSettings, ImGui::GetIO().Framerate);

    ImGui::PushFont((ImFont*)awesomeFontTitle);
    {
        pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(pos.x + ImGui::GetContentRegionAvailWidth() - (3 * 35.0f + 5.0f), posy + 5.0f));
        if (ImGui::Button(ICON_FA_ARROWS_ALT, ImVec2(30.0f, 30.0f))) {}
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
            _isHovered = false;
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                guizmoToolbarButtonsMask = TranslateButtonMask;
                setGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
            } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                guizmoToolbarButtonsMask &= ~TranslateButtonMask;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_SYNC_ALT, ImVec2(30.0f, 30.0f))) {}
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
            _isHovered = false;
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                guizmoToolbarButtonsMask = RotateButtonMask;
                setGuizmoOperation(ImGuizmo::OPERATION::ROTATE);
            } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                guizmoToolbarButtonsMask &= ~RotateButtonMask;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_EXPAND_ALT, ImVec2(30.0f, 30.0f))) {}
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
            _isHovered = false;
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                guizmoToolbarButtonsMask = ScaleButtonMask;
                setGuizmoOperation(ImGuizmo::OPERATION::SCALE);
            } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                guizmoToolbarButtonsMask &= ~ScaleButtonMask;
            }
        }
    }
    ImGui::PopFont();

    if (state & PrototypeUIState_SceneViewResized) {
        PglFramebufferResize(_camera->finalFramebuffer, _dimensions.x, _dimensions.y);
        PglFramebufferResize(_camera->postprocessingFramebuffer, _dimensions.x, _dimensions.y);
        PglFramebufferResize(_camera->gbufferFramebuffer, _dimensions.x, _dimensions.y);
        PglFramebufferResize(_camera->deferredFramebuffer, _dimensions.x, _dimensions.y);
        CameraSystemSetResolution(_camera->object->getCameraTrait(), _dimensions.x, _dimensions.y);
    }
    if (state & PrototypeUIState_HierarchyChanged) {
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
        PrototypeEngineInternalApplication::renderer->ui()->scheduleRecordPass(PrototypeUiViewMaskHierarchy);
#endif
    }

    return state;
}

void
PrototypeOpenglUiSceneView::setCamera(void* camera)
{
    _camera = static_cast<PglCamera*>(camera);
    PglFramebufferResize(_camera->finalFramebuffer, _dimensions.x, _dimensions.y);
    PglFramebufferResize(_camera->postprocessingFramebuffer, _dimensions.x, _dimensions.y);
    PglFramebufferResize(_camera->gbufferFramebuffer, _dimensions.x, _dimensions.y);
    PglFramebufferResize(_camera->deferredFramebuffer, _dimensions.x, _dimensions.y);
    CameraSystemSetResolution(_camera->object->getCameraTrait(), _dimensions.x, _dimensions.y);
}

void*
PrototypeOpenglUiSceneView::camera()
{
    return static_cast<void*>(_camera);
}

bool
PrototypeOpenglUiSceneView::isHovered()
{
    return _isHovered;
}

glm::vec2
PrototypeOpenglUiSceneView::dimensions()
{
    return _dimensions;
}

glm::vec2
PrototypeOpenglUiSceneView::cursorCoordinates()
{
    return _cursorCoordinates;
}

const float*
PrototypeOpenglUiSceneView::viewMatrix()
{
    return &_camera->object->getCameraTrait()->viewMatrix()[0][0];
}

const float*
PrototypeOpenglUiSceneView::projectionMatrix()
{
    return &_camera->object->getCameraTrait()->projectionMatrix()[0][0];
}

bool
PrototypeOpenglUiSceneView::onMouse(i32 button, i32 action, i32 mods)
{
    if (!_isHovered) return false;

    return false;
}

bool
PrototypeOpenglUiSceneView::onMouseMove(f64 xpos, f64 ypos)
{
    if (!_isHovered) return false;

    return false;
}

bool
PrototypeOpenglUiSceneView::onMouseDrag(i32 button, f64 xoffset, f64 yoffset)
{
    if (!_isHovered) return false;

    if (PrototypeEngineInternalApplication::window->isMouseDown().z) {
        CameraSystemRotate(_camera->object->getCameraTrait(), (f32)xoffset, (f32)yoffset);
    }
    return false;
}

bool
PrototypeOpenglUiSceneView::onMouseScroll(f64 xoffset, f64 yoffset)
{
    if (!_isHovered) return false;

    CameraSystemRotate(_camera->object->getCameraTrait(), (f32)xoffset, (f32)yoffset);
    return false;
}

bool
PrototypeOpenglUiSceneView::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || !_isHovered) {
        _input->releaseAllKeyboardKeys();
        return false;
    }

    _input->onKeyboard(key, scancode, action, mods);

    if (action == GLFW_PRESS) {
        if (PrototypeEngineInternalApplication::window->isShiftDown()) {
            if (key == GLFW_KEY_T) {
                setGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
            } else if (key == GLFW_KEY_R) {
                setGuizmoOperation(ImGuizmo::OPERATION::ROTATE);
            } else if (key == GLFW_KEY_S) {
                // TODO:
                // Scaling is not yet supported throughout from physical point of view
                // it will look scaled but won't physically act accordingly
                // keep that commented for the moment
                setGuizmoOperation(ImGuizmo::OPERATION::SCALE);
            }
        } else {
            if (key == GLFW_KEY_TAB) { toggleGuizmoMode(); }
        }

        // spawning stuff in scene
        {
            Camera*     cam                 = _camera->object->getCameraTrait();
            const auto& cameraPosition      = cam->position();
            const auto& cameraRotation      = cam->rotation();
            const auto& cameraResolution    = cam->resolution();
            const auto& camViewMatrix       = cam->viewMatrix();
            const auto& camProjectionMatrix = cam->projectionMatrix();

            glm::vec3 ray;
            PrototypeMaths::projectRayFromClipSpacePoint(
              ray, camViewMatrix, camProjectionMatrix, _cursorCoordinates.x, _cursorCoordinates.y, _dimensions.x, _dimensions.y);
            glm::vec3 pos   = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
            glm::vec3 rot   = { cameraRotation.x, cameraRotation.y, 0.0f };
            const f32 speed = 10.0f;
            ray.x *= speed;
            ray.y *= speed;
            ray.z *= speed;

            if (key == GLFW_KEY_1) {
                shortcutSpawnSphere(pos, rot, ray);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_2) {
                shortcutSpawnCube(pos, rot, ray);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_3) {
                shortcutSpawnConvexMesh(pos, rot, ray, "ico.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_4) {
                shortcutSpawnConvexMesh(pos, rot, ray, "monkey.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_5) {
                shortcutSpawnConvexMesh(pos, rot, ray, "capsule.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_6) {
                shortcutSpawnConvexMesh(pos, rot, ray, "cylinder.obj", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
            } else if (key == GLFW_KEY_7) {
                shortcutSpawnVehicle(pos, rot, ray, "CUBE", PROTOTYPE_DEFAULT_MATERIAL);
                PrototypeEngineInternalApplication::physics->spawnVehicle();
            }
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_G) {
            PrototypeEngineInternalApplication::physics->controlledVehiclesToggleGearDirection();
        } else if (key == GLFW_KEY_EQUAL) {
            PrototypeEngineInternalApplication::physics->requestNextVehicleAccessControl();
        } else if (key == GLFW_KEY_MINUS) {
            PrototypeEngineInternalApplication::physics->requestPreviousVehicleAccessControl();
        } else if (key == GLFW_KEY_0) {
            PrototypeEngineInternalApplication::physics->controlledVehiclesFlip();
        }
    }

    return false;
}

bool
PrototypeOpenglUiSceneView::onWindowResize(i32 width, i32 height)
{
    if (!_isHovered) return false;
    return false;
}

bool
PrototypeOpenglUiSceneView::onWindowDragDrop(i32 numFiles, const char** names)
{
    if (!_isHovered) return false;
    return false;
}

void
PrototypeOpenglUiSceneView::setGuizmoOperation(ImGuizmo::OPERATION operation)
{
    _guizmoOperation = operation;
}

ImGuizmo::OPERATION
PrototypeOpenglUiSceneView::guizmoOperation()
{
    return _guizmoOperation;
}

void
PrototypeOpenglUiSceneView::setGuizmoMode(ImGuizmo::MODE mode)
{
    _guizmoMode = mode;
}

void
PrototypeOpenglUiSceneView::toggleGuizmoMode()
{
    _guizmoMode = _guizmoMode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
}

ImGuizmo::MODE
PrototypeOpenglUiSceneView::guizmoMode()
{
    return _guizmoMode;
}

bool
PrototypeOpenglUiSceneView::isUsingGuizmo()
{
    return _isUsingGuizmo;
}

[[nodiscard]] PrototypeSceneNode*
PrototypeOpenglUiSceneView::onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size)
{
    float UVObjectIDPixel[4];

// We have that by convention
// 4th attachment is used to record the uvobjectid
#define UVObjectIDColorAttachmentId 4

    f32 u = (_cursorCoordinates.x / _dimensions.x) * _dimensions.x;
    f32 v = (1.0f - _cursorCoordinates.y / _dimensions.y) * _dimensions.y;

    glBindFramebuffer(GL_FRAMEBUFFER, _camera->deferredFramebuffer->fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + UVObjectIDColorAttachmentId);
    glReadPixels(u, v, 1, 1, GL_RGBA, GL_FLOAT, &UVObjectIDPixel);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    u32 objectId = UVObjectIDPixel[2];

    if (objectId == 0) { return nullptr; }
    auto meshRendererMap = PrototypeTraitSystem::meshRendererMap();
    auto meshRendererIt  = meshRendererMap.find(objectId);
    if (meshRendererIt != meshRendererMap.end()) {
        MeshRenderer*    mr     = PrototypeTraitSystem::meshRendererVector()[meshRendererIt->second];
        PrototypeObject* object = mr->object();
        if (object) {
            if (object->parentNode()) {
                auto node = static_cast<PrototypeSceneNode*>(object->parentNode());
                return node;
            }
        }
    }
    return nullptr;
}