
#include "AnimationSystem.h"
#include <OgreRoot.h>
#include <OgreAnimation.h>
#include "OgreEntity.h"
#include "OgreSkeletonInstance.h"
#include "OgreKeyFrame.h"
#include <Manager.h>
#include <Transform.h>
#include <RenderSystem.h>
#include <AnimationComponent.h>
#include <RenderMesh.h>
#include <LoadLua.h>
#include <list>
#include <Debuglog.h>

using namespace H;

void AnimationSystem::changeAnimation(Entity* e, std::string name, bool loop, float duration) {
    Ogre::Entity* aux = _mngr->getComponent<RenderMesh>(e)->ogreEntity;
    AnimationComponent* aC = _mngr->getComponent<AnimationComponent>(e);
    if (aux->hasAnimationState(name) && aC) {
        // Guardar variables en el componente por si se preguntan o para guardado?
        aC->currentAnimation = name;
        aC->duration = duration;
        aC->loop = loop;

        // Cambio de variables en el mapa de entidades (Se necesita desactivar y activar la animacion, ogre funciona asi)
        aC->animationState->setEnabled(false);
        aC->animationState = aux->getAnimationState(aC->currentAnimation);
        aC->animationState->setEnabled(true);
        aC->animationState->setLoop(aC->loop);
        aC->animationState->setTimePosition(0);
    }
    else if(_sceneMgr->hasAnimationState(name) && aC){
        aC->currentAnimation = name;
        aC->duration = duration;
        aC->loop = loop;

        aC->animationState->setEnabled(false);
        aC->animationState = _sceneMgr->getAnimationState(name);
        aC->animationState->setEnabled(true);
        aC->animationState->setLoop(aC->loop);
        aC->animationState->setLoop(aC->loop);
        aC->animationState->setTimePosition(0);
    }
    else {
        std::string error = "No se ha encontrado la animacion";
        DebugLog::instance()->throwLog(error);
    }
}

void AnimationSystem::initSystem()
{
    addAnimationSystem();
}

void AnimationSystem::update(double deltaTime) {
    std::list<Entity*> animatedHdlr = _mngr->getHandler(_hdlr_ANIMATORS);

    for (Entity* ent : animatedHdlr) {
        if (_mngr->hasComponent<AnimationComponent>(ent) && _mngr->hasComponent<RenderMesh>(ent)) {
            AnimationComponent* an = _mngr->getComponent<AnimationComponent>(ent);
            RenderMesh* aux = _mngr->getComponent<RenderMesh>(ent);
            if (aux->ogreEntity->hasAnimationState(an->currentAnimation) && aux->ogreEntity->getAnimationState(an->currentAnimation) != NULL) {
                _mngr->getComponent<AnimationComponent>(ent)->animationState->addTime(deltaTime);
            }
        }
    }
}

void AnimationSystem::receive(const Message* m) {
    switch (m->id)
    {
    case _m_INIT_ENTITY:
        addEntity(m->entity.entityPtr, m->entity.entityName);
        break;
    case _m_CHANGE_ANIMATION:
        if(m->change_animation.animation != _mngr->getComponent<AnimationComponent>(m->change_animation.ent)->currentAnimation)
            changeAnimation(m->change_animation.ent, m->change_animation.animation, m->change_animation.loop, m->change_animation.duration);
        break;
    case _m_DELETE_ENTITY:
        if (_mngr->hasComponent<AnimationComponent>(m->entity.entityPtr)) {
            H::AnimationComponent* aC = _mngr->getComponent<AnimationComponent>(m->entity.entityPtr);
            if (aC->animation != nullptr) {
                if (_sceneMgr->hasAnimationState(aC->animationState->getAnimationName()))
                    _sceneMgr->destroyAnimationState(aC->animationState->getAnimationName());
                if (_sceneMgr->hasAnimation(aC->animation->getName()))
                    _sceneMgr->destroyAnimation(aC->animation->getName());
            }
            
            _mngr->removeComponent<AnimationComponent>(m->entity.entityPtr);
        }
            
        break;
    }
    
}

void AnimationSystem::addAnimationSystem()
{
    _sceneMgr = _mngr->getSystem<RenderSystem>()->getSceneManager();
}

void AnimationSystem::addEntity(Entity* e, std::string name) {
    if (_mngr->hasComponent<RenderMesh>(e)) {
        Ogre::Entity* aux = _mngr->getComponent<RenderMesh>(e)->ogreEntity;
        if (aux->hasSkeleton()) {
            LoadLua* lua = LoadLua::instance();
            if (lua->hasComponent(name, "AnimationComponent")) {
                float duration = 0.0f;
                bool loop = true;
                std::string currentAnimation;

                duration = lua->loadFloatValue(name, { "AnimationComponent", "duration" });
                loop = lua->loadBoolValue(name, { "AnimationComponent", "loop" });
                currentAnimation = lua->loadStrValue(name, { "AnimationComponent", "currentAnimation" });

                AnimationComponent* animComponent = _mngr->addComponent<AnimationComponent>(e);

                // -----------------------------------------------------------
                if (lua->hasComponent(name, "Animations")) {
                    Ogre::Entity* aux = _mngr->getComponent<RenderMesh>(e)->ogreEntity;
                    int numAnimations = lua->loadIntValue(name, { "Animations", "numAnimations" });

                    for (int i = 0; i < numAnimations; i++)
                    {
                        std::string nameAnim = lua->loadStrValue(name, { "Animations", "a" + std::to_string(i),"name" });
                        _animation = _sceneMgr->createAnimation(lua->loadStrValue(name, { "Animations", "a" + std::to_string(i), "name" }), duration);

                        _animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
                        Ogre::NodeAnimationTrack* track = _animation->createNodeTrack(0);
                        track->setAssociatedNode(_mngr->getComponent<RenderMesh>(e)->sceneNode);
                        Ogre::Vector3 _keyframePos = { 0, 0, 0 };

                        Ogre::TransformKeyFrame* kf;
                        float currentTime = 0;
                        int numKeyframes = lua->loadIntValue(name, { "Animations", "a" + std::to_string(i), "numKeyframes" });
                        // Keyframe 0
                        kf = track->createNodeKeyFrame(0);
                        for (int j = 0; j < numKeyframes; j++)
                        {
                            currentTime += lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "dur" });
                            kf = track->createNodeKeyFrame(currentTime);
                            Ogre::Vector3 mov = { 0,0,0 };
                            mov.x = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "position", "x" });
                            mov.y = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "position", "y" });
                            mov.z = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "position", "z" });
                            _keyframePos += mov;
                            kf->setTranslate(_keyframePos);
                            Ogre::Vector3 rot = { 0,0,0 };
                            rot.x = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "rotation", "x" });
                            rot.y = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "rotation", "y" });
                            rot.z = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "rotation", "z" });
                            float grados = lua->loadFloatValue(name, { "Animations", "a" + std::to_string(i), "keyframes","k" + std::to_string(j), "rotation", "w" });
                            kf->setRotation(Ogre::Quaternion(Ogre::Degree(grados), rot));
                        }
                        animComponent->animationState = _sceneMgr->createAnimationState(nameAnim);
                    }

                }
                // -----------------------------------------------------------

                animComponent->loop = loop;
                animComponent->duration = duration;
                animComponent->currentAnimation = currentAnimation;


#ifdef _DEBUG

                if (aux->hasSkeleton()) {
                    Ogre::SkeletonInstance* skeleton = aux->getSkeleton();
                    std::string error = "Animaciones disponibles para la entidad:";
                    DebugLog::instance()->throwLog(error);
                    for (unsigned short i = 0; i < skeleton->getNumAnimations(); i++) {
                        std::cout << " - " << skeleton->getAnimation(i)->getName() << std::endl;
                    }
                }
#endif // DEBUG
                _mngr->setHandler(_hdlr_ANIMATORS, e);

                if (aux->hasAnimationState(animComponent->currentAnimation) && aux->getAnimationState(animComponent->currentAnimation) != NULL) {
                    animComponent->animationState = aux->getAnimationState(animComponent->currentAnimation);
                    animComponent->animationState->setEnabled(true);
                    animComponent->animationState->setLoop(animComponent->loop);
                }
                else {
                    std::string error = "No se ha encontrado la animacion";
                    DebugLog::instance()->throwLog(error);
                }

            }
            else {
                std::string error = "La entidad no tiene el componento AnimationComponent";
                DebugLog::instance()->throwLog(error);
            }
        }
    }
    else {
        std::string error = "La malla no tiene Skeleton asociado";
        DebugLog::instance()->throwLog(error);
    }

}


