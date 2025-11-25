#include "RenderSystem.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl2.h>
#include <SDL_syswm.h>


#include <Ogre.h>
#include <OgreShaderGenerator.h>
#include <LoadLua.h>

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "RenderMesh.h"
#include "Manager.h"
#include "CameraComponent.h"
#include "Transform.h"
#include "RigidBody.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Light.h"
#include "Color.h"
#include <SDL.h>

#include <Debuglog.h>

// MACRO para renderizar los colliders o no 
#define _DEBUG_COLLIDERS true

using namespace H;

RenderSystem::RenderSystem(unsigned int width, unsigned int height, bool fullScreen, const char* name)
: _width(width), _height(height), _fullScreen(fullScreen), _name(name), _nameWindow(name) {
    lightTypes = {
       {"Directional", Ogre::Light::LT_DIRECTIONAL},
       {"Point", Ogre::Light::LT_POINT},
       {"RectLight", Ogre::Light::LT_RECTLIGHT},
       {"SpotLight", Ogre::Light::LT_SPOTLIGHT}
    };
}

RenderSystem::~RenderSystem()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    delete Ogre::ImguiManager::getSingletonPtr();

    _root->clearEventTimes();

    // Limpiar fuentes
    Ogre::FontManager::getSingleton().unloadAll(); 
    Ogre::FontManager::getSingleton().removeAll();
    // Limpiar el sistema de superposiciones (Overlays)
    if (_overlaySystem != nullptr) {
        _sceneMgr->removeRenderQueueListener(_overlaySystem);
        delete _overlaySystem;
        _overlaySystem = nullptr;
    }
    _overlayManager = nullptr;

    // Limpiar la escena
    _sceneMgr->clearScene();

    // Limpiar Shader Generator
    if (Ogre::RTShader::ShaderGenerator::getSingletonPtr()) {
        Ogre::RTShader::ShaderGenerator::getSingletonPtr()->flushShaderCache();
        Ogre::RTShader::ShaderGenerator::getSingletonPtr()->destroy();
    }

    // Limpiar Resource Groups
    std::vector<std::string> groups = Ogre::ResourceGroupManager::getSingletonPtr()->getResourceGroups();
    for (std::string& name : groups) {
        Ogre::ResourceGroupManager::getSingletonPtr()->clearResourceGroup(name);
        Ogre::ResourceGroupManager::getSingletonPtr()->destroyResourceGroup(name);
    }

    // Limpiar materiales cargados
    Ogre::MaterialManager::getSingleton().unloadAll();
    Ogre::MaterialManager::getSingleton().removeAll();
    
    // Limpiar texturas
    Ogre::TextureManager::getSingleton().unloadAll();
    Ogre::TextureManager::getSingleton().removeAll();

    // Limpiar mallas (Meshes)
    Ogre::MeshManager::getSingleton().unloadAll();
    Ogre::MeshManager::getSingleton().removeAll();

    // Limpiar esqueletos
    Ogre::SkeletonManager::getSingleton().unloadAll();
    Ogre::SkeletonManager::getSingleton().removeAll();

    

    Ogre::GpuProgramManager::getSingleton().unloadAll();
    Ogre::GpuProgramManager::getSingleton().removeAll();

    // Destruir el Scene Manager
    _root->destroySceneManager(_sceneMgr);

    // Limpiar las colisiones y cualquier otro dato relacionado con renderizado
    _colliderToRender.clear();

    _root->getRenderSystem()->shutdown();

    // Limpiar el Root y las configuraciones de Ogre
    _root->shutdown();

    // Destruir el objeto Root de Ogre
    delete _root;

    // Limpiar SDL
    SDL_GLContext _glContext = SDL_GL_GetCurrentContext();
    if (_glContext) {
        SDL_GL_DeleteContext(_glContext);
        _glContext = nullptr;
    }
    SDL_DestroyWindow(_sdlWindow);
    SDL_Quit();

}

bool RenderSystem::fileExists(const std::string& path) {
    std::ifstream file(path); 
    return file.good();
}

void RenderSystem::initSystem()
{
    SDL();
    rutas();
    ogre();
    _sceneMgr = _root->createSceneManager();
    _overlaySystem = new Ogre::OverlaySystem();
    _sceneMgr->addRenderQueueListener(_overlaySystem);
    _overlayManager = Ogre::OverlayManager::getSingletonPtr();
    resources();
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    imgui();
}

void RenderSystem::update(double deltaTime)
{
    const std::vector<Entity*>& gameObjects = _mngr->getEntities(_grp_PARENTENTS);
    

    // Limpiar los colliders de la iteración anterior
    for (Ogre::ManualObject* obj : _colliderToRender) {
        _sceneMgr->destroyManualObject(obj);
        obj = nullptr;
    } 
    _colliderToRender.clear();

    //Actualizar los rayos de la escena
    updateRays(deltaTime);

    for (Entity* ent : gameObjects)
    {
        entityIteration(ent, true);
    }

    if (_mCamera != nullptr && _mngr->hasComponent<CameraComponent>(_mCamera)) {
        CameraComponent* camera = _mngr->getComponent<CameraComponent>(_mCamera);
        SDL_SetRelativeMouseMode(camera->cursorOnScreen ? SDL_FALSE : SDL_TRUE);
        Vector3F pos = _mngr->getComponent<Transform>(_mCamera)->getGlobalPosition();
        QuaternionF rot = _mngr->getComponent<Transform>(_mCamera)->getGlobalRotation();
        camera->cameraNode->setPosition(Ogre::Vector3(pos.x, pos.y, pos.z));
        if (camera->lookAt)
            camera->cameraNode->lookAt(Ogre::Vector3(camera->target.x, camera->target.y, camera->target.z), Ogre::Node::TS_WORLD);
        camera->cameraNode->setOrientation(Ogre::Quaternion(rot.w, rot.x, rot.y, rot.z));
        camera->camera->setNearClipDistance(camera->nearClipDistance);
        camera->camera->setFarClipDistance(camera->farClipDistance);
        camera->viewport->setBackgroundColour(Ogre::ColourValue(camera->backgroundColor.x, camera->backgroundColor.y, camera->backgroundColor.z));
    }

    _root->renderOneFrame();
}

void RenderSystem::entityIteration(Entity* entity,bool activeParent) {

    Transform* tr = _mngr->getComponent<Transform>(entity);

    if (_mngr->hasComponent<RenderMesh>(entity)) {

        RenderMesh* mesh = _mngr->getComponent<RenderMesh>(entity);
        if (activeParent && entity->isActive()) {
            mesh->sceneNode->setVisible(true);


            Vector3F pos = tr->getGlobalPosition();
            Vector3F scale = tr->getGlobalScale();
            QuaternionF rot = tr->getGlobalRotation();

            mesh->sceneNode->setPosition(Ogre::Vector3(pos.x, pos.y, pos.z));
            mesh->sceneNode->setOrientation(Ogre::Quaternion(rot.w, rot.x, rot.y, rot.z));
            mesh->sceneNode->setScale(Ogre::Vector3(scale.x, scale.y, scale.z));


        }
        else
            mesh->sceneNode->setVisible(false);
    }

    // Render de colliders si tiene el flag activado
    if (_DEBUG_COLLIDERS) {
        if (activeParent && entity->isActive()) {
            if (_mngr->hasComponent<BoxCollider>(entity)) {
                Collider* collider = _mngr->getComponent<BoxCollider>(entity);
                if (collider->draw && collider->active) {
                    drawCollider(collider, tr);
                }
            }
            if (_mngr->hasComponent<SphereCollider>(entity)) {
                Collider* collider = _mngr->getComponent<SphereCollider>(entity);
                if (collider->draw && collider->active) {
                    drawCollider(collider, tr);
                }
            }
            if (_mngr->hasComponent<CapsuleCollider>(entity)) {
                Collider* collider = _mngr->getComponent<CapsuleCollider>(entity);
                if (collider->draw && collider->active) { 
                    drawCollider(collider, tr); 
                }
            }
        }
    }

    std::list<Entity*> children = entity->getChildren();
    for (Entity* child : children)
    {
        entityIteration(child, activeParent && entity->isActive());
    }
    return;
}

void H::RenderSystem::deleteEntity(Entity* ent) {
    if (_mngr->hasComponent<RenderMesh>(ent)) {
        RenderMesh* rM = _mngr->getComponent<RenderMesh>(ent);
        if (rM->ogreEntity != nullptr)
        {
            _sceneMgr->destroyEntity(rM->ogreEntity);
        }
    }
    if (_mngr->hasComponent<Light>(ent)) {
        Light* l = _mngr->getComponent<Light>(ent);
        _sceneMgr->destroyLight(l->light);
    }
    if (ent == _mCamera) {
        //CameraComponent* cam = _mngr->getComponent<CameraComponent>(_mCamera);
        _window->removeViewport(0);
        _sceneMgr->destroyAllCameras();
        _mCamera = nullptr;
    }
}

void RenderSystem::receive(const Message* m)
{
    switch (m->id)
    {
    case _m_INIT_ENTITY:
        addEntity(m->entity.entityPtr, m->entity.entityName);
        break;
    case _m_CHANGE_MAINCAMERA:
        _mCamera = m->entity.entityPtr;
        break;
    case _m_DELETE_ENTITY:
        deleteEntity(m->entity.entityPtr);
        break;
    case _m_DELETE_CAMERA:
        _mCamera = nullptr;
        break;
    default:
        break;
    }
}

void RenderSystem::SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::string error = "Error al inicializar SDL: ";
        error += SDL_GetError();
        DebugLog::instance()->throwLog(error);
        return;
    }

    // Configurar SDL para OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Crear la ventana SDL
    _sdlWindow = SDL_CreateWindow(_nameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_OPENGL|_fullScreen);
    if (!_sdlWindow) {
        std::string error = "Error al crear la ventana SDL: ";
        error += SDL_GetError();
        DebugLog::instance()->throwLog(error);
        SDL_Quit();
        return;
    }


    // Crear el contexto OpenGL con SDL
    SDL_GLContext context = SDL_GL_CreateContext(_sdlWindow);
    if (!context) {
        std::string error = "Error al crear el contexto OpenGL con SDL: ";
        error += SDL_GetError();
        DebugLog::instance()->throwLog(error);
        SDL_DestroyWindow(_sdlWindow);
        SDL_Quit();
        return;
    }

    // Imprimir informacion de los handles
    std::ostringstream error;
    error << "Contexto OpenGL: " << context;
    DebugLog::instance()->throwLog(error.str());
}

void RenderSystem::rutas()
{
    // Crear el objeto Root de Ogre
   _root = new Ogre::Root("bin/Ogre/plugins/plugins.cfg", "bin/Ogre/ogre.cfg", "bin/Ogre/ogre.log");
    
}

void RenderSystem::resources()
{
    Ogre::ConfigFile cf;


    if (fileExists(H_RESOURCES_PATH_ASSETS)) {
        cf.load(H_RESOURCES_PATH_ASSETS);

        Ogre::String sec2, type2, arch2;
        for (const auto& section : cf.getSettingsBySection()) {
            sec2 = section.first;
            for (const auto& setting : section.second) {
                type2 = setting.first;
                arch2 = setting.second;
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch2, type2, sec2);
            }
        }
        std::string error = "cargados los recursos ";
        error += H_RESOURCES_PATH_ASSETS;
        DebugLog::instance()->throwLog(error);
    }
    else {
        std::string error = "Error: No se encontró el archivo resources.cfg en ";
        error += H_RESOURCES_PATH_ASSETS;
        DebugLog::instance()->throwLog(error);
    }
}

bool RenderSystem::resourceExistsInAnyGroup(const std::string& resourceName) {

    std::vector<std::string> groups = Ogre::ResourceGroupManager::getSingletonPtr()->getResourceGroups();

    for (const std::string& group : groups) {
        if (Ogre::ResourceGroupManager::getSingletonPtr()->resourceExists(group, resourceName)) {
            return true; // Si encontramos el recurso
        }
    }

    std::string error = "Error: No se encontró ";
    error += resourceName;
    DebugLog::instance()->throwLog(error);
    return false;
}

void RenderSystem::ogre()
{
    // Configurar OpenGL en Ogre
    Ogre::RenderSystem* renderSystem = _root->getRenderSystemByName("OpenGL Rendering Subsystem");
    if (!renderSystem) {
        std::string error = "No se encontro el sistema de renderizado OpenGL";
        DebugLog::instance()->throwLog(error);
        SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
        SDL_DestroyWindow(_sdlWindow);
        SDL_Quit();
        return;
    }
    _root->setRenderSystem(renderSystem);
    

    // Inicializar Ogre sin crear ventana
    if (!_root->isInitialised()) {
        _root->initialise(false);
    }
    
    // Configurar ventana de Ogre con el handle correcto
    Ogre::NameValuePairList options;
    // Obtener el handle correcto de la ventana
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(_sdlWindow, &wmInfo)) {
        std::string error = "Error obteniendo información de la ventana SDL: ";
        error += SDL_GetError();
        DebugLog::instance()->throwLog(error);
        SDL_DestroyWindow(_sdlWindow);
        SDL_Quit();
        return;
    }

    // Imprimir informacion de los handles
    std::ostringstream error;
    error << "Handle de la ventana SDL: " << wmInfo.info.win.window;
    DebugLog::instance()->throwLog(error.str());
    options["externalWindowHandle"] = std::to_string(reinterpret_cast<uintptr_t>(wmInfo.info.win.window));
    options["externalGLContext"] = std::to_string(reinterpret_cast<uintptr_t>(SDL_GL_GetCurrentContext()));
    options["FSAA"] = "0";
    options["vsync"] = "true";
    options["sRGB"] = "false";

    _window = _root->createRenderWindow(_name, _width, _height, false, &options);

    // Crear la escena
    
    Ogre::RTShader::ShaderGenerator* mShaderGenerator;
    if (Ogre::RTShader::ShaderGenerator::initialize()) {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    }
}

void RenderSystem::imgui()
{
    Ogre::ImguiManager::createSingleton();
    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, SDL_GL_GetCurrentContext());  // SDL2 ImGui initialization with Ogre window
    ImGui_ImplOpenGL2_Init();  // OpenGL version; use the correct one for your setup
}

void RenderSystem::addEntity(Entity* entity, std::string name)
{
    LoadLua* lua = LoadLua::instance();
    if (lua->hasComponent(name, "Transform")) {
        Transform* tr = _mngr->getComponent<Transform>(entity);
        if (tr == nullptr) { // por si acaso
            tr = _mngr->addComponent<Transform>(entity);
        }

        Vector3F trPos = {0,0,0};
        trPos = { 
            lua->loadFloatValue(name, {"Transform", "position", "x"}),
            lua->loadFloatValue(name, {"Transform", "position", "y"}),
            lua->loadFloatValue(name, {"Transform", "position", "z"})
        };
        tr->position = trPos;

        if (lua->hasProperty(name, { "Transform", "scale" })) {
            Vector3F trScl = { 1,1,1 };
            trScl = {
                lua->loadFloatValue(name, {"Transform", "scale", "x"}),
                lua->loadFloatValue(name, {"Transform", "scale", "y"}),
                lua->loadFloatValue(name, {"Transform", "scale", "z"})
            };
            tr->scale = trScl;
        }

        if (lua->hasProperty(name, { "Transform", "rotation" })) {
            QuaternionF trRot;
            if (lua->hasProperty(name, { "Transform", "rotation", "w" })) { // quaternion
                trRot = {
                lua->loadFloatValue(name, {"Transform", "rotation", "x"}),
                lua->loadFloatValue(name, {"Transform", "rotation", "y"}),
                lua->loadFloatValue(name, {"Transform", "rotation", "z"}),
                lua->loadFloatValue(name, {"Transform", "rotation", "w"})
                };
            }
            else { // vector3 Euler EN GRADOOS!!!
                Vector3F auxRot = {
                    lua->loadFloatValue(name, {"Transform", "rotation", "x"}),
                    lua->loadFloatValue(name, {"Transform", "rotation", "y"}),
                    lua->loadFloatValue(name, {"Transform", "rotation", "z"})
                };
                trRot = QuaternionF::fromEuler(auxRot);
            }
            tr->rotation = trRot;
        }
    }

    if (lua->hasComponent(name, "RenderMesh")) {
        std::string mesh = "";
        if (lua->hasProperty(name, { "RenderMesh", "mesh" }))
            mesh = lua->loadStrValue(name, { "RenderMesh", "mesh" });

        std::string mat = " ";
        if (lua->hasProperty(name, { "RenderMesh", "material" }))
            mat = lua->loadStrValue(name, { "RenderMesh", "material" });

        RenderMesh* rMesh = _mngr->addComponent<RenderMesh>(entity);
        rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();

        rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
        if (mesh != "" && mesh != " " && resourceExistsInAnyGroup(mesh))
        {
            rMesh->ogreEntity = _sceneMgr->createEntity(mesh);
            if (mat != " ")
                rMesh->ogreEntity->setMaterialName(mat);
            rMesh->sceneNode->attachObject(rMesh->ogreEntity);
        }
    }

    if (lua->hasComponent(name, "CameraComponent")) {


        if (_sceneMgr->getCameras().size() == 0) {
            Vector3F target = {
            lua->loadFloatValue(name, {"CameraComponent", "target", "x"}),
            lua->loadFloatValue(name, {"CameraComponent", "target", "y"}),
            lua->loadFloatValue(name, {"CameraComponent", "target", "z"})
            };
            Vector3F backgroundColor = {
            lua->loadFloatValue(name, {"CameraComponent", "backgroundColor", "r"}),
            lua->loadFloatValue(name, {"CameraComponent", "backgroundColor", "g"}),
            lua->loadFloatValue(name, {"CameraComponent", "backgroundColor", "b"})
            };

            std::string cameraName = "CameraH";
            cameraName = lua->loadStrValue(name, { "CameraComponent", "name" });
            float nearClipDistance = lua->loadFloatValue(name, { "CameraComponent", "nearClipDistance" });
            float farClipDistance = lua->loadFloatValue(name, { "CameraComponent", "farClipDistance" });
            bool showCursor = lua->loadBoolValue(name, { "CameraComponent", "showCursor" });
            bool lookAt = lua->loadBoolValue(name, { "CameraComponent", "lookAtTarget" });
            
            CameraComponent* cameraCmp = _mngr->addComponent<CameraComponent>(entity, cameraName, backgroundColor, nearClipDistance, farClipDistance, showCursor, lookAt, target);

            cameraCmp->cameraNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
            cameraCmp->yawNode = cameraCmp->cameraNode->createChildSceneNode();
            cameraCmp->pitchNode = cameraCmp->yawNode->createChildSceneNode();
            cameraCmp->camera = _sceneMgr->createCamera(cameraCmp->cameraName);
            cameraCmp->pitchNode->attachObject(cameraCmp->camera);

            cameraCmp->viewport = _window->addViewport(cameraCmp->camera);
            cameraCmp->camera->setAspectRatio(Ogre::Real(cameraCmp->viewport->getActualWidth()) / Ogre::Real(cameraCmp->viewport->getActualHeight()));

        } 
        _mngr->changeEntityId(_grp_CAMERAS, entity); 

#ifdef _DEBUG
        if (_mngr->isRunningGame()) {
            if (lua->loadBoolValue(name, { "CameraComponent", "mainCamera" }))
                _mCamera = entity;
        }
#else
        if (lua->loadBoolValue(name, { "CameraComponent", "mainCamera" }))
            _mCamera = entity;
#endif // DEBUG

    }

    if (lua->hasComponent(name, "Light")) {
        Vector3F diffuse = {
            lua->loadFloatValue(name, {"Light", "diffuseColor", "r"}),
            lua->loadFloatValue(name, {"Light", "diffuseColor", "g"}),
            lua->loadFloatValue(name, {"Light", "diffuseColor", "b"})
        };

        Vector3F specular = {
            lua->loadFloatValue(name, {"Light", "specularColor", "r"}),
            lua->loadFloatValue(name, {"Light", "specularColor", "g"}),
            lua->loadFloatValue(name, {"Light", "specularColor", "b"})
        };

        Vector3F direction = {
            lua->loadFloatValue(name, {"Light", "direction", "x"}),
            lua->loadFloatValue(name, {"Light", "direction", "y"}),
            lua->loadFloatValue(name, {"Light", "direction", "z"})
        };

        Ogre::Vector4f attenuation = {
            lua->loadFloatValue(name, {"Light", "attenuation", "range"}),
            lua->loadFloatValue(name, {"Light", "attenuation", "constant"}),
            lua->loadFloatValue(name, {"Light", "attenuation", "linear"}),
            lua->loadFloatValue(name, {"Light", "attenuation", "quadratic"})
        };
        bool castShadow = true;
        lua->loadBoolValue(name, { "Light", "castShadow" });

        Vector3F offset = Vector3F::zero();
        offset = {
            lua->loadFloatValue(name, {"Light", "offset", "x"}),
            lua->loadFloatValue(name, {"Light", "offset", "y"}),
            lua->loadFloatValue(name, {"Light", "offset", "z"})
        };

        Light* light = _mngr->addComponent<Light>(entity);
        light->light = _sceneMgr->createLight(name);
        std::string lightType = lua->loadStrValue(name, { "Light", "type" });  
        if (lightTypes.find(lightType) == lightTypes.end()) {  
           DebugLog::instance()->throwLog("[RENDER] : Tipo de luz no válido, se usará 'Directional' como predeterminado.");  
           lightType = "Directional";  
        }  
        light->light->setType(static_cast<Ogre::Light::LightTypes>(lightTypes[lightType]));

        light->light->setType(static_cast<Ogre::Light::LightTypes>(lightTypes[lightType]));
        light->light->setDiffuseColour(diffuse.x, diffuse.y, diffuse.z);  // Blanco
        light->light->setSpecularColour(specular.x, specular.y, specular.z);
        light->light->setAttenuation(attenuation.x, attenuation.y, attenuation.z, attenuation.w);
        light->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
        light->sceneNode->attachObject(light->light);
        // Tenemos en cuenta la rotacion de la entidad para aplicar el offset
        Transform* tr = _mngr->getComponent<Transform>(entity);
        Vector3F rotatedOffset = tr->rotation * offset;
        Vector3F pos = tr->position + rotatedOffset;
        light->sceneNode->setPosition(pos.x, pos.y, pos.z);
        light->sceneNode->lookAt(Ogre::Vector3(direction.x, direction.y, direction.z), Ogre::Node::TS_WORLD);
        light->light->setCastShadows(castShadow);
    }
}


QuaternionF RenderSystem::lookAt(const Vector3F& from, const Vector3F& to, const Vector3F& up) {
    Ogre::Vector3f direction = (Ogre::Vector3f(from.x, from.y, from.z) - (Ogre::Vector3f(to.x, to.y, to.z))).normalisedCopy();
    Ogre::Vector3f defaultForward = Ogre::Vector3f::UNIT_Z; // O lo que uses como "forward"
    Ogre::Vector3f oUp(up.x, up.y, up.z);

    Ogre::Quaternion rot = defaultForward.getRotationTo(direction, oUp);

    return QuaternionF(rot.w, rot.x, rot.y, rot.z).normalize();
}

void H::RenderSystem::setMaterialName(H::Entity* ent, std::string material, int subEntInd)
{
    RenderMesh* rm = _mngr->getComponent<RenderMesh>(ent);
    rm->material = material;

    if (!rm || !rm->ogreEntity) return;

    if (subEntInd == -1) {
        unsigned int count = rm->ogreEntity->getNumSubEntities();
        for (unsigned int i = 0; i < count; ++i) {
            rm->ogreEntity->getSubEntity(i)->setMaterialName(material);
        }
    }
    else {
        if (subEntInd >= 0 && subEntInd < static_cast<int>(rm->ogreEntity->getNumSubEntities())) {
            rm->ogreEntity->getSubEntity(subEntInd)->setMaterialName(material);
        }
        else {
            DebugLog::instance()->throwLog("[RENDER] : el indice de subentidad no exixte"); 
        }
    }
}





#pragma region COLLIDER_RENDER

void RenderSystem::drawCollider(Collider* collider, Transform* tr)
{
    Vector3F prePos = tr->getGlobalPosition();
    Vector3F colOffset = collider->positionOffset;
    Vector3F position = {prePos.x + colOffset.x, prePos.y + colOffset.y, prePos.z + colOffset.z};
    QuaternionF preRot = tr->getGlobalRotation();
    QuaternionF rotation = { preRot.w, preRot.x, preRot.y, preRot.z };

    if (collider->type == Collider::hBOX) {
        drawBoxCollider(position, rotation, static_cast<BoxCollider*>(collider)->halfExtents);
    }
    else if (collider->type  == Collider::hSPHERE) {
        drawSphereCollider(position, rotation, static_cast<SphereCollider*>(collider)->ratius);
    }
    else if (collider->type == Collider::hCAPSULE) {
        CapsuleCollider* capsule = static_cast<CapsuleCollider*>(collider);
        drawCapsuleCollider(position, rotation, capsule->ratius, capsule->halfHeight);
    }
}

void RenderSystem::drawBoxCollider(const Vector3F& position, const QuaternionF& rotation, const Vector3F& halfExtents)
{
    Ogre::ManualObject* manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

    std::vector<Vector3F> vertices = {
        {-halfExtents.x, -halfExtents.y, -halfExtents.z},
        { halfExtents.x, -halfExtents.y, -halfExtents.z},
        { halfExtents.x,  halfExtents.y, -halfExtents.z},
        {-halfExtents.x,  halfExtents.y, -halfExtents.z},
        {-halfExtents.x, -halfExtents.y,  halfExtents.z},
        { halfExtents.x, -halfExtents.y,  halfExtents.z},
        { halfExtents.x,  halfExtents.y,  halfExtents.z},
        {-halfExtents.x,  halfExtents.y,  halfExtents.z}
    };

    int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // Cara trasera
        4, 5, 5, 6, 6, 7, 7, 4, // Cara frontal
        0, 4, 1, 5, 2, 6, 3, 7  // Conectando caras
    };

    // Aplicar rotación y traslación correctamente
    for (int i = 0; i < 24; i += 2) {
        Vector3F v1 = rotation * vertices[indices[i]] + position;
        Vector3F v2 = rotation * vertices[indices[i + 1]] + position;
        manual->position(v1.x, v1.y, v1.z);  // CORREGIDO: usar componentes correctos
        manual->position(v2.x, v2.y, v2.z);
    }

    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
}


void RenderSystem::drawSphereCollider(const Vector3F& position, const QuaternionF& rotation, float radius)
{
    Ogre::ManualObject* manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

    const int segments = 32;

    // Círculo en el plano XY
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::TWO_PI * float(i) / float(segments);
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(x, y, 0);
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Círculo en el plano XZ
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::TWO_PI * float(i) / float(segments);
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(x, 0, z);
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Círculo en el plano YZ
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::TWO_PI * float(i) / float(segments);
        float y = radius * cos(theta);
        float z = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(0, y, z);
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
}

void RenderSystem::drawCapsuleCollider(const Vector3F& position, const QuaternionF& rotation, float radius, float halfHeight)
{
    const int segments = 16;
    Ogre::ManualObject* manual;

    // Semicircunferencia superior (plano XY)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::PI * float(i) / float(segments);
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(x, y + halfHeight, 0); // Ajuste con el radio
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Semicircunferencia inferior (plano XY)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::PI * float(i) / float(segments);
        float x = radius * cos(theta);
        float y = -radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(x, y - halfHeight, 0); // Ajuste con el radio
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Líneas verticales (plano X)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
    Vector3F pos = position + rotation * Vector3F(radius, halfHeight , 0); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    pos = position + rotation * Vector3F(radius, -halfHeight, 0); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    pos = position + rotation * Vector3F(-radius, halfHeight, 0); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    pos = position + rotation * Vector3F(-radius, -halfHeight, 0); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Semicircunferencia superior (plano YZ)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::PI * float(i) / float(segments);
        float z = radius * cos(theta);
        float y = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(0, y + halfHeight, z); // Ajuste con el radio
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Semicircunferencia inferior (plano YZ)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = Ogre::Math::PI * float(i) / float(segments);
        float z = radius * cos(theta);
        float y = -radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(0, y - halfHeight, z); // Ajuste con el radio
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Líneas verticales (plano Z)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
    pos = position + rotation * Vector3F(0, halfHeight, radius); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    pos = position + rotation * Vector3F(0, -halfHeight, radius); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    pos = position + rotation * Vector3F(0, halfHeight, -radius); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    pos = position + rotation * Vector3F(0, -halfHeight, -radius); // Ajuste con el radio
    manual->position(pos.x, pos.y, pos.z);
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Circunferencia en el plano XZ (parte superior)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments * 2; ++i) {
        float theta = Ogre::Math::TWO_PI * float(i) / float(segments * 2);
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(x, halfHeight, z); // Ajuste con el radio
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

    // Circunferencia en el plano XZ (parte inferior)
    manual = _sceneMgr->createManualObject();
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    for (int i = 0; i <= segments * 2; ++i) {
        float theta = Ogre::Math::TWO_PI * float(i) / float(segments * 2);
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        Vector3F transformed = position + rotation * Vector3F(x, -halfHeight, z); // Ajuste con el radio
        manual->position(transformed.x, transformed.y, transformed.z);
    }
    manual->end();
    _colliderToRender.push_back(manual);
    _sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
}





#pragma endregion

#pragma region RAY_RENDER

void RenderSystem::drawRay(const Vector3F& origin, const Vector3F& direction, float length, const Color& color, float ttl = 2.f) 
{
    Ogre::ManualObject* ray = _sceneMgr->createManualObject();
    ray->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

    Ogre::Vector3 ogreOrigin = Ogre::Vector3(origin.x, origin.y, origin.z);
    Ogre::Vector3 ogreDirection = Ogre::Vector3(direction.x, direction.y, direction.z);
    Ogre::ColourValue ogreColor = Ogre::ColourValue(color.r, color.g, color.b);

    Ogre::Vector3 endPoint = ogreOrigin + ogreDirection * length;

    ray->position(ogreOrigin);
    ray->colour(ogreColor);
    ray->position(endPoint);
    ray->colour(ogreColor);

    ray->end();

    // Attach a la escena
    Ogre::SceneNode* rayNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rayNode->attachObject(ray);

    RayWithTTL newRay = { ray, rayNode, ttl, 0.0f };
    _activeRays.push_back(newRay);
}

void RenderSystem::updateRays(double deltaTime) {
    for (auto it = _activeRays.begin(); it != _activeRays.end(); ) {
        it->elapsedTime += deltaTime;  // Incrementar el tiempo transcurrido

        if (it->elapsedTime >= it->timeToLive) {
            // Eliminar el rayo de la escena si su tiempo de vida ha expirado
            _sceneMgr->destroyManualObject(it->rayObject);
            it->rayNode->detachAllObjects();
            _sceneMgr->getRootSceneNode()->removeChild(it->rayNode);

            it = _activeRays.erase(it);  // Eliminar el rayo de la lista
        }
        else {
            ++it;  // Si el rayo aún vive, seguimos al siguiente
        }
    }
}

#pragma endregion
