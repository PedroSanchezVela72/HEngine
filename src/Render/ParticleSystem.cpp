
#include "ParticleSystem.h"
#include <OgreParticleEmitter.h>
#include <OgreParticleAffector.h>
#include <OgreRoot.h>
#include <Manager.h>
#include <RenderParticle.h>
#include <Transform.h>
#include <RenderSystem.h>
#include <OgreParticleSystem.h>
#include <LoadLua.h>
#include <Debuglog.h>

using namespace H;

ParticleSystem::ParticleSystem()
{

}

ParticleSystem::~ParticleSystem()
{
   
}

void ParticleSystem::initSystem()
{
}

void ParticleSystem::update(double deltaTime)
{
    std::list<Entity*> particleHdlr = _mngr->getHandler(_hdlr_PARTICLES);

    for (Entity* ent : particleHdlr) {

        Transform* tr = _mngr->getComponent<Transform>(ent);
        RenderParticle* rp = _mngr->getComponent<RenderParticle>(ent);

        if (_mngr->hasComponent<RenderParticle>(ent)) {

            Vector3F rpRotated = tr->rotation * rp->position;
            rp->sceneNode->setPosition(Ogre::Vector3(tr->position.x + rpRotated.x, tr->position.y + rpRotated.y, tr->position.z + rpRotated.z));

            Ogre::ParticleSystem* particleSystem = rp->particleSystem;           
            particleSystem->setMaterialName(rp->material);
            particleSystem->setDefaultWidth(rp->particleWidth);
            particleSystem->setDefaultHeight(rp->particleHeight);
            particleSystem->setParticleQuota(std::min(rp->quota, H_PART_SYS_MAX_QUOTA_DEF));
            particleSystem->setSortingEnabled(rp->sorted);
            particleSystem->setCullIndividually(rp->cullEach);

            Ogre::ParticleEmitter* emitter = particleSystem->getEmitter(H_PART_SYS_DEF_EMMITER);
            emitter->setDirection(Ogre::Vector3(rp->direction.x, rp->direction.y, rp->direction.z));
            emitter->setAngle(Ogre::Degree(rp->angle));
            emitter->setEmissionRate(rp->emissionRate);
            emitter->setTimeToLive(rp->timeToLive);
            emitter->setParticleVelocity(rp->minVelocity, rp->maxVelocity);
        }
    }
}

void ParticleSystem::receive(const Message* m)
{
    switch (m->id)
    {
    case _m_INIT_ENTITY:
        addEntity(m->entity.entityPtr, m->entity.entityName);
        break;
    case _m_DELETE_ENTITY:
        deleteParticleSystem(m->entity.entityPtr);
    default:
        break;
    }
}

void ParticleSystem::addEntity(Entity* entity, std::string name)
{
    Ogre::SceneManager* sceneManager = _mngr->getSystem<RenderSystem>()->getSceneManager();
    
    if (sceneManager == nullptr) {
        std::string error = "No se encontro el sceneManager";
        DebugLog::instance()->throwLog(error);
        return;
    }

    LoadLua* lua = LoadLua::instance();

    if (lua->hasComponent(name, "RenderParticle")) {

        std::string mat = "";
            mat = lua->loadStrValue(name, { "RenderParticle", "material" });

        bool addImageAffector = false;
        if (lua->hasProperty(name, { "RenderParticle", "addImageAffector" }))
            addImageAffector = lua->loadBoolValue(name, { "RenderParticle", "addImageAffector" });

        std::string image = "";
        if (addImageAffector) {
           image = lua->loadStrValue(name, { "RenderParticle", "image" });
        }
        
        int width = H_PART_DEF_WIDTH;
        if (lua->hasProperty(name, { "RenderParticle", "width" }))
            width = lua->loadIntValue(name, { "RenderParticle", "width" });

        int height = H_PART_DEF_HEIGHT;
        if (lua->hasProperty(name, { "RenderParticle", "height" }))
            height = lua->loadIntValue(name, { "RenderParticle", "height" });

        int quota = H_PART_DEF_QUOTA;
        if (lua->hasProperty(name, { "RenderParticle", "quota" }))
            quota = lua->loadIntValue(name, { "RenderParticle", "quota" });

        bool cullEach = false;
        if (lua->hasProperty(name, { "RenderParticle", "cullEach" }))
            cullEach = lua->loadBoolValue(name, { "RenderParticle", "cullEach" });

        bool sorted = false;
        if (lua->hasProperty(name, { "RenderParticle", "sorted" }))
            sorted = lua->loadBoolValue(name, { "RenderParticle", "sorted" });

        Vector3F direction = Vector3F::right();
        if (lua->hasProperty(name, { "RenderParticle", "direction"}))
        direction = {
            lua->loadFloatValue(name, {"RenderParticle", "direction", "x"}),
            lua->loadFloatValue(name, {"RenderParticle", "direction", "y"}),
            lua->loadFloatValue(name, {"RenderParticle", "direction", "z"})
        };

        Vector3F position = Vector3F::zero();
        if (lua->hasProperty(name, { "RenderParticle", "position" }))
            position = {
                lua->loadFloatValue(name, {"RenderParticle", "position", "x"}),
                lua->loadFloatValue(name, {"RenderParticle", "position", "y"}),
                lua->loadFloatValue(name, {"RenderParticle", "position", "z"})
        };

        float angle = 360.f;
        if (lua->hasProperty(name, { "RenderParticle", "angle" }))
            angle = lua->loadFloatValue(name, { "RenderParticle", "angle" });

        float emmisionRate = H_PART_DEF_RATE;
        if (lua->hasProperty(name, { "RenderParticle", "emmisionRate" }))
            emmisionRate = lua->loadFloatValue(name, { "RenderParticle", "emmisionRate" });

        float timeToLive = H_PART_DEF_TTL;
        if (lua->hasProperty(name, { "RenderParticle", "timeToLive" }))
            timeToLive = lua->loadFloatValue(name, { "RenderParticle", "timeToLive" });

        float minVelocity = H_PART_DEF_MIN_VEL;
        if (lua->hasProperty(name, { "RenderParticle", "minVelocity" }))
            minVelocity = lua->loadFloatValue(name, { "RenderParticle", "minVelocity" });

        float maxVelocity = H_PART_DEF_MAX_VEL;
        if (lua->hasProperty(name, { "RenderParticle", "maxVelocity" }))
            maxVelocity = lua->loadFloatValue(name, { "RenderParticle", "maxVelocity" });

        float rotationRangeStart = 0;
        if (lua->hasProperty(name, { "RenderParticle", "rotationRangeStart" }))
            rotationRangeStart = lua->loadFloatValue(name, { "RenderParticle", "rotationRangeStart" });

        float rotationRangeEnd = 0;
        if (lua->hasProperty(name, { "RenderParticle", "rotationRangeEnd" }))
            rotationRangeEnd = lua->loadFloatValue(name, { "RenderParticle", "rotationRangeEnd" });

        float rotationSpeedRangeStart = 0;
        if (lua->hasProperty(name, { "RenderParticle", "rotationSpeedRangeStart" }))
            rotationSpeedRangeStart = lua->loadFloatValue(name, { "RenderParticle", "rotationSpeedRangeStart" });

        float rotationSpeedRangeEnd = 0;
        if (lua->hasProperty(name, { "RenderParticle", "rotationSpeedRangeEnd" }))
            rotationSpeedRangeEnd = lua->loadFloatValue(name, { "RenderParticle", "rotationSpeedRangeEnd" });

        float scaleRate = 1;
        if (lua->hasProperty(name, { "RenderParticle", "scaleRate" }))
            scaleRate = lua->loadFloatValue(name, { "RenderParticle", "scaleRate" });

        RenderParticle* rSys = _mngr->addComponent<RenderParticle>(entity, mat, width, height, quota, cullEach, sorted, direction, position,
        angle, emmisionRate, timeToLive, minVelocity, maxVelocity, addImageAffector, image, rotationRangeStart, rotationRangeEnd, 
        rotationSpeedRangeStart, rotationSpeedRangeEnd, scaleRate);

        _mngr->setHandler(_hdlr_PARTICLES, entity);

        rSys->particleSystem = sceneManager->createParticleSystem();

        rSys->particleSystem->addEmitter("Point");
        rSys->particleSystem->setMaterialName(rSys->material);
        rSys->particleSystem->setDefaultWidth(rSys->particleWidth);
        rSys->particleSystem->setDefaultHeight(rSys->particleHeight);
        rSys->particleSystem->setParticleQuota(std::min(rSys->quota, H_PART_SYS_MAX_QUOTA_DEF)); 
        rSys->particleSystem->setSortingEnabled(rSys->sorted);
        rSys->particleSystem->setCullIndividually(rSys->cullEach);

        Ogre::ParticleEmitter* emitter = rSys->particleSystem->getEmitter(H_PART_SYS_DEF_EMMITER);
        emitter->setDirection(Ogre::Vector3(rSys->direction.x, rSys->direction.y, rSys->direction.z));
        emitter->setAngle(Ogre::Degree(rSys->angle));
        emitter->setEmissionRate(rSys->emissionRate);
        emitter->setTimeToLive(rSys->timeToLive);
        emitter->setParticleVelocity(rSys->minVelocity, rSys->maxVelocity);

        Ogre::ParticleAffector* rotatorAffector = rSys->particleSystem->addAffector("Rotator");
        rotatorAffector->setParameter("rotation_range_start", std::to_string(rSys->rotationRangeStart));
        rotatorAffector->setParameter("rotation_range_end", std::to_string(rSys->rotationRangeEnd));
        rotatorAffector->setParameter("rotation_speed_range_start", std::to_string(rSys->rotationSpeedRangeStart));
        rotatorAffector->setParameter("rotation_speed_range_end", std::to_string(rSys->rotationSpeedRangeEnd));
        
        Ogre::ParticleAffector* scalerAffector = rSys->particleSystem->addAffector("Scaler");
        scalerAffector->setParameter("rate", std::to_string(rSys->rate));

        if (addImageAffector) {
            rSys->particleSystem->addAffector("ColourImage");
            rSys->particleSystem->getAffector(COLOUR_IMAGE)->setParameter("image", rSys->image);
        }

        Transform* tr = _mngr->getComponent<Transform>(entity);

        rSys->sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        rSys->sceneNode->attachObject(rSys->particleSystem);  
        rSys->sceneNode->setPosition(Ogre::Vector3(tr->position.x + rSys->position.x, tr->position.y + rSys->position.y, tr->position.z + rSys->position.z));
    }
}

void ParticleSystem::deleteParticleSystem(Entity* e)
{
    Ogre::SceneManager* sceneManager = _mngr->getSystem<RenderSystem>()->getSceneManager();

    if (sceneManager == nullptr) {
        std::string error = "No se encontro el sceneManager";
        DebugLog::instance()->throwLog(error);
        return;
    }

    if (!_mngr->hasComponent<RenderParticle>(e)) {
        return;
    }

    _mngr->deleteHandler(_hdlr_PARTICLES, e);
    
    RenderParticle* rSys = _mngr->getComponent<RenderParticle>(e);
    
    
    sceneManager->destroyParticleSystem(rSys->particleSystem);
    sceneManager->destroySceneNode(rSys->sceneNode);

}
