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

#pragma warning(disable : 4003)

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include "gl4/gl.h"
#define GL_MAJOR 4
#define GL_MINOR 1

#include <GLFW/glfw3.h>

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#include <PrototypeCommon/Maths.h>
#include <PrototypeCommon/MemoryPool.h>

#include <array>
#include <functional>
#include <memory>
#include <vector>

struct PrototypeMaterial;
struct PrototypeMeshBuffer;
struct PrototypeShaderBuffer;
struct PrototypeTextureBuffer;

struct PrototypeSceneNode;

struct PglGeometryInfo
{
    struct POSITION
    {
        const static size_t INDEX  = 0;
        const static size_t STRIDE = 0;
        const static size_t LENGTH = 4;
        const static size_t SIZE   = sizeof(f32) * LENGTH;
    };
    struct NORMAL
    {
        const static size_t INDEX  = 1;
        const static size_t STRIDE = POSITION::SIZE;
        const static size_t LENGTH = 4;
        const static size_t SIZE   = sizeof(f32) * LENGTH;
    };
    struct COLOR
    {
        const static size_t INDEX  = 2;
        const static size_t STRIDE = NORMAL::STRIDE + NORMAL::SIZE;
        const static size_t LENGTH = 4;
        const static size_t SIZE   = sizeof(f32) * LENGTH;
    };
    const static size_t LENGTH = POSITION::LENGTH + NORMAL::LENGTH + COLOR::LENGTH;
    const static size_t SIZE   = POSITION::SIZE + NORMAL::SIZE + COLOR::SIZE;
};

struct PglGeometry
{
    GLuint      vao;
    GLuint      vbo;
    GLuint      eabo;
    GLenum      type;
    u32         indexCount;
    std::string name;

    bool operator<(const PglGeometry& o) const { return vao < o.vao; }
};

struct PglTexture
{
    GLuint      id;
    GLenum      target;
    GLenum      type;
    GLenum      format;
    GLint       internalFormat;
    GLsizei     width;
    GLsizei     height;
    std::string name;

    bool operator<(const PglTexture& o) const { return id < o.id; }
};

struct PglShader
{
    GLuint                                         program;
    std::string                                    name;
    std::vector<std::string>                       textureData;
    std::vector<std::pair<std::string, f32>>       floatData;
    std::vector<std::pair<std::string, glm::vec2>> vec2Data;
    std::vector<std::pair<std::string, glm::vec3>> vec3Data;
    std::vector<std::pair<std::string, glm::vec4>> vec4Data;

    bool operator<(const PglShader& o) const { return program < o.program; }
};

struct PglMaterial
{
    PglShader*                                     shader;
    std::vector<PglTexture*>                       textures;
    glm::vec3                                      baseColor;
    f32                                            metallic;
    f32                                            roughness;
    std::string                                    name;
    std::vector<std::string>                       textureData;
    std::vector<std::pair<std::string, f32>>       floatData;
    std::vector<std::pair<std::string, glm::vec2>> vec2Data;
    std::vector<std::pair<std::string, glm::vec3>> vec3Data;
    std::vector<std::pair<std::string, glm::vec4>> vec4Data;
};

struct PglFramebuffer
{
    GLuint                   fbo;
    GLuint                   rbo;
    PglGeometry              surface;
    PglShader*               shader;
    std::vector<PglTexture>  colorAttachments;
    std::vector<std::string> colorAttachmentsNames;
    PglTexture               depthAttachment;
    std::string              name;
};

struct PglUniformBufferObject
{
    GLuint id;
};

struct PglCamera
{
    PrototypeSceneNode*     node;
    PrototypeObject*        object;
    PglFramebuffer*         deferredFramebuffer;
    PglFramebuffer*         gbufferFramebuffer;
    PglFramebuffer*         postprocessingFramebuffer;
    PglFramebuffer*         finalFramebuffer;
    PglUniformBufferObject* ubo;
    const float*            viewMatrix;
    const float*            projectionMatrix;
};

struct PglSkybox
{
    const PglShader*            skyShader;
    const PglShader*            hdrShader;
    const PglShader*            irradianceShader;
    const PglShader*            prefilterShader;
    const PglShader*            brdfShader;
    std::shared_ptr<PglTexture> skyTexture;
    std::shared_ptr<PglTexture> hdrTexture;
    std::shared_ptr<PglTexture> irradianceTexture;
    std::shared_ptr<PglTexture> prefilterTexture;
    std::shared_ptr<PglTexture> brdfTexture;
};

// enum PglRecordedEvent_
// {
//     // meshes
//     PglRecordedEvent_MeshBind,
//     PglRecordedEvent_MeshUnBind,

//     // shaders
//     PglRecordedEvent_ShaderBind,
//     PglRecordedEvent_ShaderBindTexture,
//     PglRecordedEvent_ShaderBindFloat,
//     PglRecordedEvent_ShaderBindVec2,
//     PglRecordedEvent_ShaderBindVec3,
//     PglRecordedEvent_ShaderBindVec4,
//     PglRecordedEvent_ShaderBindMat2,
//     PglRecordedEvent_ShaderBindMat3,
//     PglRecordedEvent_ShaderBindMat4,
//     PglRecordedEvent_ShaderUnBind,

//     // textures
//     PglRecordedEvent_TextureBind,
//     PglRecordedEvent_TextureActive,
//     PglRecordedEvent_TextureUnBind,
// };

struct PglCall_glAbstract
{
    virtual void call() const = 0;
};

#define PROTOTYPE_BUILD_STRUCTURE_FIELD(A)          A;
#define PROTOTYPE_BUILD_STRUCTURE_FIELD_EXTENDED(A) A;

#define PGLCALL_DECLARATION(PFN_NAME, ...)                                                                                       \
    struct PglCall_##PFN_NAME : PglCall_glAbstract                                                                               \
    {                                                                                                                            \
        PROTOTYPE_FOR_EACH_X(PROTOTYPE_BUILD_STRUCTURE_FIELD, PROTOTYPE_BUILD_STRUCTURE_FIELD_EXTENDED, __VA_ARGS__)             \
        virtual void                             call() const override;                                                          \
        static MemoryPool<PglCall_##PFN_NAME, 4> _allocator;                                                                     \
    };

PGLCALL_DECLARATION(glBindVertexArray, GLuint index)
PGLCALL_DECLARATION(glDrawElements, GLenum mode, GLsizei count, GLenum type, const void* indices)
PGLCALL_DECLARATION(glDrawElementsInstanced, GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instanceCount)
PGLCALL_DECLARATION(glDrawArrays, GLenum mode, GLint firstIndex, GLsizei count)
PGLCALL_DECLARATION(glDrawArraysInstanced, GLenum mode, GLint firstIndex, GLsizei count, GLsizei instanceCount)

PGLCALL_DECLARATION(glUseProgram, GLuint program)
PGLCALL_DECLARATION(glUniform1ui, GLint location, GLuint v0)
PGLCALL_DECLARATION(glUniform1i, GLint location, GLint v0)
PGLCALL_DECLARATION(glUniform1fv, GLint location, GLsizei count, const GLfloat* value)
PGLCALL_DECLARATION(glUniform2fv, GLint location, GLsizei count, const GLfloat* value)
PGLCALL_DECLARATION(glUniform3fv, GLint location, GLsizei count, const GLfloat* value)
PGLCALL_DECLARATION(glUniform4fv, GLint location, GLsizei count, const GLfloat* value)
PGLCALL_DECLARATION(glUniformMatrix2fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
PGLCALL_DECLARATION(glUniformMatrix3fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
PGLCALL_DECLARATION(glUniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

PGLCALL_DECLARATION(glBindTexture, GLenum target, GLuint id)
PGLCALL_DECLARATION(glActiveTexture, GLenum texture)

PGLCALL_DECLARATION(glUniformBlockBinding, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)

extern void
PglCallResetAllocations();

#undef PROTOTYPE_BUILD_STRUCTURE_FIELD
#undef PROTOTYPE_BUILD_STRUCTURE_FIELD_EXTENDED

struct PrototypeOpenglCommand
{
    void                record(PglCall_glAbstract* call);
    void                dispatch();
    PglCall_glAbstract* instruction() { return _instruction; }

  private:
    PglCall_glAbstract* _instruction;
};

PROTOTYPE_EXTERN bool
PglUploadMeshFromBuffer(const PrototypeMeshBuffer* meshBuffer, PglGeometry* geometry);
PROTOTYPE_EXTERN void
PglReleaseMesh(PglGeometry* mesh);

PROTOTYPE_EXTERN bool
PglUploadShaderFromBuffer(const PrototypeShaderBuffer* shaderBuffer, PglShader* shader);
PROTOTYPE_EXTERN void
PglReleaseShader(PglShader* shader);

PROTOTYPE_EXTERN bool
PglUploadTextureFromBuffer(const PrototypeTextureBuffer* textureBuffer, PglTexture* texture);
PROTOTYPE_EXTERN void
PglReleaseTexture(PglTexture* texture);

PROTOTYPE_EXTERN bool
PglUploadFramebufferFromData(PglShader* shader, PglFramebuffer* framebuffer, i32 width, i32 height, bool withDepthAttachment);
PROTOTYPE_EXTERN void
PglReleaseFramebuffer(PglFramebuffer* framebuffer);
PROTOTYPE_EXTERN void
PglFramebufferResize(PglFramebuffer* framebuffer, i32 width, i32 height);
PROTOTYPE_EXTERN void
PglFramebufferStartRecording2D(PglFramebuffer* framebuffer);
PROTOTYPE_EXTERN void
PglFramebufferStartRecording3D(PglFramebuffer* framebuffer);
PROTOTYPE_EXTERN void
PglFramebufferEndRecording(PglFramebuffer* framebuffer);
PROTOTYPE_EXTERN void
PglFramebufferStartRendering(PglFramebuffer* framebuffer);
PROTOTYPE_EXTERN void
PglFramebufferEndRendering(PglFramebuffer* framebuffer);
PROTOTYPE_EXTERN void
PglFramebufferCaptureFrame(PglFramebuffer* framebuffer, void* pixels, size_t colorAttachmentIndex);

PROTOTYPE_EXTERN bool
PglUploadSkybox(const PglGeometry* cube,
                const PglShader*   skyShader,
                const PglShader*   hdrShader,
                const PglShader*   irradianceShader,
                const PglShader*   prefilterShader,
                const PglShader*   brdfShader,
                PglSkybox*         skybox);
PROTOTYPE_EXTERN void
PglReleaseSkybox(PglSkybox* skybox);

PROTOTYPE_EXTERN bool
PglUploadUniformBufferObject(const size_t bytes, const size_t index, PglUniformBufferObject* ubo);
PROTOTYPE_EXTERN void
PglReleaseUniformBufferObject(PglUniformBufferObject* ubo);