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

#include "PrototypeOpenGL.h"

#include "../core/PrototypeDatabase.h"
#include "../core/PrototypeEngine.h"
#include "../core/PrototypeMeshBuffer.h"
#include "../core/PrototypeRenderer.h"
#include "../core/PrototypeShaderBuffer.h"
#include "../core/PrototypeTextureBuffer.h"
#include "../core/PrototypeUI.h"

#include <PrototypeCommon/Logger.h>

#include <algorithm>
#include <regex>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#pragma warning(disable : 4003)

#define PROTOTYPE_BUILD_OPENGL_CALL(A)          A
#define PROTOTYPE_BUILD_OPENGL_CALL_EXTENDED(A) A,

#define PGLCALL_DEFINITION(PFN_NAME, ...)                                                                                        \
    void                              PglCall_##PFN_NAME::call() const { PFN_NAME(__VA_ARGS__); }                                \
    MemoryPool<PglCall_##PFN_NAME, 4> PglCall_##PFN_NAME::_allocator;

PGLCALL_DEFINITION(glBindVertexArray, index)
PGLCALL_DEFINITION(glDrawElements, mode, count, type, indices)
PGLCALL_DEFINITION(glDrawElementsInstanced, mode, count, type, indices, instanceCount)
PGLCALL_DEFINITION(glDrawArrays, mode, firstIndex, count)
PGLCALL_DEFINITION(glDrawArraysInstanced, mode, firstIndex, count, instanceCount)

PGLCALL_DEFINITION(glUseProgram, program)
PGLCALL_DEFINITION(glUniform1ui, location, v0)
PGLCALL_DEFINITION(glUniform1i, location, v0)
PGLCALL_DEFINITION(glUniform1fv, location, count, value)
PGLCALL_DEFINITION(glUniform2fv, location, count, value)
PGLCALL_DEFINITION(glUniform3fv, location, count, value)
PGLCALL_DEFINITION(glUniform4fv, location, count, value)
PGLCALL_DEFINITION(glUniformMatrix2fv, location, count, transpose, value)
PGLCALL_DEFINITION(glUniformMatrix3fv, location, count, transpose, value)
PGLCALL_DEFINITION(glUniformMatrix4fv, location, count, transpose, value)

PGLCALL_DEFINITION(glBindTexture, target, id)
PGLCALL_DEFINITION(glActiveTexture, texture)

PGLCALL_DEFINITION(glUniformBlockBinding, program, uniformBlockIndex, uniformBlockBinding)

#define PGLCALL_DECLARATION_RESET_ALLOCATIONS_CALL(DECL)          PglCall_##DECL::_allocator.clear();
#define PGLCALL_DECLARATION_RESET_ALLOCATIONS_CALL_EXTENDED(DECL) PglCall_##DECL::_allocator.clear();

extern void
PglCallResetAllocations()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    PROTOTYPE_FOR_EACH_X(PGLCALL_DECLARATION_RESET_ALLOCATIONS_CALL,
                         PGLCALL_DECLARATION_RESET_ALLOCATIONS_CALL_EXTENDED,
                         glBindVertexArray,
                         glDrawElements,
                         glDrawArrays,
                         glUseProgram,
                         glUniform1i,
                         glUniform1fv,
                         glUniform2fv,
                         glUniform3fv,
                         glUniform4fv,
                         glUniformMatrix2fv,
                         glUniformMatrix3fv,
                         glUniformMatrix4fv);
#else
    PROTOTYPE_FOR_EACH_X(PGLCALL_DECLARATION_RESET_ALLOCATIONS_CALL,
                         PGLCALL_DECLARATION_RESET_ALLOCATIONS_CALL_EXTENDED,
                         glBindVertexArray,
                         glDrawElements,
                         glDrawArrays,
                         glUseProgram,
                         glUniform1i,
                         glUniform1fv,
                         glUniform2fv,
                         glUniform3fv,
                         glUniform4fv,
                         glUniformMatrix2fv,
                         glUniformMatrix3fv,
                         glUniformMatrix4fv);
#endif
}

#undef PROTOTYPE_BUILD_OPENGL_CALL
#undef PROTOTYPE_BUILD_OPENGL_CALL_EXTENDED

void
PrototypeOpenglCommand::record(PglCall_glAbstract* call)
{
    _instruction = call;
    // va_list argsList;
    // switch (event) {
    //     case PglRecordedEvent_MeshBind: {
    //         va_start(argsList, 3);
    //         auto a   = std::make_unique<PglCall_glBindVertexArray>();
    //         a->index = (GLuint)va_arg(argsList, GLuint);
    //         _instructions.push_back(std::move(a));
    //         auto b          = std::make_unique<PglCall_glDrawArrays>();
    //         b->mode         = GL_TRIANGLES;
    //         b->firstIndex   = (GLint)va_arg(argsList, GLint);
    //         b->indicesCount = (GLsizei)va_arg(argsList, GLsizei);
    //         _instructions.push_back(std::move(b));
    //     } break;

    //     case PglRecordedEvent_MeshUnBind: {
    //         auto a   = std::make_unique<PglCall_glBindVertexArray>();
    //         a->index = 0;
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBind: {
    //         va_start(argsList, 1);
    //         auto a     = std::make_unique<PglCall_glUseProgram>();
    //         a->program = (GLuint)va_arg(argsList, GLuint);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindTexture: {
    //         va_start(argsList, 2);
    //         auto a      = std::make_unique<PglCall_glUniform1i>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->v0       = (GLint)va_arg(argsList, GLint);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindFloat: {
    //         va_start(argsList, 3);
    //         auto a      = std::make_unique<PglCall_glUniform1fv>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->count    = (GLsizei)va_arg(argsList, GLsizei);
    //         a->value    = (const GLfloat*)va_arg(argsList, const GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindVec2: {
    //         va_start(argsList, 3);
    //         auto a      = std::make_unique<PglCall_glUniform2fv>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->count    = (GLsizei)va_arg(argsList, GLsizei);
    //         a->value    = (GLfloat*)va_arg(argsList, GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindVec3: {
    //         va_start(argsList, 3);
    //         auto a      = std::make_unique<PglCall_glUniform3fv>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->count    = (GLsizei)va_arg(argsList, GLsizei);
    //         a->value    = (GLfloat*)va_arg(argsList, GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindVec4: {
    //         va_start(argsList, 3);
    //         auto a      = std::make_unique<PglCall_glUniform4fv>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->count    = (GLsizei)va_arg(argsList, GLsizei);
    //         a->value    = (GLfloat*)va_arg(argsList, GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindMat2: {
    //         va_start(argsList, 3);
    //         auto a      = std::make_unique<PglCall_glUniformMatrix2fv>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->count    = (GLsizei)va_arg(argsList, GLsizei);
    //         a->value    = (GLfloat*)va_arg(argsList, GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindMat3: {
    //         va_start(argsList, 3);
    //         auto a      = std::make_unique<PglCall_glUniformMatrix3fv>();
    //         a->location = (GLint)va_arg(argsList, GLint);
    //         a->count    = (GLsizei)va_arg(argsList, GLsizei);
    //         a->value    = (GLfloat*)va_arg(argsList, GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderBindMat4: {
    //         va_start(argsList, 3);
    //         auto a       = std::make_unique<PglCall_glUniformMatrix4fv>();
    //         a->location  = (GLint)va_arg(argsList, GLint);
    //         a->count     = (GLsizei)va_arg(argsList, GLsizei);
    //         a->transpose = (GLboolean)va_arg(argsList, GLboolean);
    //         a->value     = (GLfloat*)va_arg(argsList, GLfloat*);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_ShaderUnBind: {
    //         auto a     = std::make_unique<PglCall_glUseProgram>();
    //         a->program = 0;
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_TextureBind: {
    //         va_start(argsList, 2);
    //         auto a    = std::make_unique<PglCall_glBindTexture>();
    //         a->target = (GLenum)va_arg(argsList, GLenum);
    //         a->id     = (GLuint)va_arg(argsList, GLuint);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_TextureActive: {
    //         va_start(argsList, 1);
    //         auto a     = std::make_unique<PglCall_glActiveTexture>();
    //         a->texture = (GLenum)va_arg(argsList, GLenum);
    //         _instructions.push_back(std::move(a));
    //     } break;

    //     case PglRecordedEvent_TextureUnBind: {
    //         va_start(argsList, 1);
    //         auto a    = std::make_unique<PglCall_glBindTexture>();
    //         a->target = (GLenum)va_arg(argsList, GLenum);
    //         a->id     = 0;
    //         _instructions.push_back(std::move(a));
    //     } break;
    // }
    // va_end(argsList);
}

void
PrototypeOpenglCommand::dispatch()
{
    if (_instruction) { _instruction->call(); }
}

GLuint
shader_compile(PglShader* shader, const std::string shaderPath, const std::string source, const GLenum type)
{
    GLuint shader_id = glCreateShader(type);
    auto   src       = static_cast<const GLchar*>(source.c_str());
    auto   len       = static_cast<const GLint>(source.length());
    glShaderSource(shader_id, 1, &src, &len);
    glCompileShader(shader_id);
    GLint isCompiled = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader_id, maxLength, &maxLength, &errorLog[0]);
        std::string log(errorLog.begin(), errorLog.end() - 1);
        glDeleteShader(shader_id);
        std::string type_str = "unknown_type shader";
        if (type == GL_VERTEX_SHADER) {
            type_str = "vertex shader";
        } else if (type == GL_FRAGMENT_SHADER) {
            type_str = "fragment shader";
        }
        char buffer[4096];
        snprintf(buffer, sizeof(buffer), "SHADER_NAME: %s\n%s\n", shader->name.c_str(), log.c_str());
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
        PrototypeErrorDialog errorDialog = {};
        errorDialog.configure(type == GL_VERTEX_SHADER ? "VERTEX SHADER COMPILATION FAILED"
                                                       : "FRAGMENT SHADER COMPILATION FAILED",
                              std::string(buffer),
                              false,
                              false);
        PrototypeEngineInternalApplication::renderer->ui()->pushErrorDialog(std::move(errorDialog));
#endif
        // ui_show_shader_error("Shader error", std::string(buffer, buffer + 512), false);
        return 0;
    }
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PrototypeEngineInternalApplication::renderer->ui()->popErrorDialog();
#endif
    // ui_hide_shader_error();
    return shader_id;
}

static GLuint
shader_link(std::vector<GLuint> shaders)
{
    GLuint program = glCreateProgram();
    for (auto shader : shaders) { glAttachShader(program, shader); }
    glLinkProgram(program);
    GLint isLinked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        std::string log(infoLog.begin(), infoLog.end() - 1);
        for (auto shader : shaders) { glDetachShader(program, shader); }
        glDeleteProgram(program);
        program = 0;
        PrototypeLogger::error("failed to link program", log.c_str());
        return 0;
    }
    for (auto shader : shaders) { glDetachShader(program, shader); }
    // ui_hide_shader_error();
    return program;
}

PROTOTYPE_EXTERN bool
PglUploadMeshFromBuffer(const PrototypeMeshBuffer* meshBuffer, PglGeometry* geometry)
{
    geometry->name = meshBuffer->name();
    GLuint vao;
    glGenVertexArrays(1, &vao);
    geometry->vao = vao;
    glBindVertexArray(geometry->vao);
    geometry->indexCount = static_cast<u32>(meshBuffer->source().indices.size());
    if (geometry->indexCount < std::numeric_limits<u8>::min()) {
        geometry->type = GL_UNSIGNED_BYTE;
    } else if (geometry->indexCount < std::numeric_limits<u16>::min()) {
        geometry->type = GL_UNSIGNED_SHORT;
    } else {
        geometry->type = GL_UNSIGNED_INT;
    }
    GLuint vbo;
    glGenBuffers(1, &vbo);
    geometry->vbo = vbo;
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 meshBuffer->source().vertices.size() * PglGeometryInfo::SIZE,
                 &meshBuffer->source().vertices[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(PglGeometryInfo::POSITION::INDEX);
    glVertexAttribPointer(PglGeometryInfo::POSITION::INDEX,
                          PglGeometryInfo::POSITION::LENGTH,
                          GL_FLOAT,
                          GL_FALSE,
                          PglGeometryInfo::SIZE,
                          (void*)(PglGeometryInfo::POSITION::STRIDE));
    glEnableVertexAttribArray(PglGeometryInfo::NORMAL::INDEX);
    glVertexAttribPointer(PglGeometryInfo::NORMAL::INDEX,
                          PglGeometryInfo::NORMAL::LENGTH,
                          GL_FLOAT,
                          GL_FALSE,
                          PglGeometryInfo::SIZE,
                          (void*)(PglGeometryInfo::NORMAL::STRIDE));
    glEnableVertexAttribArray(PglGeometryInfo::COLOR::INDEX);
    glVertexAttribPointer(PglGeometryInfo::COLOR::INDEX,
                          PglGeometryInfo::COLOR::LENGTH,
                          GL_FLOAT,
                          GL_FALSE,
                          PglGeometryInfo::SIZE,
                          (void*)(PglGeometryInfo::COLOR::STRIDE));
    GLuint eabo;
    glGenBuffers(1, &eabo);
    geometry->eabo = eabo;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->eabo);
    if (geometry->type == GL_UNSIGNED_BYTE) {
        std::vector<u8> casted(meshBuffer->source().indices.size());
        std::transform(meshBuffer->source().indices.begin(),
                       meshBuffer->source().indices.end(),
                       casted.begin(),
                       [](const auto& a) { return static_cast<u8>(a); });
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, casted.size() * sizeof(u8), &casted[0], GL_STATIC_DRAW);
    } else if (geometry->type == GL_UNSIGNED_SHORT) {
        std::vector<u16> casted(meshBuffer->source().indices.size());
        std::transform(meshBuffer->source().indices.begin(),
                       meshBuffer->source().indices.end(),
                       casted.begin(),
                       [](const auto& a) { return static_cast<u16>(a); });
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, casted.size() * sizeof(u16), &casted[0], GL_STATIC_DRAW);
    } else if (geometry->type == GL_UNSIGNED_INT) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     meshBuffer->source().indices.size() * sizeof(u32),
                     &meshBuffer->source().indices[0],
                     GL_STATIC_DRAW);
    }
    glBindVertexArray(0);
    return true;
}

PROTOTYPE_EXTERN void
PglReleaseMesh(PglGeometry* mesh)
{
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->eabo);
}

PROTOTYPE_EXTERN bool
PglUploadShaderFromBuffer(const PrototypeShaderBuffer* shaderBuffer, PglShader* shader)
{
    shader->name = shaderBuffer->name();
    std::vector<GLuint> shadersIds;
    bool                successful = true;

    for (size_t i = 0; i < shaderBuffer->sources().size(); ++i) {
        GLenum type = GL_VERTEX_SHADER;
        switch (shaderBuffer->sources()[i]->type) {
            case PrototypeShaderBufferSourceType_VertexShader: type = GL_VERTEX_SHADER; break;
            case PrototypeShaderBufferSourceType_FragmentShader: type = GL_FRAGMENT_SHADER; break;
            case PrototypeShaderBufferSourceType_Count: {
                PrototypeLogger::fatal("Unimplemented!(Unreachable)");
            } break;
        }
        GLuint shaderId = shader_compile(shader, shader->name, shaderBuffer->sources()[i]->code, type);
        if (shaderId == 0) {
            successful = false;
            break;
        }
        shadersIds.push_back(shaderId);
    }

    if (successful) {
        shader->program = shader_link(shadersIds);
    } else {
        shader->program = 0;
    }

    for (size_t i = 0; i < shadersIds.size(); ++i) { glDeleteShader(shadersIds[i]); }

    return true;
}

PROTOTYPE_EXTERN void
PglReleaseShader(PglShader* shader)
{
    glDeleteProgram(shader->program);
}

PROTOTYPE_EXTERN bool
PglUploadTextureFromBuffer(const PrototypeTextureBuffer* textureBuffer, PglTexture* texture)
{
    texture->name           = textureBuffer->name();
    texture->width          = textureBuffer->source().width;
    texture->height         = textureBuffer->source().height;
    texture->internalFormat = GL_COMPRESSED_RGBA; // GL_RGBA;
    texture->format         = GL_RGBA;
    texture->target         = GL_TEXTURE_2D;
    texture->type           = GL_UNSIGNED_BYTE;
    glGenTextures(1, &texture->id);
    glBindTexture(texture->target, texture->id);
    glTexImage2D(texture->target,
                 0,
                 texture->internalFormat,
                 texture->width,
                 texture->height,
                 0,
                 texture->format,
                 texture->type,
                 &textureBuffer->source().data[0]);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST); // GL_NICEST | GL_DONT_CARE
    glGenerateMipmap(texture->target);

    glBindTexture(texture->target, 0);
    return true;
}

PROTOTYPE_EXTERN void
PglReleaseTexture(PglTexture* texture)
{
    glDeleteTextures(1, &texture->id);
}

PROTOTYPE_EXTERN bool
PglUploadFramebufferFromData(PglShader* shader, PglFramebuffer* framebuffer, i32 width, i32 height, bool withDepthAttachment)
{
    if (framebuffer->colorAttachments.empty()) {
        PrototypeLogger::fatal("You cannot have a framebuffer with no color attachments !");
        return false;
    }
    framebuffer->shader = shader;
    // color attachments
    framebuffer->colorAttachmentsNames.resize(framebuffer->colorAttachments.size());
    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        framebuffer->colorAttachmentsNames[i]           = std::string("tex").append(std::to_string(i));
        framebuffer->colorAttachments[i].target         = GL_TEXTURE_2D;
        framebuffer->colorAttachments[i].internalFormat = GL_RGBA32F;
        framebuffer->colorAttachments[i].format         = GL_RGBA;
        framebuffer->colorAttachments[i].type           = GL_FLOAT;
        framebuffer->colorAttachments[i].width          = width;
        framebuffer->colorAttachments[i].height         = height;
        GLuint textureId;
        glGenTextures(1, &textureId);
        framebuffer->colorAttachments[i].id = textureId;
        glBindTexture(framebuffer->colorAttachments[i].target, textureId);
        glTexImage2D(framebuffer->colorAttachments[i].target,
                     0,
                     framebuffer->colorAttachments[i].internalFormat,
                     framebuffer->colorAttachments[i].width,
                     framebuffer->colorAttachments[i].height,
                     0,
                     framebuffer->colorAttachments[i].format,
                     framebuffer->colorAttachments[i].type,
                     0);
        glTexParameteri(framebuffer->colorAttachments[i].target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(framebuffer->colorAttachments[i].target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(framebuffer->colorAttachments[i].target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(framebuffer->colorAttachments[i].target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(framebuffer->colorAttachments[i].target, 0);
    }
    // depth attachment
    if (withDepthAttachment) {
        framebuffer->depthAttachment.target         = GL_TEXTURE_2D;
        framebuffer->depthAttachment.internalFormat = GL_DEPTH_COMPONENT24;
        framebuffer->depthAttachment.format         = GL_DEPTH_COMPONENT;
        framebuffer->depthAttachment.type           = GL_UNSIGNED_INT;
        framebuffer->depthAttachment.width          = width;
        framebuffer->depthAttachment.height         = height;
        GLuint textureId;
        glGenTextures(1, &textureId);
        framebuffer->depthAttachment.id = textureId;
        glBindTexture(framebuffer->depthAttachment.target, textureId);
        glTexImage2D(framebuffer->depthAttachment.target,
                     0,
                     framebuffer->depthAttachment.internalFormat,
                     framebuffer->depthAttachment.width,
                     framebuffer->depthAttachment.height,
                     0,
                     framebuffer->depthAttachment.format,
                     framebuffer->depthAttachment.type,
                     0);
        glTexParameteri(framebuffer->depthAttachment.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(framebuffer->depthAttachment.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(framebuffer->depthAttachment.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(framebuffer->depthAttachment.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(framebuffer->depthAttachment.target, 0);
    } else {
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        framebuffer->rbo = rbo;
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    // plane surface
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        framebuffer->surface.vao = vao;
        glBindVertexArray(vao);
        std::vector<f32> square = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
        size_t          sizeofVertex         = 5;
        size_t          sizeofVertexPosition = square.size() / sizeofVertex;
        std::vector<u8> indices(sizeofVertexPosition);
        std::iota(indices.begin(), indices.end(), 0);
        framebuffer->surface.indexCount = static_cast<GLsizei>(indices.size());
        framebuffer->surface.type       = GL_UNSIGNED_BYTE;
        const size_t VERTEX_SIZE        = 3 * sizeof(f32) + 2 * sizeof(f32);
        const size_t POSITION_STRIDE    = 0;
        const size_t TEXCOORD_STRIDE    = 3 * sizeof(f32);
        GLuint       vbo, eabo;
        glGenBuffers(1, &vbo);
        framebuffer->surface.vbo = vbo;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(f32), &square[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void*)(POSITION_STRIDE));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void*)(TEXCOORD_STRIDE));
        glGenBuffers(1, &eabo);
        framebuffer->surface.eabo = eabo;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u8), &indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
    // create fb
    {
        std::vector<GLenum> attachments(framebuffer->colorAttachments.size());
        GLuint              fbo;
        glGenFramebuffers(1, &fbo);
        framebuffer->fbo = fbo;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // color attachments
        for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            glBindTexture(framebuffer->colorAttachments[i].target, framebuffer->colorAttachments[i].id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, framebuffer->colorAttachments[i].id, 0);
        }

        if (withDepthAttachment) {
            // depth attachment
            glBindTexture(framebuffer->depthAttachment.target, framebuffer->depthAttachment.id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthAttachment.id, 0);
        } else {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->rbo);
        }

        // Draw buffers needed
        glDrawBuffers(static_cast<GLsizei>(attachments.size()), &attachments[0]);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            auto glstatus = glGetError();
            if (glstatus != GL_NO_ERROR) { PrototypeLogger::error("framebuffer is incomplete"); }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return true;
}

PROTOTYPE_EXTERN void
PglReleaseFramebuffer(PglFramebuffer* framebuffer)
{
    glDeleteVertexArrays(1, &framebuffer->surface.vao);
    glDeleteBuffers(1, &framebuffer->surface.vbo);
    glDeleteBuffers(1, &framebuffer->surface.eabo);
    framebuffer->surface = {};
    framebuffer->shader  = nullptr;
    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        glDeleteTextures(1, &framebuffer->colorAttachments[i].id);
    }
    framebuffer->colorAttachments.clear();
    glDeleteTextures(1, &framebuffer->depthAttachment.id);
    glDeleteRenderbuffers(1, &framebuffer->rbo);
    glDeleteFramebuffers(1, &framebuffer->fbo);
}

PROTOTYPE_EXTERN void
PglFramebufferResize(PglFramebuffer* framebuffer, i32 width, i32 height)
{
    if (framebuffer->colorAttachments[0].width == width && framebuffer->colorAttachments[0].height == height) { return; }
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
    // color attachments texture resize
    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        framebuffer->colorAttachments[i].width  = width;
        framebuffer->colorAttachments[i].height = height;
        glBindTexture(framebuffer->colorAttachments[i].target, framebuffer->colorAttachments[i].id);
        glTexImage2D(framebuffer->colorAttachments[i].target,
                     0,
                     framebuffer->colorAttachments[i].internalFormat,
                     width,
                     height,
                     0,
                     framebuffer->colorAttachments[i].format,
                     framebuffer->colorAttachments[i].type,
                     0);
        glBindTexture(framebuffer->colorAttachments[i].target, 0);
    }
    // depth attachment texture resize
    if (framebuffer->rbo == 0) {

        framebuffer->depthAttachment.width  = width;
        framebuffer->depthAttachment.height = height;
        glBindTexture(framebuffer->depthAttachment.target, framebuffer->depthAttachment.id);
        glTexImage2D(framebuffer->depthAttachment.target,
                     0,
                     framebuffer->depthAttachment.internalFormat,
                     width,
                     height,
                     0,
                     framebuffer->depthAttachment.format,
                     framebuffer->depthAttachment.type,
                     0);
        glBindTexture(framebuffer->depthAttachment.target, 0);
    } else {
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PROTOTYPE_EXTERN void
PglFramebufferStartRecording2D(PglFramebuffer* framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);

    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer->colorAttachments[i].id, 0);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthAttachment.id, 0);

    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, framebuffer->colorAttachments[0].width, framebuffer->colorAttachments[0].height);
    glClear(GL_COLOR_BUFFER_BIT);
}

PROTOTYPE_EXTERN void
PglFramebufferStartRecording3D(PglFramebuffer* framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);

    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer->colorAttachments[i].id, 0);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthAttachment.id, 0);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, framebuffer->colorAttachments[0].width, framebuffer->colorAttachments[0].height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

PROTOTYPE_EXTERN void
PglFramebufferEndRecording(PglFramebuffer* framebuffer)
{
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PROTOTYPE_EXTERN void
PglFramebufferStartRendering(PglFramebuffer* framebuffer)
{
    glUseProgram(framebuffer->shader->program);
    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(framebuffer->colorAttachments[i].target, framebuffer->colorAttachments[i].id);
        glUniform1i(glGetUniformLocation(framebuffer->shader->program, framebuffer->colorAttachmentsNames[i].c_str()), i);
    }
}

PROTOTYPE_EXTERN void
PglFramebufferEndRendering(PglFramebuffer* framebuffer)
{
    glBindVertexArray(framebuffer->surface.vao);
    glDrawElements(GL_TRIANGLES, framebuffer->surface.indexCount, framebuffer->surface.type, 0);
    glBindVertexArray(0);
    for (size_t i = 0; i < framebuffer->colorAttachments.size(); ++i) {
        glBindTexture(framebuffer->colorAttachments[i].target, 0);
    }
    glUseProgram(0);
}

PROTOTYPE_EXTERN void
PglFramebufferCaptureFrame(PglFramebuffer* framebuffer, void* pixels, size_t colorAttachmentIndex)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0,
                 0,
                 framebuffer->colorAttachments[colorAttachmentIndex].width,
                 framebuffer->colorAttachments[colorAttachmentIndex].height,
                 framebuffer->colorAttachments[colorAttachmentIndex].format,
                 framebuffer->colorAttachments[colorAttachmentIndex].type,
                 pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PROTOTYPE_EXTERN bool
PglUploadSkybox(const PglGeometry* cube,
                const PglShader*   skyShader,
                const PglShader*   hdrShader,
                const PglShader*   irradianceShader,
                const PglShader*   prefilterShader,
                const PglShader*   brdfShader,
                PglSkybox*         skybox)
{
    const u32 cubeSize       = 512;
    const u32 irradianceSize = 32;
    const u32 prefilterSize  = 256;

    // Plane
    auto   plane = std::make_shared<PglGeometry>();
    GLuint vao;
    glGenVertexArrays(1, &vao);
    plane->vao = vao;
    glBindVertexArray(vao);
    std::vector<f32> square = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                                1.0f,  1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    u32              sizeofVertex         = 5;
    u32              sizeofVertexPosition = square.size() / sizeofVertex;
    std::vector<u8>  indices(sizeofVertexPosition);
    std::iota(indices.begin(), indices.end(), 0);
    plane->indexCount         = static_cast<GLsizei>(indices.size());
    plane->type               = GL_UNSIGNED_BYTE;
    const u32 VERTEX_SIZE     = 3 * sizeof(f32) + 2 * sizeof(f32);
    const u32 POSITION_STRIDE = 0;
    const u32 TEXCOORD_STRIDE = 3 * sizeof(f32);
    GLuint    vbo, eabo;
    glGenBuffers(1, &vbo);
    plane->vbo = vbo;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(f32), &square[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void*)(POSITION_STRIDE));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void*)(TEXCOORD_STRIDE));
    glGenBuffers(1, &eabo);
    plane->eabo = eabo;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u8), &indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);

    // Shaders
    skybox->skyShader        = skyShader;
    skybox->hdrShader        = hdrShader;
    skybox->irradianceShader = irradianceShader;
    skybox->prefilterShader  = prefilterShader;
    skybox->brdfShader       = brdfShader;

    // Textures
    skybox->skyTexture        = std::make_shared<PglTexture>();
    skybox->hdrTexture        = std::make_shared<PglTexture>();
    skybox->irradianceTexture = std::make_shared<PglTexture>();
    skybox->prefilterTexture  = std::make_shared<PglTexture>();
    skybox->brdfTexture       = std::make_shared<PglTexture>();

    // 6 Cube Faces Projection
    glm::mat4 skySnapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 skySnapViews[6]   = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    GLuint fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cubeSize, cubeSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    GLuint skyTexture;
    glGenTextures(1, &skyTexture);
    skybox->skyTexture->id             = skyTexture;
    skybox->skyTexture->target         = GL_TEXTURE_CUBE_MAP;
    skybox->skyTexture->internalFormat = GL_RGBA16F;
    skybox->skyTexture->width          = cubeSize;
    skybox->skyTexture->height         = cubeSize;
    skybox->skyTexture->format         = GL_RGBA;
    skybox->skyTexture->type           = GL_FLOAT;
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->skyTexture->id);
    for (size_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     skybox->skyTexture->internalFormat,
                     skybox->skyTexture->width,
                     skybox->skyTexture->height,
                     0,
                     skybox->skyTexture->format,
                     skybox->skyTexture->type,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    const char* skyboxTexturePath = PROTOTYPE_TEXTURE_PATH(PROTOTYPE_DEFAULT_SKYBOX);
    if (!stbi_is_hdr(skyboxTexturePath)) {
        PrototypeLogger::error("Skybox texture is not of type hdr <%s>", skyboxTexturePath);
        return false;
    }
    i32 components;
    stbi_set_flip_vertically_on_load(true);
    f32* data = stbi_loadf(skyboxTexturePath, &skybox->hdrTexture->width, &skybox->hdrTexture->height, &components, 0);
    if (data == nullptr) {
        PrototypeLogger::error("Failed to load data from hdr texture <%s>", skyboxTexturePath);
        return false;
    }
    GLuint hdrTexture;
    glGenTextures(1, &hdrTexture);
    skybox->hdrTexture->id             = hdrTexture;
    skybox->hdrTexture->target         = GL_TEXTURE_2D;
    skybox->hdrTexture->internalFormat = GL_RGB32F;
    skybox->hdrTexture->format         = GL_RGB;
    skybox->hdrTexture->type           = GL_FLOAT;
    glBindTexture(GL_TEXTURE_2D, skybox->hdrTexture->id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 skybox->hdrTexture->internalFormat,
                 skybox->hdrTexture->width,
                 skybox->hdrTexture->height,
                 0,
                 skybox->hdrTexture->format,
                 skybox->hdrTexture->type,
                 data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    glUseProgram(skybox->hdrShader->program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skybox->hdrTexture->id);
    glUniform1i(glGetUniformLocation(skybox->hdrShader->program, "texHdr"), 0);
    glUniformMatrix4fv(glGetUniformLocation(skybox->hdrShader->program, "Projection"),
                       1,
                       0,
                       static_cast<const GLfloat*>(&skySnapProjection[0][0]));

    glViewport(0, 0, cubeSize, cubeSize);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (size_t i = 0; i < 6; i++) {
        glUniformMatrix4fv(
          glGetUniformLocation(skybox->hdrShader->program, "View"), 1, 0, static_cast<const GLfloat*>(&skySnapViews[i][0][0]));
        glFramebufferTexture2D(
          GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skybox->skyTexture->id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(cube->vao);
        glDrawElements(GL_TRIANGLES, cube->indexCount, cube->type, 0);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->skyTexture->id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    GLuint irradianceTexture;
    glGenTextures(1, &irradianceTexture);
    skybox->irradianceTexture->id             = irradianceTexture;
    skybox->irradianceTexture->target         = GL_TEXTURE_CUBE_MAP;
    skybox->irradianceTexture->internalFormat = GL_RGBA16F;
    skybox->irradianceTexture->width          = irradianceSize;
    skybox->irradianceTexture->height         = irradianceSize;
    skybox->irradianceTexture->format         = GL_RGBA;
    skybox->irradianceTexture->type           = GL_FLOAT;
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->irradianceTexture->id);
    for (size_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     skybox->irradianceTexture->internalFormat,
                     skybox->irradianceTexture->width,
                     skybox->irradianceTexture->height,
                     0,
                     skybox->irradianceTexture->format,
                     skybox->irradianceTexture->type,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, irradianceSize, irradianceSize);

    glUseProgram(skybox->irradianceShader->program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->skyTexture->id);
    glUniform1i(glGetUniformLocation(skybox->irradianceShader->program, "texIrradiance"), 0);
    glUniformMatrix4fv(glGetUniformLocation(skybox->irradianceShader->program, "Projection"),
                       1,
                       0,
                       static_cast<const GLfloat*>(&skySnapProjection[0][0]));

    glViewport(0, 0, irradianceSize, irradianceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (size_t i = 0; i < 6; i++) {
        glUniformMatrix4fv(glGetUniformLocation(skybox->irradianceShader->program, "View"),
                           1,
                           0,
                           static_cast<const GLfloat*>(&skySnapViews[i][0][0]));
        glFramebufferTexture2D(
          GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skybox->irradianceTexture->id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(cube->vao);
        glDrawElements(GL_TRIANGLES, cube->indexCount, cube->type, 0);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint prefilterTexture;
    glGenTextures(1, &prefilterTexture);
    skybox->prefilterTexture->id             = prefilterTexture;
    skybox->prefilterTexture->target         = GL_TEXTURE_CUBE_MAP;
    skybox->prefilterTexture->internalFormat = GL_RGBA16F;
    skybox->prefilterTexture->width          = prefilterSize;
    skybox->prefilterTexture->height         = prefilterSize;
    skybox->prefilterTexture->format         = GL_RGBA;
    skybox->prefilterTexture->type           = GL_FLOAT;
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->prefilterTexture->id);
    for (size_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     skybox->prefilterTexture->internalFormat,
                     skybox->prefilterTexture->width,
                     skybox->prefilterTexture->height,
                     0,
                     skybox->prefilterTexture->format,
                     skybox->prefilterTexture->type,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->prefilterTexture->id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glUseProgram(skybox->prefilterShader->program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->skyTexture->id);
    glUniform1i(glGetUniformLocation(skybox->prefilterShader->program, "texPrefilter"), 0);
    glUniformMatrix4fv(glGetUniformLocation(skybox->prefilterShader->program, "Projection"),
                       1,
                       0,
                       static_cast<const GLfloat*>(&skySnapProjection[0][0]));

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    u32 maxMipmapLevels = 5;
    for (u32 m = 0; m < maxMipmapLevels; m++) {
        u32 mipmapWidth  = prefilterSize * std::pow(0.5, m);
        u32 mipmapHeight = prefilterSize * std::pow(0.5, m);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mipmapWidth, mipmapHeight);
        glViewport(0, 0, mipmapWidth, mipmapHeight);
        f32 roughness = (f32)m / (f32)(maxMipmapLevels - 1);
        glUniform1fv(glGetUniformLocation(skybox->prefilterShader->program, "Roughness"), 1, &roughness);
        for (u32 i = 0; i < 6; i++) {
            glUniformMatrix4fv(glGetUniformLocation(skybox->prefilterShader->program, "View"),
                               1,
                               0,
                               static_cast<const GLfloat*>(&skySnapViews[i][0][0]));
            glFramebufferTexture2D(
              GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skybox->prefilterTexture->id, m);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(cube->vao);
            glDrawElements(GL_TRIANGLES, cube->indexCount, cube->type, 0);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    skybox->brdfTexture->target = GL_TEXTURE_2D;
    skybox->brdfTexture->width  = cubeSize;
    skybox->brdfTexture->height = cubeSize;

    GLuint brdfTexture;
    glGenTextures(1, &brdfTexture);
    skybox->brdfTexture->id             = brdfTexture;
    skybox->brdfTexture->target         = GL_TEXTURE_2D;
    skybox->brdfTexture->internalFormat = GL_RG32F;
    skybox->brdfTexture->width          = cubeSize;
    skybox->brdfTexture->height         = cubeSize;
    skybox->brdfTexture->format         = GL_RG;
    skybox->brdfTexture->type           = GL_FLOAT;
    glBindTexture(GL_TEXTURE_2D, skybox->brdfTexture->id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 skybox->brdfTexture->internalFormat,
                 skybox->brdfTexture->width,
                 skybox->brdfTexture->height,
                 0,
                 skybox->brdfTexture->format,
                 skybox->brdfTexture->type,
                 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cubeSize, cubeSize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skybox->brdfTexture->id, 0);

    glViewport(0, 0, cubeSize, cubeSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(skybox->brdfShader->program);
    glBindVertexArray(plane->vao);
    glDrawElements(GL_TRIANGLES, plane->indexCount, plane->type, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);

    PglReleaseMesh(plane.get());
    return true;
}

PROTOTYPE_EXTERN void
PglReleaseSkybox(PglSkybox* skybox)
{
    PglReleaseTexture(skybox->skyTexture.get());
    PglReleaseTexture(skybox->hdrTexture.get());
    PglReleaseTexture(skybox->irradianceTexture.get());
    PglReleaseTexture(skybox->prefilterTexture.get());
    PglReleaseTexture(skybox->brdfTexture.get());
}

PROTOTYPE_EXTERN bool
PglUploadUniformBufferObject(const size_t bytes, const size_t index, PglUniformBufferObject* ubo)
{
    GLuint id;
    glGenBuffers(1, &id);
    ubo->id = id;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->id);
    glBufferData(GL_UNIFORM_BUFFER, bytes, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, index, ubo->id, 0, bytes);
    return true;
}

PROTOTYPE_EXTERN void
PglReleaseUniformBufferObject(PglUniformBufferObject* ubo)
{
    glDeleteBuffers(1, &ubo->id);
}