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

#include "PrototypeOpenglRenderer.h"
#include "../core/PrototypeCameraSystem.h"
#include "../core/PrototypeDatabase.h"
#include "../core/PrototypeFrameBuffer.h"
#include "../core/PrototypeInput.h"
#include "../core/PrototypeMaterial.h"
#include "../core/PrototypeMeshBuffer.h"
#include "../core/PrototypePhysics.h"
#include "../core/PrototypeProfiler.h"
#include "../core/PrototypeScene.h"
#include "../core/PrototypeSceneLayer.h"
#include "../core/PrototypeSceneNode.h"
#include "../core/PrototypeShaderBuffer.h"
#include "../core/PrototypeShortcuts.h"
#include "../core/PrototypeTextureBuffer.h"
#include "PrototypeOpenglWindow.h"
#include <PrototypeCommon/Algo.h>

#include <PrototypeCommon/Logger.h>

#include "../core/PrototypeEngine.h"
#include "../core/PrototypeScene.h"

#include <PrototypeTraitSystem/PrototypeTraitSystem.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fstream>
#include <math.h>

#define PROTOTYPE_REGISTER_PROFILER_FUNCTIONS()                                                                                  \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::update");                            \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::update",                        \
                                                                       { 0.498f, 0.235f, 0.552f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::render3D");                          \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::render3D",                      \
                                                                       { 0.066f, 0.647f, 0.474f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::render2D");                          \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::render2D",                      \
                                                                       { 0.223f, 0.411f, 0.674f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::switchScenes");                      \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::switchScenes",                  \
                                                                       { 0.949f, 0.717f, 0.003f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::scheduleRecordPass");                \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::scheduleRecordPass",            \
                                                                       { 0.905f, 0.247f, 0.454f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::beginRecordPass");                   \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::beginRecordPass",               \
                                                                       { 0.501f, 0.729f, 0.352f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::endRecordPass");                     \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::endRecordPass",                 \
                                                                       { 0.901f, 0.513f, 0.062f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::onMeshBufferGpuUpload");             \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::onMeshBufferGpuUpload",         \
                                                                       { 0.0f, 0.525f, 0.584f, 1.0f });                          \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::onShaderBufferGpuUpload");           \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::onShaderBufferGpuUpload",       \
                                                                       { 0.811f, 0.109f, 0.564f, 1.0f });                        \
    PrototypeEngineInternalApplication::profiler->addTimelineItem("PrototypeOpenglRenderer::onTextureBufferGpuUpload");          \
    PrototypeEngineInternalApplication::profiler->setTimelineItemColor("PrototypeOpenglRenderer::onTextureBufferGpuUpload",      \
                                                                       { 0.976f, 0.482f, 0.447f, 1.0f });

#define PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()                                                                             \
    static auto profilerT1 = std::chrono::high_resolution_clock::now();                                                          \
    profilerT1             = std::chrono::high_resolution_clock::now();

#define PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(NAME)                                                                           \
    static auto profilerT2      = std::chrono::high_resolution_clock::now();                                                     \
    profilerT2                  = std::chrono::high_resolution_clock::now();                                                     \
    static u32 profilerDuration = 0;                                                                                             \
    profilerDuration            = std::chrono::duration_cast<std::chrono::microseconds>(profilerT2 - profilerT1).count();        \
    PrototypeEngineInternalApplication::profiler->markTimelineItem(#NAME, profilerDuration);

void
PrototypeOpenglRenderer::onMaterialShaderUpdate(PglMaterial* material, PglShader* shader)
{
    PrototypeAlgoCopyNewValues(shader->floatData, material->floatData);
    PrototypeAlgoCopyNewValues(shader->vec2Data, material->vec2Data);
    PrototypeAlgoCopyNewValues(shader->vec3Data, material->vec3Data);
    PrototypeAlgoCopyNewValues(shader->vec4Data, material->vec4Data);
    PrototypeAlgoCopyNewValues(shader->textureData, material->textureData);
}

PrototypeOpenglRenderer::PrototypeOpenglRenderer(PrototypeOpenglWindow* window)
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
  : _editorSceneCamera({})
//   , _editorPaintCamera({})
#else
  : _mainCamera({})
#endif
  , _window(window)
  , _fullscreenFramebuffer(nullptr)
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
  , _ui(std::make_unique<PrototypeOpenglUI>())
#endif
  , _uiState(PrototypeUIState_None)
  , _needsRecord(true)
{}

bool
PrototypeOpenglRenderer::init()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTIONS()
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        PrototypeLogger::fatal("Couldn't load OpenGL Code");
        return false;
    }

    for (auto& pair : PrototypeEngineInternalApplication::database->meshBuffers) {
        PrototypeMeshBuffer* meshBuffer = pair.second;
        mapPrototypeMeshBuffer(meshBuffer);
    }

    for (auto& pair : PrototypeEngineInternalApplication::database->shaderBuffers) {
        PrototypeShaderBuffer* shaderBuffer = pair.second;
        mapPrototypeShaderBuffer(shaderBuffer);
    }

    for (auto& pair : PrototypeEngineInternalApplication::database->textureBuffers) {
        PrototypeTextureBuffer* textureBuffer = pair.second;
        mapPrototypeTextureBuffer(textureBuffer);
    }

    for (auto& pair : PrototypeEngineInternalApplication::database->materials) {
        const std::string& name       = pair.first;
        PrototypeMaterial* dbMaterial = pair.second;
        mapPrototypeMaterial(dbMaterial);
    }
    for (auto& pair : PrototypeEngineInternalApplication::database->framebuffers) {
        const std::string     name        = pair.first;
        PrototypeFrameBuffer* framebuffer = pair.second;
        if (_framebuffers.find(framebuffer->name()) != _framebuffers.end()) { continue; }
        auto shaderIt = _shaders.find(framebuffer->shader());
        if (shaderIt == _shaders.end()) { continue; }
        auto fb  = _framebuffersPool.newElement();
        fb->name = name;
        fb->colorAttachments.resize(framebuffer->numColorAttachments());
        PglUploadFramebufferFromData(
          shaderIt->second, fb, (i32)_window->_resolution.x, (i32)_window->_resolution.y, framebuffer->withDepthAttachment());
        _framebuffers.insert({ framebuffer->name(), fb });
    }

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->init();
#else
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
#endif

    // deferred
    auto deferredFramebufferIt = _framebuffers.find("deferred");
    if (deferredFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without deferred framebuffer"); }
    // gbuffer
    auto gbufferFramebufferIt = _framebuffers.find("gbuffer");
    if (gbufferFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without gbuffer framebuffer"); }
    // postprocessing
    auto postProcessingFramebufferIt = _framebuffers.find("postprocessing");
    if (postProcessingFramebufferIt == _framebuffers.end()) {
        PrototypeLogger::fatal("Cannot run without postprocessing framebuffer");
    }
    // final
    auto finalFramebufferIt = _framebuffers.find("final");
    if (finalFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without final framebuffer"); }

    switchScenes(PrototypeEngineInternalApplication::scene);

    // fullscreen fb
    auto fullscreenFramebufferIt = _framebuffers.find("fullscreen");
    if (fullscreenFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without framebuffer shaders"); }
    _fullscreenFramebuffer = fullscreenFramebufferIt->second;

    auto cube             = _geometries.find("CUBE")->second;
    auto skyShader        = _shaders.find("sky")->second;
    auto hdrShader        = _shaders.find("hdr")->second;
    auto irradianceShader = _shaders.find("irradiance")->second;
    auto prefilterShader  = _shaders.find("prefilter")->second;
    auto brdfShader       = _shaders.find("brdf")->second;
    _skybox               = std::make_shared<PglSkybox>();
    PglUploadSkybox(cube, skyShader, hdrShader, irradianceShader, prefilterShader, brdfShader, _skybox.get());

#ifndef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    onWindowResize(_window->_resolution.x, _window->_resolution.y);
#endif

    return true;
}

void
PrototypeOpenglRenderer::deInit()
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->deInit();
#else
    delete _input;
#endif

    for (const auto& pair : _geometries) { PglReleaseMesh(pair.second); }
    _geometries.clear();
    for (const auto& pair : _shaders) { PglReleaseShader(pair.second); }
    _shaders.clear();
    for (const auto& pair : _textures) { PglReleaseTexture(pair.second); }
    _textures.clear();
    for (const auto& pair : _framebuffers) { PglReleaseFramebuffer(pair.second); }
    _framebuffers.clear();
    for (const auto& pair : _uniformBufferObjects) { PglReleaseUniformBufferObject(pair.second); }
    _uniformBufferObjects.clear();
    _skybox.reset();
}

bool
PrototypeOpenglRenderer::update()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

#if defined(PROTOTYPE_ENGINE_DEVELOPMENT_MODE)
    _ui->sceneView()->onUpdate();
#else
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

    Camera* cam = _mainCamera.object->getCameraTrait();
    CameraSystemUpdateViewMatrix(cam, cameraLeft - cameraRight, cameraDown - cameraUp, cameraForward - cameraBackward);
    CameraSystemUpdateProjectionMatrix(cam);

    static glm::vec4 LightPosition;
    LightPosition = { 0.0f, 100.0f, 0.0f, _window->time() };
    static glm::vec4 LightColor;
    LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    glBindBuffer(GL_UNIFORM_BUFFER, _mainCamera.ubo->id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &cam->viewMatrix()[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &cam->projectionMatrix()[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &cam->znear());
    glBufferSubData(GL_UNIFORM_BUFFER, 132, 4, &cam->zfar());
    glBufferSubData(GL_UNIFORM_BUFFER, 136, 8, &_window->_mouseDown.x);
    glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, &LightPosition[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 160, 16, &LightColor[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
#endif

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::update)
#endif

    return true;
}

bool
PrototypeOpenglRenderer::render3D()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

    if (_uiState & PrototypeUIState_Iconified) return true;

    PglGeometry* cube = _geometries.find("CUBE")->second;

    glViewport(0, 0, (GLsizei)_window->_resolution.x, (GLsizei)_window->_resolution.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PglCamera& camera = _editorSceneCamera;
#else
    PglCamera& camera = _mainCamera;
#endif

    PglFramebufferStartRecording3D(camera.deferredFramebuffer);
#if 0
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glUseProgram(_skybox->skyShader->program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox->skyTexture->id);
    glUniform1i(glGetUniformLocation(_skybox->skyShader->program, "t_sky"), 0);
    glUniformMatrix4fv(glGetUniformLocation(_skybox->skyShader->program, "View"), 1, 0, camera.viewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(_skybox->skyShader->program, "Projection"), 1, 0,
    camera.projectionMatrix); glBindVertexArray(cube->vao); glDrawElements(GL_TRIANGLES, cube->indexCount,
    cube->type, 0); glBindVertexArray(0); glBindTexture(GL_TEXTURE_CUBE_MAP, 0); glUseProgram(0);
#endif
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // glDepthFunc(GL_LEQUAL);
    for (PrototypeOpenglCommand* command : _commands) { command->dispatch(); }
    glDisable(GL_CULL_FACE);
    /*static auto lineShader = _shaders["ray"];
    glUseProgram(lineShader->program);
    for () {}*/
    PglFramebufferEndRecording(camera.deferredFramebuffer);

    // scene
    PglFramebufferStartRecording2D(camera.gbufferFramebuffer);
    glDepthMask(GL_FALSE);
    // glDepthFunc(GL_LEQUAL);
    PglFramebufferStartRendering(camera.deferredFramebuffer);
    // add skybox textures and View uniform !!
    {
        // values
        {
            for (auto& pair : camera.deferredFramebuffer->shader->floatData) {
                glUniform1fv(
                  glGetUniformLocation(camera.deferredFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second);
            }
            for (auto& pair : camera.deferredFramebuffer->shader->vec2Data) {
                glUniform2fv(
                  glGetUniformLocation(camera.deferredFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second[0]);
            }
            for (auto& pair : camera.deferredFramebuffer->shader->vec3Data) {
                glUniform3fv(
                  glGetUniformLocation(camera.deferredFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second[0]);
            }
            for (auto& pair : camera.deferredFramebuffer->shader->vec4Data) {
                glUniform4fv(
                  glGetUniformLocation(camera.deferredFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second[0]);
            }
        }

        size_t t = camera.deferredFramebuffer->colorAttachments.size();
        // cube
        {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(_skybox->skyTexture->target, _skybox->skyTexture->id);
            glUniform1i(glGetUniformLocation(camera.deferredFramebuffer->shader->program, "texSky"), t);
        }

        // irradaince
        ++t;
        {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(_skybox->irradianceTexture->target, _skybox->irradianceTexture->id);
            glUniform1i(glGetUniformLocation(camera.deferredFramebuffer->shader->program, "texIrradiance"), t);
        }

        // prefilter
        ++t;
        {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(_skybox->prefilterTexture->target, _skybox->prefilterTexture->id);
            glUniform1i(glGetUniformLocation(camera.deferredFramebuffer->shader->program, "texPrefilter"), t);
        }

        // brdf
        ++t;
        {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(_skybox->brdfTexture->target, _skybox->brdfTexture->id);
            glUniform1i(glGetUniformLocation(camera.deferredFramebuffer->shader->program, "texBrdf"), t);
        }

        // depth
        ++t;
        {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(camera.deferredFramebuffer->depthAttachment.target, camera.deferredFramebuffer->depthAttachment.id);
            glUniform1i(glGetUniformLocation(camera.deferredFramebuffer->shader->program, "texDepth"), t);
        }
    }
    PglFramebufferEndRendering(camera.deferredFramebuffer);
    // glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    PglFramebufferEndRecording(camera.gbufferFramebuffer);

    // gbuffer
    PglFramebufferStartRecording2D(camera.postprocessingFramebuffer);
    glDepthMask(GL_FALSE);
    // glDepthFunc(GL_LEQUAL);
    PglFramebufferStartRendering(camera.gbufferFramebuffer);
    {
        // values
        {
            for (auto& pair : camera.gbufferFramebuffer->shader->floatData) {
                glUniform1fv(
                  glGetUniformLocation(camera.gbufferFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second);
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->vec2Data) {
                glUniform2fv(
                  glGetUniformLocation(camera.gbufferFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second[0]);
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->vec3Data) {
                glUniform3fv(
                  glGetUniformLocation(camera.gbufferFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second[0]);
            }
            for (auto& pair : camera.gbufferFramebuffer->shader->vec4Data) {
                glUniform4fv(
                  glGetUniformLocation(camera.gbufferFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second[0]);
            }
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(camera.gbufferFramebuffer->colorAttachments[0].target, camera.gbufferFramebuffer->colorAttachments[0].id);
        glUniform1i(glGetUniformLocation(camera.gbufferFramebuffer->shader->program, "tex0"), 0);
    }
    PglFramebufferEndRendering(camera.gbufferFramebuffer);
    // glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    PglFramebufferEndRecording(camera.postprocessingFramebuffer);

    // post processing
    PglFramebufferStartRecording2D(camera.finalFramebuffer);
    glDepthMask(GL_FALSE);
    // glDepthFunc(GL_LEQUAL);
    PglFramebufferStartRendering(camera.postprocessingFramebuffer);
    {
        // values
        {
            for (auto& pair : camera.postprocessingFramebuffer->shader->floatData) {
                glUniform1fv(
                  glGetUniformLocation(camera.postprocessingFramebuffer->shader->program, pair.first.c_str()), 1, &pair.second);
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->vec2Data) {
                glUniform2fv(glGetUniformLocation(camera.postprocessingFramebuffer->shader->program, pair.first.c_str()),
                             1,
                             &pair.second[0]);
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->vec3Data) {
                glUniform3fv(glGetUniformLocation(camera.postprocessingFramebuffer->shader->program, pair.first.c_str()),
                             1,
                             &pair.second[0]);
            }
            for (auto& pair : camera.postprocessingFramebuffer->shader->vec4Data) {
                glUniform4fv(glGetUniformLocation(camera.postprocessingFramebuffer->shader->program, pair.first.c_str()),
                             1,
                             &pair.second[0]);
            }
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(camera.postprocessingFramebuffer->colorAttachments[0].target,
                      camera.postprocessingFramebuffer->colorAttachments[0].id);
        glUniform1i(glGetUniformLocation(camera.postprocessingFramebuffer->shader->program, "tex0"), 0);
    }
    PglFramebufferEndRendering(camera.postprocessingFramebuffer);
    // glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    PglFramebufferEndRecording(camera.finalFramebuffer);

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::render3D)
#endif

    return true;
}

bool
PrototypeOpenglRenderer::render2D()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    _ui->beginFrame(false);
    {
        _uiState = _ui->drawFrame(_editorSceneCamera.finalFramebuffer->colorAttachments[0].id,
                                  _editorSceneCamera.finalFramebuffer->colorAttachments[0].width,
                                  _editorSceneCamera.finalFramebuffer->colorAttachments[0].height);
    }
    _ui->endFrame();
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    PglFramebufferStartRecording2D(_fullscreenFramebuffer);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    _ui->render(0, 0, (i32)_window->_resolution.x, (i32)_window->_resolution.y);
    glDisable(GL_CULL_FACE);
    PglFramebufferEndRecording(_fullscreenFramebuffer);
    PglFramebufferStartRendering(_fullscreenFramebuffer);
    PglFramebufferEndRendering(_fullscreenFramebuffer);
#else
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    PglFramebufferStartRendering(_mainCamera.finalFramebuffer);
    PglFramebufferEndRendering(_mainCamera.finalFramebuffer);
    glDisable(GL_CULL_FACE);
#endif

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::render2D)
#endif

    return true;
}

void
PrototypeOpenglRenderer::switchScenes(PrototypeScene* scene)
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

    PrototypeEngineInternalApplication::scene = scene;

    // deferred
    auto deferredFramebufferIt = _framebuffers.find("deferred");
    if (deferredFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without deferred framebuffer"); }
    // gbuffer
    auto gbufferFramebufferIt = _framebuffers.find("gbuffer");
    if (gbufferFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without gbuffer framebuffer"); }
    // postprocessing
    auto postProcessingFramebufferIt = _framebuffers.find("postprocessing");
    if (postProcessingFramebufferIt == _framebuffers.end()) {
        PrototypeLogger::fatal("Cannot run without postprocessing framebuffer");
    }
    // final
    auto finalFramebufferIt = _framebuffers.find("final");
    if (finalFramebufferIt == _framebuffers.end()) { PrototypeLogger::fatal("Cannot run without final framebuffer"); }

    auto cameraObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);

    for (auto cameraIterator = cameraObjects.begin(); cameraIterator != cameraObjects.end(); ++cameraIterator) {
        PrototypeSceneNode* cameraNode = (PrototypeSceneNode*)(*cameraIterator)->parentNode();
        if (cameraNode) {
            if (cameraNode->name() == "SceneCamera") {
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
                _editorSceneCamera.deferredFramebuffer       = deferredFramebufferIt->second;
                _editorSceneCamera.gbufferFramebuffer        = gbufferFramebufferIt->second;
                _editorSceneCamera.postprocessingFramebuffer = postProcessingFramebufferIt->second;
                _editorSceneCamera.finalFramebuffer          = finalFramebufferIt->second;
                _editorSceneCamera.object                    = *cameraIterator;
                _editorSceneCamera.node = static_cast<PrototypeSceneNode*>(_editorSceneCamera.object->parentNode());
                _editorSceneCamera.viewMatrix =
                  static_cast<const GLfloat*>(&_editorSceneCamera.object->getCameraTrait()->viewMatrix()[0][0]);
                _editorSceneCamera.projectionMatrix =
                  static_cast<const GLfloat*>(&_editorSceneCamera.object->getCameraTrait()->projectionMatrix()[0][0]);
                // set the corresponding ubo
                {
                    auto   commonUbo = _uniformBufferObjectsPool.newElement();
                    size_t bytes     = 0;
                    bytes += sizeof(float) * 16; // view matrix
                    bytes += sizeof(float) * 16; // projection matrix
                    bytes += sizeof(float) * 4;  // near, far, mousex, mousey
                    bytes += sizeof(float) * 4;  // directional light position (x y z), time
                    bytes += sizeof(float) * 4;  // directional light color (x y z), (empty)
                    PglUploadUniformBufferObject(bytes, 0, commonUbo);
                    _uniformBufferObjects.insert({ "Common", commonUbo });
                    _editorSceneCamera.ubo = commonUbo;
                }
                _ui->sceneView()->setCamera(&_editorSceneCamera);
#endif
            } else if (cameraNode->name() == "MainCamera") {
#ifndef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
                _mainCamera.deferredFramebuffer       = deferredFramebufferIt->second;
                _mainCamera.gbufferFramebuffer        = gbufferFramebufferIt->second;
                _mainCamera.postprocessingFramebuffer = postProcessingFramebufferIt->second;
                _mainCamera.finalFramebuffer          = finalFramebufferIt->second;
                _mainCamera.object                    = *cameraIterator;
                _mainCamera.node                      = static_cast<PrototypeSceneNode*>(_mainCamera.object->parentNode());
                _mainCamera.viewMatrix = static_cast<const GLfloat*>(&_mainCamera.object->getCameraTrait()->viewMatrix()[0][0]);
                _mainCamera.projectionMatrix =
                  static_cast<const GLfloat*>(&_mainCamera.object->getCameraTrait()->projectionMatrix()[0][0]);
                // set the corresponding ubo
                {
                    auto   commonUbo = _uniformBufferObjectsPool.newElement();
                    size_t bytes     = 0;
                    bytes += sizeof(float) * 16; // view matrix
                    bytes += sizeof(float) * 16; // projection matrix
                    bytes += sizeof(float) * 4;  // near, far, mousex, mousey
                    bytes += sizeof(float) * 4;  // directional light position (x y z), time
                    bytes += sizeof(float) * 4;  // directional light color (x y z), (empty)
                    PglUploadUniformBufferObject(bytes, 0, commonUbo);
                    _uniformBufferObjects.insert({ "Common", commonUbo });
                    _mainCamera.ubo = commonUbo;
                }
#endif
            } else if (cameraNode->name() == "PaintCamera") {
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
                _editorPaintCamera.deferredFramebuffer       = deferredFramebufferIt->second;
                _editorPaintCamera.gbufferFramebuffer        = gbufferFramebufferIt->second;
                _editorPaintCamera.postprocessingFramebuffer = postProcessingFramebufferIt->second;
                _editorPaintCamera.finalFramebuffer          = finalFramebufferIt->second;
                _editorPaintCamera.object                    = *cameraIterator;
                _editorPaintCamera.node = static_cast<PrototypeSceneNode*>(_editorPaintCamera.object->parentNode());
                _editorPaintCamera.viewMatrix =
                  static_cast<const GLfloat*>(&_editorPaintCamera.object->getCameraTrait()->viewMatrix()[0][0]);
                _editorPaintCamera.projectionMatrix =
                  static_cast<const GLfloat*>(&_editorPaintCamera.object->getCameraTrait()->projectionMatrix()[0][0]);
#endif
            }
        }
    }

    PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    PrototypeEngineInternalApplication::physics->scheduleRecordPass();

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::switchScenes)
#endif
}

void
PrototypeOpenglRenderer::scheduleRecordPass()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

    _needsRecord = true;
    PglCallResetAllocations();
    _commandsPool.clear();
    _commands.clear();

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::scheduleRecordPass)
#endif
}

void
PrototypeOpenglRenderer::beginRecordPass()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

    if (!_needsRecord) return;
    _needsRecord = false;

    // <shader use>
    //              <texture data>
    //              <vec4>
    //              <vec3>
    //              <vec2>
    //              <float>
    //                  <matrices (Model)>
    //                  <mesh>

    struct MeshDrawCommand
    {
        GLenum  mode;
        GLenum  type;
        GLsizei count;
    };

    struct OrderedDrawSubCommand
    {
        PrototypeOpenglCommand*                                  modelMatrixCommand = {};
        PrototypeOpenglCommand*                                  objectIdCommand    = {};
        std::unordered_map<GLuint, std::vector<MeshDrawCommand>> meshCommands;
    };
    struct OrderedDrawCommand
    {
        PrototypeOpenglCommand*              shaderCommand = nullptr;
        std::vector<PrototypeOpenglCommand*> texturesCommand;
        std::vector<PrototypeOpenglCommand*> vec4sCommand;
        std::vector<PrototypeOpenglCommand*> vec3sCommand;
        std::vector<PrototypeOpenglCommand*> vec2sCommand;
        std::vector<PrototypeOpenglCommand*> floatsCommand;
        std::vector<OrderedDrawSubCommand*>  subCommands;
    };
    MemoryPool<OrderedDrawCommand, 100>                                              _orderedCommandsPool;
    MemoryPool<OrderedDrawSubCommand, 100>                                           _orderedSubCommandsPool;
    std::unordered_map<GLuint, std::unordered_map<std::string, OrderedDrawCommand*>> orderedCommands;

    std::vector<PrototypeSceneNode*> selectedNodes;
    for (const auto& selectedNode : PrototypeEngineInternalApplication::scene->selectedNodes()) {
        selectedNodes.push_back(selectedNode);
    }
    PrototypeEngineInternalApplication::scene->clearSelectedNodes();
    auto meshRendererObjects = PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(
      PrototypeTraitTypeMaskMeshRenderer | PrototypeTraitTypeMaskTransform);

    for (const auto& meshRendererObject : meshRendererObjects) {
        PrototypeObject* object = meshRendererObject;
        if (((PrototypeSceneNode*)object->parentNode())->absoluteLayer()->name() != "default") continue;
        MeshRenderer* mr        = object->getMeshRendererTrait();
        Transform*    transform = object->getTransformTrait();
        for (const auto& meshMaterialPair : mr->data()) {
            auto meshRendererMaterialIt = PrototypeEngineInternalApplication::database->materials.find(meshMaterialPair.material);
            if (meshRendererMaterialIt == PrototypeEngineInternalApplication::database->materials.end()) { continue; }
            auto geometryPair = _geometries.find(meshMaterialPair.mesh);
            auto materialPair = _materials.find(meshRendererMaterialIt->second->name());
            if (geometryPair != _geometries.end() && materialPair != _materials.end()) {
                const PglGeometry*              geometry           = geometryPair->second;
                const PglMaterial*              material           = materialPair->second;
                const PglShader*                shader             = material->shader;
                const std::vector<PglTexture*>& textures           = material->textures;
                OrderedDrawCommand              orderedDrawCommand = {};
                glUniformBlockBinding(shader->program, glGetUniformBlockIndex(shader->program, "Common"), 0);
                auto commandIt = orderedCommands[material->shader->program].find(material->name);
                if (commandIt != orderedCommands[material->shader->program].end()) {
                    OrderedDrawSubCommand* subCommand = _orderedSubCommandsPool.newElement();
                    {
                        {
                            auto a                         = PglCall_glUniformMatrix4fv::_allocator.newElement();
                            a->location                    = glGetUniformLocation(shader->program, "Model");
                            a->count                       = 1;
                            a->transpose                   = 0;
                            a->value                       = (GLfloat*)&transform->modelScaled()[0][0];
                            subCommand->modelMatrixCommand = _commandsPool.newElement();
                            subCommand->modelMatrixCommand->record(a);
                        }
                        {
                            auto a                      = PglCall_glUniform1ui::_allocator.newElement();
                            a->location                 = glGetUniformLocation(shader->program, "ObjectId");
                            a->v0                       = object->id();
                            subCommand->objectIdCommand = _commandsPool.newElement();
                            subCommand->objectIdCommand->record(a);
                        }
                        {
                            MeshDrawCommand meshDrawCommand = {};
                            switch (meshMaterialPair.polygonMode) {
                                case MeshRendererPolygonMode_POINT: meshDrawCommand.mode = GL_POINTS; break;
                                case MeshRendererPolygonMode_LINE: meshDrawCommand.mode = GL_LINES; break;
                                case MeshRendererPolygonMode_FILL: meshDrawCommand.mode = GL_TRIANGLES; break;
                                default: break;
                            }
                            meshDrawCommand.type  = geometry->type;
                            meshDrawCommand.count = geometry->indexCount;
                            subCommand->meshCommands[geometry->vao].push_back(std::move(meshDrawCommand));
                        }
                    }
                    commandIt->second->subCommands.push_back(subCommand);
                } else {
                    OrderedDrawCommand* mainCommand = _orderedCommandsPool.newElement();
                    {
                        {
                            auto a                     = PglCall_glUseProgram::_allocator.newElement();
                            a->program                 = shader->program;
                            mainCommand->shaderCommand = _commandsPool.newElement();
                            mainCommand->shaderCommand->record(a);
                        }
                        {
                            size_t t = 0;
                            for (const auto& texture : material->textureData) {
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glUniform1i::_allocator.newElement();
                                    a->location                 = glGetUniformLocation(shader->program, texture.c_str());
                                    a->v0                       = t;
                                    cmd->record(a);
                                    mainCommand->texturesCommand.push_back(cmd);
                                }
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glActiveTexture::_allocator.newElement();
                                    a->texture                  = GL_TEXTURE0 + t;
                                    cmd->record(a);
                                    mainCommand->texturesCommand.push_back(cmd);
                                }
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glBindTexture::_allocator.newElement();
                                    a->target                   = material->textures[t]->target;
                                    a->id                       = material->textures[t]->id;
                                    cmd->record(a);
                                    mainCommand->texturesCommand.push_back(cmd);
                                }
                                ++t;
                            }
                        }
                        {
                            // base color
                            {
                                PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                auto                    a   = PglCall_glUniform3fv::_allocator.newElement();
                                a->location                 = glGetUniformLocation(shader->program, "BaseColor");
                                a->count                    = 1;
                                a->value                    = (GLfloat*)&material->baseColor[0];
                                cmd->record(a);
                                mainCommand->vec3sCommand.push_back(cmd);
                            };
                            // metallic
                            {
                                PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                auto                    a   = PglCall_glUniform1fv::_allocator.newElement();
                                a->location                 = glGetUniformLocation(shader->program, "Metallic");
                                a->count                    = 1;
                                a->value                    = (GLfloat*)&material->metallic;
                                cmd->record(a);
                                mainCommand->floatsCommand.push_back(cmd);
                            };
                            // roughness
                            {
                                PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                auto                    a   = PglCall_glUniform1fv::_allocator.newElement();
                                a->location                 = glGetUniformLocation(shader->program, "Roughness");
                                a->count                    = 1;
                                a->value                    = (GLfloat*)&material->roughness;
                                cmd->record(a);
                                mainCommand->floatsCommand.push_back(cmd);
                            };
                        }
                        {
                            for (const auto& pair : material->vec4Data) {
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glUniform4fv::_allocator.newElement();
                                    a->location                 = glGetUniformLocation(shader->program, pair.first.c_str());
                                    a->count                    = 1;
                                    a->value                    = (GLfloat*)&pair.second;
                                    cmd->record(a);
                                    mainCommand->vec4sCommand.push_back(cmd);
                                }
                            }
                        }
                        {
                            for (const auto& pair : material->vec3Data) {
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glUniform3fv::_allocator.newElement();
                                    a->location                 = glGetUniformLocation(shader->program, pair.first.c_str());
                                    a->count                    = 1;
                                    a->value                    = (GLfloat*)&pair.second;
                                    cmd->record(a);
                                    mainCommand->vec3sCommand.push_back(cmd);
                                }
                            }
                        }
                        {
                            for (const auto& pair : material->vec2Data) {
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glUniform2fv::_allocator.newElement();
                                    a->location                 = glGetUniformLocation(shader->program, pair.first.c_str());
                                    a->count                    = 1;
                                    a->value                    = (GLfloat*)&pair.second;
                                    cmd->record(a);
                                    mainCommand->vec2sCommand.push_back(cmd);
                                }
                            }
                        }
                        {
                            for (const auto& pair : material->floatData) {
                                {
                                    PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                                    auto                    a   = PglCall_glUniform1fv::_allocator.newElement();
                                    a->location                 = glGetUniformLocation(shader->program, pair.first.c_str());
                                    a->count                    = 1;
                                    a->value                    = (GLfloat*)&pair.second;
                                    cmd->record(a);
                                    mainCommand->floatsCommand.push_back(cmd);
                                }
                            }
                        }
                        {
                            OrderedDrawSubCommand* subCommand = _orderedSubCommandsPool.newElement();
                            {
                                {
                                    auto a                         = PglCall_glUniformMatrix4fv::_allocator.newElement();
                                    a->location                    = glGetUniformLocation(shader->program, "Model");
                                    a->count                       = 1;
                                    a->transpose                   = 0;
                                    a->value                       = (GLfloat*)&transform->modelScaled()[0][0];
                                    subCommand->modelMatrixCommand = _commandsPool.newElement();
                                    subCommand->modelMatrixCommand->record(a);
                                }
                                {
                                    auto a                      = PglCall_glUniform1ui::_allocator.newElement();
                                    a->location                 = glGetUniformLocation(shader->program, "ObjectId");
                                    a->v0                       = object->id();
                                    subCommand->objectIdCommand = _commandsPool.newElement();
                                    subCommand->objectIdCommand->record(a);
                                }
                                {
                                    MeshDrawCommand meshDrawCommand = {};
                                    switch (meshMaterialPair.polygonMode) {
                                        case MeshRendererPolygonMode_POINT: meshDrawCommand.mode = GL_POINTS; break;
                                        case MeshRendererPolygonMode_LINE: meshDrawCommand.mode = GL_LINES; break;
                                        case MeshRendererPolygonMode_FILL: meshDrawCommand.mode = GL_TRIANGLES; break;
                                        default: break;
                                    }
                                    meshDrawCommand.type  = geometry->type;
                                    meshDrawCommand.count = geometry->indexCount;
                                    subCommand->meshCommands[geometry->vao].push_back(std::move(meshDrawCommand));
                                }
                            }
                            mainCommand->subCommands.push_back(subCommand);
                        }
                    }
                    orderedCommands[material->shader->program].insert({ material->name, mainCommand });
                }
            }
        }
    }

    for (auto& shaderMatPair : orderedCommands) {
        _commands.push_back(shaderMatPair.second.begin()->second->shaderCommand);
        for (const auto& matPair : shaderMatPair.second) {
            _commands.insert(_commands.end(), matPair.second->texturesCommand.begin(), matPair.second->texturesCommand.end());
            _commands.insert(_commands.end(), matPair.second->vec4sCommand.begin(), matPair.second->vec4sCommand.end());
            _commands.insert(_commands.end(), matPair.second->vec3sCommand.begin(), matPair.second->vec3sCommand.end());
            _commands.insert(_commands.end(), matPair.second->vec2sCommand.begin(), matPair.second->vec2sCommand.end());
            _commands.insert(_commands.end(), matPair.second->floatsCommand.begin(), matPair.second->floatsCommand.end());
            for (auto& subCommand : matPair.second->subCommands) {
                _commands.push_back(subCommand->modelMatrixCommand);
                _commands.push_back(subCommand->objectIdCommand);
                for (auto& meshPair : subCommand->meshCommands) {
                    {
                        PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                        auto                    a   = PglCall_glBindVertexArray::_allocator.newElement();
                        a->index                    = meshPair.first;
                        cmd->record(a);
                        _commands.push_back(cmd);
                    }
                    {
                        PrototypeOpenglCommand* cmd = _commandsPool.newElement();
                        auto                    a   = PglCall_glDrawElements::_allocator.newElement();
                        a->mode                     = meshPair.second[0].mode;  // GL_POINTS | GL_LINES | GL_TRIANGLES
                        a->type                     = meshPair.second[0].type;  // geometry->type;
                        a->count                    = meshPair.second[0].count; // geometry->indexCount;
                        a->indices                  = nullptr;
                        // a->instanceCount            = meshPair.second.size();
                        cmd->record(a);
                        _commands.push_back(cmd);
                    }
                }
            }
        }
    }
    for (auto selectedNode : selectedNodes) { PrototypeEngineInternalApplication::scene->addSelectedNode(selectedNode); }
    _window->resetDeltaTime();

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::beginRecordPass)
#endif
}

void
PrototypeOpenglRenderer::endRecordPass()
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_BEGIN()
#endif

#if defined(PROTOTYPE_ENABLE_PROFILER)
    PROTOTYPE_REGISTER_PROFILER_FUNCTION_END(PrototypeOpenglRenderer::endRecordPass)
#endif
}

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
PrototypeUI*
PrototypeOpenglRenderer::ui()
{
    return _ui.get();
}
#endif

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
// Camera* PrototypeOpenglRenderer::editorGameCamera() {}

Camera*
PrototypeOpenglRenderer::editorSceneCamera()
{
    return static_cast<PglCamera*>(_ui->sceneView()->camera())->object->getCameraTrait();
}

// Camera* PrototypeOpenglRenderer::editorPaintCamera() {}
#else
Camera*
PrototypeOpenglRenderer::mainCamera()
{
    return _mainCamera.object->getCameraTrait();
}
#endif

void
PrototypeOpenglRenderer::mapPrototypeMeshBuffer(PrototypeMeshBuffer* meshBuffer)
{
    if (_geometries.find(meshBuffer->name()) != _geometries.end()) { return; }
    auto geometry        = _geometriesPool.newElement();
    meshBuffer->userData = (void*)geometry;
    PglUploadMeshFromBuffer(meshBuffer, geometry);
    _geometries.insert({ meshBuffer->name(), geometry });
}

void
PrototypeOpenglRenderer::mapPrototypeShaderBuffer(PrototypeShaderBuffer* shaderBuffer)
{
    if (_shaders.find(shaderBuffer->name()) != _shaders.end()) { return; }
    auto shader            = _shadersPool.newElement();
    shaderBuffer->userData = (void*)shader;
    PglUploadShaderFromBuffer(shaderBuffer, shader);
    for (const auto& source : shaderBuffer->sources()) {
        if (source->type == PrototypeShaderBufferSourceType_VertexShader) {
            PrototypeAlgoCopyNewValues(source->bindingSource.floatData, shader->floatData);
            PrototypeAlgoCopyNewValues(source->bindingSource.vec2Data, shader->vec2Data);
            PrototypeAlgoCopyNewValues(source->bindingSource.vec3Data, shader->vec3Data);
            PrototypeAlgoCopyNewValues(source->bindingSource.vec4Data, shader->vec4Data);
        } else if (source->type == PrototypeShaderBufferSourceType_FragmentShader) {
            PrototypeAlgoCopyNewValues(source->bindingSource.textureData, shader->textureData);
        }
    }
    _shaders.insert({ shaderBuffer->name(), shader });
}

void
PrototypeOpenglRenderer::mapPrototypeTextureBuffer(PrototypeTextureBuffer* textureBuffer)
{
    if (_textures.find(textureBuffer->name()) != _textures.end()) { return; }
    auto texture            = _texturesPool.newElement();
    textureBuffer->userData = (void*)texture;
    PglUploadTextureFromBuffer(textureBuffer, texture);
    _textures.insert({ textureBuffer->name(), texture });
}

void
PrototypeOpenglRenderer::mapPrototypeMaterial(PrototypeMaterial* material)
{
    if (_materials.find(material->name()) != _materials.end()) { return; }
    const PrototypeShaderBuffer* dbMaterialShaderBuffer = material->shader();
    auto                         mat                    = _materialsPool.newElement();
    mat->name                                           = material->name();
    mat->shader                                         = _shaders[dbMaterialShaderBuffer->name()];
    mat->baseColor                                      = material->baseColor();
    mat->metallic                                       = material->metallic();
    mat->roughness                                      = material->roughness();
    onMaterialShaderUpdate(mat, mat->shader);
    mat->textures.resize(mat->textureData.size());
    for (size_t t = 0; t < mat->textures.size(); ++t) { mat->textures[t] = _textures[material->textures()[t]->name()]; }
    _materials.insert({ material->name(), mat });
}

void
PrototypeOpenglRenderer::onMeshBufferGpuUpload(PrototypeMeshBuffer* meshBuffer)
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    static auto profilerT1 = std::chrono::high_resolution_clock::now();
#endif

    if (meshBuffer->userData) {
        PglGeometry* geometry = static_cast<PglGeometry*>(meshBuffer->userData);
        PglReleaseMesh(geometry);
        PglUploadMeshFromBuffer(meshBuffer, geometry);
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    }

#if defined(PROTOTYPE_ENABLE_PROFILER)
    static auto profilerT2       = std::chrono::high_resolution_clock::now();
    static auto profilerDuration = std::chrono::duration_cast<std::chrono::microseconds>(profilerT2 - profilerT1).count();
    PrototypeEngineInternalApplication::profiler->markTimelineItem("PrototypeOpenglRenderer::onMeshBufferGpuUpload",
                                                                   profilerDuration);
#endif
}

void
PrototypeOpenglRenderer::onShaderBufferGpuUpload(PrototypeShaderBuffer* shaderBuffer)
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    static auto profilerT1 = std::chrono::high_resolution_clock::now();
#endif

    if (shaderBuffer->userData) {
        PglShader* shader = static_cast<PglShader*>(shaderBuffer->userData);
        PglReleaseShader(shader);
        PglUploadShaderFromBuffer(shaderBuffer, shader);
        for (const auto& source : shaderBuffer->sources()) {
            if (source->type == PrototypeShaderBufferSourceType_VertexShader) {
                PrototypeAlgoCopyNewValues(source->bindingSource.floatData, shader->floatData);
                PrototypeAlgoCopyNewValues(source->bindingSource.vec2Data, shader->vec2Data);
                PrototypeAlgoCopyNewValues(source->bindingSource.vec3Data, shader->vec3Data);
                PrototypeAlgoCopyNewValues(source->bindingSource.vec4Data, shader->vec4Data);
            } else if (source->type == PrototypeShaderBufferSourceType_FragmentShader) {
                PrototypeAlgoCopyNewValues(source->bindingSource.textureData, shader->textureData);
            }
        }
        const auto& materials = ((PrototypeOpenglRenderer*)PrototypeEngineInternalApplication::renderer)->materials();
        for (const auto& pair : materials) {
            PglMaterial* material = pair.second;
            if (material->shader == shader) {
                ((PrototypeOpenglRenderer*)PrototypeEngineInternalApplication::renderer)
                  ->onMaterialShaderUpdate(material, shader);
            }
        }
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    }

#if defined(PROTOTYPE_ENABLE_PROFILER)
    static auto profilerT2       = std::chrono::high_resolution_clock::now();
    static auto profilerDuration = std::chrono::duration_cast<std::chrono::microseconds>(profilerT2 - profilerT1).count();
    PrototypeEngineInternalApplication::profiler->markTimelineItem("PrototypeOpenglRenderer::onShaderBufferGpuUpload",
                                                                   profilerDuration);
#endif
}

void
PrototypeOpenglRenderer::onTextureBufferGpuUpload(PrototypeTextureBuffer* textureBuffer)
{
#if defined(PROTOTYPE_ENABLE_PROFILER)
    static auto profilerT1 = std::chrono::high_resolution_clock::now();
#endif

    if (textureBuffer->userData) {
        PglTexture* texture = static_cast<PglTexture*>(textureBuffer->userData);
        PglReleaseTexture(texture);
        PglUploadTextureFromBuffer(textureBuffer, texture);
        PrototypeEngineInternalApplication::renderer->scheduleRecordPass();
    }

#if defined(PROTOTYPE_ENABLE_PROFILER)
    static auto profilerT2       = std::chrono::high_resolution_clock::now();
    static auto profilerDuration = std::chrono::duration_cast<std::chrono::microseconds>(profilerT2 - profilerT1).count();
    PrototypeEngineInternalApplication::profiler->markTimelineItem("PrototypeOpenglRenderer::onTextureBufferGpuUpload",
                                                                   profilerDuration);
#endif
}

void
PrototypeOpenglRenderer::fetchCamera(const std::string& name, void** data)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (name == "SceneCamera") {
        *data = &_editorSceneCamera;
    } else {
        *data = nullptr;
    }
#else
    *data = &_mainCamera;
#endif
}

void
PrototypeOpenglRenderer::fetchDefaultMesh(void** data)
{
    auto it = _geometries.find(PROTOTYPE_DEFAULT_MESH);
    if (it != _geometries.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchMesh(const std::string& name, void** data)
{
    auto it = _geometries.find(name);
    if (it != _geometries.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchDefaultShader(void** data)
{
    auto it = _shaders.find(PROTOTYPE_DEFAULT_SHADER);
    if (it != _shaders.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchShader(const std::string& name, void** data)
{
    auto it = _shaders.find(name);
    if (it != _shaders.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchDefaultTexture(void** data)
{
    auto it = _textures.find("default.jpg");
    if (it != _textures.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchTexture(const std::string& name, void** data)
{
    auto it = _textures.find(name);
    if (it != _textures.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchDefaultMaterial(void** data)
{
    auto it = _materials.find(PROTOTYPE_DEFAULT_MATERIAL);
    if (it != _materials.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchMaterial(const std::string& name, void** data)
{
    auto it = _materials.find(name);
    if (it != _materials.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchDefaultFramebuffer(void** data)
{
    auto it = _framebuffers.find(PROTOTYPE_DEFAULT_FRAMEBUFFER);
    if (it != _framebuffers.end()) { *data = it->second; }
}

void
PrototypeOpenglRenderer::fetchFramebuffer(const std::string& name, void** data)
{
    auto it = _framebuffers.find(name);
    if (it != _framebuffers.end()) { *data = it->second; }
}

// [[nodiscard]] PrototypeSceneNode*
// PrototypeOpenglRenderer::onSceneViewClick(const glm::vec2& coordinates, const glm::vec2& Size)
// {
//     /*Camera*     cam                 = _mainCamera.object->getCameraTrait();
//     const auto& camPosition         = cam->position();
//     const auto& camViewMatrix       = cam->viewMatrix();
//     const auto& camProjectionMatrix = cam->projectionMatrix();
//     glm::vec3   ray;*/
//     float UVObjectIDPixel[4];

// #define UVObjectIDColorAttachmentId 4

// #ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
//     glm::vec2 dimensions        = _ui->sceneView().dimensions();
//     glm::vec2 cursorCoordinates = _ui->sceneView().cursorCoordinates();
//     /*PrototypeMaths::projectRayFromClipSpacePoint(ray,
//                                                  camViewMatrix,
//                                                  camProjectionMatrix,
//                                                  _ui->sceneViewCursorCoordinates().x,
//                                                  _ui->sceneViewCursorCoordinates().y,
//                                                  _ui->sceneViewSize().x,
//                                                  _ui->sceneViewSize().y);*/
//     f32 u = (cursorCoordinates.x / dimensions.x) * dimensions.x;
//     f32 v = (1.0f - cursorCoordinates.y / dimensions.y) * dimensions.y;

//     glBindFramebuffer(GL_FRAMEBUFFER, _mainCamera.deferredFramebuffer->fbo);
//     glReadBuffer(GL_COLOR_ATTACHMENT0 + UVObjectIDColorAttachmentId);
//     glReadPixels(u, v, 1, 1, GL_RGBA, GL_FLOAT, &UVObjectIDPixel);
//     glReadBuffer(GL_NONE);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
// #else
//     /*PrototypeMaths::projectRayFromClipSpacePoint(ray,
//                                                  camViewMatrix,
//                                                  camProjectionMatrix,
//                                                  _window->_mouseLocation.x,
//                                                  _window->_mouseLocation.y,
//                                                  _window->_resolution.x,
//                                                  _window->_resolution.y);*/
//     f32 u = ((2.0f * _window->_mouseLocation.x) / _window->_resolution.x - 1.0f) * _window->_resolution.x;
//     f32 v = (1.0f - (2.0f * _window->_mouseLocation.y) / _window->_resolution.y) * _window->_resolution.y;

//     // snprintf(buff, sizeof(buff), "uv [%f, %f]", u, v);
//     // PrototypeLogger::log(__FILE__, __LINE__, buff);

//     glBindFramebuffer(GL_FRAMEBUFFER, _mainCamera.deferredFramebuffer->fbo);
//     glReadBuffer(GL_COLOR_ATTACHMENT0 + UVObjectIDColorAttachmentId);
//     glReadPixels(u, v, 1, 1, GL_RGBA, GL_FLOAT, &UVObjectIDPixel);
//     glReadBuffer(GL_NONE);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
// #endif

//     u32 objectId = UVObjectIDPixel[2];

//     if (objectId == 0) { return nullptr; }
//     auto meshRendererMap = PrototypeTraitSystem::meshRendererMap();
//     auto meshRendererIt  = meshRendererMap.find(objectId);
//     if (meshRendererIt != meshRendererMap.end()) {
//         MeshRenderer*    mr     = PrototypeTraitSystem::meshRendererVector()[meshRendererIt->second];
//         PrototypeObject* object = mr->object();
//         if (object) {
//             if (object->parentNode()) {
//                 auto node = static_cast<PrototypeSceneNode*>(object->parentNode());
//                 return node;
//             }
//         }
//     }
//     /*auto optHit =
//       PrototypeEngineInternalApplication::physics->raycast({ camPosition.x, camPosition.y, camPosition.z }, ray, cam->zfar());
//     if (optHit) {
//         auto hit = optHit.value();
//         if (hit && hit->parentNode()) {
//             auto node = static_cast<PrototypeSceneNode*>(hit->parentNode());
//             return node;
//         }
//     }*/
//     return nullptr;
// }

void
PrototypeOpenglRenderer::onMouse(i32 button, i32 action, i32 mods)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouse(button, action, mods)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#endif
}

void
PrototypeOpenglRenderer::onMouseMove(f64 xpos, f64 ypos)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouseMove(xpos, ypos)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#endif
}

void
PrototypeOpenglRenderer::onMouseDrag(i32 button, f64 xoffset, f64 yoffset)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouseDrag(button, xoffset, yoffset)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#else
    if (_window->_mouseDown.z) { CameraSystemRotate(_mainCamera.object->getCameraTrait(), (f32)xoffset, (f32)yoffset); }
#endif
}

void
PrototypeOpenglRenderer::onMouseScroll(f64 xoffset, f64 yoffset)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onMouseScroll(xoffset, yoffset)) return;
    if (_ui->needsMouse() && !_ui->sceneView()->isHovered()) return;
#else
    CameraSystemRotate(_mainCamera.object->getCameraTrait(), (f32)xoffset, (f32)yoffset);
#endif
}

void
PrototypeOpenglRenderer::onKeyboard(i32 key, i32 scancode, i32 action, i32 mods)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onKeyboard(key, scancode, action, mods)) return;
    if (_ui->needsKeyboard() || (_ui->needsMouse() && !_ui->sceneView()->isHovered())) return;
#else
    _input->onKeyboard(key, scancode, action, mods);
    if (action == GLFW_PRESS) {
        // spawning stuff in scene
        {
            Camera*     cam                 = _mainCamera.object->getCameraTrait();
            const auto& cameraPosition      = cam->position();
            const auto& cameraRotation      = cam->rotation();
            const auto& cameraResolution    = cam->resolution();
            const auto& camViewMatrix       = cam->viewMatrix();
            const auto& camProjectionMatrix = cam->projectionMatrix();

            glm::vec3 ray;
            PrototypeMaths::projectRayFromClipSpacePoint(ray,
                                                         camViewMatrix,
                                                         camProjectionMatrix,
                                                         _window->_mouseLocation.x,
                                                         _window->_mouseLocation.y,
                                                         _window->_resolution.x,
                                                         _window->_resolution.y);
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
#endif
}

void
PrototypeOpenglRenderer::onWindowResize(i32 width, i32 height)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onWindowResize(width, height)) return;
#endif

    // _actions.push_back([this]() { _framebufferResized = true; });
    PglFramebufferResize(_fullscreenFramebuffer, width, height);
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    Camera*   cam        = _editorSceneCamera.object->getCameraTrait();
    glm::vec2 dimensions = _ui->sceneView()->dimensions();
    PglFramebufferResize(_editorSceneCamera.finalFramebuffer, dimensions.x, dimensions.y);
    PglFramebufferResize(_editorSceneCamera.postprocessingFramebuffer, dimensions.x, dimensions.y);
    PglFramebufferResize(_editorSceneCamera.gbufferFramebuffer, dimensions.x, dimensions.y);
    PglFramebufferResize(_editorSceneCamera.deferredFramebuffer, dimensions.x, dimensions.y);
    CameraSystemSetResolution(cam, dimensions.x, dimensions.y);
#else
    Camera* cam = _mainCamera.object->getCameraTrait();
    PglFramebufferResize(_mainCamera.finalFramebuffer, width, height);
    PglFramebufferResize(_mainCamera.postprocessingFramebuffer, width, height);
    PglFramebufferResize(_mainCamera.gbufferFramebuffer, width, height);
    PglFramebufferResize(_mainCamera.deferredFramebuffer, width, height);
    CameraSystemSetResolution(cam, width, height);
#endif
}

void
PrototypeOpenglRenderer::onWindowDragDrop(i32 numFiles, const char** names)
{
#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
    if (_ui->sceneView()->onWindowDragDrop(numFiles, names)) return;
#endif
}

const std::unordered_map<std::string, PglGeometry*>&
PrototypeOpenglRenderer::geometries() const
{
    return _geometries;
}

const std::unordered_map<std::string, PglShader*>&
PrototypeOpenglRenderer::shaders() const
{
    return _shaders;
}

const std::unordered_map<std::string, PglTexture*>&
PrototypeOpenglRenderer::textures() const
{
    return _textures;
}

const std::unordered_map<std::string, PglMaterial*>&
PrototypeOpenglRenderer::materials() const
{
    return _materials;
}

const std::unordered_map<std::string, PglFramebuffer*>&
PrototypeOpenglRenderer::framebuffers() const
{
    return _framebuffers;
}

#ifdef PROTOTYPE_ENGINE_DEVELOPMENT_MODE
PglCamera&
PrototypeOpenglRenderer::pglEditorSceneCamera()
{
    return _editorSceneCamera;
}

PglCamera&
PrototypeOpenglRenderer::pglEditorPaintCamera()
{
    return _editorPaintCamera;
}
#else
PglCamera&
PrototypeOpenglRenderer::pglMainCamera()
{
    return _mainCamera;
}
#endif