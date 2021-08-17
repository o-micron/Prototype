#pragma once

#include <stdint.h>

#if defined(__EMSCRIPTEN__)
#define PROTOTYPE_INTERFACE_PLATFORM_NAME "WEBKIT"
#define PROTOTYPE_INTERFACE_PLATFORM_WEBKIT
#define PROTOTYPE_INTERFACE_FORCE_INLINE inline
#define PROTOTYPE_INTERFACE_FORCE_NO_INLINE
#define PROTOTYPE_INTERFACE_DYNAMIC_FN_CALL
#elif defined(_WIN32) || defined(_WIN64)
#define PROTOTYPE_INTERFACE_PLATFORM_NAME "WINDOWS"
#define PROTOTYPE_INTERFACE_PLATFORM_WINDOWS
#define PROTOTYPE_INTERFACE_FORCE_INLINE               __forceinline
#define PROTOTYPE_INTERFACE_FORCE_NO_INLINE            _declspec(noinline)
#define PROTOTYPE_INTERFACE_DYNAMIC_FN_CALL            __stdcall
#define PROTOTYPE_INTERFACE_DLL_HANDLE_TYPE            HINSTANCE
#define PROTOTYPE_INTERFACE_DLL_OPEN(A, B)             LoadLibrary(A)
#define PROTOTYPE_INTERFACE_DLL_GET_PROC_ADDRESS(A, B) GetProcAddress(A, B)
#define PROTOTYPE_INTERFACE_DLL_CLOSE(A)               FreeLibrary(A)
#define PROTOTYPE_INTERFACE_FUNCTION_NAME              __FUNCTION__
#elif defined(__linux__)
#define PROTOTYPE_INTERFACE_PLATFORM_NAME "LINUX"
#define PROTOTYPE_INTERFACE_PLATFORM_LINUX
#define PROTOTYPE_INTERFACE_FORCE_INLINE inline
#define PROTOTYPE_INTERFACE_FORCE_NO_INLINE
#define PROTOTYPE_INTERFACE_DYNAMIC_FN_CALL
#define PROTOTYPE_INTERFACE_DLL_HANDLE_TYPE            void*
#define PROTOTYPE_INTERFACE_DLL_OPEN(A, B)             dlopen(A, B)
#define PROTOTYPE_INTERFACE_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
#define PROTOTYPE_INTERFACE_DLL_CLOSE(A)               dlclose(A)
#define PROTOTYPE_INTERFACE_FUNCTION_NAME              __func__
#elif defined(__APPLE__)
#define PROTOTYPE_INTERFACE_PLATFORM_NAME "DARWIN"
#define PROTOTYPE_INTERFACE_PLATFORM_DARWIN
#define PROTOTYPE_INTERFACE_FORCE_INLINE inline
#define PROTOTYPE_INTERFACE_FORCE_NO_INLINE
#define PROTOTYPE_INTERFACE_DYNAMIC_FN_CALL
#define PROTOTYPE_INTERFACE_DLL_HANDLE_TYPE            void*
#define PROTOTYPE_INTERFACE_DLL_OPEN(A, B)             dlopen(A, B)
#define PROTOTYPE_INTERFACE_DLL_GET_PROC_ADDRESS(A, B) dlsym(A, B)
#define PROTOTYPE_INTERFACE_DLL_CLOSE(A)               dlclose(A)
#define PROTOTYPE_INTERFACE_FUNCTION_NAME              __func__
#else
#error "Platform not supported."
#endif

#if defined(__cplusplus)
#define PROTOTYPE_INTERFACE_EXTERN extern "C"
#else
#define PROTOTYPE_INTERFACE_EXTERN extern
#endif

#if defined(PROTOTYPE_INTERFACE_PLATFORM_WEBKIT) && defined(PROTOTYPE_INTERFACE_EXPORT_DYNAMIC)
#define PROTOTYPE_INTERFACE_API __attribute__((visibility("default")))
#elif defined(PROTOTYPE_INTERFACE_PLATFORM_WINDOWS) && defined(PROTOTYPE_INTERFACE_EXPORT_DYNAMIC)
#define PROTOTYPE_INTERFACE_API __declspec(dllexport)
#elif defined(PROTOTYPE_INTERFACE_PLATFORM_WINDOWS) && defined(PROTOTYPE_INTERFACE_IMPORT_DYNAMIC)
#define PROTOTYPE_INTERFACE_API __declspec(dllimport)
#elif defined(PROTOTYPE_INTERFACE_PLATFORM_DARWIN) && defined(PROTOTYPE_INTERFACE_EXPORT_DYNAMIC)
#define PROTOTYPE_INTERFACE_API __attribute__((visibility("default")))
#elif defined(PROTOTYPE_INTERFACE_PLATFORM_LINUX) && defined(PROTOTYPE_INTERFACE_EXPORT_DYNAMIC)
#define PROTOTYPE_INTERFACE_API __attribute__((visibility("default")))
#else
#define PROTOTYPE_INTERFACE_API
#endif

struct PrototypeEngineContext;
struct PrototypeLoggerData;

// ----------------------------------------------------------------------------------------------------------
// Common structures
// ----------------------------------------------------------------------------------------------------------

// Vector 2 (not a vec4 aligned ..)
PROTOTYPE_INTERFACE_EXTERN struct PROTOTYPE_INTERFACE_API FieldVec2
{
    float x, y;
};

// Vector 3 (not a vec4 aligned ..)
PROTOTYPE_INTERFACE_EXTERN struct PROTOTYPE_INTERFACE_API FieldVec3
{
    float x, y, z;
};

// Vector 4
PROTOTYPE_INTERFACE_EXTERN struct PROTOTYPE_INTERFACE_API FieldVec4
{
    float x, y, z, w;
};

// Matrix 4x4
PROTOTYPE_INTERFACE_EXTERN struct PROTOTYPE_INTERFACE_API FieldMat4
{
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
};

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// Engine sync functions
// ----------------------------------------------------------------------------------------------------------

// Call this function on plugin load event
// This function will help the engine sychronize some internal states
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
LoadContext(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData);

// Call this function on plugin reload event
// This function will help the engine sychronize some internal states
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ReloadContext(PrototypeEngineContext* engineContext, PrototypeLoggerData* loggerData);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// OBJECT DETAILS TRAIT
// ----------------------------------------------------------------------------------------------------------

// Destroys the given object and removes it from scene
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectDestroy(void* object);

// Returns the id of the given object
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectGetId(void* object, int64_t* id);

// Returns the name of the object
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectGetName(void* object, const char** name);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// CAMERA TRAIT
// ----------------------------------------------------------------------------------------------------------

// Adds a camera trait to an object iif the object doesn't have a camera trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddCameraTrait(void* object);

// Returns true or false whether the object has a camera trait or not
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasCameraTrait(void* object);

// Removes the camera trait from an object iif the object has a camera trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveCameraTrait(void* object);

// Returns the main camera used by the current scene
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
MainCamera(void** camera);

// get camera look sensitivity factor
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetLookSensitivityFactor(void* camera, float& factor);

// set camera look sensitivity factor
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetLookSensitivityFactor(void* camera, float factor);

// get camera move sensitivity factor
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetMoveSensitivityFactor(void* camera, float& factor);

// set camera move sensitivity factor
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetMoveSensitivityFactor(void* camera, float factor);

// get camera rotation (pitch, yaw)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetRotationPitchYaw(void* camera, FieldVec2& pitchYaw);

// set camera rotation (pitch, yaw)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetRotationPitchYaw(void* camera, FieldVec2 pitchYaw);

// rotate camera interpolated (pitch, yaw)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraRotate(void* camera, FieldVec2 pitchYaw);

// // get camera rotation quaternion (w, x, y, z)
// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraGetRotationQuaternion(void* camera, FieldVec4& q);

// // set camera rotation quaternion (w, x, y, z)
// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraSetRotationQuaternion(void* camera, FieldVec4 q);

// get camera translation
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetTranslation(void* camera, FieldVec3& translation);

// set camera translation
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetTranslation(void* camera, FieldVec3 translation);

// translate camera interpolated
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraTranslate(void* camera, FieldVec3 translation);

// get camera view matrix 4x4
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetViewMatrix(void* camera, FieldMat4& viewMat);

// set camera view matrix 4x4
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetViewMatrix(void* camera, FieldMat4 viewMat);

// get camera mode (true if orbital, false if free-camera)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraIsOrbital(void* camera, bool& orbital);

// set camera mode (true if orbital, false if free-camera)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetOrbital(void* camera, bool orbital);

// get camera field of view
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetFieldOfView(void* camera, float& fov);

// set camera field of view
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetFieldOfView(void* camera, float fov);

// get camera aspect ratio
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetAspectRatio(void* camera, float& aspectRatio);

// set camera aspect ratio
// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraSetAspectRatio(void* camera, float aspectRatio);

// get camera znear
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetZNear(void* camera, float& znear);

// set camera znear
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetZNear(void* camera, float znear);

// get camera zfar
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetZFar(void* camera, float& zfar);

// set camera zfar
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetZFar(void* camera, float zfar);

// get camera resolution (width, height)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetResolution(void* camera, FieldVec2& resolution);

// set camera resolution (width, height)
// PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
// CameraSetResolution(void* camera, FieldVec2 resolution);

// get camera projection matrix 4x4
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraGetProjectionMatrix(void* camera, FieldMat4& projectionMat);

// set camera projection matrix 4x4
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
CameraSetProjectionMatrix(void* camera, FieldMat4 projectionMat);
// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// COLLIDER TRAIT
// ----------------------------------------------------------------------------------------------------------

// Adds a collider trait to an object iif the object doesn't have a collider trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddColliderTrait(void* object);

// Returns true or false whether the object has a collider trait or not
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasColliderTrait(void* object);

// Removes the collider trait from an object iif the object has a collider trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveColliderTrait(void* object);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// MESH RENDERER TRAIT
// ----------------------------------------------------------------------------------------------------------

// Adds a mesh renderer trait to an object iif the object doesn't have a mesh renderer trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddMeshRendererTrait(void* object);

// Returns true or false whether the object has a mesh renderer trait or not
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasMeshRendererTrait(void* object);

// Removes the mesh renderer trait from an object iif the object has a mesh renderer trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveMeshRendererTrait(void* object);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// RIGIDBODY TRAIT
// ----------------------------------------------------------------------------------------------------------

// Adds a rigidbody trait to an object iif the object doesn't have a rigidbody trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddRigidbodyTrait(void* object);

// Returns true or false whether the object has a rigidbody trait or not
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasRigidbodyTrait(void* object);

// Removes the rigidbody trait from an object iif the object has a rigidbody trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveRigidbodyTrait(void* object);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// SCRIPT TRAIT
// ----------------------------------------------------------------------------------------------------------

// Adds a script trait to an object iif the object doesn't have a script trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddScriptTrait(void* object);

// Returns true or false whether the object has a script trait or not
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasScriptTrait(void* object);

// Removes the script trait from an object iif the object has a script trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveScriptTrait(void* object);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// TRANSFORM TRAIT
// ----------------------------------------------------------------------------------------------------------

// Adds a transform trait to an object iif the object doesn't have a transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectAddTransformTrait(void* object);

// Returns true or false whether the object has a transform trait or not
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API bool
ObjectHasTransformTrait(void* object);

// Removes the transform trait from an object iif the object has a transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
ObjectRemoveTransformTrait(void* object);

// Returns the local forward vector of the object's transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitGetForward(void* object, FieldVec3& forward);

// Returns the translation vector of the object's transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitGetTranslation(void* object, FieldVec3& translation);

// Returns the euler angles rotation of the object's transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitGetRotation(void* object, FieldVec3& eulerAngles);

// Sets the translation vector of the object's transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitSetTranslation(void* object, const FieldVec3& translation);

// Sets the euler angles rotation of the object's transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitSetRotation(void* object, const FieldVec3& eulerAngles);

// Sets the scale of the object's transform trait
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitSetScale(void* object, const FieldVec3& scale);

// Translates the object by velocity in the given direction
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitTranslate(void* object, const FieldVec3& direction, float velocity);

// Rotates the object by velocity in the given euler angles
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
TransformTraitRotate(void* object, const FieldVec3& angle, float velocity);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// LOGGIN INFO
// ----------------------------------------------------------------------------------------------------------

// Prints a message to the console tab
// Provide a filename and line number along with the text you want to log
// Providing the correct file and line is crucial to allow jumping back to code from the editor when needed
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
consoleLog(void* object, const char* file, int line, const char* text);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// SPAWNING OBJECTS
// ----------------------------------------------------------------------------------------------------------

// Spawn a cube as a child object to the given object
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
spawnCube();

// Spawn a sphere as a child object to the given object
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
spawnSphere();

// Spawn a convex mesh as a child object to the given object
// The name of the mesh must match on of the meshes loaded by the engine
// Check the meshes tab
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
spawnConvexMesh(const char* name);

// Spawn a static triangulated mesh as a child object to the given object
// The name of the mesh must match on of the meshes loaded by the engine
// Check the meshes tab
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
spawnTriMesh(const char* name);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// TIMING DETAILS
// ----------------------------------------------------------------------------------------------------------

// Returns the time since the engine started or since the last time reset
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API double
Time();

// Returns the delta time (between last 2 frames)
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API double
DeltaTime();

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// RENDERING DETAILS
// ----------------------------------------------------------------------------------------------------------

// Returns the size of the scene view [width, height]
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
RendererGetSceneViewSize(FieldVec2& viewSize);

// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// PHYSICS DETAILS
// ----------------------------------------------------------------------------------------------------------

// Does project a ray from the given origin alongside the given direction with the given ray length
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
PhysicsRaycastFromLocationAndDirection(void** hitObject, const FieldVec3& origin, const FieldVec3& direction, float rayLength);

// Does project a ray from viewport using the x, y and using the given ray length
// Note: x and y are not normalized, they're ranging between the sceneview [0 .. width] and [0 .. height]
// Call RendererGetSceneViewSize to get the width and height of the scene view if you need to
PROTOTYPE_INTERFACE_EXTERN PROTOTYPE_INTERFACE_API void
PhysicsRaycastFromMainCameraViewport(void** hitObject, double x, double y, float rayLength);

// ----------------------------------------------------------------------------------------------------------