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

#include "../core/PrototypePipelines.h"

#include "PrototypeOpenglUI.h"

#include "PrototypeOpenglRenderer.h"

#include "../core/PrototypeDatabase.h"

#include "../core/PrototypeCameraSystem.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"

#include "../core/PrototypeMaterial.h"
#include "../core/PrototypeMeshBuffer.h"
#include "../core/PrototypePluginInstance.h"
#include "../core/PrototypeShaderBuffer.h"
#include "../core/PrototypeShortcuts.h"
#include "../core/PrototypeTextureBuffer.h"

#include "../core/PrototypeEngine.h"
#include "../core/PrototypeWindow.h"

#include "../core/PrototypePhysics.h"

#include "../core/PrototypeProfiler.h"

#include <PrototypeCommon/IO.h>
#include <PrototypeCommon/Logger.h>
#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/printf.h>
#include <inttypes.h>
#include <sstream>

#define GLSL_VERSION std::string("#version 410 core")

#define COMPONENT_UI_HEADER(NAME)                                                                                                \
    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |                     \
                                           ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed |                    \
                                           ImGuiTreeNodeFlags_SpanAvailWidth;                                                    \
    {                                                                                                                            \
        static bool is_open = true;                                                                                              \
        ImGuiHelpers::BeginGroupPanel(#NAME, ImVec2(-1.0f, 0.0f), is_open, _awesomeFontTrait);                                   \
        if (is_open) {                                                                                                           \
            ImGui::Indent(16.0f);

#define COMPONENT_UI_FOOTER()                                                                                                    \
    }                                                                                                                            \
    ImGui::Unindent(16.0f);                                                                                                      \
    ImGuiHelpers::EndGroupPanel();                                                                                               \
    }

static float TEXT_BASE_WIDTH  = 1.0f;
static float TEXT_BASE_HEIGHT = 1.0f;

static ImFont* _defaultFont         = nullptr; // 8 bytes
static ImFont* _awesomeFont         = nullptr; // 8 bytes
static ImFont* _awesomeFontTrait    = nullptr; // 8 bytes
static ImFont* _awesomeFontTitle    = nullptr; // 8 bytes
static ImFont* _awesomeFontBtnIcons = nullptr; // 8 bytes

static PrototypeConfigurationSelection _assetConfigSelection;

static void
onCameraEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(Camera)

    void* VoidSceneCamera;
    ((PrototypeOpenglRenderer*)PrototypeEngineInternalApplication::renderer)->fetchCamera("SceneCamera", &VoidSceneCamera);
    PglCamera&       camera = *((PglCamera*)VoidSceneCamera);
    Camera*          cam    = o->getCameraTrait();
    static glm::vec3 translation;
    static glm::vec2 rotation;
    translation = cam->position();
    rotation    = cam->rotation();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("camera content table", 2, tableFlags)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Position");
        ImGui::TableSetColumnIndex(1);
        PrototypeImguiVec3WidgetUntitled(
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
          3.0f,
          -2.0f,
          "X",
          "Y",
          "Z")
        {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Rotation");
        ImGui::TableSetColumnIndex(1);
        PrototypeImguiVec2WidgetUntitled(
          20.0f,
          if (ImGui::DragFloat("##camera rotation x", &rotation.x, 0.1f, -90.0f, 90.0f)) {
              CameraSystemSetRotation(cam, rotation);
          },
          if (ImGui::DragFloat("##camera rotation y", &rotation.y, 0.1f, -360.0f, 360.0f)) {
              CameraSystemSetRotation(cam, rotation);
          },
          3.0f,
          -2.0f,
          "X",
          "Y")
        {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Look Sensitivity");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::DragFloat("##Look Sensitivity", &cam->lookSensitivityMut(), 0.001f, 0.001f, 10.0f)) {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Move Sensitivity");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::DragFloat("##Move Sensitivity", &cam->moveSensitivityMut(), 0.001f, 0.001f, 10.0f)) {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Field of view");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::DragFloat("##Field of view", &cam->fovMut(), 0.01f, 5.0f, 120.0f)) {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Near plane");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::DragFloat("##Near plane", &cam->nearMut(), 0.1f, 0.1f, 100.0f)) {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Far plane");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::DragFloat("##Far plane", &cam->farMut(), 1.0f, 1.0f, 10000.0f)) {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Orbital");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Checkbox("##Orbital", &cam->orbitalMut())) {}

        // deferred stage
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Deferred pass");
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText(
              "##Deferred pass", (char*)camera.deferredFramebuffer->name.c_str(), camera.deferredFramebuffer->name.size());
            ImGui::PopItemFlag();
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FRAMEBUFFER")) {
                    IM_ASSERT(payload->DataSize == sizeof(PglFramebuffer*));
                    auto framebuffer           = *(PglFramebuffer**)payload->Data;
                    camera.deferredFramebuffer = framebuffer;
                    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                }
                ImGui::EndDragDropTarget();
            }

            for (auto& pair : camera.deferredFramebuffer->shader->floatData) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat(fmt::format("##{}", pair.first).c_str(), &pair.second);
            }
            for (auto& pair : camera.deferredFramebuffer->shader->vec2Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat2(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
            for (auto& pair : camera.deferredFramebuffer->shader->vec3Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat3(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
            for (auto& pair : camera.deferredFramebuffer->shader->vec4Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat4(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
        }

        // gbuffer stage
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("GBuffer pass");
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText(
              "##GBuffer pass", (char*)camera.gbufferFramebuffer->name.c_str(), camera.gbufferFramebuffer->name.size());
            ImGui::PopItemFlag();
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FRAMEBUFFER")) {
                    IM_ASSERT(payload->DataSize == sizeof(PglFramebuffer*));
                    auto framebuffer          = *(PglFramebuffer**)payload->Data;
                    camera.gbufferFramebuffer = framebuffer;
                    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                }
                ImGui::EndDragDropTarget();
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->floatData) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat(fmt::format("##{}", pair.first).c_str(), &pair.second);
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->vec2Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat2(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->vec3Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat3(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->vec4Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat4(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
        }

        // post processing stage
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Post-processing pass");
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText("##Post-processing pass",
                             (char*)camera.postprocessingFramebuffer->name.c_str(),
                             camera.postprocessingFramebuffer->name.size());
            ImGui::PopItemFlag();
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FRAMEBUFFER")) {
                    IM_ASSERT(payload->DataSize == sizeof(PglFramebuffer*));
                    auto framebuffer                 = *(PglFramebuffer**)payload->Data;
                    camera.postprocessingFramebuffer = framebuffer;
                    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                }
                ImGui::EndDragDropTarget();
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->floatData) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat(fmt::format("##{}", pair.first).c_str(), &pair.second);
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->vec2Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat2(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->vec3Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat3(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->vec4Data) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(pair.first.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat4(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
            }
        }

        ImGui::EndTable();
    }

    COMPONENT_UI_FOOTER()
}

static void
onColliderEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(Collider)

    Collider* collider = o->getColliderTrait();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("collider content table", 2, tableFlags)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Shape");
        ImGui::TableSetColumnIndex(1);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
        ImGui::Button((char*)collider->nameRef().c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COLLIDER")) {
                IM_ASSERT(payload->DataSize == sizeof(PrototypeClipboard));
                PrototypeClipboard clipboard = *(PrototypeClipboard*)payload->Data;
                PrototypeEngineInternalApplication::physics->updateCollider(o, clipboard.data[0]);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::EndTable();
    }

    COMPONENT_UI_FOOTER()
}

static void
onMeshRendererEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(MeshRenderer)

    MeshRenderer* mr = o->getMeshRendererTrait();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("mesh renderer content table", 2, tableFlags)) {

        static int numberOfAttachedMeshMaterialPairs = 1;
        numberOfAttachedMeshMaterialPairs            = mr->data().size();
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Count");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::InputInt("##numberOfAttachedMeshMaterialPairs", &numberOfAttachedMeshMaterialPairs)) {
            if (numberOfAttachedMeshMaterialPairs < 1) { numberOfAttachedMeshMaterialPairs = 1; }
            size_t originalSize = mr->data().size();
            mr->data().resize(numberOfAttachedMeshMaterialPairs, mr->data()[originalSize - 1]);
        }
        for (size_t mmi = 0; mmi < mr->data().size(); ++mmi) {
            ImGui::PushID(mmi);
            MeshRendererPolygonMode_& polygonMode  = mr->data()[mmi].polygonMode;
            std::string&              meshName     = mr->data()[mmi].mesh;
            std::string&              materialName = mr->data()[mmi].material;
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Material");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                if (ImGui::Button((char*)fmt::format("{}##{}", materialName, mmi).c_str(),
                                  ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f))) {
                    PglMaterial* material;
                    PrototypeEngineInternalApplication::renderer->fetchMaterial(materialName, (void**)&material);
                    if (material) {
                        _assetConfigSelection.type     = PrototypeConfigurationSelectionAssetType_Material;
                        _assetConfigSelection.material = material;
                    }
                }
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL")) {
                        IM_ASSERT(payload->DataSize == sizeof(PglMaterial*));
                        PglMaterial* material = *(PglMaterial**)payload->Data;
                        materialName          = material->name;
                        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
            }
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Mesh");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                if (ImGui::Button((char*)fmt::format("{}##{}", meshName, mmi).c_str(),
                                  ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f))) {
                    PglGeometry* mesh;
                    PrototypeEngineInternalApplication::renderer->fetchMesh(meshName, (void**)&mesh);
                    if (mesh) {
                        _assetConfigSelection.type = PrototypeConfigurationSelectionAssetType_Mesh;
                        _assetConfigSelection.mesh = mesh;
                    }
                }
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MESH")) {
                        IM_ASSERT(payload->DataSize == sizeof(PglGeometry*));
                        PglGeometry* mesh = *(PglGeometry**)payload->Data;
                        meshName          = mesh->name;
                        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
            }
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Polygon mode");
                ImGui::TableSetColumnIndex(1);
                const char* polygonModes[MeshRendererPolygonMode_COUNT] = { "POINT", "LINE", "FILL" };
                if (ImGui::Combo("##PolygonMode", (int*)&polygonMode, polygonModes, MeshRendererPolygonMode_COUNT)) {
                    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                }
            }
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    COMPONENT_UI_FOOTER()
}

static void
onRigidbodyEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(Rigidbody)

    Rigidbody* rb = o->getRigidbodyTrait();

    static glm::vec3 linearVelocity;
    static f32       linearDamping;
    static glm::vec3 angularVelocity;
    static f32       angularDamping;
    static f32       mass;

    linearVelocity  = rb->linearVelocity();
    linearDamping   = rb->linearDamping();
    angularVelocity = rb->angularVelocity();
    angularDamping  = rb->angularDamping();
    mass            = rb->mass();

    static bool isTriggerCollider;
    isTriggerCollider = rb->isTrigger();

    static bool isStaticCollider;
    isStaticCollider = rb->isStatic();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("rigidbody content table", 2, tableFlags)) {

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("trigger");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Checkbox("##trigger", &isTriggerCollider)) {
            rb->setTrigger(isTriggerCollider);
            PrototypeEngineInternalApplication::physics->updateRigidbodyTrigger(o);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("static");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Checkbox("##static", &isStaticCollider)) {
            rb->setStatic(isStaticCollider);
            PrototypeEngineInternalApplication::physics->updateRigidbodyStatic(o);
        }

        if (!rb->isStatic()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Linear velocity");
            ImGui::TableSetColumnIndex(1);
            PrototypeImguiVec3WidgetUntitled(
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
              3.0f,
              -2.0f,
              "X",
              "Y",
              "Z")
            {}

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Linear damping");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::DragFloat("##linear damping", &linearDamping, 0.1f, 0.01f, 1000.0f)) {
                rb->setLinearDamping(linearDamping);
                PrototypeEngineInternalApplication::physics->updateRigidbodyLinearDamping(o);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Angular velocity");
            ImGui::TableSetColumnIndex(1);
            PrototypeImguiVec3WidgetUntitled(
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
              3.0f,
              -2.0f,
              "X",
              "Y",
              "Z")
            {}

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Angular damping");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::DragFloat("##angular damping", &angularDamping, 0.1f, 0.01f, 1000.0f)) {
                rb->setAngularDamping(angularDamping);
                PrototypeEngineInternalApplication::physics->updateRigidbodyAngularDamping(o);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Mass");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::DragFloat("##mass", &mass, 0.1f, 0.0f, 1000.0f)) {
                rb->setMass(mass);
                PrototypeEngineInternalApplication::physics->updateRigidbodyMass(o);
            }

            ImGuiWindow* window = ImGui::GetCurrentWindow();

            // Lock Linear
            {
                static bool linearLocked = false;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Lock Position");
                ImGui::TableSetColumnIndex(1);
                PrototypeImguiVec3WidgetUntitled(
                  20.0f,
                  if (ImGui::Checkbox("##Lock linear X", &rb->lockLinearXMut())) linearLocked = true,
                  if (ImGui::Checkbox("##Lock linear Y", &rb->lockLinearYMut())) linearLocked = true,
                  if (ImGui::Checkbox("##Lock linear Z", &rb->lockLinearZMut())) linearLocked = true,
                  3.0f,
                  -2.0f,
                  "X",
                  "Y",
                  "Z")
                {}

                if (linearLocked) { PrototypeEngineInternalApplication::physics->updateRigidbodyLockLinear(o); }
            }

            // Lock Angles
            {
                static bool angularLocked = false;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Lock Rotation");
                ImGui::TableSetColumnIndex(1);
                PrototypeImguiVec3WidgetUntitled(
                  20.0f,
                  if (ImGui::Checkbox("##Lock angular X", &rb->lockAngularXMut())) angularLocked = true,
                  if (ImGui::Checkbox("##Lock angular Y", &rb->lockAngularYMut())) angularLocked = true,
                  if (ImGui::Checkbox("##Lock angular Z", &rb->lockAngularZMut())) angularLocked = true,
                  3.0f,
                  -2.0f,
                  "X",
                  "Y",
                  "Z")
                {}

                if (angularLocked) { PrototypeEngineInternalApplication::physics->updateRigidbodyLockAngular(o); }
            }
        }

        ImGui::EndTable();
    }

    COMPONENT_UI_FOOTER()
}

static void
onScriptEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(Script)

    Script* script = o->getScriptTrait();

    if (script->codeLinks.empty()) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
        ImGui::Button((char*)"Drag and drop your scripts here", ImVec2(ImGui::GetContentRegionAvailWidth(), 60.0f));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCRIPT")) {
                IM_ASSERT(payload->DataSize == sizeof(PrototypePluginInstance*));
                PrototypePluginInstance* pluginInstance = *(PrototypePluginInstance**)payload->Data;
                ScriptCodeLink           link           = {};
                pluginInstance->linkScript(&link);
                if (script->codeLinks.find(link.filepath) == script->codeLinks.end()) {
                    script->codeLinks.insert({ link.filepath, link });
                    PrototypePluginInstance::safeCallStartProtocol(&link, o);
                }
            }
            ImGui::EndDragDropTarget();
        }
    } else {
        static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                            ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                            ImGuiTableFlags_PreciseWidths;
        if (ImGui::BeginTable("script content table", 2, tableFlags)) {
            {
                int i = 0;
                for (auto codeLinkPair : script->codeLinks) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Script %i", i);
                    ImGui::TableSetColumnIndex(1);

                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                    ImGui::Button((char*)codeLinkPair.second.filepath.c_str(),
                                  ImVec2(ImGui::GetContentRegionAvailWidth() - 35.0f, 30.0f));
                    ImGui::PopStyleColor(4);
                    ImGui::PopStyleVar();
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCRIPT")) {
                            IM_ASSERT(payload->DataSize == sizeof(PrototypePluginInstance*));
                            PrototypePluginInstance* pluginInstance = *(PrototypePluginInstance**)payload->Data;
                            PrototypePluginInstance::safeCallEndProtocol(&codeLinkPair.second, o);
                            pluginInstance->linkScript(&codeLinkPair.second);
                            PrototypePluginInstance::safeCallStartProtocol(&codeLinkPair.second, o);
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::SameLine();
                    ImGui::PushID(++i);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_WHITE, 0.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                    if (ImGui::Button(fmt::format("X##script remove button {}", i).c_str(),
                                      ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f))) {
                        PrototypePipelineQueue queue = {};
                        auto cmd      = std::make_unique<PrototypePipelineCommand_shortcutEditorRemoveScriptFromObject>();
                        cmd->object   = o;
                        cmd->codeLink = codeLinkPair.second.filepath;
                        queue.record(std::move(cmd));
                        PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                    }
                    ImGui::PopStyleColor(4);
                    ImGui::PopStyleVar();
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCRIPT")) {
                IM_ASSERT(payload->DataSize == sizeof(PrototypePluginInstance*));
                PrototypePluginInstance* pluginInstance = *(PrototypePluginInstance**)payload->Data;
                ScriptCodeLink           link           = {};
                pluginInstance->linkScript(&link);
                if (script->codeLinks.find(link.filepath) == script->codeLinks.end()) {
                    script->codeLinks.insert({ link.filepath, link });
                    PrototypePluginInstance::safeCallStartProtocol(&link, o);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    COMPONENT_UI_FOOTER()
}

static void
onTransformEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(Transform)

    static glm::vec3 position, rotation, scale;
    Transform*       tr = o->getTransformTrait();
    position            = tr->position();
    rotation            = tr->rotation();
    scale               = tr->scale();

    bool positionChanged = false, rotationChanged = false, scaleChanged = false;

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("transform content table", 2, tableFlags)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Position");
        ImGui::TableSetColumnIndex(1);
        PrototypeImguiVec3WidgetUntitled(
          20.0f,
          if (ImGui::DragFloat("##position x", &position.x, 0.1f, -1000.0f, 1000.0f)) { positionChanged = true; },
          if (ImGui::DragFloat("##position y", &position.y, 0.1f, -1000.0f, 1000.0f)) { positionChanged = true; },
          if (ImGui::DragFloat("##position z", &position.z, 0.1f, -1000.0f, 1000.0f)) { positionChanged = true; },
          3.0f,
          -2.0f,
          "X",
          "Y",
          "Z")
        {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Rotation");
        ImGui::TableSetColumnIndex(1);
        PrototypeImguiVec3WidgetUntitled(
          20.0f,
          if (ImGui::DragFloat("##rotation x", &rotation.x, 0.1f, -360.0f, 360.0f)) { rotationChanged = true; },
          if (ImGui::DragFloat("##rotation y", &rotation.y, 0.1f, -360.0f, 360.0f)) { rotationChanged = true; },
          if (ImGui::DragFloat("##rotation z", &rotation.z, 0.1f, -360.0f, 360.0f)) { rotationChanged = true; },
          3.0f,
          -2.0f,
          "X",
          "Y",
          "Z")
        {}

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("scale");
        ImGui::TableSetColumnIndex(1);
        PrototypeImguiVec3WidgetUntitled(
          20.0f,
          if (ImGui::DragFloat("##scale x", &scale.x, 0.1f, 0.1f, 10.0f)) {
              if (scale.x >= 0.1f) { scaleChanged = true; }
          },
          if (ImGui::DragFloat("##scale y", &scale.y, 0.1f, 0.1f, 10.0f)) {
              if (scale.y >= 0.1f) { scaleChanged = true; }
          },
          if (ImGui::DragFloat("##scale z", &scale.z, 0.1f, 0.1f, 10.0f)) {
              if (scale.z >= 0.1f) { scaleChanged = true; }
          },
          3.0f,
          -2.0f,
          "X",
          "Y",
          "Z")
        {}
        ImGui::EndTable();
    }

    if (positionChanged || rotationChanged) {
        ImGuizmo::Enable(false);
        glm::mat4 model;
        PrototypeMaths::buildModelMatrix(model, tr->position(), rotation);
        PrototypeMaths::buildModelMatrixWithScale(model, tr->scale());
        tr->setModelScaled(&model[0][0]);
        tr->updateComponentsFromMatrix();
        if (o->hasColliderTrait()) { tr->setNeedsPhysicsSync(true); }
        ImGuizmo::Enable(true);
        tr->positionMut().x = position.x;
        tr->positionMut().y = position.y;
        tr->positionMut().z = position.z;
        tr->rotationMut().x = rotation.x;
        tr->rotationMut().y = rotation.y;
        tr->rotationMut().z = rotation.z;
    }
    if (scaleChanged) {
        ImGuizmo::Enable(false);
        glm::mat4 model;
        PrototypeMaths::buildModelMatrix(model, position, rotation);
        tr->setModel(&model[0][0]);
        if (o->hasColliderTrait()) { PrototypeEngineInternalApplication::physics->scaleCollider(o, scale); }
        PrototypeMaths::buildModelMatrixWithScale(model, scale);
        tr->setModelScaled(&model[0][0]);
        tr->updateComponentsFromMatrix();

        tr->scaleMut().x = scale.x;
        tr->scaleMut().y = scale.y;
        tr->scaleMut().z = scale.z;
        ImGuizmo::Enable(true);
    }

    COMPONENT_UI_FOOTER()
}

static void
onVehicleChasisEditDispatch(PrototypeObject* o)
{
    COMPONENT_UI_HEADER(VehicleChasis);

    VehicleChasis* vch = o->getVehicleChasisTrait();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGui::BeginTable("transform content table", 2, tableFlags)) {

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Acceleration Pedal");
        ImGui::TableSetColumnIndex(1);
        static bool accelerationPedal;
        accelerationPedal = vch->accelerationPedal();
        if (ImGui::Checkbox("##Acceleration Pedal", &accelerationPedal)) { vch->setAccelerationPedal(accelerationPedal); }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Brake Pedal");
        ImGui::TableSetColumnIndex(1);
        static bool brakePedal;
        brakePedal = vch->brakePedal();
        if (ImGui::Checkbox("##Brake Pedal", &brakePedal)) { vch->setBrakePedal(brakePedal); }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Right Steer");
        ImGui::TableSetColumnIndex(1);
        static bool rightSteer;
        rightSteer = vch->rightSteer();
        if (ImGui::Checkbox("##Right Steer", &rightSteer)) { vch->setRightSteer(rightSteer); }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Left Steer");
        ImGui::TableSetColumnIndex(1);
        static bool leftSteer;
        leftSteer = vch->leftSteer();
        if (ImGui::Checkbox("##Left Steer", &leftSteer)) { vch->setLeftSteer(leftSteer); }

        {
            // Front Right Wheel
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Front right wheel");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                char*            name          = "";
                PrototypeObject* wheelFRObject = vch->wheelFRObject();
                if (wheelFRObject) {
                    PrototypeSceneNode* wheelFRNode = (PrototypeSceneNode*)wheelFRObject->parentNode();
                    name                            = (char*)wheelFRNode->name().c_str();
                }
                ImGui::Button(name, ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                        IM_ASSERT(payload->DataSize == sizeof(PrototypeSceneNode*));
                        PrototypeSceneNode* node   = *(PrototypeSceneNode**)payload->Data;
                        auto                optObj = node->object();
                        if (optObj.has_value()) {
                            auto obj = optObj.value();
                            vch->setWheelFRObject(obj);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            // Front Left Wheel
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Front left wheel");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                char*            name          = "";
                PrototypeObject* wheelFLObject = vch->wheelFLObject();
                if (wheelFLObject) {
                    PrototypeSceneNode* wheelFLNode = (PrototypeSceneNode*)wheelFLObject->parentNode();
                    name                            = (char*)wheelFLNode->name().c_str();
                }
                ImGui::Button(name, ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                        IM_ASSERT(payload->DataSize == sizeof(PrototypeSceneNode*));
                        PrototypeSceneNode* node   = *(PrototypeSceneNode**)payload->Data;
                        auto                optObj = node->object();
                        if (optObj.has_value()) {
                            auto obj = optObj.value();
                            vch->setWheelFLObject(obj);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            // Backward Right Wheel
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Back right wheel");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                char*            name          = "";
                PrototypeObject* wheelBRObject = vch->wheelBRObject();
                if (wheelBRObject) {
                    PrototypeSceneNode* wheelBRNode = (PrototypeSceneNode*)wheelBRObject->parentNode();
                    name                            = (char*)wheelBRNode->name().c_str();
                }
                ImGui::Button(name, ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                        IM_ASSERT(payload->DataSize == sizeof(PrototypeSceneNode*));
                        PrototypeSceneNode* node   = *(PrototypeSceneNode**)payload->Data;
                        auto                optObj = node->object();
                        if (optObj.has_value()) {
                            auto obj = optObj.value();
                            vch->setWheelBRObject(obj);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            // Backward Left Wheel
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Back left wheel");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                char*            name          = "";
                PrototypeObject* wheelBLObject = vch->wheelBLObject();
                if (wheelBLObject) {
                    PrototypeSceneNode* wheelBLNode = (PrototypeSceneNode*)wheelBLObject->parentNode();
                    name                            = (char*)wheelBLNode->name().c_str();
                }
                ImGui::Button(name, ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                        IM_ASSERT(payload->DataSize == sizeof(PrototypeSceneNode*));
                        PrototypeSceneNode* node   = *(PrototypeSceneNode**)payload->Data;
                        auto                optObj = node->object();
                        if (optObj.has_value()) {
                            auto obj = optObj.value();
                            vch->setWheelBLObject(obj);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
        ImGui::EndTable();
    }

    COMPONENT_UI_FOOTER();
}

PrototypeOpenglUI::PrototypeOpenglUI()
  : _clipboard({})
  , _playBtnImg(0)
  , _reloadBtnImg(0)
  , _pauseBtnImg(0)
  , _importBtnImg(0)
  , _exportBtnImg(0)
  , _isBuffersChanged(false)
  , _freezeResizingViews(false)
  , _recordMask(PrototypeUiViewMaskAll)
  , _openViewsMask(PrototypeUiViewMaskAll)
  , _sceneView({})
{}

bool
PrototypeOpenglUI::init()
{
    _recordedInstructions = PrototypeOpenglUIInstructions();
    _assetConfigSelection = PrototypeConfigurationSelection();

    ImGui::CreateContext();
    ImPlot::CreateContext();
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
        icons_config.MergeMode     = true;
        icons_config.PixelSnapH    = true;
        icons_config.GlyphOffset.y = 0.0f;
        _awesomeFont               = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 10.0f, &icons_config, icons_ranges);
    }

    // awesome font traits
    {
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig         icons_config;
        icons_config.MergeMode     = false;
        icons_config.PixelSnapH    = true;
        icons_config.GlyphOffset.y = 2.0f;
        _awesomeFontTrait          = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 13.0f, &icons_config, icons_ranges);
    }

    // awesome font title
    {
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig         icons_config;
        icons_config.MergeMode     = false;
        icons_config.PixelSnapH    = true;
        icons_config.GlyphOffset.y = 0.0f;
        _awesomeFontTitle          = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 18.0f, &icons_config, icons_ranges);
    }

    // awesome font button icons
    {
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig         icons_config;
        icons_config.MergeMode     = false;
        icons_config.PixelSnapH    = true;
        icons_config.GlyphOffset.y = -1.0f;
        _awesomeFontBtnIcons       = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 20.0f, &icons_config, icons_ranges);
    }

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)PrototypeEngineInternalApplication::window->handle(), true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION.c_str());
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.AntiAliasedLines       = false;
    style.AntiAliasedLinesUseTex = false;
    style.AntiAliasedFill        = false;
    style.CurveTessellationTol   = 0.1f;
    style.CircleSegmentMaxError  = 100.0f;

    style.ChildRounding     = 0.f;
    style.GrabRounding      = 0.f;
    style.WindowRounding    = 0.f;
    style.ScrollbarRounding = 0.f;
    style.ScrollbarSize     = 10.0f;
    style.FrameRounding     = 0.f;
    style.TabRounding       = 0.f;
    style.TabBorderSize     = 0.0f;
    style.WindowTitleAlign  = ImVec2(0.01f, 0.5f);

    // style.Colors[ImGuiCol_Text]         = ImVec4(PROTOTYPE_WHITE, 1.00f);
    // style.Colors[ImGuiCol_TextDisabled] = ImVec4(PROTOTYPE_LIGHTESTGRAY, 1.00f);
    // style.Colors[ImGuiCol_WindowBg]     = ImVec4(PROTOTYPE_DARKESTGRAY, 1.0f);
    // style.Colors[ImGuiCol_ChildBg]      = ImVec4(PROTOTYPE_DARKESTGRAY, 1.0f);
    // style.Colors[ImGuiCol_PopupBg]      = ImVec4(PROTOTYPE_DARKESTGRAY, 1.00f);
    // // style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.76f, 0.26f, 0.26f, 1.00f);
    // // style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.26f, 0.76f, 0.26f, 1.00f);
    // // style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.26f, 0.26f, 0.76f, 1.00f);
    // style.Colors[ImGuiCol_Border]             = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_BorderShadow]       = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_FrameBg]            = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    // style.Colors[ImGuiCol_FrameBgHovered]     = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    // style.Colors[ImGuiCol_FrameBgActive]      = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    // style.Colors[ImGuiCol_TitleBg]            = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_TitleBgActive]      = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_MenuBarBg]          = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_Tab]                = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_TabUnfocused]       = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_TabHovered]         = ImVec4(PROTOTYPE_DARKERGRAY, 1.00f);
    // style.Colors[ImGuiCol_TabActive]          = ImVec4(PROTOTYPE_DARKESTGRAY, 1.00f);
    // style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(PROTOTYPE_DARKESTGRAY, 1.00f);
    // // style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    // // style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    // // style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    // // style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    // // style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    // style.Colors[ImGuiCol_SliderGrab]           = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(PROTOTYPE_GRAY, 1.00f);
    // style.Colors[ImGuiCol_Button]               = ImVec4(0.193f, 0.193f, 0.193f, 1.000f);
    // style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.314f, 0.315f, 0.317f, 1.000f);
    // style.Colors[ImGuiCol_ButtonActive]         = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    // style.Colors[ImGuiCol_Header]               = ImVec4(PROTOTYPE_DARKGRAY, 1.00f);
    // style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(PROTOTYPE_GRAY, 1.00f);
    // style.Colors[ImGuiCol_HeaderActive]         = ImVec4(PROTOTYPE_GRAY, 1.00f);
    // style.Colors[ImGuiCol_PlotLines]            = ImVec4(PROTOTYPE_GRAY, 1.00f);
    // style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(PROTOTYPE_LIGHTESTGRAY, 1.00f);
    // style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(PROTOTYPE_GRAY, 1.00f);
    // style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(PROTOTYPE_LIGHTESTGRAY, 1.00f);
    // // style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
    // // style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

    style.Colors[ImGuiCol_Border]                = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(1.0f, 0.232f, 0.0f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(1.0f, 0.231f, 0.0f, 0.784f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(1.0f, 1.0f, 1.0f, 0.38f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.09f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Tab]                   = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_TabActive]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    // register traits onEdits
    Camera::setOnEditDispatchHandler(onCameraEditDispatch);
    Collider::setOnEditDispatchHandler(onColliderEditDispatch);
    MeshRenderer::setOnEditDispatchHandler(onMeshRendererEditDispatch);
    Rigidbody::setOnEditDispatchHandler(onRigidbodyEditDispatch);
    Script::setOnEditDispatchHandler(onScriptEditDispatch);
    Transform::setOnEditDispatchHandler(onTransformEditDispatch);
    VehicleChasis::setOnEditDispatchHandler(onVehicleChasisEditDispatch);

    {
        PglTexture* playBtnTexture;
        PrototypeEngineInternalApplication::renderer->fetchTexture("icons/play.png", (void**)&playBtnTexture);
        if (playBtnTexture) {
            _playBtnImg = playBtnTexture->id;
        } else {
            _playBtnImg = 0;
        }
    }
    {
        PglTexture* pauseBtnTexture;
        PrototypeEngineInternalApplication::renderer->fetchTexture("icons/pause.png", (void**)&pauseBtnTexture);
        if (pauseBtnTexture) {
            _pauseBtnImg = pauseBtnTexture->id;
        } else {
            _pauseBtnImg = 0;
        }
    }
    {
        PglTexture* reloadBtnTexture;
        PrototypeEngineInternalApplication::renderer->fetchTexture("icons/reload.png", (void**)&reloadBtnTexture);
        if (reloadBtnTexture) {
            _reloadBtnImg = reloadBtnTexture->id;
        } else {
            _reloadBtnImg = 0;
        }
    }
    {
        PglTexture* importBtnTexture;
        PrototypeEngineInternalApplication::renderer->fetchTexture("icons/import.png", (void**)&importBtnTexture);
        if (importBtnTexture) {
            _importBtnImg = importBtnTexture->id;
        } else {
            _importBtnImg = 0;
        }
    }
    {
        PglTexture* exportBtnTexture;
        PrototypeEngineInternalApplication::renderer->fetchTexture("icons/export.png", (void**)&exportBtnTexture);
        if (exportBtnTexture) {
            _exportBtnImg = exportBtnTexture->id;
        } else {
            _exportBtnImg = 0;
        }
    }

    _sceneView.onInit();

    return true;
}

void
PrototypeOpenglUI::deInit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void
PrototypeOpenglUI::scheduleRecordPass(PrototypeUiViewMaskType mask)
{
    _recordMask |= mask;
}

void
PrototypeOpenglUI::beginRecordPass()
{
    if (_recordMask == 0) return;

    // filter nodes
    /*{
        _recordedInstructions.sceneNodes.clear();
        const auto&                                  layers              = PrototypeEngineInternalApplication::scene->layers();
        int                                          layersChildrenCount = layers.size();
        std::unordered_map<u32, PrototypeSceneNode*> nextGeneration;
        for (const auto& pair : layers) {
            size_t validChildrenCount = 0;
            for (auto pair : pair.second->nodes()) {
                if (pair.second->name().find(_recordedInstructions.sceneNodesSearchBuffer) != std::string::npos) {
                    nextGeneration[pair.first] = pair.second;
                    ++validChildrenCount;
                }
            }
            PrototypeOpenglUIInstructions::InternalSceneNode node = {};
            node.layer                                            = pair.second;
            node.isLayer                                          = true;
            node.ChildIdx                                         = layersChildrenCount;
            node.ChildCount                                       = validChildrenCount;
            layersChildrenCount += node.ChildCount;
            _recordedInstructions.sceneNodes.push_back(std::move(node));
        }
        while (!nextGeneration.empty()) {
            std::unordered_map<u32, PrototypeSceneNode*> nextChildren;
            for (const auto& pair : nextGeneration) {
                size_t validChildrenCount = 0;
                for (auto pair : pair.second->nodes()) {
                    if (pair.second->name().find(_recordedInstructions.sceneNodesSearchBuffer) != std::string::npos) {
                        nextChildren[pair.first] = pair.second;
                        ++validChildrenCount;
                    }
                }
                PrototypeOpenglUIInstructions::InternalSceneNode node = {};
                node.node                                             = pair.second;
                node.isLayer                                          = false;
                node.ChildIdx                                         = layersChildrenCount;
                node.ChildCount                                       = validChildrenCount;
                layersChildrenCount += node.ChildCount;
                _recordedInstructions.sceneNodes.push_back(std::move(node));
            }
            nextGeneration.clear();
            nextGeneration = nextChildren;
        }
    }*/

    // Filter nodes dfs
    if ((_recordMask & PrototypeUiViewMaskHierarchy) == PrototypeUiViewMaskHierarchy) {
        _recordMask &= ~PrototypeUiViewMaskHierarchy;
        _recordedInstructions.sceneNodes.clear();
        const auto&                              layers = PrototypeEngineInternalApplication::scene->layers();
        std::function<bool(PrototypeSceneNode*)> lambda = [&](PrototypeSceneNode* node) -> bool {
            node->unsetMatched();
            node->unsetPartialMatched();
            for (const auto& pair : node->nodes()) {
                if (lambda(pair.second)) { node->setPartialMatched(); }
            }
            if (node->name().find(_recordedInstructions.sceneNodesSearchBuffer) != std::string::npos) { node->setMatched(); }
            return node->isMatching() | node->isPartialMatching();
        };

        for (const auto& layerPair : layers) {
            layerPair.second->unsetMatched();
            layerPair.second->unsetPartialMatched();
            for (const auto& nodePair : layerPair.second->nodes()) {
                if (lambda(nodePair.second)) { layerPair.second->setPartialMatched(); }
            }
            if (layerPair.second->name().find(_recordedInstructions.sceneNodesSearchBuffer) != std::string::npos) {
                layerPair.second->setMatched();
            }
        }
    }

    // Filter meshes
    if ((_recordMask & PrototypeUiViewMaskMeshes) == PrototypeUiViewMaskMeshes) {
        _recordMask &= ~PrototypeUiViewMaskMeshes;
        _recordedInstructions.meshes.clear();
        for (const auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
            size_t matchIndex = pair.first.find(_recordedInstructions.meshesSearchBuff);
            if (matchIndex != std::string::npos) {
                PglGeometry* mesh;
                PrototypeEngineInternalApplication::renderer->fetchMesh(pair.first, (void**)&mesh);
                if (mesh) { _recordedInstructions.meshes.push_back(mesh); }
            }
        }
    }

    // Filter colliders
    if ((_recordMask & PrototypeUiViewMaskColliders) == PrototypeUiViewMaskColliders) {
        _recordMask &= ~PrototypeUiViewMaskColliders;
        _recordedInstructions.colliders.clear();
        const static std::vector<std::string> defaultColliders = { "PLANE", "CUBE", "SPHERE" };
        for (const std::string& defCol : defaultColliders) {
            size_t matchIndex = defCol.find(_recordedInstructions.collidersSearchBuff);
            if (matchIndex != std::string::npos) { _recordedInstructions.colliders.push_back({ "", defCol }); }
        }
        for (const auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
            size_t matchIndex = pair.first.find(_recordedInstructions.collidersSearchBuff);
            if (matchIndex != std::string::npos) {
                // CONVEX //
                _recordedInstructions.colliders.push_back({ "(CONVEX) ", pair.first });

                // TRIMESH //
                _recordedInstructions.colliders.push_back({ "(TRIMESH) ", pair.first });
            }
        }
    }

    // Filter shaders
    if ((_recordMask & PrototypeUiViewMaskShaders) == PrototypeUiViewMaskShaders) {
        _recordMask &= ~PrototypeUiViewMaskShaders;
        _recordedInstructions.shaders.clear();
        for (const auto& pair : PrototypeEngineInternalApplication::database->shaderBuffers) {
            size_t matchIndex = pair.first.find(_recordedInstructions.shadersSearchBuff);
            if (matchIndex != std::string::npos) {
                PglShader* shader;
                PrototypeEngineInternalApplication::renderer->fetchShader(pair.first, (void**)&shader);
                if (shader) { _recordedInstructions.shaders.push_back(shader); }
            }
        }
    }

    // Filter textures
    if ((_recordMask & PrototypeUiViewMaskTextures) == PrototypeUiViewMaskTextures) {
        _recordMask &= ~PrototypeUiViewMaskTextures;
        _recordedInstructions.textures.clear();
        for (const auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) {
            size_t matchIndex = pair.first.find(_recordedInstructions.texturesSearchBuff);
            if (matchIndex != std::string::npos) {
                PglTexture* texture;
                PrototypeEngineInternalApplication::renderer->fetchTexture(pair.first, (void**)&texture);
                if (texture) { _recordedInstructions.textures.push_back(texture); }
            }
        }
    }

    // Filter materials
    if ((_recordMask & PrototypeUiViewMaskMaterials) == PrototypeUiViewMaskMaterials) {
        _recordMask &= ~PrototypeUiViewMaskMaterials;
        _recordedInstructions.materials.clear();
        for (const auto& pair : PrototypeEngineInternalApplication::database->materials) {
            size_t matchIndex = pair.first.find(_recordedInstructions.materialsSearchBuff);
            if (matchIndex != std::string::npos) {
                PglMaterial* material;
                PrototypeEngineInternalApplication::renderer->fetchMaterial(pair.first, (void**)&material);
                if (material) { _recordedInstructions.materials.push_back(material); }
            }
        }
    }

    _recordMask = 0;
    PrototypeEngineInternalApplication::window->resetDeltaTime();
}

void
PrototypeOpenglUI::endRecordPass()
{}

void
PrototypeOpenglUI::beginFrame(bool changed)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

PrototypeUIState_
PrototypeOpenglUI::drawFrame(u32 fbid, i32 width, i32 height)
{
    enum class DockingUILayouts
    {
        LayoutA,
        LayoutB,

        Count
    };
    static DockingUILayouts layout                                                  = DockingUILayouts::LayoutA;
    const char*             layoutsNames[static_cast<int>(DockingUILayouts::Count)] = { PROTOTYPE_STRINGIFY(LayoutA),
                                                                            PROTOTYPE_STRINGIFY(LayoutB) };
    static int              layoutComboIndex                                        = static_cast<int>(DockingUILayouts::LayoutA);

    PrototypeUIState_ state = PrototypeUIState_None;

    ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    {
        if (ImGui::Begin("Main DockSpace", (bool*)0, mainWindowFlags)) {
            {
                if (ImGui::BeginChild("Toolbar",
                                      ImVec2(ImGui::GetContentRegionAvailWidth(), 35.0f),
                                      false,
                                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    float barWidth  = ImGui::GetContentRegionAvailWidth();
                    f32   leftPosX  = ImGui::GetCursorPosX() + 5.0f;
                    f32   rightPosX = barWidth - 5.0f;
                    f32   midPosX   = (rightPosX - leftPosX) / 2.0f;

                    // left side
                    {
                        ImGui::SetCursorPosX(leftPosX);
                        ImGui::SetNextItemWidth(135.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(50.0f, 5.0f));
                        {
                            if (ImGui::Combo("##DockingLayouts",
                                             &layoutComboIndex,
                                             layoutsNames,
                                             static_cast<int>(DockingUILayouts::Count))) {}
                        }
                        ImGui::PopStyleVar();
                        leftPosX += 135.0f;
                    }

                    // mid side
                    {
                        midPosX -= 25.0f;
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(midPosX);
                        static bool isPlaying = true;
                        isPlaying             = PrototypeEngineInternalApplication::physics->isPlaying();
                        if (ImGui::Button(isPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY, ImVec2(25.0f, 25.0f))) {
                            if (isPlaying) {
                                PrototypeEngineInternalApplication::physics->pause();
                            } else {
                                PrototypeEngineInternalApplication::physics->play();
                            }
                        }
                        midPosX += 25.0f;

                        ImGui::SameLine();
                        ImGui::SetCursorPosX(midPosX);
                        if (!_isBuffersChanged) {
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                            ImGui::Button(ICON_FA_BOLT, ImVec2(25.0f, 25.0f));
                            ImGui::PopStyleVar();
                            ImGui::PopItemFlag();
                        } else {
                            if (ImGui::Button(ICON_FA_BOLT, ImVec2(25.0f, 25.0f))) {
                                _isBuffersChanged = false;
                                for (const auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
                                    pair.second->commitChange();
                                }
                                for (const auto& pair : PrototypeEngineInternalApplication::database->shaderBuffers) {
                                    pair.second->commitChange();
                                }
                                for (const auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) {
                                    pair.second->commitChange();
                                }
                                for (const auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) {
                                    pair.second->commitChange();
                                }
                                PrototypePipelineQueue queue = {};
                                for (const auto& pair : PrototypeEngineInternalApplication::database->pluginInstances) {
                                    auto cmd = std::make_unique<PrototypePipelineCommand_shortcutEditorCommitReloadPlugin>();
                                    cmd->pluginInstance = pair.second;
                                    queue.record(std::move(cmd));
                                }
                                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_LIGHTERGRAY, 1.0f));
                                ImGui::SetTooltip("Reload");
                                ImGui::PopStyleColor();
                                ImGui::PopStyleVar();
                            }
                        }
                        midPosX += 25.0f;
                    }

                    // right side
                    {}
                }
                ImGui::EndChild();
            }
            if (ImGui::DockBuilderGetNode(ImGui::GetID("TheDockspace")) == nullptr || _freezeResizingViews ||
                layoutComboIndex != static_cast<int>(layout)) {
                layout              = static_cast<DockingUILayouts>(layoutComboIndex);
                ImGuiID dockspaceId = ImGui::GetID("TheDockspace");
                ImGui::DockBuilderRemoveNode(dockspaceId); // Clear out existing layout
                ImGui::DockBuilderAddNode(dockspaceId,
                                          ImGuiDockNodeFlags_DockSpace); // Add empty node

                ImGuiID dock_main_id = dockspaceId; // This variable will track the document node, however we are not using it
                                                    // here as we aren't docking anything into it.

                // Default Layout
                // ------------------------------------------------------ ------------------------------------------------------
                // |        |                               |           | |        |                               |           |
                // |        |                               |           | |        |                               |           |
                // |        |                               |           | |        |                               |           |
                // |   E    |            G                  |           | |        |            G                  |           |
                // |        |                               |           | |        |                               |           |
                // |        |                               |     A     | |   A    |                               |     H     |
                // |        |                               |           | |        |                               |           |
                // |----------------------------------------|           | |        |-------------------------------------------|
                // |                  C                     |           | |        |           |                               |
                // |                                        |           | |        |    E      |             F                 |
                // ------------------------------------------------------ ------------------------------------------------------
                {
                    switch (layout) {
                        case DockingUILayouts::LayoutA: {
                            ImGuiID dock_id_a, dock_id_b, dock_id_c, dock_id_d, dock_id_e, dock_id_f, dock_id_g;
                            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, &dock_id_a, &dock_id_b);
                            ImGui::DockBuilderSplitNode(dock_id_b, ImGuiDir_Down, 0.25f, &dock_id_c, &dock_id_d);
                            ImGui::DockBuilderSplitNode(dock_id_d, ImGuiDir_Left, 0.25f, &dock_id_e, &dock_id_f);
                            ImGui::DockBuilderSplitNode(dock_id_f, ImGuiDir_Right, 0.25f, nullptr, &dock_id_g);
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
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_g);
                                node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            ImGui::DockBuilderDockWindow("3D View", dock_id_g);
                            ImGui::DockBuilderDockWindow("3D Paint", dock_id_g);
                            ImGui::DockBuilderDockWindow("Game", dock_id_g);
                            ImGui::DockBuilderDockWindow("Texture Editor", dock_id_g);
                            ImGui::DockBuilderDockWindow("Text Editor", dock_id_g);
                            ImGui::DockBuilderDockWindow("Input Settings", dock_id_g);
                            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_e);
                            ImGui::DockBuilderDockWindow("Traits", dock_id_a);
                            ImGui::DockBuilderDockWindow("Properties", dock_id_a);
                            ImGui::DockBuilderDockWindow("Settings", dock_id_a);
#if defined(PROTOTYPE_ENABLE_PROFILER)
                            ImGui::DockBuilderDockWindow("Profiler", dock_id_c);
#endif
                            ImGui::DockBuilderDockWindow("Colliders", dock_id_c);
                            ImGui::DockBuilderDockWindow("Materials", dock_id_c);
                            ImGui::DockBuilderDockWindow("Textures", dock_id_c);
                            ImGui::DockBuilderDockWindow("Shaders", dock_id_c);
                            ImGui::DockBuilderDockWindow("Meshes", dock_id_c);
                            ImGui::DockBuilderDockWindow("Scenes", dock_id_c);
                            ImGui::DockBuilderDockWindow("Framebuffers", dock_id_c);
                            ImGui::DockBuilderDockWindow("Console", dock_id_c);
                            ImGui::DockBuilderDockWindow("Scripts", dock_id_c);
                            ImGui::DockBuilderDockWindow("Blueprints", dock_id_c);
                        } break;

                        case DockingUILayouts::LayoutB: {
                            ImGuiID dock_id_a, dock_id_b, dock_id_c, dock_id_d, dock_id_e, dock_id_f, dock_id_g, dock_id_h,
                              dock_id_i;
                            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, &dock_id_a, &dock_id_b);
                            ImGui::DockBuilderSplitNode(dock_id_b, ImGuiDir_Down, 0.25f, &dock_id_c, &dock_id_d);
                            ImGui::DockBuilderSplitNode(dock_id_c, ImGuiDir_Left, 0.25f, &dock_id_e, &dock_id_f);
                            ImGui::DockBuilderSplitNode(dock_id_d, ImGuiDir_Right, 0.25f, &dock_id_h, &dock_id_g);
                            ImGui::DockBuilderSplitNode(dock_id_g, ImGuiDir_Right, 0.25f, nullptr, &dock_id_i);
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
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_g);
                                node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_h);
                                node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_i);
                                node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            ImGui::DockBuilderDockWindow("3D View", dock_id_i);
                            ImGui::DockBuilderDockWindow("3D Paint", dock_id_i);
                            ImGui::DockBuilderDockWindow("Game", dock_id_i);
                            ImGui::DockBuilderDockWindow("Texture Editor", dock_id_i);
                            ImGui::DockBuilderDockWindow("Text Editor", dock_id_i);
                            ImGui::DockBuilderDockWindow("Input Settings", dock_id_i);
                            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_a);
                            ImGui::DockBuilderDockWindow("Traits", dock_id_h);
                            ImGui::DockBuilderDockWindow("Properties", dock_id_h);
                            ImGui::DockBuilderDockWindow("Settings", dock_id_h);
#if defined(PROTOTYPE_ENABLE_PROFILER)
                            ImGui::DockBuilderDockWindow("Profiler", dock_id_e);
#endif
                            ImGui::DockBuilderDockWindow("Colliders", dock_id_f);
                            ImGui::DockBuilderDockWindow("Materials", dock_id_f);
                            ImGui::DockBuilderDockWindow("Textures", dock_id_f);
                            ImGui::DockBuilderDockWindow("Shaders", dock_id_f);
                            ImGui::DockBuilderDockWindow("Meshes", dock_id_f);
                            ImGui::DockBuilderDockWindow("Scenes", dock_id_f);
                            ImGui::DockBuilderDockWindow("Framebuffers", dock_id_f);
                            ImGui::DockBuilderDockWindow("Console", dock_id_f);
                            ImGui::DockBuilderDockWindow("Scripts", dock_id_f);
                            ImGui::DockBuilderDockWindow("Blueprints", dock_id_f);
                        } break;
                    }
                }

                ImGui::DockBuilderFinish(dockspaceId);
                _freezeResizingViews = false;

                TEXT_BASE_WIDTH  = ImGui::CalcTextSize("A").x;
                TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
            }

            ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(PROTOTYPE_DARKERGRAY, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGuiID dockspaceId = ImGui::GetID("TheDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), 0);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        ImGui::End();
    }
    ImGui::PopStyleVar(4);

    static ImVec2 WindowPadding1 = ImVec2(1.0f, 1.0f);
    static ImVec2 WindowPadding2 = ImVec2(2.0f, 2.0f);
    static ImVec2 WindowPadding3 = ImVec2(3.0f, 3.0f);
    static ImVec2 WindowPadding4 = ImVec2(4.0f, 4.0f);
    static ImVec2 WindowPadding5 = ImVec2(5.0f, 5.0f);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding1);
    {
        if (ImGui::Begin("Hierarchy", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskHierarchy;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            static char* hierarchySearchBuff = &_recordedInstructions.sceneNodesSearchBuffer[0];
            // table
            {
                static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBodyUntilResize |
                                               ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_PreciseWidths;

                if (ImGui::BeginTable("##scene table", 1, flags)) {
                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is
                    // On
                    char buf[256];
                    snprintf(buf, sizeof(buf), " %s", PrototypeEngineInternalApplication::scene->name().c_str());
                    ImGui::TableSetupColumn(buf, 0);
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    if (ImGui::InputTextWithHint("##hierarchy search input text",
                                                 ICON_FA_SEARCH " Search for objects ...",
                                                 hierarchySearchBuff,
                                                 sizeof(_recordedInstructions.sceneNodesSearchBuffer))) {
                        scheduleRecordPass(PrototypeUiViewMaskHierarchy);
                    }

                    /*struct MySceneTreeNode
                    {
                        static void DisplayNode(const PrototypeOpenglUIInstructions::InternalSceneNode*              node,
                                                const std::vector<PrototypeOpenglUIInstructions::InternalSceneNode>& all_nodes,
                                                PrototypeUIState_&                                                   state,
                                                ImGuiListClipper&                                                    clipper,
                                                int&                                                                 row_n,
                                                bool& clipperHasSpace)
                        {
                            const bool is_folder = (node->ChildCount > 0);

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            if (is_folder) {
                                static ImGuiTreeNodeFlags base_flags =
                                  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                                  ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnDoubleClick;

                                bool open;
                                if (!node->isLayer) {
                                    if (node->node->isSelected()) {
                                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(PROTOTYPE_GREEN, 1.0f));
                                    } else {
                                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(PROTOTYPE_GRAY, 1.0f));
                                    }
                                    open = ImGui::TreeNodeEx(node->node->name().c_str(),
                                                             base_flags,
                                                             "%s [%i/%zu]",
                                                             node->node->name().c_str(),
                                                             node->ChildCount,
                                                             node->node->nodes().size());
                                    ImGui::PopStyleColor();
                                } else {
                                    open = ImGui::TreeNodeEx(node->layer->name().c_str(),
                                                             base_flags,
                                                             "%s [%i/%zu]",
                                                             node->layer->name().c_str(),
                                                             node->ChildCount,
                                                             node->layer->nodes().size());
                                }
                                if (open) {
                                    for (int child_n = 0; child_n < node->ChildCount; child_n++) {
                                        DisplayNode(
                                          &all_nodes[node->ChildIdx + child_n], all_nodes, state, clipper, ++row_n,
                    clipperHasSpace);
                                    }
                                    ImGui::TreePop();
                                }
                            } else {
                                ImGuiTreeNodeFlags nodeTreeNodeFlags =
                                  ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                                  ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                                if (!node->isLayer) {
                                    if (node->node->isSelected()) {
                                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(PROTOTYPE_GREEN, 1.0f));
                                    } else {
                                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(PROTOTYPE_GRAY, 1.0f));
                                    }
                                    bool open = ImGui::TreeNodeEx(
                                      node->node->name().c_str(), nodeTreeNodeFlags, "%s", node->node->name().c_str());
                                    ImGui::PopStyleColor();
                                    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                                        ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                                        ImVec2 dragDelta = ImGui::GetMouseDragDelta();
                                        if (dragDelta.x == 0.0f && dragDelta.y == 0.0f) {
                                            PrototypeEngineInternalApplication::scene->clearSelectedNodes();
                                            node->node->select();
                                        }
                                    }
                                    if (open) { ImGui::TreePop(); }
                                } else {
                                    if (ImGui::TreeNodeEx(
                                          node->layer->name().c_str(), nodeTreeNodeFlags, "%s", node->layer->name().c_str())) {
                                        ImGui::TreePop();
                                    }
                                }
                            }
                        }
                    };

                    if (!_recordedInstructions.sceneNodes.empty()) {
                        ImGuiListClipper clipper;
                        clipper.Begin(_recordedInstructions.sceneNodes.size());
                        bool clipperHasSpace = (clipper.Step());
                        int  row_n           = clipper.DisplayStart;
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(PROTOTYPE_LIGHTGRAY, 1.0f));
                        MySceneTreeNode::DisplayNode(&_recordedInstructions.sceneNodes[0],
                                                     _recordedInstructions.sceneNodes,
                                                     state,
                                                     clipper,
                                                     row_n,
                                                     clipperHasSpace);
                        ImGui::PopStyleColor();
                    }*/

                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is
                    // On

                    // Simple storage to output a dummy file-system.
                    struct MyTreeNode
                    {
                        static void DisplayNodeDetails(PrototypeSceneNode* node, PrototypeUIState_& state)
                        {
                            auto             optObj = node->object();
                            PrototypeObject* object = optObj.value();
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                            if (ImGui::BeginPopupContextItem(node->name().c_str())) {
                                if (ImGui::Selectable("Add child node")) {
                                    PrototypePipelineQueue queue = {};
                                    auto cmd = std::make_unique<PrototypePipelineCommand_shortcutEditorAddSceneNodeToNode>();
                                    cmd->parentNode = node;
                                    cmd->position   = glm::vec3(0.0f, 2.0f, 0.0f);
                                    cmd->rotation   = glm::vec3(0.0f, 0.0f, 0.0f);
                                    cmd->dir        = glm::vec3(0.0f, 0.0f, 0.0f);
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
                                ImGui::Separator();
                                for (MASK_TYPE itrait = 0; itrait < PrototypeTraitTypeCount; ++itrait) {
                                    if (ImGui::Selectable(((object->has(1ULL << itrait) ? ICON_FA_CHECK " " : "  ") +
                                                           PrototypeTraitTypeAbsoluteStringArray[itrait] + " trait")
                                                            .c_str())) {
                                        if (object->has(1ULL << itrait)) {
                                            PrototypePipelineQueue queue = {};
                                            auto                   cmd   = std::make_unique<
                                              PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                                            cmd->object    = object;
                                            cmd->traitMask = (1ULL << itrait);
                                            queue.record(std::move(cmd));
                                            PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                        } else {
                                            PrototypePipelineQueue queue = {};
                                            auto                   cmd   = std::make_unique<
                                              PrototypePipelineCommand_shortcutEditorSelectedSceneNodeAddTraits>();
                                            cmd->object    = object;
                                            cmd->traitMask = (1ULL << itrait);
                                            queue.record(std::move(cmd));
                                            PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                        }
                                    }
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::PopStyleVar();

                            // 0 left btn
                            // 1 right btn
                            // 2 middle btn
                            if (ImGui::IsMouseReleased(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                                ImVec2 dragDelta = ImGui::GetMouseDragDelta();
                                if (dragDelta.x == 0.0f && dragDelta.y == 0.0f) {
                                    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
                                    node->select();
                                }
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
                            ImGuiTreeNodeFlags nodeTreeNodeFlags =
                              ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth |
                              ImGuiTreeNodeFlags_ClipLabelForTrailingButton | ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
                              ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                            if (node->nodes().empty()) { nodeTreeNodeFlags |= ImGuiTreeNodeFlags_Leaf; }
                            if (node->isSelected()) { nodeTreeNodeFlags |= ImGuiTreeNodeFlags_Selected; }

                            bool open = false;
                            if (node->isMatching()) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                open = ImGui::TreeNodeEx(node->name().c_str(), nodeTreeNodeFlags, "%s", node->name().c_str());
                                DisplayNodeDetails(node, state);
                            } else if (node->isPartialMatching()) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(PROTOTYPE_GRAY, 1.0f));
                                open = ImGui::TreeNodeEx(node->name().c_str(), nodeTreeNodeFlags, "%s", node->name().c_str());
                                DisplayNodeDetails(node, state);
                                ImGui::PopStyleColor();
                            }

                            if (open) {
                                for (const auto& pair : node->nodes()) { MyTreeNode::DisplayNode(pair.second, state); }
                                ImGui::TreePop();
                            }
                        }
                        static void DisplayLayerDetails(PrototypeSceneLayer* layer, PrototypeUIState_& state)
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                            if (ImGui::BeginPopupContextItem(layer->name().c_str())) {
                                if (ImGui::Selectable("Add child node")) {
                                    PrototypePipelineQueue queue = {};
                                    auto cmd = std::make_unique<PrototypePipelineCommand_shortcutEditorAddSceneNodeToLayer>();
                                    cmd->parentLayer = layer;
                                    cmd->position    = glm::vec3(0.0f, 2.0f, 0.0f);
                                    cmd->rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
                                    cmd->dir         = glm::vec3(0.0f, 0.0f, 0.0f);
                                    queue.record(std::move(cmd));
                                    PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                                }
                                ImGui::EndPopup();
                            }
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
                              ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth |
                              ImGuiTreeNodeFlags_ClipLabelForTrailingButton | ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
                              ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                            const auto& nodes = layer->nodes();
                            if (nodes.empty()) { layerTreeNodeFlags |= ImGuiTreeNodeFlags_Leaf; }
                            bool open = false;
                            if (layer->isMatching()) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                open = ImGui::TreeNodeEx(layer->name().c_str(), layerTreeNodeFlags, "%s", layer->name().c_str());
                                DisplayLayerDetails(layer, state);
                            } else if (layer->isPartialMatching()) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(PROTOTYPE_GRAY, 1.0f));
                                open = ImGui::TreeNodeEx(layer->name().c_str(), layerTreeNodeFlags, "%s", layer->name().c_str());
                                DisplayLayerDetails(layer, state);
                                ImGui::PopStyleColor();
                            }
                            if (open) {
                                for (const auto& pair : nodes) { MyTreeNode::DisplayNode(pair.second, state); }
                                ImGui::TreePop();
                            }
                        }
                    };
                    const auto& layers = PrototypeEngineInternalApplication::scene->layers();
                    for (const auto& pair : layers) { MyTreeNode::DisplayLayer(pair.second, state); }
                    ImGui::EndTable();
                }
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskHierarchy;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    {
        if (ImGui::Begin("3D View", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMask3DView;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            state = _sceneView.onDrawFrame(state, _awesomeFontTitle);
        } else {
            _openViewsMask &= ~PrototypeUiViewMask3DView;
            state |= PrototypeUIState_Iconified;
        }
        ImGui::End();

        if (ImGui::Begin("3D Paint", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMask3DPaint;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMask3DPaint;
        }
        ImGui::End();

        if (ImGui::Begin("Game", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskGame;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskGame;
        }
        ImGui::End();

        if (ImGui::Begin("Texture Editor", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskTextureEditor;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskTextureEditor;
        }
        ImGui::End();

#if defined(PROTOTYPE_ENABLE_PROFILER)
        if (ImGui::Begin("Profiler", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskProfiler;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            auto&  timelineItems = PrototypeEngineInternalApplication::profiler->getTimelineItems();
            ImVec2 available     = ImGui::GetContentRegionAvail();
            ImVec2 graphSize(available.x, (available.y / timelineItems.size()) - (timelineItems.size() - 1));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
            {
                ImPlot::SetNextPlotLimitsX(-25, PROTOTYPE_PROFILER_BUFFER_LENGTH + 25, ImGuiCond_FirstUseEver);
                ImPlot::SetNextPlotLimitsY(0, 6000, ImGuiCond_FirstUseEver);
                if (ImPlot::BeginPlot("##RollingProfiler", NULL, "microseconds", ImGui::GetContentRegionAvail(), 0, 0, 0)) {
                    const auto& xaxis = PrototypeEngineInternalApplication::profiler->xaxis();
                    for (const auto& item : timelineItems) {
                        if (!item.second.isVisisble) continue;
                        ImPlot::PushStyleColor(ImPlotCol_Line, *((ImVec4*)&item.second.color.x));
                        // ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                        // ImPlot::PlotShaded(identifier.name, &xvalues[0], &yvalues[0], xvalues.size(), 0.0f);
                        // ImPlot::PopStyleVar();
                        ImPlot::PlotLine(
                          item.second.name.c_str(), &xaxis[0], &item.second.values[0], xaxis.size(), 0, sizeof(float));
                    }
                    ImPlot::EndPlot();
                }
            }
            ImGui::PopStyleVar();
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskProfiler;
        }
        ImGui::End();
#endif // #if defined(PROTOTYPE_ENABLE_PROFILER)
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(45.0f, 45.0f));
    {
        if (ImGui::Begin("Input Settings", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskInputSettings;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                                ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                                ImGuiTableFlags_PreciseWidths;

            const std::vector<std::pair<std::string, std::string>> mouseShortcuts = {
                { " Mouse Left click", "Select an object in Scene View" }, { " Mouse Right click + Drag Left", "Look Left" },
                { " Mouse Right click + Drag Right", "Look Right" },       { " Mouse Right click + Drag Up", "Look Up" },
                { " Mouse Right click + Drag Down", "Look Down" },
            };

            if (ImGui::BeginTable("##inputs mouse shortcuts table", 2, tableFlags)) {
                ImGui::TableSetupColumn(" " ICON_FA_MOUSE " Mouse", 0);
                ImGui::TableHeadersRow();
                ImGuiListClipper clipper;
                clipper.Begin(mouseShortcuts.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        const auto& pair = mouseShortcuts[row];
                        ImGui::Text("%s", pair.first.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", pair.second.c_str());
                    }
                }
                ImGui::EndTable();
            }
            ImGui::NewLine();

            const std::vector<std::pair<std::string, std::string>> keyboardShortcuts = {
                { " Left Shift + T", "Switch guizmo to translation mode" },
                { " Left Shift + S", "Switch guizmo to scaling mode" },
                { " Left Shift + R", "Switch guizmo to rotation mode" },
                { " Tab", "Switch guizmo between World/Local transformation" },
                { " Arrow Up", "Accelerate vehicle" },
                { " Arrow Down", "Brake vehicle" },
                { " Arrow Right", "Stear vehicle right" },
                { " Arrow Left", "Stear vehicle left" },
                { " G", "Toggle vehicle gears Backward/Forward" },
                { " 0", "Flip car" },
                { " 7", "Spawn car" },
                { " 1", "Spawn Sphere" },
                { " 2", "Spawn Cube" },
                { " 3", "Spawn icosphere" },
                { " 4", "Spawn monkey" },
                { " 5", "Spawn capsule" },
                { " 6", "Spawn cylinder" },
                { " W", "Move camera forward" },
                { " S", "Move camera backward" },
                { " D", "Move camera right" },
                { " A", "Move camera left" },
                { " i", "Move camera up" },
                { " k", "Move camera down" }
            };

            if (ImGui::BeginTable("##inputs keyboard shortcuts table", 2, tableFlags)) {
                ImGui::TableSetupColumn(" " ICON_FA_KEYBOARD " Keyboard", 0);
                ImGui::TableHeadersRow();
                ImGuiListClipper clipper;
                clipper.Begin(keyboardShortcuts.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        const auto& pair = keyboardShortcuts[row];
                        ImGui::Text("%s", pair.first.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", pair.second.c_str());
                    }
                }
                ImGui::EndTable();
            }
            ImGui::NewLine();

        } else {
            _openViewsMask &= ~PrototypeUiViewMaskInputSettings;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    {
        if (ImGui::Begin("Text Editor", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskTextEditor;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            static auto        current_shaderBuffer_item = PrototypeEngineInternalApplication::database->shaderBuffers.begin();
            static auto        current_shaderBuffer_type_item      = current_shaderBuffer_item->second->sources().begin();
            static const char* current_shaderBuffer_type_item_name = "Vertex Shader";
            static char        shader_code_buf[102400]             = { 0 };
            bool               value_changed                       = false;
            ImGui::PushFont((ImFont*)_awesomeFontTitle);
            if (ImGui::Button(ICON_FA_SAVE, ImVec2(25.0f, 25.0f))) {
                std::string code(shader_code_buf, strlen(shader_code_buf));
                PrototypeIo::writeFileBlock((*current_shaderBuffer_type_item)->fullpath.c_str(), code);
            }
            ImGui::PopFont();
            ImGui::SameLine();
            if (ImGui::BeginCombo("##shader files text editor", current_shaderBuffer_item->first.c_str(), ImGuiComboFlags_None)) {
                for (auto it = PrototypeEngineInternalApplication::database->shaderBuffers.begin();
                     it != PrototypeEngineInternalApplication::database->shaderBuffers.end();
                     ++it) {
                    ImGui::PushID((void*)it->first.c_str());
                    const bool  item_selected = (it == current_shaderBuffer_item);
                    const char* item_text;
                    if (ImGui::Selectable(it->first.c_str(), item_selected)) {
                        value_changed = true;
                        memset(shader_code_buf, 0, sizeof(shader_code_buf));
                        current_shaderBuffer_item      = it;
                        current_shaderBuffer_type_item = it->second->sources().begin();
                        switch ((*current_shaderBuffer_type_item)->type) {
                            case PrototypeShaderBufferSourceType_VertexShader:
                                current_shaderBuffer_type_item_name = "Vertex Shader";
                                break;
                            case PrototypeShaderBufferSourceType_FragmentShader:
                                current_shaderBuffer_type_item_name = "Fragment Shader";
                                break;
                            default: break;
                        }
                    }
                    if (item_selected) ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            if (ImGui::BeginCombo("##shader file type text editor", current_shaderBuffer_type_item_name, ImGuiComboFlags_None)) {
                for (auto it = current_shaderBuffer_item->second->sources().begin();
                     it != current_shaderBuffer_item->second->sources().end();
                     ++it) {
                    ImGui::PushID((void*)(*it)->type);
                    const bool  item_selected = (current_shaderBuffer_type_item == it);
                    const char* item_text;
                    switch ((*it)->type) {
                        case PrototypeShaderBufferSourceType_VertexShader: item_text = "Vertex Shader"; break;
                        case PrototypeShaderBufferSourceType_FragmentShader: item_text = "Fragment Shader"; break;
                        default: break;
                    }
                    if (ImGui::Selectable(item_text, item_selected)) {
                        value_changed = true;
                        memset(shader_code_buf, 0, sizeof(shader_code_buf));
                        current_shaderBuffer_type_item      = it;
                        current_shaderBuffer_type_item_name = item_text;
                    }
                    if (item_selected) ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            if (shader_code_buf[0] == '\0') {
                std::string code;
                PrototypeIo::readFileBlock((*current_shaderBuffer_type_item)->fullpath.c_str(), code);
                memcpy(shader_code_buf, code.c_str(), code.size());
            }
            ImGui::InputTextMultiline("##shader multiline input",
                                      shader_code_buf,
                                      sizeof(shader_code_buf),
                                      ImGui::GetContentRegionAvail(),
                                      ImGuiInputTextFlags_AllowTabInput);
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskTextEditor;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Traits", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskTraits;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            const auto& selectedNodes = PrototypeEngineInternalApplication::scene->selectedNodes();
            if (!selectedNodes.empty()) {
                PrototypeSceneNode*     first  = *selectedNodes.begin();
                static PrototypeObject* object = nullptr;
                object                         = first->object().value();

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                if (ImGui::BeginPopupContextWindow(first->name().c_str())) {
                    if (ImGui::Selectable("Add child node")) {
                        PrototypePipelineQueue queue = {};
                        auto cmd        = std::make_unique<PrototypePipelineCommand_shortcutEditorAddSceneNodeToNode>();
                        cmd->parentNode = first;
                        cmd->position   = glm::vec3(0.0f, 2.0f, 0.0f);
                        cmd->rotation   = glm::vec3(0.0f, 0.0f, 0.0f);
                        cmd->dir        = glm::vec3(0.0f, 0.0f, 0.0f);
                        queue.record(std::move(cmd));
                        PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                    }
                    if (ImGui::Selectable("Delete")) {
                        PrototypePipelineQueue queue = {};
                        auto                   cmd   = std::make_unique<PrototypePipelineCommand_shortcutEditorRemoveSceneNode>();
                        cmd->node                    = first;
                        queue.record(std::move(cmd));
                        PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                    }
                    ImGui::Separator();
                    for (MASK_TYPE itrait = 0; itrait < PrototypeTraitTypeCount; ++itrait) {
                        if (ImGui::Selectable(((object->has(1ULL << itrait) ? ICON_FA_CHECK " " : "  ") +
                                               PrototypeTraitTypeAbsoluteStringArray[itrait] + " trait")
                                                .c_str())) {
                            if (object->has(1ULL << itrait)) {
                                PrototypePipelineQueue queue = {};
                                auto                   cmd =
                                  std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeRemoveTraits>();
                                cmd->object    = object;
                                cmd->traitMask = (1ULL << itrait);
                                queue.record(std::move(cmd));
                                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                            } else {
                                PrototypePipelineQueue queue = {};
                                auto cmd = std::make_unique<PrototypePipelineCommand_shortcutEditorSelectedSceneNodeAddTraits>();
                                cmd->object    = object;
                                cmd->traitMask = (1ULL << itrait);
                                queue.record(std::move(cmd));
                                PrototypePipelines::shortcutsQueue.push_back(std::move(queue));
                            }
                        }
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();

                static char nodeNameBuffer[128];
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
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskTraits;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Properties", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskProperties;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            switch (_assetConfigSelection.type) {
                case PrototypeConfigurationSelectionAssetType_Mesh: {
                    if (_assetConfigSelection.mesh) {
                        static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                                            ImGuiTableFlags_NoBordersInBodyUntilResize |
                                                            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_PreciseWidths;
                        if (ImGui::BeginTable("mesh configuration content table", 2, tableFlags)) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("name");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", _assetConfigSelection.mesh->name.c_str());

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("type");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%i", _assetConfigSelection.mesh->type);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Indices count");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%lu", _assetConfigSelection.mesh->indexCount);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("vertex array object");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%lu", _assetConfigSelection.mesh->vao);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("vertex buffer object");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%lu", _assetConfigSelection.mesh->vbo);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Element array buffer object");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%lu", _assetConfigSelection.mesh->eabo);

                            ImGui::EndTable();
                        }
                    }
                } break;

                case PrototypeConfigurationSelectionAssetType_Shader: {
                    if (_assetConfigSelection.shader) {
                        static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                                            ImGuiTableFlags_NoBordersInBodyUntilResize |
                                                            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_PreciseWidths;
                        if (ImGui::BeginTable("shader configuration content table", 2, tableFlags)) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("name");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", _assetConfigSelection.shader->name.c_str());

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("program");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%lu", _assetConfigSelection.shader->program);

                            for (const auto& texture : _assetConfigSelection.shader->textureData) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("2D Texture");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextUnformatted(texture.c_str());
                            }

                            for (const auto& pair : _assetConfigSelection.shader->floatData) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%f", pair.second);
                            }

                            for (const auto& pair : _assetConfigSelection.shader->vec2Data) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%f, %f", pair.second.r, pair.second.g);
                            }

                            for (const auto& pair : _assetConfigSelection.shader->vec3Data) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%f, %f, %f", pair.second.r, pair.second.g, pair.second.b);
                            }

                            for (const auto& pair : _assetConfigSelection.shader->vec4Data) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%f, %f, %f, %f", pair.second.r, pair.second.g, pair.second.b, pair.second.a);
                            }

                            ImGui::EndTable();
                        }
                    }
                } break;

                case PrototypeConfigurationSelectionAssetType_Texture: {
                    if (_assetConfigSelection.texture) {
                        static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                                            ImGuiTableFlags_NoBordersInBodyUntilResize |
                                                            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_PreciseWidths;
                        if (ImGui::BeginTable("texture configuration content table", 2, tableFlags)) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("name");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", _assetConfigSelection.texture->name.c_str());

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("type");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%i", _assetConfigSelection.texture->type);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("id");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%lu", _assetConfigSelection.texture->id);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("width");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%zu", _assetConfigSelection.texture->width);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("height");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%zu", _assetConfigSelection.texture->height);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("internal format");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%i", _assetConfigSelection.texture->internalFormat);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("format");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%i", _assetConfigSelection.texture->format);

                            ImGui::EndTable();
                        }
                    }
                } break;

                case PrototypeConfigurationSelectionAssetType_Material: {
                    if (_assetConfigSelection.material) {
                        static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                                            ImGuiTableFlags_NoBordersInBodyUntilResize |
                                                            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_PreciseWidths;
                        if (ImGui::BeginTable("material configuration content table", 2, tableFlags)) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("name");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", _assetConfigSelection.material->name.c_str());

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("shader");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                            ImGui::Button((char*)_assetConfigSelection.material->shader->name.c_str(),
                                          ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SHADER")) {
                                    IM_ASSERT(payload->DataSize == sizeof(PglShader*));
                                    PglShader* shader                      = *(PglShader**)payload->Data;
                                    size_t     originalSize                = _assetConfigSelection.material->textureData.size();
                                    _assetConfigSelection.material->shader = shader;
                                    ((PrototypeOpenglRenderer*)PrototypeEngineInternalApplication::renderer)
                                      ->onMaterialShaderUpdate(_assetConfigSelection.material, shader);
                                    _assetConfigSelection.material->textures.resize(
                                      _assetConfigSelection.material->textureData.size());
                                    _assetConfigSelection.material->textures.shrink_to_fit();
                                    for (size_t s = originalSize; s < _assetConfigSelection.material->textures.size(); ++s) {
                                        PrototypeEngineInternalApplication::renderer->fetchDefaultTexture(
                                          (void**)&_assetConfigSelection.material->textures[s]);
                                    }
                                    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                                }
                                ImGui::EndDragDropTarget();
                            }
                            ImGui::PopStyleColor(4);
                            ImGui::PopStyleVar();

                            for (size_t t = 0; t < _assetConfigSelection.material->textures.size(); ++t) {
                                auto tdata = _assetConfigSelection.material->textureData.begin();
                                std::advance(tdata, t);
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("%s", (*tdata).c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GRAY, 1.0f));
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(PROTOTYPE_DARKESTGRAY, 0.0f));
                                ImGui::Button((char*)_assetConfigSelection.material->textures[t]->name.c_str(),
                                              ImVec2(ImGui::GetContentRegionAvailWidth(), 30.0f));
                                if (ImGui::BeginDragDropTarget()) {
                                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE")) {
                                        IM_ASSERT(payload->DataSize == sizeof(PglTexture*));
                                        PglTexture* texture                         = *(PglTexture**)payload->Data;
                                        _assetConfigSelection.material->textures[t] = texture;
                                        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
                                    }
                                    ImGui::EndDragDropTarget();
                                }
                                ImGui::PopStyleColor(4);
                                ImGui::PopStyleVar();
                            }

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Base Color");
                            ImGui::TableSetColumnIndex(1);
                            if (ImGui::ColorEdit3("##Base Color",
                                                  &_assetConfigSelection.material->baseColor[0],
                                                  ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar |
                                                    ImGuiColorEditFlags_DisplayRGB)) {}

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Metallic");
                            ImGui::TableSetColumnIndex(1);
                            if (ImGui::DragFloat("##Metallic", &_assetConfigSelection.material->metallic, 0.01f, 0.0f, 1.0f)) {
                                glm::clamp(_assetConfigSelection.material->metallic, 0.0f, 1.0f);
                            }

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Roughness");
                            ImGui::TableSetColumnIndex(1);
                            if (ImGui::DragFloat("##Roughness", &_assetConfigSelection.material->roughness, 0.01f, 0.0f, 1.0f)) {
                                glm::clamp(_assetConfigSelection.material->roughness, 0.0f, 1.0f);
                            }

                            // VALUES
                            for (auto& pair : _assetConfigSelection.material->floatData) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::DragFloat(fmt::format("##{}", pair.first).c_str(), &pair.second);
                            }
                            for (auto& pair : _assetConfigSelection.material->vec2Data) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::DragFloat2(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
                            }
                            for (auto& pair : _assetConfigSelection.material->vec3Data) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::DragFloat3(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
                            }
                            for (auto& pair : _assetConfigSelection.material->vec4Data) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(pair.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::DragFloat4(fmt::format("##{}", pair.first).c_str(), &pair.second[0]);
                            }

                            ImGui::EndTable();
                        }
                    }

                } break;

                case PrototypeConfigurationSelectionAssetType_None:
                case PrototypeConfigurationSelectionAssetType_Count:
                default: break;
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskProperties;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Settings", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskSettings;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            static char searchBuffer[128];
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint(
                  "##settings search input text", ICON_FA_FILTER " Filter settings ...", searchBuffer, sizeof(searchBuffer))) {}
            ImGui::NewLine();

            static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                                ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                                ImGuiTableFlags_PreciseWidths;

#if defined(PROTOTYPE_ENABLE_PROFILER)
            static bool isProfilerOpen = false;
            ImGuiHelpers::BeginGroupPanel("Profiler", ImVec2(-1.0f, 0.0f), isProfilerOpen, _awesomeFont);
            if (isProfilerOpen) {
                ImGui::Indent(16.0f);
                auto& timelineItems = PrototypeEngineInternalApplication::profiler->getTimelineItems();
                for (auto& item : timelineItems) {
                    ImGui::PushID(item.first.c_str());
                    {
                        ImGuiHelpers::BeginGroupPanel(item.first.c_str(), ImVec2(-1.0f, 0.0f), item.second.isOpen, _awesomeFont);
                        if (item.second.isOpen) {
                            ImGui::Indent(16.0f);
                            if (ImGui::BeginTable("##profiler content table", 2, tableFlags)) {
                                {
                                    ImGui::TableNextRow();
                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::TextUnformatted("visible");
                                    ImGui::TableSetColumnIndex(1);
                                    ImGui::Checkbox("##visible", &item.second.isVisisble);
                                }
                                {
                                    ImGui::TableNextRow();
                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::TextUnformatted("color");
                                    ImGui::TableSetColumnIndex(1);
                                    ImGui::ColorEdit4("##color",
                                                      &item.second.color.x,
                                                      ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar |
                                                        ImGuiColorEditFlags_DisplayRGB);
                                }
                                ImGui::EndTable();
                            }
                            ImGui::Unindent(16.0f);
                            ImGuiHelpers::EndGroupPanel();
                        }
                    }
                    ImGui::PopID();
                }
                ImGui::Unindent(16.0f);
                ImGuiHelpers::EndGroupPanel();
            }
#endif // #if defined(PROTOTYPE_ENABLE_PROFILER)

            static bool isThemeOpen = false;
            ImGuiHelpers::BeginGroupPanel("Theme", ImVec2(-1.0f, 0.0f), isThemeOpen, _awesomeFont);
            if (isThemeOpen) {
                ImGui::Indent(16.0f);
                if (ImGui::BeginTable("##settings color content table", 2, tableFlags)) {
                    ImGuiStyle& style = ImGui::GetStyle();

                    static const std::map<std::string, std::pair<std::string, bool*>> boolProperties = {
                        { "Anti-aliased lines", { "##UI [Bool] Anti-aliased lines", &style.AntiAliasedLines } },
                        { "Anti-aliased lines use tex",
                          { "##UI [Bool] Anti-aliased lines use tex", &style.AntiAliasedLinesUseTex } },
                        { "Anti-aliased fill", { "##UI [Bool] Anti-aliased fill", &style.AntiAliasedFill } }
                    };

                    static const std::map<std::string, std::pair<std::string, float*>> floatProperties = {
                        { "Curve tessellation tol", { "##UI [Float] Curve tessellation tol", &style.CurveTessellationTol } },
                        { "Circle segment max error", { "##UI [Float] Circle segment max error", &style.CircleSegmentMaxError } },
                        { "Child rounding", { "##UI [Float] Child rounding", &style.ChildRounding } },
                        { "Window rounding", { "##UI [Float] Window rounding", &style.WindowRounding } },
                        { "Grab rounding", { "##UI [Float] Grab rounding", &style.GrabRounding } },
                        { "Scrollbar rounding", { "##UI [Float] Scrollbar rounding", &style.ScrollbarRounding } },
                        { "Scrollbar size", { "##UI [Float] Scrollbar size", &style.ScrollbarSize } },
                        { "Frame rounding", { "##UI [Float] Frame rounding", &style.FrameRounding } },
                        { "Tab rounding", { "##UI [Float] Tab rounding", &style.TabRounding } },
                        { "Tab border size", { "##UI [Float] Tab border size", &style.TabBorderSize } }
                    };

                    static const std::map<std::string, std::pair<std::string, float*>> vec2Properties = {
                        { "Window title align", { "##UI [Vec2] Window title align", &style.WindowTitleAlign.x } }
                    };

                    static const std::map<std::string, std::pair<std::string, float*>> vec4Properties = {
                        { "Button", { "##UI [Vec4] Button", &style.Colors[ImGuiCol_Button].x } },
                        { "Button hovered", { "##UI [Vec4] Button hovered", &style.Colors[ImGuiCol_ButtonHovered].x } },
                        { "Button active", { "##UI [Vec4] Button active", &style.Colors[ImGuiCol_ButtonActive].x } },
                        { "Text", { "##UI [Vec4] Text", &style.Colors[ImGuiCol_Text].x } },
                        { "Text selected bg", { "##UI [Vec4] Text selected bg", &style.Colors[ImGuiCol_TextSelectedBg].x } },
                        { "Text disabled", { "##UI [Vec4] Text disabled", &style.Colors[ImGuiCol_TextDisabled].x } },
                        { "Window bg", { "##UI [Vec4] Window bg", &style.Colors[ImGuiCol_WindowBg].x } },
                        { "Child bg", { "##UI [Vec4] Child bg", &style.Colors[ImGuiCol_ChildBg].x } },
                        { "Popup bg", { "##UI [Vec4] Popup bg", &style.Colors[ImGuiCol_PopupBg].x } },
                        { "Border", { "##UI [Vec4] Border", &style.Colors[ImGuiCol_Border].x } },
                        { "Border shadow", { "##UI [Vec4] Border shadow", &style.Colors[ImGuiCol_BorderShadow].x } },
                        { "Frame bg", { "##UI [Vec4] Frame bg", &style.Colors[ImGuiCol_FrameBg].x } },
                        { "Frame bg hovered", { "##UI [Vec4] Frame bg hovered", &style.Colors[ImGuiCol_FrameBgHovered].x } },
                        { "Frame bg active", { "##UI [Vec4] Frame bg active", &style.Colors[ImGuiCol_FrameBgActive].x } },
                        { "Title bg", { "##UI [Vec4] Title bg", &style.Colors[ImGuiCol_TitleBg].x } },
                        { "Title bg collapsed",
                          { "##UI [Vec4] Title bg collapsed", &style.Colors[ImGuiCol_TitleBgCollapsed].x } },
                        { "Title bg active", { "##UI [Vec4] Title bg active", &style.Colors[ImGuiCol_TitleBgActive].x } },
                        { "Menubar bg", { "##UI [Vec4] Menubar bg", &style.Colors[ImGuiCol_MenuBarBg].x } },
                        { "Tab", { "##UI [Vec4] Tab", &style.Colors[ImGuiCol_Tab].x } },
                        { "Tab unfocused", { "##UI [Vec4] Tab unfocused", &style.Colors[ImGuiCol_TabUnfocused].x } },
                        { "Tab hovered", { "##UI [Vec4] Tab hovered", &style.Colors[ImGuiCol_TabHovered].x } },
                        { "Tab active", { "##UI [Vec4] Tab active", &style.Colors[ImGuiCol_TabActive].x } },
                        { "Tab unfocused active",
                          { "##UI [Vec4] Tab unfocused active", &style.Colors[ImGuiCol_TabUnfocusedActive].x } },
                        { "Scrollbar bg", { "##UI [Vec4] Scrollbar bg", &style.Colors[ImGuiCol_ScrollbarBg].x } },
                        { "Scrollbar grab", { "##UI [Vec4] Scrollbar grab", &style.Colors[ImGuiCol_ScrollbarGrab].x } },
                        { "Scrollbar grab hovered",
                          { "##UI [Vec4] Scrollbar grab hovered", &style.Colors[ImGuiCol_ScrollbarGrabHovered].x } },
                        { "Scrollbar grab active",
                          { "##UI [Vec4] Scrollbar grab active", &style.Colors[ImGuiCol_ScrollbarGrabActive].x } },
                        { "Header", { "##UI [Vec4] Header", &style.Colors[ImGuiCol_Header].x } },
                        { "Header hovered", { "##UI [Vec4] Header hovered", &style.Colors[ImGuiCol_HeaderHovered].x } },
                        { "Header active", { "##UI [Vec4] Header active", &style.Colors[ImGuiCol_HeaderActive].x } },
                        { "Plot lines", { "##UI [Vec4] Plot lines", &style.Colors[ImGuiCol_PlotLines].x } },
                        { "Plot lines hovered",
                          { "##UI [Vec4] Plot lines hovered", &style.Colors[ImGuiCol_PlotLinesHovered].x } },
                        { "Plot histogram", { "##UI [Vec4] Plot histogram", &style.Colors[ImGuiCol_PlotHistogram].x } },
                        { "Plot histogram hovered",
                          { "##UI [Vec4] Plot histogram hovered", &style.Colors[ImGuiCol_PlotHistogramHovered].x } },
                        { "Resize grip", { "##UI [Vec4] Resize grip", &style.Colors[ImGuiCol_ResizeGrip].x } },
                        { "Resize grip hovered",
                          { "##UI [Vec4] Resize grip hovered", &style.Colors[ImGuiCol_ResizeGripHovered].x } },
                        { "Resize grip active", { "##UI [Vec4] Resize grip active", &style.Colors[ImGuiCol_ResizeGripActive].x } }
                    };

                    for (const auto& pair : boolProperties) {
                        if (pair.first.find(searchBuffer) != std::string::npos) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(pair.first.c_str());
                            ImGui::TableSetColumnIndex(1);
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                            ImGui::Checkbox(pair.second.first.c_str(), pair.second.second);
                        }
                    }

                    for (const auto& pair : floatProperties) {
                        if (pair.first.find(searchBuffer) != std::string::npos) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(pair.first.c_str());
                            ImGui::TableSetColumnIndex(1);
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                            ImGui::DragFloat(pair.second.first.c_str(), pair.second.second);
                        }
                    }

                    for (const auto& pair : vec2Properties) {
                        if (pair.first.find(searchBuffer) != std::string::npos) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(pair.first.c_str());
                            ImGui::TableSetColumnIndex(1);
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                            ImGui::DragFloat2(pair.second.first.c_str(), pair.second.second);
                        }
                    }

                    for (const auto& pair : vec4Properties) {
                        if (pair.first.find(searchBuffer) != std::string::npos) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(pair.first.c_str());
                            ImGui::TableSetColumnIndex(1);
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                            ImGui::ColorEdit4(pair.second.first.c_str(),
                                              pair.second.second,
                                              ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar |
                                                ImGuiColorEditFlags_DisplayRGB);
                        }
                    }

                    ImGui::EndTable();
                }
                ImGui::Unindent(16.0f);
                ImGuiHelpers::EndGroupPanel();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskSettings;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Scenes", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskScenes;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
            if (ImGui::BeginTable("##scenes table", 1, flags)) {
                for (const auto& pair : PrototypeEngineInternalApplication::database->scenes) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::Text("%s", pair.first.c_str());
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        PrototypeEngineInternalApplication::renderer->switchScenes(pair.second);
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskScenes;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Meshes", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskMeshes;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            float posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##meshes search input text",
                                         ICON_FA_FILTER " Filter meshes ...",
                                         _recordedInstructions.meshesSearchBuff,
                                         sizeof(_recordedInstructions.meshesSearchBuff))) {
                scheduleRecordPass(PrototypeUiViewMaskMeshes);
            }
            ImGui::TextColored(ImVec4(PROTOTYPE_GREEN, 1.0f), "%zu items", _recordedInstructions.meshes.size());

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
            if (ImGui::BeginTable("##meshes table", 1, flags)) {
                ImGuiListClipper clipper;
                clipper.Begin(_recordedInstructions.meshes.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        PglGeometry* mesh = _recordedInstructions.meshes[row];
                        ImGui::Text("%s", mesh->name.c_str());
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            _assetConfigSelection.mesh = mesh;
                            _assetConfigSelection.type = PrototypeConfigurationSelectionAssetType_Mesh;
                        }
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_MESH", &mesh, sizeof(PglGeometry*));
                            ImGui::Text("%s", mesh->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskMeshes;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Shaders", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskShaders;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            float posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##shaders search input text",
                                         ICON_FA_FILTER " Filter shaders ...",
                                         _recordedInstructions.shadersSearchBuff,
                                         sizeof(_recordedInstructions.shadersSearchBuff))) {
                scheduleRecordPass(PrototypeUiViewMaskShaders);
            }
            ImGui::TextColored(ImVec4(PROTOTYPE_GREEN, 1.0f), "%zu items", _recordedInstructions.shaders.size());

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

            if (ImGui::BeginTable("##shaders table", 1, flags)) {
                ImGuiListClipper clipper;
                clipper.Begin(_recordedInstructions.shaders.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        PglShader* shader = _recordedInstructions.shaders[row];

                        ImGui::Text("%s", shader->name.c_str());
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            _assetConfigSelection.shader = shader;
                            _assetConfigSelection.type   = PrototypeConfigurationSelectionAssetType_Shader;
                        }
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_SHADER", &shader, sizeof(PglShader*));
                            ImGui::Text("%s", shader->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskShaders;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Textures", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskTextures;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            float posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##textures search input text",
                                         ICON_FA_FILTER " Filter textures ...",
                                         _recordedInstructions.texturesSearchBuff,
                                         sizeof(_recordedInstructions.texturesSearchBuff))) {
                scheduleRecordPass(PrototypeUiViewMaskTextures);
            }
            ImGui::TextColored(ImVec4(PROTOTYPE_GREEN, 1.0f), "%zu items", _recordedInstructions.textures.size());

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
            if (ImGui::BeginTable("##textures table", 1, flags)) {
                ImGuiListClipper clipper;
                clipper.Begin(_recordedInstructions.textures.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        PglTexture* texture = _recordedInstructions.textures[row];

                        ImGui::Text("%s", texture->name.c_str());
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            _assetConfigSelection.texture = texture;
                            _assetConfigSelection.type    = PrototypeConfigurationSelectionAssetType_Texture;
                        }
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_TEXTURE", &texture, sizeof(PglTexture*));
                            ImGui::Text("%s", texture->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskTextures;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Materials", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskMaterials;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            if (ImGui::BeginPopupContextWindow("##materials popup context menu")) {
                if (ImGui::Selectable("Add new material")) {
                    static std::vector<const char*> shadersCombo;
                    static std::vector<const char*> texturesCombo;
                    shadersCombo.clear();
                    texturesCombo.clear();
                    for (const auto& shaderPair : PrototypeEngineInternalApplication::database->shaderBuffers) {
                        shadersCombo.push_back(shaderPair.first.c_str());
                    }
                    for (const auto& texturePair : PrototypeEngineInternalApplication::database->textureBuffers) {
                        texturesCombo.push_back(texturePair.first.c_str());
                    }
                    PrototypeUiMaterialForm form = {};
                    auto shaderIt = PrototypeEngineInternalApplication::database->shaderBuffers.find(shadersCombo[form.shader()]);
                    if (shaderIt != PrototypeEngineInternalApplication::database->shaderBuffers.end()) {
                        size_t originalSize = shaderIt->second->sources()[1]->bindingSource.textureData.size();
                        form.textures().resize(shaderIt->second->sources()[1]->bindingSource.textureData.size());
                        for (size_t i = originalSize; i < form.textures().size(); ++i) { form.textures()[i] = 0; }
                    }
                    form.configure([&](PrototypeUiMaterialForm& thisForm) {
                        ImGui::PushID(_materialforms.size());
                        ImGui::Text("Material name");
                        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(PROTOTYPE_LIGHTGRAY, 1.0f));
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                        ImGui::InputText("##Add new material", thisForm.name(), thisForm.sizeofName());
                        ImGui::PopStyleColor();
                        if (ImGui::Combo(
                              "##add new material shader combo", &thisForm.shader(), shadersCombo.data(), shadersCombo.size())) {
                            auto shaderIt =
                              PrototypeEngineInternalApplication::database->shaderBuffers.find(shadersCombo[thisForm.shader()]);
                            if (shaderIt != PrototypeEngineInternalApplication::database->shaderBuffers.end()) {
                                size_t originalSize = shaderIt->second->sources()[1]->bindingSource.textureData.size();
                                thisForm.textures().resize(shaderIt->second->sources()[1]->bindingSource.textureData.size());
                                for (size_t i = originalSize; i < thisForm.textures().size(); ++i) { thisForm.textures()[i] = 0; }
                            }
                        }
                        for (size_t i = 0; i < thisForm.textures().size(); ++i) {
                            ImGui::PushID(i);
                            if (ImGui::Combo("##add new material texture combo",
                                             &thisForm.textures()[i],
                                             texturesCombo.data(),
                                             texturesCombo.size())) {}
                            ImGui::PopID();
                        }
                        if (ImGui::ColorEdit3("##Add new material base Color",
                                              &thisForm.baseColor().x,
                                              ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar |
                                                ImGuiColorEditFlags_DisplayRGB)) {}
                        if (ImGui::DragFloat("##Metallic", &thisForm.metallic(), 0.01f, 0.0f, 1.0f)) {
                            glm::clamp(thisForm.metallic(), 0.0f, 1.0f);
                        }
                        if (ImGui::DragFloat("##Roughness", &thisForm.roughness(), 0.01f, 0.0f, 1.0f)) {
                            glm::clamp(thisForm.roughness(), 0.0f, 1.0f);
                        }
                        if (ImGui::Button("Create")) {
                            std::vector<std::string> namedTextures(thisForm.textures().size());
                            for (size_t i = 0; i < namedTextures.size(); ++i) {
                                namedTextures[i] = texturesCombo[thisForm.textures()[i]];
                            }
                            nlohmann::json j = { { "name", std::string(thisForm.name()) },
                                                 { "shader", std::string(shadersCombo[thisForm.shader()]) },
                                                 { "textures", namedTextures },
                                                 { "baseColor",
                                                   { thisForm.baseColor().x, thisForm.baseColor().y, thisForm.baseColor().z } },
                                                 { "metallic", thisForm.metallic() },
                                                 { "roughness", thisForm.roughness() } };
                            PrototypeMaterial::from_json(j);
                            auto materialIt =
                              PrototypeEngineInternalApplication::database->materials.find(std::string(thisForm.name()));
                            if (materialIt != PrototypeEngineInternalApplication::database->materials.end()) {
                                PrototypeEngineInternalApplication::renderer->mapPrototypeMaterial(materialIt->second);
                                scheduleRecordPass(PrototypeUiViewMaskMaterials);
                            }
                            ImGui::PopID();
                            return true;
                        }
                        ImGui::PopID();
                        return false;
                    });
                    _materialforms.push(std::move(form));
                }
                ImGui::EndPopup();
            }

            float posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##materials search input text",
                                         ICON_FA_FILTER " Filter materials ...",
                                         _recordedInstructions.materialsSearchBuff,
                                         sizeof(_recordedInstructions.materialsSearchBuff))) {
                scheduleRecordPass(PrototypeUiViewMaskMaterials);
            }
            ImGui::TextColored(ImVec4(PROTOTYPE_GREEN, 1.0f), "%zu items", _recordedInstructions.materials.size());

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

            if (ImGui::BeginTable("##materials table", 1, flags)) {
                ImGuiListClipper clipper;
                clipper.Begin(_recordedInstructions.materials.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        PglMaterial* material = _recordedInstructions.materials[row];

                        ImGui::Text("%s", material->name.c_str());
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            _assetConfigSelection.material = material;
                            _assetConfigSelection.type     = PrototypeConfigurationSelectionAssetType_Material;
                        }
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_MATERIAL", &material, sizeof(PglMaterial*));
                            ImGui::Text("%s", material->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskMaterials;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Framebuffers", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskFramebuffers;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            static char framebuffersSearchBuff[128];
            float       posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##framebuffers search input text",
                                         ICON_FA_FILTER " Filter framebuffers ...",
                                         framebuffersSearchBuff,
                                         sizeof(framebuffersSearchBuff))) {}

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

            if (ImGui::BeginTable("##framebuffers table", 1, flags)) {
                for (const auto& pair : PrototypeEngineInternalApplication::database->framebuffers) {
                    size_t matchIndex = pair.first.find(framebuffersSearchBuff);
                    if (matchIndex != std::string::npos) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        ImGui::Text("%s", pair.first.substr(0, matchIndex).c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            PglFramebuffer* framebuffer;
                            PrototypeEngineInternalApplication::renderer->fetchFramebuffer(pair.first, (void**)&framebuffer);
                            ImGui::SetDragDropPayload("_FRAMEBUFFER", &framebuffer, sizeof(PglFramebuffer*));
                            ImGui::Text("%s", pair.first.c_str());
                            ImGui::EndDragDropSource();
                        }
                        ImGui::SameLine(0.0f, 0.0f);
                        ImGui::TextColored(ImVec4(PROTOTYPE_BLUE, 1.0f),
                                           "%s",
                                           pair.first.substr(matchIndex, strlen(framebuffersSearchBuff)).c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            PglFramebuffer* framebuffer;
                            PrototypeEngineInternalApplication::renderer->fetchFramebuffer(pair.first, (void**)&framebuffer);
                            ImGui::SetDragDropPayload("_FRAMEBUFFER", &framebuffer, sizeof(PglFramebuffer*));
                            ImGui::Text("%s", pair.first.c_str());
                            ImGui::EndDragDropSource();
                        }
                        ImGui::SameLine(0.0f, 0.0f);
                        ImGui::Text("%s", pair.first.substr(matchIndex + strlen(framebuffersSearchBuff)).c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            PglFramebuffer* framebuffer;
                            PrototypeEngineInternalApplication::renderer->fetchFramebuffer(pair.first, (void**)&framebuffer);
                            ImGui::SetDragDropPayload("_FRAMEBUFFER", &framebuffer, sizeof(PglFramebuffer*));
                            ImGui::Text("%s", pair.first.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskFramebuffers;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Console", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskConsole;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            static char consoleSearchBuff[128];
            float       posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - 70.0f);
            if (ImGui::InputTextWithHint("##console search input text",
                                         ICON_FA_FILTER " Filter logs ...",
                                         consoleSearchBuff,
                                         sizeof(consoleSearchBuff))) {}
            ImGui::SameLine();
            ImGui::SetCursorPosX(posx - 55.0f);
            if (ImGui::Button("Clear##console logs")) { PrototypeLogger::data()->logs.clear(); }

            {
                static const float TEXT_BASE_WIDTH  = ImGui::CalcTextSize("A").x;
                static const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

                static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

                if (ImGui::BeginTable("##console table", 1, flags)) {
                    for (const auto& refLog : PrototypeLogger::data()->logs) {
                        size_t matchIndex = refLog.text.find(consoleSearchBuff);
                        if (matchIndex != std::string::npos) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(ImVec4(PROTOTYPE_GREEN, 1.0f), "[LOG] ");
                            ImGui::SameLine();
                            ImGui::Text("%s", refLog.text.substr(0, matchIndex).c_str());
                            ImGui::SameLine(0.0f, 0.0f);
                            ImGui::TextColored(ImVec4(PROTOTYPE_BLUE, 1.0f),
                                               "%s",
                                               refLog.text.substr(matchIndex, strlen(consoleSearchBuff)).c_str());
                            ImGui::SameLine(0.0f, 0.0f);
                            ImGui::Text("%s", refLog.text.substr(matchIndex + strlen(consoleSearchBuff)).c_str());
                            if (ImGui::IsItemHovered()) {
                                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_LIGHTERGRAY, 1.0f));
                                ImGui::SetTooltip("%s:%i", refLog.filepath.c_str(), refLog.line);
                                ImGui::PopStyleColor();
                                ImGui::PopStyleVar();
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskConsole;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Scripts", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskScripts;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
            static PrototypeUiPrompt pr = {};
            static char              scriptsSearchBuff[128];
            float                    posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##scripts search input text",
                                         ICON_FA_FILTER " Filter scripts ...",
                                         scriptsSearchBuff,
                                         sizeof(scriptsSearchBuff))) {}
            if (ImGui::BeginPopupContextWindow("##scripts popup context menu")) {
                if (ImGui::Selectable("Add new C++ script")) {
                    pr.configure(
                      "Create a new C++ script",
                      "Pick a name for your C++ script",
                      "Create",
                      true,
                      [this](std::string name) {
                          if (name.size() < 5) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure("Error", "Name is too short.\n5 < CppScriptNameLength < 30", true, true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          if (name.size() >= 30) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure("Error", "Name is too long.\n5 < CppScriptNameLength  < 30", true, true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          if (std::find_if(name.begin(), name.end(), [](char a) { return !isalpha(a); }) != name.end()) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure(
                                "Error", "Name must contain only alphabetic characters.\na .. z, A .. Z", true, true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          if (PrototypeIo::checkIfPluginProjectExists(name.c_str())) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure("Error",
                                                  "There's already C++/Rust script with that name.\nTry picking another name.",
                                                  true,
                                                  true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          PrototypeIo::createNewCppPluginProject(name.c_str());
                          return true;
                      },
                      []() { return true; });
                    _prompts.push(pr);
                } else if (ImGui::Selectable("Add new Rust script")) {
                    pr.configure(
                      "Create a new rust script",
                      "Pick a name for your rust script",
                      "Create",
                      true,
                      [this](std::string name) {
                          if (name.size() < 5) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure("Error", "Name is too short.\n5 < RustScriptNameLength < 30", true, true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          if (name.size() >= 30) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure("Error", "Name is too long.\n5 < RustScriptNameLength < 30", true, true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          if (std::find_if(name.begin(), name.end(), [](char a) { return !isalpha(a); }) != name.end()) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure(
                                "Error", "Name must contain only alphabetic characters.\na .. z, A .. Z", true, true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          if (PrototypeIo::checkIfPluginProjectExists(name.c_str())) {
                              PrototypeErrorDialog errDialog = {};
                              errDialog.configure("Error",
                                                  "There's already C++/Rust script with that name.\nTry picking another name.",
                                                  true,
                                                  true);
                              _errorDialogs.push(std::move(errDialog));
                              return false;
                          }
                          PrototypeIo::createNewRustPluginProject(name.c_str());
                          return true;
                      },
                      []() { return true; });
                    _prompts.push(pr);
                }
                ImGui::EndPopup();
            }

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

            if (ImGui::BeginTable("##scripts table", 1, flags)) {
                for (const auto& pair : PrototypeEngineInternalApplication::database->pluginInstances) {
                    size_t matchIndex = pair.second->name().find(scriptsSearchBuff);
                    if (matchIndex != std::string::npos) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        ImGui::Text("%s", pair.second->name().substr(0, matchIndex).c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_SCRIPT", &pair.second, sizeof(PrototypePluginInstance*));
                            ImGui::Text("%s", pair.second->name().c_str());
                            ImGui::EndDragDropSource();
                        }
                        ImGui::SameLine(0.0f, 0.0f);
                        ImGui::TextColored(ImVec4(PROTOTYPE_BLUE, 1.0f),
                                           "%s",
                                           pair.second->name().substr(matchIndex, strlen(scriptsSearchBuff)).c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_SCRIPT", &pair.second, sizeof(PrototypePluginInstance*));
                            ImGui::Text("%s", pair.second->name().c_str());
                            ImGui::EndDragDropSource();
                        }
                        ImGui::SameLine(0.0f, 0.0f);
                        ImGui::Text("%s", pair.second->name().substr(matchIndex + strlen(scriptsSearchBuff)).c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("_SCRIPT", &pair.second, sizeof(PrototypePluginInstance*));
                            ImGui::Text("%s", pair.second->name().c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskScripts;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Blueprints", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskBlueprints;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskBlueprints;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding4);
    {
        if (ImGui::Begin("Colliders", (bool*)0, windowFlags)) {
            _openViewsMask |= PrototypeUiViewMaskColliders;
            {
                ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                if (dockNode) dockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            }

            float posx = ImGui::GetContentRegionAvailWidth();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            if (ImGui::InputTextWithHint("##colliders search input text",
                                         ICON_FA_FILTER " Filter colliders ...",
                                         _recordedInstructions.collidersSearchBuff,
                                         sizeof(_recordedInstructions.collidersSearchBuff))) {
                scheduleRecordPass(PrototypeUiViewMaskColliders);
            }
            ImGui::TextColored(ImVec4(PROTOTYPE_GREEN, 1.0f), "%zu items", _recordedInstructions.colliders.size());

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
            if (ImGui::BeginTable("##colliders table", 1, flags)) {
                ImGuiListClipper clipper;
                clipper.Begin(_recordedInstructions.colliders.size());
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        auto colliderPair = _recordedInstructions.colliders[row];

                        ImGui::Text("%s%s", colliderPair.first.c_str(), colliderPair.second.c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            _clipboard.type[0] = "_COLLIDER";
                            _clipboard.data[0] = colliderPair.first.append(colliderPair.second);
                            ImGui::SetDragDropPayload("_COLLIDER", &_clipboard, sizeof(PrototypeClipboard));
                            ImGui::Text("%s", _clipboard.data[0].c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                }
                ImGui::EndTable();
            }
        } else {
            _openViewsMask &= ~PrototypeUiViewMaskColliders;
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    // draw forms
    {
        if (!_materialforms.empty()) {
            ImGuiWindowFlags promptWindowFlags =
              ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
            ImVec2 window_pos       = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
            ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            auto resolution = PrototypeEngineInternalApplication::window->resolution();
            ImGui::SetNextWindowSize(ImVec2(resolution.x / 3, 300), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(1.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(PROTOTYPE_DARKGRAY, 1.0f));
            if (ImGui::Begin("Add new material", (bool*)0, promptWindowFlags)) {
                PrototypeUiMaterialForm& form = _materialforms.top();
                if (form.content()(form)) { _materialforms.pop(); }
                ImGui::End();
            }
            ImGui::PopStyleColor();
        }
    }

    // draw prompts
    {
        if (_errorDialogs.empty()) {
            if (!_prompts.empty()) {
                ImGuiWindowFlags promptWindowFlags =
                  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
                ImVec2 window_pos       = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
                ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                auto resolution = PrototypeEngineInternalApplication::window->resolution();
                ImGui::SetNextWindowSize(ImVec2(resolution.x / 3, 200), ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowBgAlpha(1.0f);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(PROTOTYPE_DARKGRAY, 1.0f));
                if (ImGui::Begin(_prompts.top().title().c_str(), (bool*)0, promptWindowFlags)) {
                    ImGui::Text("%s", _prompts.top().text().c_str());
                    static char buffer[32];
                    static char label[256];
                    sprintf(label, "##%zu%s", _prompts.size(), _prompts.top().title().c_str());
                    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(PROTOTYPE_LIGHTGRAY, 1.0f));
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::InputText(label, buffer, sizeof(buffer));
                    ImGui::PopStyleColor();
                    if (ImGui::Button(_prompts.top().buttonText().c_str())) {
                        if (_prompts.top().onSuccess()(std::string(buffer))) { _prompts.pop(); }
                    }
                    if (ImGui::Button("cancel")) { _prompts.pop(); }
                    ImGui::End();
                }
                ImGui::PopStyleColor();
            }
        }
    }
    // draw shader erros
    {
        if (!_errorDialogs.empty()) {
            ImGuiWindowFlags errorWindowFlags =
              ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
            ImVec2 window_pos       = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
            ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGui::SetNextWindowBgAlpha(1.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(PROTOTYPE_DARKGRAY, 1.0f));
            if (ImGui::Begin(_errorDialogs.top().title().c_str(), (bool*)0, errorWindowFlags)) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImVec2 avail         = ImGui::GetContentRegionAvail();
                ImVec2 inputTextSize = _errorDialogs.top().canCancel() ? ImVec2(avail.x, avail.y - 60.0f) : avail;
                ImGui::InputTextMultiline("##draw shader error input text",
                                          (char*)_errorDialogs.top().text().c_str(),
                                          _errorDialogs.top().text().size(),
                                          inputTextSize,
                                          ImGuiInputTextFlags_Multiline | ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemFlag();
                if (_errorDialogs.top().canCancel()) {
                    if (ImGui::Button("cancel", ImGui::GetContentRegionAvail())) { _errorDialogs.pop(); }
                }
                ImGui::End();
            }
            ImGui::PopStyleColor();
        }
    }

    return state;
}

void
PrototypeOpenglUI::endFrame()
{
    ImGui::EndFrame();
}

void
PrototypeOpenglUI::update()
{
    _sceneView.onUpdate();
}

void
PrototypeOpenglUI::render(i32 x, i32 y, i32 width, i32 height)
{
    ImGui::Render();
    glViewport(x, y, width, height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGuiIO& io = ImGui::GetIO();
    // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //     GLFWwindow* backup_current_context = glfwGetCurrentContext();
    //     ImGui::UpdatePlatformWindows();
    //     ImGui::RenderPlatformWindowsDefault();
    //     glfwMakeContextCurrent(backup_current_context);
    // }
}

void
PrototypeOpenglUI::pushErrorDialog(PrototypeErrorDialog errDialog)
{
    _errorDialogs.push(errDialog);
}

void
PrototypeOpenglUI::popErrorDialog()
{
    if (!_errorDialogs.empty()) { _errorDialogs.pop(); }
}

void
PrototypeOpenglUI::signalBuffersChanged(bool status)
{
    _isBuffersChanged = status;
}

bool
PrototypeOpenglUI::isBuffersChanged()
{
    return _isBuffersChanged;
}

bool
PrototypeOpenglUI::needsMouse()
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool
PrototypeOpenglUI::needsKeyboard()
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

PrototypeUiViewMaskType
PrototypeOpenglUI::openedViewsMask()
{
    return _openViewsMask;
}

PrototypeUiView*
PrototypeOpenglUI::sceneView()
{
    return &_sceneView;
}