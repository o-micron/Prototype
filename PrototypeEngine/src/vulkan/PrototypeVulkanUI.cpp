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

#include "PrototypeVulkanUI.h"

#include "../core/PrototypeCameraSystem.h"
#include "../core/PrototypeDatabase.h"
#include "../core/PrototypePhysics.h"
#include "../core/PrototypePipelines.h"
#include "../core/PrototypeShortcuts.h"
#include "PrototypeVulkanRenderer.h"
#include "PrototypeVulkanWindow.h"

#include "../core/PrototypeProfiler.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"

#include "../core/PrototypeEngine.h"
#include "../core/PrototypeWindow.h"

#include <PrototypeCommon/Logger.h>
#include <PrototypeCommon/Maths.h>
#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <inttypes.h>

static void
onCameraEditDispatch(PrototypeObject* o)
{
    // view
    ImGui::NewLine();
    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth;
    if (ImGui::CollapsingHeader(PrototypeTraitTypeAbsoluteStringCamera, node_flags)) {
        char buff[64];
        snprintf(buff, sizeof(buff), "##%s properties menu context " PRIu64, PrototypeTraitTypeAbsoluteStringCamera, o->id());
        if (ImGui::BeginPopupContextItem(buff)) {
            if (ImGui::Selectable("Delete " PrototypeTraitTypeAbsoluteStringCamera " trait")) {
                PrototypePipelineQueue queue = {};
                auto cmd       = std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                cmd->object    = o;
                cmd->traitMask = PrototypeTraitTypeMaskCamera;
                queue.record(std::move(cmd));
                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
            }
            ImGui::EndPopup();
        }
        Camera*          cam = o->getCameraTrait();
        static glm::vec3 translation;
        translation = cam->position();
        PrototypeImguiVec3Widget(
          "Translation",
          20.0f,
          if (ImGui::DragFloat("##camera position x", &translation.x, 0.1f, -1000.0f, 1000.0f)) {
              CameraSystemSetTranslation(cam, translation);
          },
          if (ImGui::DragFloat("##camera position y", &translation.y, 0.1f, -1000.0f, 1000.0f)) {
              CameraSystemSetTranslation(cam, translation);
          },
          if (ImGui::DragFloat("##camera position z", &translation.z, 0.1f, -1000.0f, 1000.0f)) {
              CameraSystemSetTranslation(cam, translation);
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y",
          "Z");
        static glm::vec2 rotation;
        rotation = cam->rotation();
        PrototypeImguiVec2Widget(
          "Rotation",
          20.0f,
          if (ImGui::DragFloat("##camera rotation x", &rotation.x, 0.1f, -80.0f, 80.0f)) {
              CameraSystemSetRotation(cam, rotation);
          },
          if (ImGui::DragFloat("##camera rotation y", &rotation.y, 0.1f, 0.0f, 360.0f)) {
              CameraSystemSetRotation(cam, rotation);
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y");
        if (ImGui::DragFloat("Look Sensitivity", &cam->lookSensitivityMut(), 0.001f, 0.001f, 10.0f)) {}
        if (ImGui::DragFloat("Move Sensitivity", &cam->moveSensitivityMut(), 0.001f, 0.001f, 10.0f)) {}
        if (ImGui::DragFloat("Field of view", &cam->fovMut(), 0.01f, 5.0f, 120.0f)) {}
        if (ImGui::DragFloat("Near plane", &cam->nearMut(), 0.01f, 0.001f, 100.0f)) {}
        if (ImGui::DragFloat("Far plane", &cam->farMut(), 0.01f, 0.1f, 10000.0f)) {}
        if (ImGui::Checkbox("Orbital", &cam->orbitalMut())) {}

        PvtCamera& mainCamera = ((PrototypeVulkanRenderer*)PrototypeEngineInternalApplication::renderer)->pvtMainCamera();
        // // deferred stage
        // {
        //     ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //     ImGui::InputText("Deferred stage framebuffer",
        //                      (char*)mainCamera.deferredFramebuffer->name.c_str(),
        //                      mainCamera.deferredFramebuffer->name.size());
        //     ImGui::PopItemFlag();
        //     if (ImGui::BeginDragDropTarget()) {
        //         if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FRAMEBUFFER")) {
        //             IM_ASSERT(payload->DataSize == sizeof(PglFramebuffer*));
        //             auto framebuffer               = *(PglFramebuffer**)payload->Data;
        //             mainCamera.deferredFramebuffer = framebuffer;
        //             PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //         }
        //         ImGui::EndDragDropTarget();
        //     }
        //     for (auto& pair : mainCamera.deferredFramebuffer->shader->floatData) {
        //         ImGui::DragFloat(pair.first.c_str(), &pair.second);
        //     }
        //     for (auto& pair : mainCamera.deferredFramebuffer->shader->vec2Data) {
        //         ImGui::DragFloat2(pair.first.c_str(), &pair.second[0]);
        //     }
        //     for (auto& pair : mainCamera.deferredFramebuffer->shader->vec3Data) {
        //         ImGui::DragFloat3(pair.first.c_str(), &pair.second[0]);
        //     }
        //     for (auto& pair : mainCamera.deferredFramebuffer->shader->vec4Data) {
        //         ImGui::DragFloat4(pair.first.c_str(), &pair.second[0]);
        //     }
        // }
        // // gbuffer stage
        // {
        //     ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //     ImGui::InputText("GBuffer stage framebuffer",
        //                      (char*)mainCamera.gbufferFramebuffer->name.c_str(),
        //                      mainCamera.gbufferFramebuffer->name.size());
        //     ImGui::PopItemFlag();
        //     if (ImGui::BeginDragDropTarget()) {
        //         if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FRAMEBUFFER")) {
        //             IM_ASSERT(payload->DataSize == sizeof(PglFramebuffer*));
        //             auto framebuffer              = *(PglFramebuffer**)payload->Data;
        //             mainCamera.gbufferFramebuffer = framebuffer;
        //             PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //         }
        //         ImGui::EndDragDropTarget();
        //     }
        //     for (auto& pair : mainCamera.gbufferFramebuffer->shader->floatData) {
        //         ImGui::DragFloat(pair.first.c_str(), &pair.second);
        //     }
        //     for (auto& pair : mainCamera.gbufferFramebuffer->shader->vec2Data) {
        //         ImGui::DragFloat2(pair.first.c_str(), &pair.second[0]);
        //     }
        //     for (auto& pair : mainCamera.gbufferFramebuffer->shader->vec3Data) {
        //         ImGui::DragFloat3(pair.first.c_str(), &pair.second[0]);
        //     }
        //     for (auto& pair : mainCamera.gbufferFramebuffer->shader->vec4Data) {
        //         ImGui::DragFloat4(pair.first.c_str(), &pair.second[0]);
        //     }
        // }
        // // post processing stage
        // {
        //     ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //     ImGui::InputText("Post-processing stage framebuffer",
        //                      (char*)mainCamera.postprocessingFramebuffer->name.c_str(),
        //                      mainCamera.postprocessingFramebuffer->name.size());
        //     ImGui::PopItemFlag();
        //     if (ImGui::BeginDragDropTarget()) {
        //         if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FRAMEBUFFER")) {
        //             IM_ASSERT(payload->DataSize == sizeof(PglFramebuffer*));
        //             auto framebuffer                     = *(PglFramebuffer**)payload->Data;
        //             mainCamera.postprocessingFramebuffer = framebuffer;
        //             PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //         }
        //         ImGui::EndDragDropTarget();
        //     }
        //     for (auto& pair : mainCamera.postprocessingFramebuffer->shader->floatData) {
        //         ImGui::DragFloat(pair.first.c_str(), &pair.second);
        //     }
        //     for (auto& pair : mainCamera.postprocessingFramebuffer->shader->vec2Data) {
        //         ImGui::DragFloat2(pair.first.c_str(), &pair.second[0]);
        //     }
        //     for (auto& pair : mainCamera.postprocessingFramebuffer->shader->vec3Data) {
        //         ImGui::DragFloat3(pair.first.c_str(), &pair.second[0]);
        //     }
        //     for (auto& pair : mainCamera.postprocessingFramebuffer->shader->vec4Data) {
        //         ImGui::DragFloat4(pair.first.c_str(), &pair.second[0]);
        //     }
        // }
        ImGui::Separator();
    }
}

static void
onColliderEditDispatch(PrototypeObject* o)
{
    ImGui::NewLine();
    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet;
    if (ImGui::CollapsingHeader(PrototypeTraitTypeAbsoluteStringCollider, node_flags)) {
        char buff[64];
        snprintf(buff, sizeof(buff), "##%s properties menu context " PRIu64, PrototypeTraitTypeAbsoluteStringCollider, o->id());
        if (ImGui::BeginPopupContextItem(buff)) {
            if (ImGui::Selectable("Delete " PrototypeTraitTypeAbsoluteStringCollider " trait")) {
                PrototypePipelineQueue queue = {};
                auto cmd       = std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                cmd->object    = o;
                cmd->traitMask = PrototypeTraitTypeMaskCollider;
                queue.record(std::move(cmd));
                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
            }
            ImGui::EndPopup();
        }
        Collider* collider = o->getColliderTrait();
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText("Shape", (char*)collider->nameRef().c_str(), collider->nameRef().size());
            ImGui::PopItemFlag();
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COLLIDER")) {
                    IM_ASSERT(payload->DataSize == sizeof(std::string**));
                    std::string* data = *(std::string**)payload->Data;
                    PrototypeEngineInternalApplication::physics->updateCollider(o, *data);
                }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::Separator();
    }
}

static void
onMeshRendererEditDispatch(PrototypeObject* o)
{
    ImGui::NewLine();
    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet;
    if (ImGui::CollapsingHeader(PrototypeTraitTypeAbsoluteStringMeshRenderer, node_flags)) {
        char buff[64];
        snprintf(
          buff, sizeof(buff), "##%s properties menu context " PRIu64, PrototypeTraitTypeAbsoluteStringMeshRenderer, o->id());
        if (ImGui::BeginPopupContextItem(buff)) {
            if (ImGui::Selectable("Delete " PrototypeTraitTypeAbsoluteStringMeshRenderer " trait")) {
                PrototypePipelineQueue queue = {};
                auto cmd       = std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                cmd->object    = o;
                cmd->traitMask = PrototypeTraitTypeMaskMeshRenderer;
                queue.record(std::move(cmd));
                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
            }
            ImGui::EndPopup();
        }
        MeshRenderer* mr = o->getMeshRendererTrait();
        // {
        //     ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //     ImGui::InputText("Material", (char*)mr->material().c_str(), mr->material().size());
        //     ImGui::PopItemFlag();
        //     if (ImGui::BeginDragDropTarget()) {
        //         if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL")) {
        //             IM_ASSERT(payload->DataSize == sizeof(PglMaterial*));
        //             PglMaterial* material = *(PglMaterial**)payload->Data;
        //             mr->setMaterial(material->name);
        //             PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //         }
        //         ImGui::EndDragDropTarget();
        //     }
        //     ImGui::Indent();
        //     {
        //         PglMaterial* material;
        //         PrototypeEngineInternalApplication::renderer->fetchMaterial(mr->material(), (void**)&material);

        //         // shader
        //         {
        //             ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //             ImGui::InputText("Shader", (char*)material->shader->name.c_str(), material->shader->name.size());
        //             ImGui::PopItemFlag();
        //             if (ImGui::BeginDragDropTarget()) {
        //                 if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SHADER")) {
        //                     IM_ASSERT(payload->DataSize == sizeof(PglShader*));
        //                     PglShader* shader       = *(PglShader**)payload->Data;
        //                     size_t     originalSize = material->textureData.size();
        //                     material->shader        = shader;
        //                     ((PrototypeOpenglRenderer*)PrototypeEngineInternalApplication::renderer)
        //                       ->onMaterialShaderUpdate(material, shader);
        //                     material->textures.resize(material->textureData.size());
        //                     material->textures.shrink_to_fit();
        //                     for (size_t s = originalSize; s < material->textures.size(); ++s) {
        //                         PrototypeEngineInternalApplication::renderer->fetchDefaultTexture((void**)&material->textures[s]);
        //                     }
        //                     PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //                 }
        //                 ImGui::EndDragDropTarget();
        //             }
        //         }

        //         // textures
        //         {
        //             for (size_t t = 0; t < material->textures.size(); ++t) {
        //                 ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //                 ImGui::InputText(material->textureData[t].c_str(),
        //                                  (char*)material->textures[t]->name.c_str(),
        //                                  material->textures[t]->name.size());
        //                 ImGui::PopItemFlag();
        //                 if (ImGui::BeginDragDropTarget()) {
        //                     if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE")) {
        //                         IM_ASSERT(payload->DataSize == sizeof(PglTexture*));
        //                         PglTexture* texture   = *(PglTexture**)payload->Data;
        //                         material->textures[t] = texture;
        //                         PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //                     }
        //                     ImGui::EndDragDropTarget();
        //                 }
        //             }
        //         }

        //         // values
        //         {
        //             for (auto& pair : material->floatData) { ImGui::InputFloat(pair.first.c_str(), &pair.second); }
        //             for (auto& pair : material->vec2Data) { ImGui::InputFloat2(pair.first.c_str(), &pair.second[0]); }
        //             for (auto& pair : material->vec3Data) { ImGui::InputFloat3(pair.first.c_str(), &pair.second[0]); }
        //             for (auto& pair : material->vec4Data) { ImGui::InputFloat4(pair.first.c_str(), &pair.second[0]); }
        //         }
        //     }
        //     ImGui::Unindent();
        // }
        // {
        //     ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        //     ImGui::InputText("Mesh", (char*)mr->mesh().c_str(), mr->mesh().size());
        //     ImGui::PopItemFlag();
        //     if (ImGui::BeginDragDropTarget()) {
        //         if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MESH")) {
        //             IM_ASSERT(payload->DataSize == sizeof(PglGeometry*));
        //             PglGeometry* mesh = *(PglGeometry**)payload->Data;
        //             mr->setMesh(mesh->name);
        //             PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
        //         }
        //         ImGui::EndDragDropTarget();
        //     }
        // }
        ImGui::Separator();
    }
}

static void
onRigidbodyEditDispatch(PrototypeObject* o)
{
    ImGui::NewLine();
    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet;
    if (ImGui::CollapsingHeader(PrototypeTraitTypeAbsoluteStringRigidbody, node_flags)) {
        char buff[64];
        snprintf(buff, sizeof(buff), "##%s properties menu context " PRIu64, PrototypeTraitTypeAbsoluteStringRigidbody, o->id());
        if (ImGui::BeginPopupContextItem(buff)) {
            if (ImGui::Selectable("Delete " PrototypeTraitTypeAbsoluteStringRigidbody " trait")) {
                PrototypePipelineQueue queue = {};
                auto cmd       = std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                cmd->object    = o;
                cmd->traitMask = PrototypeTraitTypeMaskRigidbody;
                queue.record(std::move(cmd));
                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
            }
            ImGui::EndPopup();
        }
        Rigidbody* rb = o->getRigidbodyTrait();

        static glm::vec3 linearVelocity;
        linearVelocity = rb->linearVelocity();
        PrototypeImguiVec3Widget(
          "Linear velocity",
          20.0f,
          if (ImGui::DragFloat("##linear velocity x", &linearVelocity.x, 0.1f, -1000.0f, 1000.0f)) {
              rb->setLinearVelocity(linearVelocity);
              PrototypeEngineInternalApplication::physics->updateRigidbodyLinearVelocity(o);
          },
          if (ImGui::DragFloat("##linear velocity y", &linearVelocity.y, 0.1f, -1000.0f, 1000.0f)) {
              rb->setLinearVelocity(linearVelocity);
              PrototypeEngineInternalApplication::physics->updateRigidbodyLinearVelocity(o);
          },
          if (ImGui::DragFloat("##linear velocity z", &linearVelocity.z, 0.1f, -1000.0f, 1000.0f)) {
              rb->setLinearVelocity(linearVelocity);
              PrototypeEngineInternalApplication::physics->updateRigidbodyLinearVelocity(o);
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y",
          "Z");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        static f32 linearDamping;
        linearDamping = rb->linearDamping();
        ImGui::Text("Linear damping");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
        if (ImGui::DragFloat("##linear damping", &linearDamping, 0.1f, -1000.0f, 1000.0f)) {
            rb->setLinearDamping(linearDamping);
            PrototypeEngineInternalApplication::physics->updateRigidbodyLinearDamping(o);
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        static glm::vec3 angularVelocity;
        angularVelocity = rb->angularVelocity();
        PrototypeImguiVec3Widget(
          "Angular velocity",
          20.0f,
          if (ImGui::DragFloat("##angular velocity x", &angularVelocity.x, 0.1f, -1000.0f, 1000.0f)) {
              rb->setAngularVelocity(angularVelocity);
              PrototypeEngineInternalApplication::physics->updateRigidbodyAngularVelocity(o);
          },
          if (ImGui::DragFloat("##angular velocity y", &angularVelocity.y, 0.1f, -1000.0f, 1000.0f)) {
              rb->setAngularVelocity(angularVelocity);
              PrototypeEngineInternalApplication::physics->updateRigidbodyAngularVelocity(o);
          },
          if (ImGui::DragFloat("##angular velocity z", &angularVelocity.z, 0.1f, -1000.0f, 1000.0f)) {
              rb->setAngularVelocity(angularVelocity);
              PrototypeEngineInternalApplication::physics->updateRigidbodyAngularVelocity(o);
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y",
          "Z");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        static f32 angularDamping;
        angularDamping = rb->angularDamping();
        ImGui::Text("Angular damping");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
        if (ImGui::DragFloat("##angular damping", &angularDamping, 0.1f, -1000.0f, 1000.0f)) {
            rb->setAngularDamping(angularDamping);
            PrototypeEngineInternalApplication::physics->updateRigidbodyAngularDamping(o);
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        static f32 mass;
        mass = rb->mass();
        ImGui::Text("Mass");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
        if (ImGui::DragFloat("##mass", &mass, 0.1f, 0.0f, 1000.0f)) {
            rb->setMass(mass);
            PrototypeEngineInternalApplication::physics->updateRigidbodyMass(o);
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        ImGuiWindow* window = ImGui::GetCurrentWindow();

        // Lock Linear
        {
            static bool linearLocked = false;
            PrototypeImguiVec3Widget("Lock Position",
                                     20.0f,
                                     if (ImGui::Checkbox("##Lock linear X", &rb->lockLinearXMut())) linearLocked = true,
                                     if (ImGui::Checkbox("##Lock linear Y", &rb->lockLinearYMut())) linearLocked = true,
                                     if (ImGui::Checkbox("##Lock linear Z", &rb->lockLinearZMut())) linearLocked = true,
                                     30.0f,
                                     0.0f,
                                     -2.0f,
                                     "X",
                                     "Y",
                                     "Z");
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            if (linearLocked) { PrototypeEngineInternalApplication::physics->updateRigidbodyLockLinear(o); }
        }

        // Lock Angles
        {
            static bool angularLocked = false;
            PrototypeImguiVec3Widget("Lock Rotation",
                                     20.0f,
                                     if (ImGui::Checkbox("##Lock angular X", &rb->lockAngularXMut())) angularLocked = true,
                                     if (ImGui::Checkbox("##Lock angular Y", &rb->lockAngularYMut())) angularLocked = true,
                                     if (ImGui::Checkbox("##Lock angular Z", &rb->lockAngularZMut())) angularLocked = true,
                                     30.0f,
                                     0.0f,
                                     -2.0f,
                                     "X",
                                     "Y",
                                     "Z");
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            if (angularLocked) { PrototypeEngineInternalApplication::physics->updateRigidbodyLockAngular(o); }
        }
        ImGui::Separator();
    }
}

static void
onTransformEditDispatch(PrototypeObject* o)
{
    ImGui::NewLine();
    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet;
    if (ImGui::CollapsingHeader(PrototypeTraitTypeAbsoluteStringTransform, node_flags)) {
        char buff[64];
        snprintf(buff, sizeof(buff), "##%s properties menu context " PRIu64, PrototypeTraitTypeAbsoluteStringTransform, o->id());
        if (ImGui::BeginPopupContextItem(buff)) {
            if (ImGui::Selectable("Delete " PrototypeTraitTypeAbsoluteStringTransform " trait")) {
                PrototypePipelineQueue queue = {};
                auto cmd       = std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                cmd->object    = o;
                cmd->traitMask = PrototypeTraitTypeMaskTransform;
                queue.record(std::move(cmd));
                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
            }
            ImGui::EndPopup();
        }
        static glm::vec3 position, rotation, scale;
        Transform*       tr = o->getTransformTrait();
        position            = tr->position();
        rotation            = tr->rotation();
        scale               = tr->scale();

        PrototypeImguiVec3Widget(
          "Position",
          20.0f,
          if (ImGui::DragFloat("##position x", &position.x, 0.1f, -1000.0f, 1000.0f)) {
              ImGuizmo::Enable(false);
              glm::mat4 model;
              PrototypeMaths::buildModelMatrix(model, position, tr->rotation());
              PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
              tr->setModelScaled(&model[0][0]);
              tr->updateComponentsFromMatrix();
              if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
              ImGuizmo::Enable(true);
          },
          if (ImGui::DragFloat("##position y", &position.y, 0.1f, -1000.0f, 1000.0f)) {
              ImGuizmo::Enable(false);
              glm::mat4 model;
              PrototypeMaths::buildModelMatrix(model, position, tr->rotation());
              PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
              tr->setModelScaled(&model[0][0]);
              tr->updateComponentsFromMatrix();
              if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
              ImGuizmo::Enable(true);
          },
          if (ImGui::DragFloat("##position z", &position.z, 0.1f, -1000.0f, 1000.0f)) {
              ImGuizmo::Enable(false);
              glm::mat4 model;
              PrototypeMaths::buildModelMatrix(model, position, tr->rotation());
              PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
              tr->setModelScaled(&model[0][0]);
              tr->updateComponentsFromMatrix();
              if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
              ImGuizmo::Enable(true);
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y",
          "Z");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        PrototypeImguiVec3Widget(
          "Rotation",
          20.0f,
          if (ImGui::DragFloat("##rotation x", &rotation.x, 0.1f, -360.0f, 360.0f)) {
              ImGuizmo::Enable(false);
              glm::mat4 model;
              PrototypeMaths::buildModelMatrix(model, tr->position(), rotation);
              PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
              tr->setModelScaled(&model[0][0]);
              tr->updateComponentsFromMatrix();
              if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
              ImGuizmo::Enable(true);
          },
          if (ImGui::DragFloat("##rotation y", &rotation.y, 0.1f, -360.0f, 360.0f)) {
              ImGuizmo::Enable(false);
              glm::mat4 model;
              PrototypeMaths::buildModelMatrix(model, tr->position(), rotation);
              PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
              tr->setModelScaled(&model[0][0]);
              tr->updateComponentsFromMatrix();
              if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
              ImGuizmo::Enable(true);
          },
          if (ImGui::DragFloat("##rotation z", &rotation.z, 0.1f, -360.0f, 360.0f)) {
              ImGuizmo::Enable(false);
              glm::mat4 model;
              PrototypeMaths::buildModelMatrix(model, tr->position(), rotation);
              PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
              tr->setModelScaled(&model[0][0]);
              tr->updateComponentsFromMatrix();
              if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
              ImGuizmo::Enable(true);
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y",
          "Z");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        PrototypeImguiVec3Widget(
          "Scale",
          20.0f,
          if (ImGui::DragFloat("##scale x", &scale.x, 0.1f, 0.01f, 100.0f)) {
              if (!(scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)) {
                  ImGuizmo::Enable(false);
                  glm::mat4 model;
                  PrototypeMaths::buildModelMatrix(model, tr->position(), tr->rotation());
                  tr->setModel(&model[0][0]);
                  if (o->hasColliderTrait()) { PrototypeEngineInternalApplication::physics->scaleCollider(o, scale); }
                  PrototypeMaths::buildModelMatrixWithScale(model, scale);
                  tr->setModelScaled(&model[0][0]);
                  tr->updateComponentsFromMatrix();
                  ImGuizmo::Enable(true);
              }
          },
          if (ImGui::DragFloat("##scale y", &scale.y, 0.1f, 0.01f, 100.0f)) {
              if (!(scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)) {
                  ImGuizmo::Enable(false);
                  glm::mat4 model;
                  PrototypeMaths::buildModelMatrix(model, tr->position(), tr->rotation());
                  tr->setModel(&model[0][0]);
                  if (o->hasColliderTrait()) { PrototypeEngineInternalApplication::physics->scaleCollider(o, scale); }
                  PrototypeMaths::buildModelMatrixWithScale(model, scale);
                  tr->setModelScaled(&model[0][0]);
                  tr->updateComponentsFromMatrix();
                  ImGuizmo::Enable(true);
              }
          },
          if (ImGui::DragFloat("##scale z", &scale.z, 0.1f, 0.01f, 100.0f)) {
              if (!(scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)) {
                  ImGuizmo::Enable(false);
                  glm::mat4 model;
                  PrototypeMaths::buildModelMatrix(model, tr->position(), tr->rotation());
                  tr->setModel(&model[0][0]);
                  if (o->hasColliderTrait()) { PrototypeEngineInternalApplication::physics->scaleCollider(o, scale); }
                  PrototypeMaths::buildModelMatrixWithScale(model, scale);
                  tr->setModelScaled(&model[0][0]);
                  tr->updateComponentsFromMatrix();
                  ImGuizmo::Enable(true);
              }
          },
          70.0f,
          0.0f,
          -2.0f,
          "X",
          "Y",
          "Z");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
        ImGui::Separator();
    }
}

static void
drawSceneNode(PrototypeSceneNode* node, const std::unordered_set<PrototypeSceneNode*>& selectedNodes)
{
    static ImGuiTreeNodeFlags base_flags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGuiTreeNodeFlags node_flags = base_flags;

    char buffid[4];
    if (selectedNodes.find(node) != selectedNodes.end()) { node_flags |= ImGuiTreeNodeFlags_Selected; }
    snprintf(buffid, sizeof(buffid), "%lu", node->id());
    bool node_open = ImGui::TreeNodeEx((void*)node, node_flags, "%s", node->name().c_str());
    if (ImGui::IsItemClicked()) {
        PrototypeEngineInternalApplication::scene->clearSelectedNodes();
        node->select();
    }
    if (node_open) {
        for (auto childPair : node->nodes()) {
            drawSceneNode(childPair.second, PrototypeEngineInternalApplication::scene->selectedNodes());
        }
        ImGui::TreePop();
    }
}

PrototypeVulkanUI::PrototypeVulkanUI()
  : _isBuffersChanged(false)
  , _defaultFont(nullptr)
  , _awesomeFont(nullptr)
  , _renderPass(nullptr)
  , _swapChainRebuild(false)
  , _freezeResizingViews(false)
  , _sceneView({})
{}

bool
PrototypeVulkanUI::init()
{
    auto renderer = (PrototypeVulkanRenderer*)(PrototypeEngineInternalApplication::renderer);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    (void)io;
    io.IniFilename = nullptr;

    // default font
    {
        _defaultFont = io.Fonts->AddFontFromFileTTF(PROTOTYPE_FONT_PATH("RobotoMono-Bold.ttf"), 18);
    }

    // awesome font
    {
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig         icons_config;
        icons_config.MergeMode  = true;
        icons_config.PixelSnapH = true;
        _awesomeFont            = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 10.0f, &icons_config, icons_ranges);
    }

    _wd.Frames = (ImGui_ImplVulkanH_Frame*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_Frame) * renderer->_swapchain.images.size());
    _wd.FrameSemaphores = (ImGui_ImplVulkanH_FrameSemaphores*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameSemaphores) *
                                                                       renderer->_swapchain.images.size());

    createVulkanData();

    // bridge imgui
    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)PrototypeEngineInternalApplication::window->handle(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                  = renderer->_instance;
    init_info.PhysicalDevice            = renderer->_physicalDevice;
    init_info.Device                    = renderer->_device;
    init_info.QueueFamily               = renderer->_queueFamilies.indices.graphics;
    init_info.Queue                     = renderer->_queueFamilies.queues.graphics;
    init_info.PipelineCache             = nullptr;
    init_info.DescriptorPool            = _descriptor.pool;
    init_info.Allocator                 = nullptr;
    init_info.MinImageCount             = MAX_FRAMES_IN_FLIGHT;
    init_info.ImageCount                = renderer->_swapchain.images.size();
    init_info.CheckVkResultFn           = VK_CHECK;
    ImGui_ImplVulkan_Init(&init_info, _renderPass);

    // upload fonts
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool                 = _command.pool;
        allocInfo.commandBufferCount          = 1;
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(renderer->_device, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        {
            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        }
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submitInfo{};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;
        vkQueueSubmit(renderer->_queueFamilies.queues.graphics, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(renderer->_queueFamilies.queues.graphics);
        vkFreeCommandBuffers(renderer->_device, _command.pool, 1, &commandBuffer);
    }

    {
        _swapChainRebuild = false;
        // ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);
        // ImGui_ImplVulkanH_CreateOrResizeWindow(renderer->_instance,
        //                                        renderer->_physicalDevice,
        //                                        renderer->_device,
        //                                        &_wd,
        //                                        renderer->_queueFamilies.indices.graphics,
        //                                        nullptr,
        //                                        renderer->_window->resolution().x,
        //                                        renderer->_window->resolution().y,
        //                                        MAX_FRAMES_IN_FLIGHT);
    }

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.AntiAliasedLines       = false;
    style.AntiAliasedLinesUseTex = false;
    style.AntiAliasedFill        = false;
    style.CurveTessellationTol   = 0.1f;
    style.CircleSegmentMaxError  = 10.0f;

    style.ChildRounding     = 0.f;
    style.GrabRounding      = 0.f;
    style.WindowRounding    = 0.f;
    style.ScrollbarRounding = 0.f;
    style.FrameRounding     = 5.f;
    style.TabRounding       = 5.f;
    style.TabBorderSize     = 0.0f;
    style.WindowTitleAlign  = ImVec2(0.01f, 0.5f);

    style.Colors[ImGuiCol_Text]         = ImVec4(PROTOTYPE_WHITE, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(PROTOTYPE_LIGHTESTGRAY, 1.00f);
    style.Colors[ImGuiCol_WindowBg]     = ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f);
    style.Colors[ImGuiCol_ChildBg]      = ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f);
    style.Colors[ImGuiCol_PopupBg]      = ImVec4(PROTOTYPE_DARKESTGRAY, 0.00f);
    // style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.76f, 0.26f, 0.26f, 1.00f);
    // style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.26f, 0.76f, 0.26f, 1.00f);
    // style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.26f, 0.26f, 0.76f, 1.00f);
    style.Colors[ImGuiCol_Border]             = ImVec4(PROTOTYPE_DARKGRAY, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]       = ImVec4(PROTOTYPE_DARKGRAY, 0.00f);
    style.Colors[ImGuiCol_FrameBg]            = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]     = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]      = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    style.Colors[ImGuiCol_TitleBg]            = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]      = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]          = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_Tab]                = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]       = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_TabHovered]         = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    style.Colors[ImGuiCol_TabActive]          = ImVec4(PROTOTYPE_DARKESTGRAY, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(PROTOTYPE_DARKESTGRAY, 1.00f);
    // style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    // style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    // style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    // style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    // style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(PROTOTYPE_GRAY, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.193f, 0.193f, 0.193f, 1.000f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.314f, 0.315f, 0.317f, 1.000f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_Header]               = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(PROTOTYPE_GRAY, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(PROTOTYPE_GRAY, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(PROTOTYPE_GRAY, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(PROTOTYPE_LIGHTESTGRAY, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(PROTOTYPE_GRAY, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(PROTOTYPE_LIGHTESTGRAY, 1.00f);
    // style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
    // style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

    // snprintf(renderingSettings,
    //          sizeof(renderingSettings),
    //          "%s\nOpenGL: %i.%i - GLSL: %s",
    //          glGetString(GL_RENDERER),
    //          GL_MAJOR,
    //          GL_MINOR,
    //          GLSL_VERSION.substr(9, GLSL_VERSION.length() - 9).c_str());

    // register traits onEdits
    Camera::setOnEditDispatchHandler(onCameraEditDispatch);
    Collider::setOnEditDispatchHandler(onColliderEditDispatch);
    MeshRenderer::setOnEditDispatchHandler(onMeshRendererEditDispatch);
    Rigidbody::setOnEditDispatchHandler(onRigidbodyEditDispatch);
    Transform::setOnEditDispatchHandler(onTransformEditDispatch);

    // {
    //     PglTexture* playBtnTexture;
    //     PrototypeEngineInternalApplication::renderer->fetchTexture("icons/play.png", (void**)&playBtnTexture);
    //     if (playBtnTexture) {
    //         _playBtnImg = playBtnTexture->id;
    //     } else {
    //         _playBtnImg = 0;
    //     }
    // }
    // {
    //     PglTexture* pauseBtnTexture;
    //     PrototypeEngineInternalApplication::renderer->fetchTexture("icons/pause.png", (void**)&pauseBtnTexture);
    //     if (pauseBtnTexture) {
    //         _pauseBtnImg = pauseBtnTexture->id;
    //     } else {
    //         _pauseBtnImg = 0;
    //     }
    // }
    // {
    //     PglTexture* reloadBtnTexture;
    //     PrototypeEngineInternalApplication::renderer->fetchTexture("icons/reload.png", (void**)&reloadBtnTexture);
    //     if (reloadBtnTexture) {
    //         _reloadBtnImg = reloadBtnTexture->id;
    //     } else {
    //         _reloadBtnImg = 0;
    //     }
    // }
    // {
    //     PglTexture* importBtnTexture;
    //     PrototypeEngineInternalApplication::renderer->fetchTexture("icons/import.png", (void**)&importBtnTexture);
    //     if (importBtnTexture) {
    //         _importBtnImg = importBtnTexture->id;
    //     } else {
    //         _importBtnImg = 0;
    //     }
    // }
    // {
    //     PglTexture* exportBtnTexture;
    //     PrototypeEngineInternalApplication::renderer->fetchTexture("icons/export.png", (void**)&exportBtnTexture);
    //     if (exportBtnTexture) {
    //         _exportBtnImg = exportBtnTexture->id;
    //     } else {
    //         _exportBtnImg = 0;
    //     }
    // }

    _sceneView.onInit();

    return true;
}

void
PrototypeVulkanUI::deInit()
{
    destroyVulkanData();

    auto renderer = (PrototypeVulkanRenderer*)(PrototypeEngineInternalApplication::renderer);
    for (uint32_t i = 0; i < renderer->_swapchain.images.size(); i++) {
        vkDestroyFence(renderer->_device, _wd.Frames[i].Fence, nullptr);

        if (_wd.Frames[i].CommandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(renderer->_device, _wd.Frames[i].CommandPool, nullptr);
        }
        if (_wd.FrameSemaphores[i].ImageAcquiredSemaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(renderer->_device, _wd.FrameSemaphores[i].ImageAcquiredSemaphore, nullptr);
        }
        if (_wd.FrameSemaphores[i].RenderCompleteSemaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(renderer->_device, _wd.FrameSemaphores[i].RenderCompleteSemaphore, nullptr);
        }
    }
    IM_FREE(_wd.Frames);
    IM_FREE(_wd.FrameSemaphores);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void
PrototypeVulkanUI::scheduleRecordPass(PrototypeUiViewMaskType mask)
{}

void
PrototypeVulkanUI::beginRecordPass()
{}

void
PrototypeVulkanUI::endRecordPass()
{}

void
PrototypeVulkanUI::beginFrame(bool changed)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

PrototypeUIState_
PrototypeVulkanUI::drawFrame(u32 fbid, i32 width, i32 height)
{
    PrototypeUIState_ state = PrototypeUIState_None;

    ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (ImGui::Begin("Main DockSpace", (bool*)0, mainWindowFlags)) {
        {
            if (ImGui::BeginChild("Toolbar",
                                  ImVec2(ImGui::GetContentRegionAvailWidth(), 35.0f),
                                  false,
                                  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
                ImGui::Dummy(ImVec2(0.0f, 3.5f));
                ImGui::Dummy(ImVec2(5.0f, 0.0f));
                ImGui::SameLine();
                f32 posx = ImGui::GetCursorPosX();
                if (ImGui::Button("import", ImVec2(25.0f, 25.0f))) {}
                if (ImGui::IsItemHovered()) {
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_YELLOW, 1.0f));
                    ImGui::SetTooltip("import scene");
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(posx + 25.0f);
                if (ImGui::Button("export", ImVec2(25.0f, 25.0f))) {
                    PrototypeEngineInternalApplication::window->setNeedsInspector();
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_YELLOW, 1.0f));
                    ImGui::SetTooltip("export scene");
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                }

                ImGui::SameLine();
                posx = ImGui::GetContentRegionAvailWidth();
                ImGui::SetCursorPosX(posx + 5.0f);
                static bool isPlaying = PrototypeEngineInternalApplication::physics->isPlaying();
                if (ImGui::Button((isPlaying ? "pause" : "play"), ImVec2(25.0f, 25.0f))) {
                    if (isPlaying) {
                        PrototypeEngineInternalApplication::physics->pause();
                    } else {
                        PrototypeEngineInternalApplication::physics->play();
                    }
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_YELLOW, 1.0f));
                    ImGui::SetTooltip("play/pause physics simulation");
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                }

                ImGui::SameLine();
                ImGui::SetCursorPosX(posx + 25.0f + 5.0f);
                if (!_isBuffersChanged) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    ImGui::Button("reload", ImVec2(25.0f, 25.0f));
                    ImGui::PopStyleVar();
                    ImGui::PopItemFlag();
                } else {
                    if (ImGui::Button("reload", ImVec2(25.0f, 25.0f))) {
                        _isBuffersChanged = false;
                        // for (const auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
                        // pair.second->dispatchGpuUpload(); } for (const auto& pair :
                        // PrototypeEngineInternalApplication::database->shaderBuffers) { pair.second->dispatchGpuUpload(); } for
                        // (const auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) {
                        // pair.second->dispatchGpuUpload(); }
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_YELLOW, 1.0f));
                        ImGui::SetTooltip("Reload");
                        ImGui::PopStyleColor();
                        ImGui::PopStyleVar();
                    }
                }

                ImGui::PopStyleVar();
            }
            ImGui::EndChild();
        }
        if (ImGui::DockBuilderGetNode(ImGui::GetID("TheDockspace")) == NULL || _freezeResizingViews) {
            ImGuiID dockspace_id = ImGui::GetID("TheDockspace");
            ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
            ImGui::DockBuilderAddNode(dockspace_id,
                                      ImGuiDockNodeFlags_DockSpace); // Add empty node

            ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it
                                                 // here as we aren't docking anything into it.

            // Default Layout
            // ------------------------------------------------------
            // |        |                               |           |
            // |        |                               |           |
            // |        |                               |           |
            // |   E    |            G                  |           |
            // |        |                               |           |
            // |        |                               |     A     |
            // |        |                               |           |
            // |----------------------------------------|           |
            // |                  C                     |           |
            // |                                        |           |
            // ------------------------------------------------------
            {
                ImGuiID dock_id_a, dock_id_b, dock_id_c, dock_id_d, dock_id_e, dock_id_f, dock_id_g;
                ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, &dock_id_a, &dock_id_b);
                ImGui::DockBuilderSplitNode(dock_id_b, ImGuiDir_Down, 0.25f, &dock_id_c, &dock_id_d);
                ImGui::DockBuilderSplitNode(dock_id_d, ImGuiDir_Left, 0.25f, &dock_id_e, &dock_id_f);
                ImGui::DockBuilderSplitNode(dock_id_f, ImGuiDir_Right, 0.25f, NULL, &dock_id_g);
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_main_id);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_a);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_b);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_c);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_d);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_e);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }
                {
                    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_f);
                    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                }

                ImGui::DockBuilderDockWindow("Scene View", dock_id_g);
                ImGui::DockBuilderDockWindow("Texture Editor", dock_id_g);
                ImGui::DockBuilderDockWindow("3D Painting", dock_id_g);
                ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_e);
                ImGui::DockBuilderDockWindow("Properties", dock_id_a);
                ImGui::DockBuilderDockWindow("Rendering Information", dock_id_c);
                ImGui::DockBuilderDockWindow("Profiler", dock_id_c);
                ImGui::DockBuilderDockWindow("Colliders", dock_id_c);
                ImGui::DockBuilderDockWindow("Materials", dock_id_c);
                ImGui::DockBuilderDockWindow("Textures", dock_id_c);
                ImGui::DockBuilderDockWindow("Shaders", dock_id_c);
                ImGui::DockBuilderDockWindow("Meshes", dock_id_c);
                ImGui::DockBuilderDockWindow("Scenes", dock_id_c);
                ImGui::DockBuilderDockWindow("Framebuffers", dock_id_c);
            }

            ImGui::DockBuilderFinish(dockspace_id);
            _freezeResizingViews = false;
        }

        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(PROTOTYPE_DARKERGRAY, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGuiID dockspace_id = ImGui::GetID("TheDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
    ImGui::End();
    ImGui::PopStyleVar(4);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    {
        if (ImGui::Begin("Scene Hierarchy", (bool*)0, windowFlags)) {
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            // table
            {
                const float TEXT_BASE_WIDTH  = ImGui::CalcTextSize("A").x;
                const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

                static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

                if (ImGui::BeginTable("##scene table", 1, flags)) {
                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                    ImGui::TableSetupColumn(PrototypeEngineInternalApplication::scene->name().c_str(), 0);
                    ImGui::TableHeadersRow();

                    // Simple storage to output a dummy file-system.
                    struct MyTreeNode
                    {
                        static void DisplayNodeDetails(PrototypeSceneNode* node, PrototypeUIState_& state)
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_YELLOW, 1.0f));
                            if (ImGui::BeginPopupContextItem()) {
                                if (ImGui::Selectable("Add child node")) {
                                    PrototypePipelineQueue queue = {};
                                    auto cmd = std::make_unique<PrototypePipelineCommand_shortcutEditorAddSceneNodeToNode>();
                                    cmd->parentNode = node;
                                    queue.record(std::move(cmd));
                                    PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                }
                                if (ImGui::Selectable("Delete")) {
                                    PrototypePipelineQueue queue = {};
                                    auto cmd  = std::make_unique<PrototypePipelineCommand_shortcutEditorRemoveSceneNode>();
                                    cmd->node = node;
                                    queue.record(std::move(cmd));
                                    PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                }
                                for (size_t itrait = 0; itrait < PrototypeTraitTypeCount; ++itrait) {
                                    if (ImGui::Selectable(
                                          ("Add " + PrototypeTraitTypeAbsoluteStringArray[itrait] + " trait").c_str())) {
                                        PrototypePipelineQueue queue = {};
                                        auto                   cmd =
                                          std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeAddTraits>();
                                        cmd->object    = node->object().value();
                                        cmd->traitMask = (1 << itrait);
                                        queue.record(std::move(cmd));
                                        PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                    }
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::PopStyleColor();
                            ImGui::PopStyleVar();
                            if (ImGui::IsItemClicked()) {
                                PrototypeEngineInternalApplication::scene->clearSelectedNodes();
                                node->select();
                            }
                            if (ImGui::BeginDragDropSource()) {
                                ImGui::SetDragDropPayload("_TREENODE", &node, sizeof(PrototypeSceneNode*));
                                ImGui::Text("%s", node->name().c_str());
                                ImGui::EndDragDropSource();
                            }
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                                    IM_ASSERT(payload->DataSize == sizeof(PrototypeSceneNode*));
                                    PrototypeSceneNode* payloadNode   = *(PrototypeSceneNode**)payload->Data;
                                    PrototypeSceneNode* oldParentNode = payloadNode->parentNode();
                                    if (oldParentNode) {
                                        oldParentNode->moveNode(payloadNode, node);
                                        state |= PrototypeUIState_HierarchyChanged;
                                    } else {
                                        PrototypeSceneLayer* oldParentLayer = payloadNode->parentLayer();
                                        if (oldParentLayer) {
                                            oldParentLayer->moveNode(payloadNode, node);
                                            state |= PrototypeUIState_HierarchyChanged;
                                        }
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }
                        }
                        static void DisplayNode(PrototypeSceneNode* node, PrototypeUIState_& state)
                        {
                            ImGuiTreeNodeFlags nodeTreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
                                                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                                                   ImGuiTreeNodeFlags_SpanFullWidth;
                            const auto& nodes         = node->nodes();
                            const auto& selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
                            if (nodes.empty()) { nodeTreeNodeFlags |= ImGuiTreeNodeFlags_Leaf; }
                            if (selectedNodes.find(node) != selectedNodes.end()) {
                                nodeTreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
                            }
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            bool open = ImGui::TreeNodeEx(node->name().c_str(), nodeTreeNodeFlags);
                            DisplayNodeDetails(node, state);
                            if (open) {
                                for (const auto& pair : nodes) { MyTreeNode::DisplayNode(pair.second, state); }
                                ImGui::TreePop();
                            }
                        }
                        static void DisplayLayerDetails(PrototypeSceneLayer* layer, PrototypeUIState_& state)
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_YELLOW, 1.0f));
                            if (ImGui::BeginPopupContextItem()) {
                                if (ImGui::Selectable("Add child node")) {
                                    PrototypePipelineQueue queue = {};
                                    auto cmd = std::make_unique<PrototypePipelineCommand_shortcutEditorAddSceneNodeToLayer>();
                                    cmd->parentLayer = layer;
                                    queue.record(std::move(cmd));
                                    PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::PopStyleColor();
                            ImGui::PopStyleVar();
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                                    IM_ASSERT(payload->DataSize == sizeof(PrototypeSceneNode*));
                                    PrototypeSceneNode* payloadNode   = *(PrototypeSceneNode**)payload->Data;
                                    PrototypeSceneNode* oldParentNode = payloadNode->parentNode();
                                    if (oldParentNode) {
                                        oldParentNode->moveNode(payloadNode, layer);
                                        state |= PrototypeUIState_HierarchyChanged;
                                    } else {
                                        PrototypeSceneLayer* oldParentLayer = payloadNode->parentLayer();
                                        if (oldParentLayer) { oldParentLayer->moveNode(payloadNode, layer); }
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }
                        }
                        static void DisplayLayer(PrototypeSceneLayer* layer, PrototypeUIState_& state)
                        {
                            ImGuiTreeNodeFlags layerTreeNodeFlags =
                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                              ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
                            const auto& nodes = layer->nodes();
                            if (nodes.empty()) { layerTreeNodeFlags |= ImGuiTreeNodeFlags_Leaf; }

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            bool open = ImGui::TreeNodeEx(layer->name().c_str(), layerTreeNodeFlags);
                            DisplayLayerDetails(layer, state);
                            if (open) {
                                for (const auto& pair : nodes) { MyTreeNode::DisplayNode(pair.second, state); }
                                ImGui::TreePop();
                            }
                        }
                    };
                    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(PROTOTYPE_GREEN, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(PROTOTYPE_GREEN, 1.00f));
                    const auto& layers = PrototypeEngineInternalApplication::scene->layers();
                    for (const auto& pair : layers) { MyTreeNode::DisplayLayer(pair.second, state); }
                    ImGui::PopStyleColor(2);
                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Scene View", (bool*)0, windowFlags)) {
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            state = _sceneView.onDrawFrame(state, _awesomeFont);
        } else {
            state |= PrototypeUIState_Iconified;
        }
        ImGui::End();

        if (ImGui::Begin("Texture Editor", (bool*)0, windowFlags)) {
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
        }
        ImGui::End();

        if (ImGui::Begin("3D Painting", (bool*)0, windowFlags)) {
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
        }
        ImGui::End();

        if (ImGui::Begin("Profiler", (bool*)0, windowFlags)) {
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            // ImVec2 available = ImGui::GetContentRegionAvail();
            // ImVec2 graphSize(available.x, (available.y / ProfilerKey_Count) - (ProfilerKey_Count - 1));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
            {
                // struct ProfilerKeyIdentifier
                // {
                //     const char* name;
                //     ImVec4      color;
                // };
                // for (size_t k = 0; k < ProfilerKey_Count; ++k) {
                //     const auto& data       = PrototypeEngineInternalApplication::profiler->values((ProfilerKey_)k);
                //     const auto& identifier = identifiers[k];
                //     ImGui::PushStyleColor(ImGuiCol_Border, identifier.color);
                //     ImGui::PushStyleColor(ImGuiCol_PlotHistogram, identifier.color);
                //     ImGui::PlotHistogram("##values",
                //                          data.data(),
                //                          data.size(),
                //                          0,
                //                          identifier.name,
                //                          PROTOTYPE_PROFILER_BUFFER_HEIGHT_MIN,
                //                          PROTOTYPE_PROFILER_BUFFER_HEIGHT_MAX,
                //                          graphSize);
                //     ImGui::PopStyleColor(2);
                // }
            }
            ImGui::PopStyleVar();
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    if (ImGui::Begin("Properties", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        const auto& selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
        if (!selectedNodes.empty()) {
            PrototypeSceneNode* first = *selectedNodes.begin();
            static char         nodeNameBuffer[256];
            snprintf(nodeNameBuffer, sizeof(nodeNameBuffer), "%s", first->name().c_str());
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputText("##Node Name", nodeNameBuffer, sizeof(nodeNameBuffer))) {
                first->renameNode(std::string(nodeNameBuffer));
            }
            auto optObj = first->object();
            if (optObj.has_value()) {
                auto obj = optObj.value();
                ImGui::NewLine();
                obj->onEdit();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Scenes", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->scenes) { ImGui::Text("%s", pair.first.c_str()); }
    }
    ImGui::End();

    if (ImGui::Begin("Meshes", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
            ImGui::Text("%s", pair.first.c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // PglGeometry* mesh;
                // PrototypeEngineInternalApplication::renderer->fetchMesh(pair.first, (void**)&mesh);
                // ImGui::SetDragDropPayload("_MESH", &mesh, sizeof(PglGeometry*));
                // ImGui::Text("%s", pair.first.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Shaders", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->shaderBuffers) {
            ImGui::Text("%s", pair.first.c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // PglShader* shader;
                // PrototypeEngineInternalApplication::renderer->fetchShader(pair.first, (void**)&shader);
                // ImGui::SetDragDropPayload("_SHADER", &shader, sizeof(PglShader*));
                // ImGui::Text("%s", pair.first.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Textures", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) {
            ImGui::Text("%s", pair.first.c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // PglTexture* texture;
                // PrototypeEngineInternalApplication::renderer->fetchTexture(pair.first, (void**)&texture);
                // ImGui::SetDragDropPayload("_TEXTURE", &texture, sizeof(PglTexture*));
                // ImGui::Text("%s", pair.first.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Materials", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->materials) {
            ImGui::Text("%s", pair.first.c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // PglMaterial* material;
                // PrototypeEngineInternalApplication::renderer->fetchMaterial(pair.first, (void**)&material);
                // ImGui::SetDragDropPayload("_MATERIAL", &material, sizeof(PglMaterial*));
                // ImGui::Text("%s", pair.first.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Framebuffers", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->framebuffers) {
            ImGui::Text("%s", pair.first.c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // PglFramebuffer* framebuffer;
                // PrototypeEngineInternalApplication::renderer->fetchFramebuffer(pair.first, (void**)&framebuffer);
                // ImGui::SetDragDropPayload("_FRAMEBUFFER", &framebuffer, sizeof(PglFramebuffer*));
                // ImGui::Text("%s", pair.first.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Colliders", (bool*)0, windowFlags)) {
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        static const char* plane = "PLANE";
        ImGui::Text("%s", plane);
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            // ImGui::SetDragDropPayload("_COLLIDER", &plane[0], sizeof(std::string*));
            // ImGui::Text("%s", plane);
            ImGui::EndDragDropSource();
        }
        static const char* cube = "CUBE";
        ImGui::Text("%s", cube);
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            // ImGui::SetDragDropPayload("_COLLIDER", &cube[0], sizeof(std::string*));
            // ImGui::Text("%s", cube);
            ImGui::EndDragDropSource();
        }
        static const char* sphere = "SPHERE";
        ImGui::Text("%s", sphere);
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            // ImGui::SetDragDropPayload("_COLLIDER", &sphere[0], sizeof(std::string*));
            // ImGui::Text("%s", sphere);
            ImGui::EndDragDropSource();
        }
        static std::vector<std::string> meshBuffers;
        meshBuffers.resize(PrototypeEngineInternalApplication::database->meshBuffers.size() * 2 + 1);
        size_t              mbci = 0;
        static std::string* draggedCollider;
        for (const auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
            meshBuffers[mbci] = std::string("(CONVEX) ").append(pair.first);
            ImGui::Text("%s", meshBuffers[mbci].c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // draggedCollider = &meshBuffers[mbci];
                // ImGui::SetDragDropPayload("_COLLIDER", &draggedCollider, sizeof(std::string**));
                // ImGui::Text("%s", meshBuffers[mbci].c_str());
                ImGui::EndDragDropSource();
            }
            ++mbci;
            meshBuffers[mbci] = std::string("(TRIMESH) ").append(pair.first);
            ImGui::Text("%s", meshBuffers[mbci].c_str());
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // draggedCollider = &meshBuffers[mbci];
                // ImGui::SetDragDropPayload("_COLLIDER", &draggedCollider, sizeof(std::string**));
                // ImGui::Text("%s", meshBuffers[mbci].c_str());
                ImGui::EndDragDropSource();
            }
            ++mbci;
        }
    }
    ImGui::End();

    if (ImGui::Begin("Rendering Information", (bool*)0, windowFlags)) {
        // auto t1 = std::chrono::high_resolution_clock::now();
        {
            ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
            if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        }
        // char infoText[512];
        // snprintf(infoText, sizeof(infoText), "%s\n%.1f fps", renderingSettings, ImGui::GetIO().Framerate);
        // ImGui::Text("%s", infoText);
        // ImGui::Text("%s", fmt::format("{}\n{} FPS", renderingSettings, ImGui::GetIO().Framerate).c_str());
        // auto t2       = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        // PrototypeEngineInternalApplication::profiler->mark(ProfilerKey_TEMPORARY, (f32)duration);
    }
    ImGui::End();

    // draw shader erros
    {
        if (!_errorDialogs.empty()) {
            ImVec2 window_pos       = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
            ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGui::SetNextWindowBgAlpha(1.0f);
            if (ImGui::Begin(_errorDialogs.top().title().c_str(), (bool*)0, 0)) {
                ImGui::Text("%s", _errorDialogs.top().text().c_str());
            }
            ImGui::End();
        }
    }

    return state;
}

void
PrototypeVulkanUI::endFrame()
{
    // ImGui::EndFrame();
}

void
PrototypeVulkanUI::update()
{
    _sceneView.onUpdate();
}

void
PrototypeVulkanUI::render(i32 x, i32 y, i32 width, i32 height)
{
    ImGui::Render();
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    memcpy(&_wd.ClearValue.color.float32[0], clearColor, 4 * sizeof(f32));
}

void
PrototypeVulkanUI::pushErrorDialog(PrototypeErrorDialog errDialog)
{
    _errorDialogs.push(errDialog);
}

void
PrototypeVulkanUI::popErrorDialog()
{
    _errorDialogs.pop();
}

void
PrototypeVulkanUI::signalBuffersChanged(bool status)
{
    _isBuffersChanged = status;
}

bool
PrototypeVulkanUI::isBuffersChanged()
{
    return _isBuffersChanged;
}

bool
PrototypeVulkanUI::needsMouse()
{
    return false;
}

bool
PrototypeVulkanUI::needsKeyboard()
{
    return false;
}

PrototypeUiViewMaskType
PrototypeVulkanUI::openedViewsMask()
{
    return 0;
}

PrototypeUiView*
PrototypeVulkanUI::sceneView()
{
    return &_sceneView;
}

void
PrototypeVulkanUI::frameRender()
{
    auto                     renderer = (PrototypeVulkanRenderer*)(PrototypeEngineInternalApplication::renderer);
    ImGui_ImplVulkanH_Frame* fd       = &_wd.Frames[_wd.SemaphoreIndex];
    vkWaitForFences(renderer->_device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);

    VkSemaphore image_acquired_semaphore  = _wd.FrameSemaphores[_wd.SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = _wd.FrameSemaphores[_wd.SemaphoreIndex].RenderCompleteSemaphore;

    VkResult result = vkAcquireNextImageKHR(
      renderer->_device, _wd.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &_wd.FrameIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateVulkanData();
        return;
    }

    fd = &_wd.Frames[_wd.FrameIndex];
    VK_CHECK(vkWaitForFences(renderer->_device, 1, &fd->Fence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(renderer->_device, 1, &fd->Fence));

    {
        VK_CHECK(vkResetCommandPool(renderer->_device, fd->CommandPool, 0));
        VkCommandBufferBeginInfo info = {};
        info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(fd->CommandBuffer, &info));
    }
    {
        VkRenderPassBeginInfo info    = {};
        info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass               = _wd.RenderPass;
        info.framebuffer              = fd->Framebuffer;
        info.renderArea.extent.width  = _wd.Width;
        info.renderArea.extent.height = _wd.Height;
        info.clearValueCount          = 1;
        info.pClearValues             = &_wd.ClearValue;
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImDrawData* mainDrawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(mainDrawData, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    VK_CHECK(vkEndCommandBuffer(fd->CommandBuffer));
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo         info       = {};
        info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount         = 1;
        info.pWaitSemaphores            = &image_acquired_semaphore;
        info.pWaitDstStageMask          = &wait_stage;
        info.commandBufferCount         = 1;
        info.pCommandBuffers            = &fd->CommandBuffer;
        info.signalSemaphoreCount       = 1;
        info.pSignalSemaphores          = &render_complete_semaphore;
        VK_CHECK(vkQueueSubmit(renderer->_queueFamilies.queues.graphics, 1, &info, fd->Fence));
    }
}

void
PrototypeVulkanUI::framePresent()
{
    if (_swapChainRebuild) return;
    auto             renderer                  = (PrototypeVulkanRenderer*)(PrototypeEngineInternalApplication::renderer);
    VkSemaphore      render_complete_semaphore = _wd.FrameSemaphores[_wd.SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info                      = {};
    info.sType                                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount                    = 1;
    info.pWaitSemaphores                       = &render_complete_semaphore;
    info.swapchainCount                        = 1;
    info.pSwapchains                           = &_wd.Swapchain;
    info.pImageIndices                         = &_wd.FrameIndex;
    VkResult err                               = vkQueuePresentKHR(renderer->_queueFamilies.queues.presentation, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        _swapChainRebuild = true;
        destroyVulkanData();
        createVulkanData();
        return;
    }
    VK_CHECK(err);
    _wd.SemaphoreIndex = (_wd.SemaphoreIndex + 1) % MAX_FRAMES_IN_FLIGHT; // Now we can use the next set of semaphores
    // _wd.FrameIndex     = (_wd.FrameIndex + 1) % _wd.ImageCount;
}

void
PrototypeVulkanUI::createVulkanData()
{
    auto renderer  = (PrototypeVulkanRenderer*)(PrototypeEngineInternalApplication::renderer);
    _wd.FrameIndex = 0;

    // create a specific render pass for imgui
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format                  = renderer->_swapchain.format.format;
        colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
        // imgui drawn over main rendered image (LOADED IMAGE), so, don't clear !
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment            = 0;
        colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorAttachmentRef;

        std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };

        VkSubpassDependency dependency = {};
        dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass          = 0;
        dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask       = 0;
        dependency.dstStageMask        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount        = (u32)attachments.size();
        renderPassCreateInfo.pAttachments           = attachments.data();
        renderPassCreateInfo.subpassCount           = 1;
        renderPassCreateInfo.pSubpasses             = &subpass;
        renderPassCreateInfo.dependencyCount        = 1;
        renderPassCreateInfo.pDependencies          = &dependency;

        VK_CHECK(vkCreateRenderPass(renderer->_device, &renderPassCreateInfo, nullptr, &_renderPass));
    }

    // create a specific descriptor pool for imgui
    {
        std::vector<VkDescriptorPoolSize> poolSizes = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                                                        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

        VkDescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.poolSizeCount              = (u32)poolSizes.size();
        poolCreateInfo.pPoolSizes                 = poolSizes.data();
        poolCreateInfo.maxSets                    = (u32)(renderer->_swapchain.images.size());

        VK_CHECK(vkCreateDescriptorPool(renderer->_device, &poolCreateInfo, nullptr, &_descriptor.pool));
    }

    // create pool
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex        = (u32)renderer->_queueFamilies.indices.graphics;
        commandPoolCreateInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        VK_CHECK(vkCreateCommandPool(renderer->_device, &commandPoolCreateInfo, nullptr, &_command.pool));
    }

    // create command buffers
    {
        _command.buffers.resize(renderer->_swapchain.framebuffers.size());
        VkCommandBufferAllocateInfo allocCreateInfo = {};
        allocCreateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocCreateInfo.commandPool                 = _command.pool;
        allocCreateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocCreateInfo.commandBufferCount          = (u32)_command.buffers.size();
        vkAllocateCommandBuffers(renderer->_device, &allocCreateInfo, _command.buffers.data());
    }

    _wd.Width         = renderer->_swapchain.extent.width;
    _wd.Height        = renderer->_swapchain.extent.height;
    _wd.Swapchain     = renderer->_swapchain.swapchain;
    _wd.RenderPass    = _renderPass;
    _wd.Surface       = renderer->_surface;
    _wd.SurfaceFormat = renderer->_swapchain.format;
    _wd.PresentMode   = renderer->_swapchain.presentMode;
    _wd.ImageCount    = renderer->_swapchain.images.size();
    _wd.Pipeline      = renderer->_graphicsPipeline.pipeline;
    _wd.FrameIndex    = 0;

    // create framebuffers for imgui
    {
        VkFramebufferCreateInfo info = {};
        info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass              = _wd.RenderPass;
        info.attachmentCount         = 1;
        info.width                   = _wd.Width;
        info.height                  = _wd.Height;
        info.layers                  = 1;
        for (uint32_t i = 0; i < renderer->_swapchain.images.size(); i++) {
            ImGui_ImplVulkanH_Frame* fd = &_wd.Frames[i];
            fd->Backbuffer              = renderer->_swapchain.images[i];
            fd->BackbufferView          = renderer->_swapchain.imageViews[i];
            info.pAttachments           = &fd->BackbufferView;
            fd->CommandPool             = _command.pool;
            fd->CommandBuffer           = _command.buffers[i];
            VK_CHECK(vkCreateFramebuffer(renderer->_device, &info, nullptr, &fd->Framebuffer));
            // semaphores
            {
                ImGui_ImplVulkanH_FrameSemaphores* fd                  = &_wd.FrameSemaphores[i];
                VkSemaphoreCreateInfo              semaphoreCreateInfo = {};
                semaphoreCreateInfo.sType                              = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                VK_CHECK(vkCreateSemaphore(renderer->_device, &semaphoreCreateInfo, nullptr, &fd->ImageAcquiredSemaphore));
                VK_CHECK(vkCreateSemaphore(renderer->_device, &semaphoreCreateInfo, nullptr, &fd->RenderCompleteSemaphore));
            }
            // fences
            {
                ImGui_ImplVulkanH_Frame* fd              = &_wd.Frames[i];
                VkFenceCreateInfo        fenceCreateInfo = {};
                fenceCreateInfo.sType                    = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                fenceCreateInfo.flags                    = VK_FENCE_CREATE_SIGNALED_BIT;
                VK_CHECK(vkCreateFence(renderer->_device, &fenceCreateInfo, nullptr, &fd->Fence));
            }
        }
    }
}

void
PrototypeVulkanUI::recreateVulkanData()
{
    destroyVulkanData();
    createVulkanData();
}

void
PrototypeVulkanUI::destroyVulkanData()
{
    auto renderer = (PrototypeVulkanRenderer*)(PrototypeEngineInternalApplication::renderer);

    VK_CHECK(vkDeviceWaitIdle(renderer->_device));

    for (uint32_t i = 0; i < renderer->_swapchain.images.size(); i++) {
        if (_wd.Frames[i].BackbufferView != VK_NULL_HANDLE) {
            vkDestroyImageView(renderer->_device, _wd.Frames[i].BackbufferView, nullptr);
        }
        if (_wd.Frames[i].Framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(renderer->_device, _wd.Frames[i].Framebuffer, nullptr);
        }
    }

    vkDestroyRenderPass(renderer->_device, _renderPass, nullptr);
    vkDestroyDescriptorPool(renderer->_device, _descriptor.pool, nullptr);
}