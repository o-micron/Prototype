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

#include "PrototypeVulkanUiSceneView.h"

#include "../core/PrototypeCameraSystem.h"
#include "../core/PrototypeEngine.h"
#include "../core/PrototypeInput.h"
#include "../core/PrototypePhysics.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"
#include "../core/PrototypeShortcuts.h"
#include "PrototypeVulkanRenderer.h"
#include "PrototypeVulkanWindow.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

PrototypeVulkanUiSceneView::PrototypeVulkanUiSceneView()
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

PrototypeVulkanUiSceneView::~PrototypeVulkanUiSceneView() { delete _input; }

void
PrototypeVulkanUiSceneView::onInit()
{}

void
PrototypeVulkanUiSceneView::onUpdate()
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
}

PrototypeUIState_
PrototypeVulkanUiSceneView::onDrawFrame(PrototypeUIState_ state, void* awesomeFontTitle)
{
    ImVec2 availableContentRegion = ImGui::GetContentRegionAvail();
    // glm::vec2 resolution             = PrototypeEngineInternalApplication::window->resolution();
    // if ((i32)availableContentRegion.x != resolution.x || (i32)availableContentRegion.y != resolution.y) {
    //     state |= PrototypeUIState_SceneViewResized;
    // }
    _dimensions = { availableContentRegion.x, availableContentRegion.y };
    ImGui::Image((void*)(intptr_t)0, availableContentRegion, ImVec2(0, 1), ImVec2(1, 0));
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

                    Camera* cam = _camera->object->getCameraTrait();

                    static glm::mat4 SelectedModelMatrix;
                    SelectedModelMatrix = tr->modelScaled();
                    ImGuizmo::Manipulate(&cam->viewMatrix()[0][0],
                                         &cam->projectionMatrix()[0][0],
                                         _guizmoOperation,
                                         _guizmoMode,
                                         &SelectedModelMatrix[0][0]);
                    if (ImGuizmo::IsOver()) {
                        _isUsingGuizmo = true;
                    } else {
                        _isUsingGuizmo = false;
                    }
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
                    } else {
                        _isUsingGuizmo = false;
                    }
                }
            }
        };
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
        ImGuiIO& io           = ImGui::GetIO();
        ImVec2   pos          = ImGui::GetCursorScreenPos();
        float    mouseRegionX = io.MousePos.x - pos.x;
        float    mouseRegionY = io.MousePos.y - pos.y;
        // ImVec2   sp                                = ImGui::GetCursorPos();
        _cursorCoordinates.x = mouseRegionX;
        _cursorCoordinates.y = availableContentRegion.y - (-mouseRegionY) + 4; // hard coded due to some weird shit
        _isHovered           = true;
        static PrototypeSceneNode* hitSelectedNode = nullptr;
        if (!_isUsingGuizmo) {
            if (ImGui::IsMouseClicked(0)) {
                hitSelectedNode =
                  onSceneViewClick({ _cursorCoordinates.x, _cursorCoordinates.y }, { _dimensions.x, _dimensions.y });
            } else if (ImGui::IsMouseReleased(0)) {
                PrototypeSceneNode* node =
                  onSceneViewClick({ _cursorCoordinates.x, _cursorCoordinates.y }, { _dimensions.x, _dimensions.y });
                if (node == hitSelectedNode) {
                    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
                    if (node) { node->select(); }
                } else {
                    if (node == nullptr) { PrototypeEngineInternalApplication::scene->clearSelectedNodes(); }
                }
                hitSelectedNode = nullptr;
            }
            if (hitSelectedNode) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_LIGHTERGRAY, 1.0f));
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    ImGui::SetDragDropPayload("_TREENODE", &hitSelectedNode, sizeof(PrototypeSceneNode*));
                    ImGui::Text("%s", hitSelectedNode->name().c_str());
                    ImGui::EndDragDropSource();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
            }
        }
    } else {
        _isHovered = false;
    }
    return state;
}

void
PrototypeVulkanUiSceneView::setCamera(void* camera)
{
    _camera = static_cast<PvtCamera*>(camera);
}

void*
PrototypeVulkanUiSceneView::camera()
{
    return static_cast<void*>(_camera);
}

bool
PrototypeVulkanUiSceneView::isHovered()
{
    return _isHovered;
}

glm::vec2
PrototypeVulkanUiSceneView::dimensions()
{
    return _dimensions;
}

glm::vec2
PrototypeVulkanUiSceneView::cursorCoordinates()
{
    return _cursorCoordinates;
}

const float*
PrototypeVulkanUiSceneView::viewMatrix()
{
    return &_camera->object->getCameraTrait()->viewMatrix()[0][0];
}

const float*
PrototypeVulkanUiSceneView::projectionMatrix()
{
    return &_camera->object->getCameraTrait()->projectionMatrix()[0][0];
}

bool
PrototypeVulkanUiSceneView::onMouse(i32 button, i32 action, i32 mods)
{
    if (!_isHovered) return false;

    return false;
}

bool
PrototypeVulkanUiSceneView::onMouseMove(f64 xpos, f64 ypos)
{
    if (!_isHovered) return false;

    return false;
}

bool
PrototypeVulkanUiSceneView::onMouseDrag(i32 button, f64 xoffset, f64 yoffset)
{
    if (!_isHovered) return false;

    if (PrototypeEngineInternalApplication::window->isMouseDown().z) {
        CameraSystemRotate(_camera->object->getCameraTrait(), (f32)xoffset, (f32)yoffset);
    }
    return false;
}

bool
PrototypeVulkanUiSceneView::onMouseScroll(f64 xoffset, f64 yoffset)
{
    if (!_isHovered) return false;

    CameraSystemRotate(_camera->object->getCameraTrait(), (f32)xoffset, (f32)yoffset);
    return false;
}

bool
PrototypeVulkanUiSceneView::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
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
PrototypeVulkanUiSceneView::onWindowResize(i32 width, i32 height)
{
    if (!_isHovered) return false;

    return false;
}

bool
PrototypeVulkanUiSceneView::onWindowDragDrop(i32 numFiles, const char** names)
{
    if (!_isHovered) return false;

    return false;
}

void
PrototypeVulkanUiSceneView::setGuizmoOperation(ImGuizmo::OPERATION operation)
{
    _guizmoOperation = operation;
}

ImGuizmo::OPERATION
PrototypeVulkanUiSceneView::guizmoOperation()
{
    return _guizmoOperation;
}

void
PrototypeVulkanUiSceneView::setGuizmoMode(ImGuizmo::MODE mode)
{
    _guizmoMode = mode;
}

void
PrototypeVulkanUiSceneView::toggleGuizmoMode()
{
    _guizmoMode = _guizmoMode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
}

ImGuizmo::MODE
PrototypeVulkanUiSceneView::guizmoMode()
{
    return _guizmoMode;
}

bool
PrototypeVulkanUiSceneView::isUsingGuizmo()
{
    return _isUsingGuizmo;
}

[[nodiscard]] PrototypeSceneNode*
PrototypeVulkanUiSceneView::onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size)
{
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    Camera*     cam                 = _camera->object->getCameraTrait();
    const auto& camPosition         = cam->position();
    const auto& camViewMatrix       = cam->viewMatrix();
    const auto& camProjectionMatrix = cam->projectionMatrix();

    glm::vec3 ray;
    PrototypeMaths::projectRayFromClipSpacePoint(
      ray, camViewMatrix, camProjectionMatrix, coordinates.x, coordinates.y, Size.x, Size.y);

    auto optHit =
      PrototypeEngineInternalApplication::physics->raycast({ camPosition.x, camPosition.y, camPosition.z }, ray, cam->zfar());
    if (optHit.has_value()) {
        auto hit = optHit.value();
        if (hit && hit->parentNode()) {
            auto node = static_cast<PrototypeSceneNode*>(hit->parentNode());
            return node;
        }
    }
    return nullptr;
}