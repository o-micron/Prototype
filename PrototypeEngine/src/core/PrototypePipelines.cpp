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

#include "PrototypePipelines.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

std::vector<PrototypePipelineQueue> PrototypePipelines::shortcutsQueue;

// clang-format off

#define PROTOTYPE_PIPELINE_STRUCTURE_CALL(A)          A
#define PROTOTYPE_PIPELINE_STRUCTURE_CALL_EXTENDED(A) A,

#define PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(PFN_NAME, ...)                                                                       \
    void PrototypePipelineCommand_##PFN_NAME::call() const                                                                              \
    {                                                                                                                                   \
        PFN_NAME(__VA_ARGS__);                                                                                                          \
    }

PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorAddSceneNodeToLayer, parentLayer, position, rotation, dir)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorAddSceneNodeToNode, parentNode, position, rotation, dir)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorRemoveSceneNode, node)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorSelectedSceneNodeAddTraits, object, traitMask)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorSelectedSceneNodeRemoveTraits, object, traitMask)

PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemUpdateViewMatrix, camera, xdir, ydir, zdir)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemUpdateProjectionMatrix, camera)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetRotation, camera, rotation)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetTranslation, camera, translation)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemRotate, camera, x, y)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemTranslate, camera, x, y, z)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetResolution, camera, width, height)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetFov, camera, fov)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetNear, camera, near)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetFar, camera, far)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetViewMatrix, camera, matrix)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(CameraSystemSetProjectionMatrix, camera, matrix)

PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorRemoveScriptFromObject, object, codeLink)
PROTOTYPE_PIPELINE_COMMAND_CALL_DEFINITION(shortcutEditorCommitReloadPlugin, pluginInstance)

#undef PROTOTYPE_PIPELINE_STRUCTURE_CALL
#undef PROTOTYPE_PIPELINE_STRUCTURE_CALL_EXTENDED

// clang-format on

void
PrototypePipelineQueue::record(std::unique_ptr<PrototypePipelineAbstractCommand> command)
{
    _commands.push_back(std::move(command));
}

void
PrototypePipelineQueue::dispatch()
{
    for (const auto& command : _commands) { command->call(); }
}
