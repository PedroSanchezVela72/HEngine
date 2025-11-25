
#ifdef _DEBUG
#include "EditorSystem.h"
#include "RenderSystem.h"
#include "ImguiManager.h"
#include <Ogre.h>
#include <windows.h>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Manager.h"
#include "RenderMesh.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "LoadLua.h"
#include "PhysicsSystem.h"
#include "CameraComponent.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include <Debuglog.h>
#include "InputKeys.h"
#include "Quaternion.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


using namespace H;


EditorSystem::EditorSystem(Manager* mngr, Ogre::DEBUG_BUILD_REQUIRED::Root* root ,Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* renderWindow) :
    _mngr(mngr), _root(root), _sceneMgr(sceneMgr), _renderWindow(renderWindow), _hierarchy() {

    _ImguiMngr = Ogre::ImguiManager::getSingletonPtr();

    _axisPlanes[AXIS_X] = new Ogre::Plane({ 1,0,0 }, { 0, 0, 0 });
    _axisPlanes[AXIS_Y] = new Ogre::Plane({ 0,1,0 }, { 0, 0, 0 });
    _axisPlanes[AXIS_Z] = new Ogre::Plane({ 0,0,1 }, { 0, 0, 0 });

    _editorCamera = _mngr->addEntity(_grp_CAMERAS);
    // Posición inicial de la cámara
    _mngr->getComponent<Transform>(_editorCamera)->position = Vector3F(H_INIT_CAMERA_POS);

    // Inicialización de la cámara
    CameraComponent* cameraCmp = _mngr->addComponent<CameraComponent>(
        _editorCamera, 
        "editorCamera", 
        Vector3F(H_BACKGROUND_COLOR), 
        H_NEAR_CLIP_DIST, 
        H_FAR_CLIP_DIST, 
        true );

    cameraCmp->cameraNode = _sceneMgr->getRootSceneNode()->createChildSceneNode("editorCamera");
    cameraCmp->yawNode = cameraCmp->cameraNode->createChildSceneNode();
    cameraCmp->pitchNode = cameraCmp->yawNode->createChildSceneNode();
    cameraCmp->camera = _sceneMgr->createCamera(cameraCmp->cameraName);
    cameraCmp->pitchNode->attachObject(cameraCmp->camera);

    cameraCmp->viewport = _renderWindow->addViewport(cameraCmp->camera);
    cameraCmp->camera->setAspectRatio(Ogre::Real(cameraCmp->viewport->getActualWidth()) / Ogre::Real(cameraCmp->viewport->getActualHeight()));

    Message* m = new Message;
    m->id = _m_CHANGE_MAINCAMERA;
    m->entity.entityPtr = _editorCamera;
    _mngr->send(m);

    cameraCmp->cameraNode->setPosition(Ogre::Vector3(H_INIT_CAMERA_POS));
    QuaternionF q = QuaternionF(Vector3F(H_INIT_CAMERA_ORENTATION));
    cameraCmp->cameraNode->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));

    createArrows();
    createTorus();
    createScalators();
}

EditorSystem::~EditorSystem()
{

    _hierarchy.clear();

    if (_arrowsMode != ROTATE && _referencePlane != nullptr) {
        delete _referencePlane;
        _referencePlane = nullptr;
    }

    entityNames.clear();

    for (size_t i = 0; i < H_NUM_AXIS_PLANE; i++)
    {
        if (_axisPlanes[i] != nullptr) {
            delete _axisPlanes[i];
            _axisPlanes[i] = nullptr;
        }
        
    }

    _sceneMgr->getRootSceneNode()->removeChild("editorCamera");
    _sceneMgr->destroySceneNode("editorCamera");

    //delete[]_axisPlanes;
}

void EditorSystem::loadHierarchy() {
    LoadLua* lua = LoadLua::instance();
    lua->loadHierarchy(_hierarchy);
}

void EditorSystem::update(double deltaTime) {

    if (_moveInAxis != AXIS_NONE) {

        //Obtener la posicion del raton en la ventana
        int x, y;
        SDL_GetMouseState(&x, &y);
        //Convertirlo en escala de [0,1]
        float Realx = (float)x / _renderWindow->getWidth();
        float Realy = (float)y / _renderWindow->getHeight();

        //Crear un rayo desde la posicion del near plane al far plane de la camara
        Entity* cameraEntity = _mngr->getSystem<RenderSystem>()->getCamera();
        CameraComponent* camera = _mngr->getComponent<CameraComponent>(cameraEntity);
        Ogre::Ray ray = camera->camera->getCameraToViewportRay(Realx, Realy);
        Ogre::Vector3 point = ray.getPoint(ray.intersects(*_referencePlane).second);

        if (_arrowsMode == ROTATE) {
            Ogre::Vector3 prevFromEnt(
                _prevPoint.x - _selectedTr->position.x, 
                _prevPoint.y - _selectedTr->position.y,
                _prevPoint.z - _selectedTr->position.z);

            Ogre::Vector3 pointFromEnt(
                point.x - _selectedTr->position.x,
                point.y - _selectedTr->position.y,
                point.z - _selectedTr->position.z);

            Ogre::Vector3 crossProduct = prevFromEnt.crossProduct(pointFromEnt);

            Ogre::Vector2 prevFE2;
            Ogre::Vector2 pointFE2;
            float angle;

            switch (_moveInAxis)
            {
            case AXIS_X:
                prevFE2 = { prevFromEnt.y, prevFromEnt.z };
                pointFE2 = { pointFromEnt.y, pointFromEnt.z };
                angle = prevFE2.angleBetween(pointFE2).valueDegrees();
                if (crossProduct.x < 0)
                    angle = -angle;
                _selectedTr->rotation = _origRot.setPitch(angle);
                break;
            case AXIS_Y:
                prevFE2 = { prevFromEnt.x, prevFromEnt.z };
                pointFE2 = { pointFromEnt.x, pointFromEnt.z };
                angle = prevFE2.angleBetween(pointFE2).valueDegrees();
                if (crossProduct.y < 0)
                    angle = -angle;
                _selectedTr->rotation = _origRot.setYaw(angle);
                break;
            case AXIS_Z:
                prevFE2 = { prevFromEnt.x, prevFromEnt.y };
                pointFE2 = { pointFromEnt.x, pointFromEnt.y };
                angle = prevFE2.angleBetween(pointFE2).valueDegrees();
                if (crossProduct.z < 0)
                    angle = -angle;
                _selectedTr->rotation = _origRot.setRoll(angle);
                break;
            }
            Vector3F euler = _selectedTr->rotation.toEuler();
            _selectedTr->rotation.pitch = euler.x;
            _selectedTr->rotation.yaw = euler.y;
            _selectedTr->rotation.roll = euler.z;
        }
        else if (_arrowsMode == MOVE) {
            float distance = _axisPlanes[_moveInAxis]->getDistance(point);
            float orgDistance = _axisPlanes[_moveInAxis]->getDistance({ _prevPoint.x, _prevPoint.y, _prevPoint.z });
            switch (_moveInAxis)
            {
            case AXIS_X:
                if (_selectedTr != nullptr)
                    _selectedTr->position.x = distance - orgDistance + _origTr.x;
                break;
            case AXIS_Y:
                if (_selectedTr != nullptr)
                    _selectedTr->position.y = distance - orgDistance + _origTr.y;
                break;
            case AXIS_Z:
                if (_selectedTr != nullptr)
                    _selectedTr->position.z = distance - orgDistance + _origTr.z;
                break;
            case AXIS_NONE:
                break;
            }
        }
        else if (_arrowsMode == SCALE) {
            float distance = _axisPlanes[_moveInAxis]->getDistance(point);
            float orgDistance = _axisPlanes[_moveInAxis]->getDistance({ _prevPoint.x, _prevPoint.y, _prevPoint.z });

            float diff = distance - orgDistance;
            switch (_moveInAxis)
            {
            case AXIS_X:
                _selectedTr->scale.x = distance - orgDistance + _origTr.x;
                break;
            case AXIS_Y:
                _selectedTr->scale.y = distance - orgDistance + _origTr.y;
                break;
            case AXIS_Z:
                _selectedTr->scale.z = distance - orgDistance + _origTr.z;
                break;
            case AXIS_NONE:
                break;
            }
        }
        _unsavedChanges = true;
    }
    else {
        // Resetera el material de todas las entidades
        std::list<Entity*> resetList;
        switch (_arrowsMode)
        {
        case H::MOVE:
            resetList = _arrows->getChildren();
            break;
        case H::ROTATE:
            resetList = _torus->getChildren();
            break;
        case H::SCALE:
            resetList = _scalators->getChildren();
            break;
        default:
            resetList = std::list<Entity*>();
            break;
        }
        for (Entity* e : resetList)
        {
            if (e != nullptr)
                AlternateMaterial(e);
        }
    }

    //Las flechas persiguen a la entidad seleccionada
    if (_selectedTr != nullptr) {
        changeArrowsTipe(_arrowsMode);
        Vector3F pos = _selectedTr->getGlobalPosition();
        _mngr->getComponent<Transform>(_arrows)->position = pos;
        _mngr->getComponent<Transform>(_torus)->position = pos;
        _mngr->getComponent<Transform>(_scalators)->position = pos;
    }
   
    // Comenzar un nuevo frame de ImGui 
    _ImguiMngr->newFrame((float)deltaTime, Ogre::Rect(0, 0, H_WINDOW_WIDTH, H_WINDOW_HEIGHT));

    // Ventana del inspector
    showEntityComponentWindow(deltaTime);
    // Ventana de Play Controls
    playAndSave(deltaTime);
}

void EditorSystem::receive(const Message* m) {
    switch (m->id) {
    case msgId::_m_MOUSE_DOWN:
        if (m->mouse.button == HENGINE_RIGHT_BUTTON) {
            if (_moveInAxis != AXIS_NONE) {
                switch (_arrowsMode)
                {
                case MOVE:
                    _selectedTr->position = _origTr;
                    break;
                case ROTATE:
                    _selectedTr->rotation = _origRot;
                    break;
                case SCALE:
                    _selectedTr->scale = _origTr;
                    break;
                }

                _moveInAxis = AXIS_NONE;
            }
            else
                _rotationCamera = true;
        }
        else if (m->mouse.button == HENGINE_LEFT_BUTTON) {
            if (_arrowsMode != ROTATE && _referencePlane != nullptr) {
                delete _referencePlane;
                _referencePlane = nullptr;
            }
            raycast();
        }
        break;
    case msgId::_m_MOUSE_UP:
        if (m->mouse.button == HENGINE_RIGHT_BUTTON) {
            _rotationCamera = false;
        }
        else if (m->mouse.button == HENGINE_LEFT_BUTTON){
            _moveInAxis = AXIS_NONE;
        }
        break;
    case msgId::_m_MOUSE_MOTION:
        if (_rotationCamera)
            rotateCamera(m->mouse_motion.xrel, m->mouse_motion.yrel);
        break;
    case msgId::_m_KEY_HELD:
        moveCameraInXY(m->key.key);
        break;
    case msgId::_m_MOUSE_WHEEL:
        moveCameraInZ(m->mouse_wheel.dir);
        break;
    case msgId::_m_KEY_DOWN:
        if (_moveInAxis == AXIS_NONE) {
            if (m->key.key == HENGINE_I) {
                _arrowsMode = MOVE;
                if(_selectedTr != nullptr)
                    changeArrowsTipe(MOVE);
            }
            else if (m->key.key == HENGINE_O) {
                _arrowsMode = ROTATE;
                if (_selectedTr != nullptr)
                    changeArrowsTipe(ROTATE);
            }
            else if (m->key.key == HENGINE_P) {
                _arrowsMode = SCALE;
                if (_selectedTr != nullptr)
                    changeArrowsTipe(SCALE);
            }
        }
        break;
    case msgId::_m_INIT_ENTITY:
        entityMap[m->entity.entityName] = m->entity.entityPtr;
        break;
    default:
        break;
    }
}

std::string EditorSystem::serializeText(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\n') result += "\\n";  // Convertir salto de línea en texto
        else result += c;
    }
    return result;
}

void EditorSystem::saveHierarchyToLua (HierarchyType hierarchy) {
    LoadLua* lua = LoadLua::instance();
    lua->saveHierarchy(_hierarchy);

    std::vector<std::string> names;
    for (const auto& entity : entityMap) {
        names.push_back(entity.first);
    }
    lua->writeEntities(names);
}

void EditorSystem::playAndSave(double deltaTime) {
    ImGui::SetNextWindowPos(ImVec2(H_PLAY_CTRL_POS)); 
    ImGui::SetNextWindowSize(ImVec2(H_PLAY_CTRL_WND_WIDTH, H_PLAY_CTRL_WND_HEIGHT));
    ImGui::Begin("Play Controls", nullptr, ImGuiWindowFlags_NoCollapse); 

    //Indica el modo de las flechas
    std::string mode = "Arrows mode: ";
    switch (_arrowsMode)
    {
    case H::MOVE:
        mode += "Move";
        break;
    case H::ROTATE:
        mode += "Rotate";
        break;
    case H::SCALE:
        mode += "Scale";
        break;
    }
    ImGui::Text(mode.c_str());

    static bool showUI = true;

    ImVec4 uiButtonColor = showUI ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 uiTextColor = showUI ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Texto gris si está visible

    ImGui::PushStyleColor(ImGuiCol_Button, uiButtonColor); 
    ImGui::PushStyleColor(ImGuiCol_Text, uiTextColor);

    std::string uiButtonText = showUI ? "Hide UI" : "Show UI";
    if (ImGui::Button(uiButtonText.c_str())) {
        showUI = !showUI; 

        Message* m = new Message;
        m->id = _m_SHOW_UI_EDITOR;
        m->show_UI_editor.show = showUI;
        _mngr->send(m);
    }

    ImGui::PopStyleColor(2);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(H_PLAY_CTRL_BUTTON_COLOR));
    if (ImGui::Button("Play")) { // Botón Play
        LanzarMotorRelease();
    }
    ImGui::PopStyleColor();

    saveButton();
    ImGui::End();
}

void EditorSystem::saveButton() {
    std::string saveText = _unsavedChanges ? "Save *" : "Save";

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(H_PLAY_CTRL_BUTTON_COLOR)); 
    if (ImGui::Button(saveText.c_str())) { // Botón Save
        saveHierarchyToLua(_hierarchy);
        _unsavedChanges = false; 
    }
    ImGui::PopStyleColor();
}



void EditorSystem::showEntityComponentWindow(double deltaTime) {
  
    ImGui::SetNextWindowPos(ImVec2(H_INSPECTOR_POS));
    ImGui::SetNextWindowSize(ImVec2(H_INSPECTOR_WIDTH, H_INSPECTOR_HEIGHT)); 
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always); 

    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoCollapse); 

    static std::string selectedEntity = "";
    static std::string selectedComponent = "";

    if (ImGui::BeginCombo("Entidades", selectedEntity.empty() ? "Selecciona una entidad" : selectedEntity.c_str())) {
        for (const auto& entity : entityMap) {
            bool isSelected = (selectedEntity == entity.first);
            if (ImGui::Selectable(entity.first.c_str(), isSelected)) {
                selectedEntity = entity.first;
                selectedComponent = "";
                _selectedTr = _mngr->getComponent<Transform>(LoadLua::instance()->getEntity(selectedEntity));
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (!selectedEntity.empty() && entityMap.find(selectedEntity) != entityMap.end()) {
        ImGui::Separator();
        ImGui::Text("Componentes de %s", selectedEntity.c_str());

        if (ImGui::BeginCombo("Componentes", selectedComponent.empty() ? "Selecciona un componente" : selectedComponent.c_str())) {
            for (const auto& element : _hierarchy) {
                if (element.args[0] == selectedEntity && element.profundidad == 1) {
                    bool isSelected = (selectedComponent == element.args[1]);
                    if (ImGui::Selectable(element.args[1].c_str(), isSelected)) {
                        selectedComponent = element.args[1];
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }                
            }
            ImGui::EndCombo();
        }

        if (!selectedComponent.empty()) {
            ImGui::Separator();
            ImGui::Text("Propiedades de %s", selectedComponent.c_str());

            for (auto& element : _hierarchy) {
                if (element.profundidad > 1) {
                    if (element.args[0] == selectedEntity && element.args[1] == selectedComponent) {
                        std::string propName = element.args[element.profundidad];
                        if (element.profundidad > 2) {
                            for (size_t i = element.args.size() - 2; i >= 2; i--) {
                                propName = element.args[i] + "-" + propName;
                            }
                            if (_selectedTr && element.args[1] == "Transform") {
                                if (element.args[2] == "position") {
                                    if (element.args[3] == "x")element.fValue = _selectedTr->position.x;
                                    else if (element.args[3] == "y")element.fValue = _selectedTr->position.y;
                                    else if (element.args[3] == "z")element.fValue = _selectedTr->position.z;
                                }
                                else if (element.args[2] == "rotation") {
                                    
                                    if (element.args[3] == "x")element.fValue = _selectedTr->rotation.getPitch();
                                    else if (element.args[3] == "y")element.fValue = _selectedTr->rotation.getYaw();
                                    else if (element.args[3] == "z")element.fValue = _selectedTr->rotation.getRoll();
                                }
                                else if (element.args[2] == "scale") {
                                    if (element.args[3] == "x")element.fValue = _selectedTr->scale.x;
                                    else if (element.args[3] == "y")element.fValue = _selectedTr->scale.y;
                                    else if (element.args[3] == "z")element.fValue = _selectedTr->scale.z;
                                }
                            }
                        }
                        int ivalue = 0;
                        bool bvalue = false;
                        float fvalue = 0; 
                        std::string svalue = "";
                        switch (element.tipo) {
                        case INT:
                            ivalue = element.iValue;
                            if (ImGui::DragInt(propName.c_str(), &ivalue)) {
                                if (ivalue != element.iValue) {
                                    element.iValue = ivalue;
                                    _unsavedChanges = true;
                                }
                            }
                            break;
                        case BOOL:
                            bvalue = element.bValue;
                            if (ImGui::Checkbox(propName.c_str(), &bvalue)) {
                                if (bvalue != element.bValue) {
                                    element.bValue = bvalue;
                                    _unsavedChanges = true;
                                }
                            }
                            if (selectedComponent == "CapsuleCollider" && _unsavedChanges && element.args[element.profundidad] == "draw") {
                                CapsuleCollider* capsule = _mngr->getComponent<CapsuleCollider>(entityMap[selectedEntity]);
                                if (capsule) capsule->draw = element.bValue;
                            }
                            if (selectedComponent == "SphereCollider" && _unsavedChanges && element.args[element.profundidad] == "draw") {
                                SphereCollider* sphere = _mngr->getComponent<SphereCollider>(entityMap[selectedEntity]);
                                if (sphere) sphere->draw = element.bValue;
                            }
                            if (selectedComponent == "BoxCollider" && _unsavedChanges && element.args[element.profundidad] == "draw") {
                                BoxCollider* box = _mngr->getComponent<BoxCollider>(entityMap[selectedEntity]);
                                if (box) box->draw = element.bValue;
                            }
                            break;
                        case FLOAT:
                            fvalue = element.fValue;

                            if (_selectedTr && selectedComponent == "Transform" && element.args[2] == "rotation") {
                                if (element.args[3] == "x") fvalue = _selectedTr->rotation.getPitch();
                                else if (element.args[3] == "y") fvalue = _selectedTr->rotation.getYaw();
                                else if (element.args[3] == "z") fvalue = _selectedTr->rotation.getRoll();
                            }

                            if (ImGui::DragFloat(propName.c_str(), &fvalue)) {
                                if (fvalue != element.fValue) {
                                    element.fValue = fvalue;
                                    _unsavedChanges = true;

                                    if (_selectedTr && selectedComponent == "Transform") {
                                        if (element.args[2] == "position") {
                                            if (element.args[3] == "x") _selectedTr->position.x = fvalue;
                                            else if (element.args[3] == "y") _selectedTr->position.y = fvalue;
                                            else if (element.args[3] == "z") _selectedTr->position.z = fvalue;
                                        }
                                        else if (element.args[2] == "rotation") {
                                            if (element.args[3] == "x") _selectedTr->rotation.rotateLocalAxis('x', fvalue);
                                            else if (element.args[3] == "y") _selectedTr->rotation.rotateLocalAxis('y', fvalue);
                                            else if (element.args[3] == "z")  _selectedTr->rotation.rotateLocalAxis('z', fvalue);
                                        }
                                        else if (element.args[2] == "scale") {
                                            if (element.args[3] == "x") _selectedTr->scale.x = fvalue;
                                            else if (element.args[3] == "y") _selectedTr->scale.y = fvalue;
                                            else if (element.args[3] == "z") _selectedTr->scale.z = fvalue;
                                        }
                                    }
                                }
                            }
                            break;

                        case STRING:
                            svalue = element.sValue;
                            if (ImGui::InputText(propName.c_str(), &svalue[0], svalue.size() + 1)) {
                                if (svalue != element.sValue) {
                                    element.sValue = svalue;
                                    _unsavedChanges = true;
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }                
            }                    
        }
    }

    ImGui::End();
}

void EditorSystem::createArrows() {
    _arrows = _mngr->addEntity(_grp_PARENTENTS);
    Entity* entityArrowX = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowX);
    Entity* entityArrowY = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowY);
    Entity* entityArrowZ = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowZ);

    //Flecha X
    RenderMesh* rMesh = _mngr->addComponent<RenderMesh>(entityArrowX);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_ARROW_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setMaterialName(H_ARROW_X_MAT);
    rMesh->material = H_ARROW_X_MAT;
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    Ogre::MaterialPtr material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowX);

    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowX, Vector3F(H_ARROW_SIZE), Vector3F(H_ARROW_OFFSET), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowX, _arrows);

    //Flecha Y
    rMesh = _mngr->addComponent<RenderMesh>(entityArrowY);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_ARROW_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->ogreEntity->setMaterialName(H_ARROW_Y_MAT);
    rMesh->material = H_ARROW_Y_MAT;
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowY);

    _mngr->getComponent<Transform>(entityArrowY)->rotation = QuaternionF(Vector3F(H_ARROW_Y_ROTATION));
    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowY, Vector3F(H_ARROW_SIZE), Vector3F(H_ARROW_OFFSET), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowY, _arrows);

    //Flecha Z
    rMesh = _mngr->addComponent<RenderMesh>(entityArrowZ);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_ARROW_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->ogreEntity->setMaterialName(H_ARROW_Z_MAT);
    rMesh->material = H_ARROW_Z_MAT;
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowZ);

    _mngr->getComponent<Transform>(entityArrowZ)->rotation = QuaternionF(Vector3F(H_ARROW_Z_ROTATION));
    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowZ, Vector3F(H_ARROW_SIZE), Vector3F(H_ARROW_OFFSET), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowZ, _arrows);

    _arrows->setActive(false);
}

void EditorSystem::createTorus() {
    _torus = _mngr->addEntity(_grp_PARENTENTS);
    Entity* entityArrowX = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowX);
    Entity* entityArrowY = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowY);
    Entity* entityArrowZ = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowZ);

    //Flecha X
    RenderMesh* rMesh = _mngr->addComponent<RenderMesh>(entityArrowX);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_TORUS_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setMaterialName(H_ARROW_X_MAT);
    rMesh->material = H_ARROW_X_MAT;
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    Ogre::MaterialPtr material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowX);

    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowX, Vector3F(H_TORUS_SIZE), Vector3F::zero(), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowX, _torus);

    //Flecha Y
    rMesh = _mngr->addComponent<RenderMesh>(entityArrowY);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_TORUS_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->ogreEntity->setMaterialName(H_ARROW_Y_MAT);
    rMesh->material = H_ARROW_Y_MAT;
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowY);

    _mngr->getComponent<Transform>(entityArrowY)->rotation = QuaternionF(Vector3F(H_ARROW_Y_ROTATION));
    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowY, Vector3F(H_TORUS_SIZE), Vector3F::zero(), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowY, _torus);

    //Flecha Z
    rMesh = _mngr->addComponent<RenderMesh>(entityArrowZ);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_TORUS_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->ogreEntity->setMaterialName(H_ARROW_Z_MAT);
    rMesh->material = H_ARROW_Z_MAT;
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowZ);

    _mngr->getComponent<Transform>(entityArrowZ)->rotation = QuaternionF(Vector3F(H_ARROW_Z_ROTATION));
    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowZ, Vector3F(H_TORUS_SIZE), Vector3F::zero(), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowZ, _torus);

    _torus->setActive(false);
}


void EditorSystem::createScalators() {
    _scalators = _mngr->addEntity(_grp_PARENTENTS);
    Entity* entityArrowX = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowX);
    Entity* entityArrowY = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowY);
    Entity* entityArrowZ = _mngr->addEntity(_grp_DEBUG_AXIS);
    _mngr->setHandler(_hdlr_DEBUG_AXIS, entityArrowZ);

    //Flecha X
    RenderMesh* rMesh = _mngr->addComponent<RenderMesh>(entityArrowX);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_SCALATOR_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setMaterialName(H_ARROW_X_MAT);
    rMesh->material = H_ARROW_X_MAT;
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    Ogre::MaterialPtr material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowX);

    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowX, Vector3F(H_ARROW_SIZE), Vector3F(H_ARROW_OFFSET), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowX, _scalators);

    //Flecha Y
    rMesh = _mngr->addComponent<RenderMesh>(entityArrowY);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_SCALATOR_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->ogreEntity->setMaterialName(H_ARROW_Y_MAT);
    rMesh->material = H_ARROW_Y_MAT;
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowY);

    _mngr->getComponent<Transform>(entityArrowY)->rotation = QuaternionF(Vector3F(H_ARROW_Y_ROTATION));
    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowY, Vector3F(H_ARROW_SIZE), Vector3F(H_ARROW_OFFSET), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowY, _scalators);

    //Flecha Z
    rMesh = _mngr->addComponent<RenderMesh>(entityArrowZ);
    rMesh->ogreEntity = _sceneMgr->createEntity(H_SCALATOR_MESH);
    rMesh->sceneNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    rMesh->ogreEntity->setMaterialName(H_ARROW_Z_MAT);
    rMesh->material = H_ARROW_Z_MAT;
    rMesh->sceneNode->attachObject(rMesh->ogreEntity);
    rMesh->ogreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    material->setDepthCheckEnabled(false);
    material->setDepthWriteEnabled(false);

    AlternateMaterial(entityArrowZ);

    _mngr->getComponent<Transform>(entityArrowZ)->rotation = QuaternionF(Vector3F(H_ARROW_Z_ROTATION));
    _mngr->getSystem<PhysicsSystem>()->addBoxCollider(entityArrowZ, Vector3F(H_ARROW_SIZE), Vector3F(H_ARROW_OFFSET), _hdlr_DEBUG_AXIS);

    _mngr->addEntityParent(entityArrowZ, _scalators);

    _scalators->setActive(false);
}

void EditorSystem::changeArrowsTipe(ArrowsMode arrowsMode)
{
    std::string newMesh;
    
    switch (arrowsMode)
    {
    case H::MOVE:
        _arrows->setActive(true);
        _torus->setActive(false);
        _scalators->setActive(false);
        break;
    case H::ROTATE:
        _arrows->setActive(false);
        _torus->setActive(true);
        _scalators->setActive(false);
        break;
    case H::SCALE:
        _arrows->setActive(false);
        _torus->setActive(false);
        _scalators->setActive(true);
        break;
    default:
        _arrows->setActive(false);
        _torus->setActive(false);
        _scalators->setActive(false);
        break;
    }
}

void H::EditorSystem::AlternateMaterial(Entity* ent,bool SecondMat)
{
    RenderMesh* rMesh = _mngr->getComponent<RenderMesh>(ent);
    Ogre::MaterialPtr material = rMesh->ogreEntity->getSubEntity(0)->getMaterial();
    Ogre::Technique* tech = material->getTechnique(0);

    if (material == nullptr || tech == nullptr) return;

    if (tech->getNumPasses() >= 2) {
        tech->getPass(0)->setColourWriteEnabled(!SecondMat);
        tech->getPass(1)->setColourWriteEnabled(SecondMat);
    }
}

void EditorSystem::raycast() {
    //Obtener la posicion del raton en la ventana
    int x, y;
    SDL_GetMouseState(&x, &y);
    //Convertirlo en escala de [0,1]
    float Realx = (float)x / _renderWindow->getWidth();
    float Realy = (float)y / _renderWindow->getHeight();

    //Crear un rayo desde la posicion del near plane al far plane de la camara
    Entity* cameraEntity = _mngr->getSystem<RenderSystem>()->getCamera();
    CameraComponent* camera = _mngr->getComponent<CameraComponent>(cameraEntity);
    Ogre::Ray ray = camera->camera->getCameraToViewportRay(Realx, Realy);
    Ogre::Vector3 dir = ray.getDirection();
    Ogre::Vector3 org = ray.getOrigin();

    //Raycast del motor de fisicas para encontrar las flechas
    RaycastHit raycastInfo;
    bool hit = _mngr->getSystem<PhysicsSystem>()->raycast({ org.x,org.y,org.z }, { dir.x,dir.y,dir.z }, H_FAR_CLIP_DIST, raycastInfo, _hdlr_DEBUG_AXIS);

    if (hit) {
        int cont = 0;
        std::list<Entity*> entities;
        if (_arrowsMode == ROTATE)
            entities = _torus->getChildren();
        else if (_arrowsMode == MOVE)
            entities = _arrows->getChildren();
        else if (_arrowsMode == SCALE)
            entities = _scalators->getChildren();

        for (Entity* e : entities) {
            if (raycastInfo.entity == e) {
                AlternateMaterial(e, true);
                switch (cont)
                {
                case 0:
                    _moveInAxis = AXIS_X;
                    break;
                case 1:
                    _moveInAxis = AXIS_Y;
                    break;
                case 2:
                    _moveInAxis = AXIS_Z;
                    break;
                default:
                    _moveInAxis = AXIS_NONE;
                    break;
                }
            }
            cont++;
        }

        Ogre::Vector3 point(raycastInfo.point.x, raycastInfo.point.y, raycastInfo.point.z);
        
        Ogre::Vector3 normal(raycastInfo.normal.x, raycastInfo.normal.y, raycastInfo.normal.z);
        _referencePlane = new Ogre::Plane(normal, point);

        point = ray.getPoint(ray.intersects(*_referencePlane).second);

        _prevPoint = Vector3F(point.x, point.y, point.z);

        switch (_arrowsMode)
        {
        case MOVE:
            if(_selectedTr != nullptr)
                _origTr = _selectedTr->position;
            break;
        case ROTATE:
            if (_selectedTr != nullptr)
                _origRot = _selectedTr->rotation;
            break;
        case SCALE:
            if (_selectedTr != nullptr)
                _origTr = _selectedTr->scale;
            break;
        }
    }
}


void EditorSystem::moveCameraInXY(KeyCode key) {
    
    if (key == HENGINE_A) {
        Transform* tr = _mngr->getComponent<Transform>(_editorCamera);
        Vector3F localX = tr->rotation.rotateVector(Vector3F(1,0,0));
        tr->position = tr->position - localX * _cameraSpeed;
    }
    else if (key == HENGINE_W) {
        Transform* tr = _mngr->getComponent<Transform>(_editorCamera);
        Vector3F localY = tr->rotation.rotateVector(Vector3F(0, 1, 0));
        tr->position = tr->position + localY * _cameraSpeed;
    }
    else if (key == HENGINE_S) {
        Transform* tr = _mngr->getComponent<Transform>(_editorCamera);
        Vector3F localY = tr->rotation.rotateVector(Vector3F(0, 1, 0));
        tr->position = tr->position - localY * _cameraSpeed;
    }
    else if (key == HENGINE_D) {
        Transform* tr = _mngr->getComponent<Transform>(_editorCamera);
        Vector3F localX = tr->rotation.rotateVector(Vector3F(1, 0, 0));
        tr->position = tr->position + localX * _cameraSpeed;
    }
}

void EditorSystem::moveCameraInZ(int dir) {
    Transform* tr = _mngr->getComponent<Transform>(_editorCamera);
    Vector3F localZ = tr->rotation.rotateVector(Vector3F(0, 0, 1));
    tr->position = tr->position + localZ * _cameraSpeed * (float)- dir * 2.f;
}

void EditorSystem::rotateCamera(float x, float y) {
    Transform* tr = _mngr->getComponent<Transform>(_editorCamera); 

    // Acumular ángulos (invirtiendo el eje vertical si quieres)
    _anguloAcumulado.first -= x / 4.0f;   // Yaw (rotación alrededor de Y) 
    _anguloAcumulado.second -= y / 4.0f;  // Pitch (rotación alrededor de X) 

    // Clamp pitch entre -89 y 89 grados para evitar gimbal lock
    if (_anguloAcumulado.second > 89.0f) _anguloAcumulado.second = 89.0f;
    if (_anguloAcumulado.second < -89.0f) _anguloAcumulado.second = -89.0f;

    // Crear quaterniones desde ángulos acumulados
    QuaternionF yaw = QuaternionF::fromAngleAxis(_anguloAcumulado.first, Vector3F(0, 1, 0));
    QuaternionF pitch = QuaternionF::fromAngleAxis(_anguloAcumulado.second, Vector3F(1, 0, 0));

    // Aplicar rotación: primero yaw, luego pitch (se aplica en orden inverso al multiplicar)
    tr->rotation = yaw * pitch;
}

void EditorSystem::LanzarMotorRelease()
{
    _mngr->changeMode();
}

#endif