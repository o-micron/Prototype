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

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include "PrototypeCameraSystem.h"
#include "PrototypeShortcuts.h"

#include <set>

struct ScriptCodeLink;

struct PrototypePipelineAbstractCommand
{
    virtual ~PrototypePipelineAbstractCommand() {}
    virtual void call() const = 0;
};

// clang-format off

#define PROTOTYPE_PIPELINE_STRUCTURE_FIELD(A)          A;
#define PROTOTYPE_PIPELINE_STRUCTURE_FIELD_EXTENDED(A) A;

#define PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(PFN_NAME, ...)                                                               \
    struct PrototypePipelineCommand_##PFN_NAME : PrototypePipelineAbstractCommand                                                \
    {                                                                                                                            \
        PROTOTYPE_FOR_EACH_X(PROTOTYPE_PIPELINE_STRUCTURE_FIELD, PROTOTYPE_PIPELINE_STRUCTURE_FIELD_EXTENDED, __VA_ARGS__)       \
        virtual void call() const override;                                                                                      \
    };

PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorAddSceneNodeToLayer, PrototypeSceneLayer* parentLayer, glm::vec3 position, glm::vec3 rotation, glm::vec3 dir)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorAddSceneNodeToNode, PrototypeSceneNode* parentNode, glm::vec3 position, glm::vec3 rotation, glm::vec3 dir)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorRemoveSceneNode, PrototypeSceneNode* node)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorSelectedSceneNodeAddTraits, PrototypeObject* object, MASK_TYPE traitMask)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorSelectedSceneNodeRemoveTraits, PrototypeObject* object, MASK_TYPE traitMask)

PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemUpdateViewMatrix, Camera* camera, float xdir, float ydir, float zdir)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemUpdateProjectionMatrix, Camera* camera)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetRotation, Camera* camera, const glm::vec2& rotation)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetTranslation, Camera* camera, const glm::vec3& translation)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemRotate, Camera* camera, f32 x, f32 y)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemTranslate, Camera* camera, f32 x, f32 y, f32 z)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetResolution, Camera* camera, f32 width, f32 height)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetFov, Camera* camera, f32 fov)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetNear, Camera* camera, f32 near)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetFar, Camera* camera, f32 far)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetViewMatrix, Camera* camera, const glm::mat4& matrix)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(CameraSystemSetProjectionMatrix, Camera* camera, const glm::mat4& matrix)

PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorRemoveScriptFromObject, PrototypeObject* object, std::string codeLink)
PROTOTYPE_PIPELINE_COMMAND_CALL_DECLARATION(shortcutEditorCommitReloadPlugin, PrototypePluginInstance* pluginInstance)


#undef PROTOTYPE_PIPELINE_STRUCTURE_FIELD
#undef PROTOTYPE_PIPELINE_STRUCTURE_FIELD_EXTENDED

// clang-format on

struct PrototypePipelineQueue
{
    void record(std::unique_ptr<PrototypePipelineAbstractCommand> command);
    void dispatch();

  private:
    std::vector<std::unique_ptr<PrototypePipelineAbstractCommand>> _commands;
};

struct PrototypePipelines
{
    PrototypePipelines()  = delete;
    ~PrototypePipelines() = delete;

    static std::vector<PrototypePipelineQueue> shortcutsQueue;
};
