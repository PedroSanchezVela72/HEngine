#pragma once
#ifndef ECS_H
#define ECS_H

#include <vector>
#include <string>
#include "Vector3.h"
#include <unordered_map>
#include <map>
#include "InputKeys.h"
#include "HEngineExtensions.h"


namespace H {

	class Entity;
	class IManager;

	using uint8_t = unsigned char;

	enum property_type { BOOL, FLOAT, INT, STRING, TABLE };

	struct PropertyData {
		bool bValue;
		float fValue;
		int iValue;
		std::string sValue;
		std::vector<std::string> args;
		int profundidad = 1;
		property_type tipo;
	};

#ifndef COMPONENT_LIST_H
#define COMPONENT_LIST_H

#define COMPONENTS_LIST \
		X(_cmp_AUDIO_LISTENER) \
		X(_cmp_AUDIO_SOURCE) \
		X(_cmp_TRANSFORM) \
		X(_cmp_RENDERMESH) \
		X(_cmp_RIGIDBODY) \
		X(_cmp_COLLIDER) \
		X(_cmp_BOX_COLLIDER) \
		X(_cmp_SPHERE_COLLIDER) \
		X(_cmp_CAPSULE_COLLIDER) \
		X(_cmp_RENDER_PARTICLE) \
		X(_cmp_ANIMATION_COMPONENT) \
		X(_cmp_UI_ELEMENT) \
		X(_cmp_UI_CANVAS) \
		X(_cmp_UI_IMAGE) \
		X(_cmp_UI_BUTTON) \
		X(_cmp_UI_TEXT) \
		X(_cmp_UI_SLIDER) \
		X(_cmp_UI_INPUTFIELD) \
		X(_cmp_CAMERA) \
		X(_cmp_LIGHT)

#endif

#ifndef EXTENSION_COMPONENTS_LIST_H
#define EXTENSION_COMPONENTS_LIST_H

#define EXTENSION_COMPONENTS_LIST

#endif

	using cmpId_type = int;

	enum cmpId : cmpId_type {
#define X(name) name,
		COMPONENTS_LIST
		EXTENSION_COMPONENTS_LIST
#undef X
		_LAST_CMP_ID
	};

	constexpr cmpId_type maxComponentId = _LAST_CMP_ID;

#ifndef HANDLERS_LIST_H
#define HANDLERS_LIST_H

#define HANDLERS_LIST \
			X(_hdlr_ENTITIES) \
			X(_hdlr_PARTICLES) \
			X(_hdlr_ANIMATORS) \
			X(_hdlr_RENDER) \
			X(_hdlr_UI) \
			X(_hdlr_DEBUG_AXIS) \
			X(_hdlr_AUDIO)
#endif

#ifndef EXTENSION_HANDLERS_LIST_H
#define EXTENSION_HANDLERS_LIST_H

#define EXTENSION_HANDLERS_LIST

#endif

	using hdlrId_type = int;

	enum hdlrId : hdlrId_type {
#define X(name) name,
		HANDLERS_LIST
		EXTENSION_HANDLERS_LIST
#undef X
		_LAST_HDLR_ID
	};

	constexpr hdlrId_type maxHdlrId = _LAST_HDLR_ID;

#ifndef GROUPS_LIST_H
#define GROUPS_LIST_H

#define GROUPS_LIST \
				X(_grp_TILE) \
				X(_grp_ENTITIES) \
				X(_grp_DEBUG_AXIS) \
				X(_grp_PARENTENTS) \
				X(_grp_CAMERAS)
#endif

#ifndef EXTENSION_GROUPS_LIST_H
#define EXTENSION_GROUPS_LIST_H

#define EXTENSION_GROUPS_LIST

#endif

	using grpId_type = int;

	enum grpId : grpId_type {
#define X(name) name,
		GROUPS_LIST
		EXTENSION_GROUPS_LIST
#undef X
		_LAST_GRP_ID
	};

	constexpr grpId_type maxGroupId = _LAST_GRP_ID;

#ifndef SYSTEMS_LIST_H
#define SYSTEMS_LIST_H

#define SYSTEMS_LIST \
					X(_sys_EDITOR) \
					X(_sys_UIGAME) \
					X(_SYS_PARTICLES) \
					X(_sys_RENDER) \
					X(_sys_INPUT) \
					X(_sys_ANIM) \
					X(_sys_PHYSICS) \
					X(_sys_AUDIO)
#endif

#ifndef EXTENSION_SYSTEMS_LIST_H
#define EXTENSION_SYSTEMS_LIST_H

#define EXTENSION_SYSTEMS_LIST

#endif

	using sysId_type = uint8_t;

	enum sysId : sysId_type {
#define X(name) name,
		SYSTEMS_LIST
		EXTENSION_SYSTEMS_LIST
#undef X
		_LAST_SYS_ID
	};

	constexpr sysId_type maxSystemId = _LAST_SYS_ID;

#ifndef MESSAGES_LIST_H
#define MESSAGES_LIST_H

#define MESSAGES_LIST \
						X(_m_PLAY_SOUND) \
						X(_m_ADD_SOUND) \
						X(_m_SET_LISTENER) \
						X(_m_MOUSE_MOTION) \
						X(_m_MOUSE_WHEEL) \
						X(_m_MOUSE_DOWN) \
						X(_m_MOUSE_UP) \
						X(_m_MOUSE_HELD) \
						X(_m_KEY_DOWN) \
						X(_m_KEY_UP) \
						X(_m_KEY_HELD) \
						X(_m_CHANGE_MAINCAMERA) \
						X(_m_GAMEPAD_DOWN) \
						X(_m_GAMEPAD_UP) \
						X(_m_GAMEPAD_HELD) \
						X(_m_GAMEPAD_MOTION) \
						X(_m_INIT_ENTITY) \
						X(_m_INIT_IMGUIMANAGER) \
						X(_m_CHANGE_ANIMATION) \
						X(_m_DELETE_ENTITY) \
						X(_m_DELETE_CAMERA) \
						X(_m_ON_COLLISION_ENTER) \
						X(_m_ON_COLLISION_STAY) \
						X(_m_ON_COLLISION_EXIT) \
						X(_m_ON_TRIGGER_ENTER) \
						X(_m_ON_TRIGGER_STAY) \
						X(_m_ON_TRIGGER_EXIT) \
						X(_m_STOP_GAME) \
						X(_m_START_GAME) \
						X(_m_SHOW_UI_EDITOR)\
						X(_m_REMOVE_FONT_TEXTURE)\
						X(_m_ADD_FONT_TEXTURE)\
						X(_m_PAUSE_SOUND)\
						X(_m_STOP_SOUND)
#endif


#ifndef EXTENSION_MESSAGES_LIST_H
#define EXTENSION_MESSAGES_LIST_H

#define EXTENSION_MESSAGES_LIST

#endif

	using msgId_type = uint8_t;

	enum msgId : msgId_type {
#define X(name) name,
		MESSAGES_LIST
		EXTENSION_MESSAGES_LIST
#undef X
		_m_LAST_MSG_TYPE
	};

#ifndef MESSAGE_MACROS_H
#define MESSAGE_MACROS_H

#define BASE_MESSAGES \
    struct { int x, y; float xrel, yrel; } mouse_motion; \
    struct { int dir; } mouse_wheel; \
    struct { H::KeyCode key; } key; \
    struct { int gamepad; GamepadButton button; } gamepad; \
    struct { MouseButton button; } mouse; \
    struct { int gamepad; GamepadAxis axis; uint16_t value; } gamepad_motion; \
    struct { Entity* entityPtr; std::string entityName; } entity; \
    struct { Entity* entityA; Entity* entityB; } on_collision; \
    struct { Entity* triggerEnt; Entity* otherEnt; } on_trigger; \
    struct { std::string animation; Entity* ent; bool loop; float duration; } change_animation; \
    struct { bool show; } show_UI_editor; \
    struct { std::string soundName; std::string soundPath; } sound; \
	struct { int incScene = 0; } scene; \
	struct { float valorVolumeSlider; } sliderVolume;

#endif

#ifndef EXPORTS_MESSAGE_MACROS_H
#define EXPORTS_MESSAGE_MACROS_H

#define	EXPORTS_MESSAGES

#endif
	struct Message {
		msgId_type id;
		BASE_MESSAGES
		EXPORTS_MESSAGES
	};
}
#endif // !ECS_H