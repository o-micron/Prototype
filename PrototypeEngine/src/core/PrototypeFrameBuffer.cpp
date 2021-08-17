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

#include "PrototypeFrameBuffer.h"

#include "PrototypeDatabase.h"
#include "PrototypeEngine.h"
#include "PrototypeStaticInitializer.h"
#include <utility>

PrototypeFrameBuffer::PrototypeFrameBuffer(std::string name,
                                           std::string shader,
                                           const u32   numColorAttachments,
                                           const bool  withDepthAttachment)
  : _id(++PrototypeStaticInitializer::_framebufferUUID)
  , _name(std::move(name))
  , _shader(std::move(shader))
  , _numColorAttachments(numColorAttachments)
  , _withDepthAttachment(withDepthAttachment)
{}

const u32&
PrototypeFrameBuffer::id() const
{
    return _id;
}

const std::string&
PrototypeFrameBuffer::name() const
{
    return _name;
}

const std::string&
PrototypeFrameBuffer::shader() const
{
    return _shader;
}

const u32&
PrototypeFrameBuffer::numColorAttachments() const
{
    return _numColorAttachments;
}

const bool&
PrototypeFrameBuffer::withDepthAttachment() const
{
    return _withDepthAttachment;
}

void
PrototypeFrameBuffer::to_json(nlohmann::json& j, const PrototypeFrameBuffer& frameBuffer)
{
    j["id"]                  = frameBuffer.id();
    j["name"]                = frameBuffer.name();
    j["shader"]              = frameBuffer.shader();
    j["numColorAttachments"] = frameBuffer.numColorAttachments();
    j["withDepthAttachment"] = frameBuffer.withDepthAttachment();
}

void
PrototypeFrameBuffer::from_json(const nlohmann::json& j)
{
    if (j.is_null()) { return; }

    const char* field_name                  = "name";
    const char* field_shader                = "shader";
    const char* field_num_color_attachments = "numColorAttachments";
    const char* field_with_depth_attachment = "withDepthAttachment";

    std::string name = j.at(field_name).get<std::string>();
    if (name.empty()) return;

    if (PrototypeEngineInternalApplication::database->framebuffers.find(name) !=
        PrototypeEngineInternalApplication::database->framebuffers.end()) {
        return;
    }

    PrototypeFrameBuffer* frameBuffer =
      PrototypeEngineInternalApplication::database->allocateFramebuffer(j.at(field_name).get<std::string>(),
                                                                        j.at(field_shader).get<std::string>(),
                                                                        j.at(field_num_color_attachments).get<u32>(),
                                                                        j.at(field_with_depth_attachment).get<bool>());
    PrototypeEngineInternalApplication::database->framebuffers.insert({ frameBuffer->name(), frameBuffer });
}