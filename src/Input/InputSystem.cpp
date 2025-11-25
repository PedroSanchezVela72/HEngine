
#include "InputSystem.h"
#include <Manager.h>
#include <Debuglog.h>

#include "imgui.h"


#include "SDL.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

#include <iostream>

namespace H {
    #ifndef DEBUG_INPUT_H
    #define DEBUG_INPUT_H

    #ifdef _DEBUG

        #if H_ACTIVE_KEY == 1
            #define DEBUG_KEY(x) std::cout << "Tecla presionada: " << SDL_GetScancodeName(x) << std::endl;
        #else
            #define DEBUG_KEY(x)
        #endif

        #if H_ACTIVE_MOUSE_BUTTON == 1
            #define DEBUG_MOUSE_BUTTON(x) std::cout << "Botón del ratón presionado: " << static_cast<int>(x) << std::endl;
        #else
            #define DEBUG_MOUSE_BUTTON(x)
        #endif

        #if H_ACTIVE_MOUSE_MOTION == 1
            #define DEBUG_MOUSE_MOTION(x, y) std::cout << "El ratón está en la posición: " << x << ", " << y << std::endl;
        #else
            #define DEBUG_MOUSE_MOTION(x, y)
        #endif

        #if H_ACTIVE_MOUSE_WHEEL == 1
            #define DEBUG_MOUSE_WHEEL(y) std::cout << "La rueda esta girando en: " << y << std::endl;
        #else
            #define DEBUG_MOUSE_WHEEL(y)
        #endif

    #else //_DEBUG
        #define DEBUG_KEY(x) 
        #define DEBUG_MOUSE_BUTTON(x) 
        #define DEBUG_MOUSE_MOTION(x, y) 
        #define DEBUG_MOUSE_WHEEL(y)
    #endif // _DEBUG

    #endif // DEBUG_INPUT_H
}

using namespace H;

InputSystem::InputSystem(): _mousePosition{ 0,0 } {
    _mouseWheel = 0;
    _mousePosition = { 0, 0 };
}

InputSystem::~InputSystem() {
    for (auto controller : _controllers) {
        if (controller) {
            SDL_GameControllerClose(static_cast<SDL_GameController*>(controller));
        }
    }
    _controllers.clear();
    _keyStates.clear();
    _mouseButtonStates.clear();
    _gamepadButtonStates.clear();
    _gamepadAxisStates.clear();
}

void InputSystem::initSystem() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) {
        std::string error = "Error SDL_Init: ";
        error += SDL_GetError();
        DebugLog::instance()->throwLog(error);
    }

    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i) {
        if (SDL_IsGameController(i)) {
            _controllers.push_back(SDL_GameControllerOpen(i));
        }
    }
}

void InputSystem::update(double deltaTime) {
    SDL_Event e;

   

    while (SDL_PollEvent(&e) != 0) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (ImGui::GetIO().WantCaptureKeyboard &&
            (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)) {
            continue;
        }
        if (ImGui::GetIO().WantCaptureMouse &&
            (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEWHEEL)) {
            continue;
        }

        switch (e.type) {
        case SDL_KEYDOWN:
            if (!_keyStates[e.key.keysym.scancode]) {
                handleKeyDown(e.key.keysym.scancode);
            }
            break;
        case SDL_KEYUP:
            handleKeyUp(e.key.keysym.scancode);
            break;
        case SDL_MOUSEMOTION:
            handleMouseMotion(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!_mouseButtonStates[e.button.button]) {
                handleMouseDown(e.button.button);
            }
            _mouseButtonStates[e.button.button] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            handleMouseUp(e.button.button);
            _mouseButtonStates[e.button.button] = false;
            break;
        case SDL_MOUSEWHEEL:
            handleMouseWheel(e.wheel.y);
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            if (!_gamepadButtonStates[e.cbutton.which][e.cbutton.button]) {
                handleGamepadButtonDown(e.cbutton.which, e.cbutton.button);
            }
            _gamepadButtonStates[e.cbutton.which][e.cbutton.button] = true;
            break;
        case SDL_CONTROLLERBUTTONUP:
            handleGamepadButtonUp(e.cbutton.which, e.cbutton.button);
            _gamepadButtonStates[e.cbutton.which][e.cbutton.button] = false;
            break;
        case SDL_CONTROLLERAXISMOTION:
            handleGamepadAxisMotion(e.caxis.which, e.caxis.axis, e.caxis.value);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (e.cdevice.which < _controllers.size() && _controllers[e.cdevice.which]) {
                SDL_GameControllerClose(static_cast<SDL_GameController*>(_controllers[e.cdevice.which]));
                _controllers[e.cdevice.which] = nullptr;
            }
            break;
        case SDL_CONTROLLERDEVICEADDED:
            if (e.cdevice.which >= 0 && e.cdevice.which < SDL_NumJoysticks()) {
                if (SDL_IsGameController(e.cdevice.which)) {
                    SDL_GameController* newController = SDL_GameControllerOpen(e.cdevice.which);
                    if (newController) {
                        _controllers.push_back(newController);
                    }
                }
            }
            break;
        case SDL_QUIT:
#ifdef _DEBUG
            if (_mngr->isRunningGame())
                _mngr->changeMode();
            else
#endif
                _mngr->exit();
            break;
        }
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (keyboardState[i]) {
            handleKeyHeld(static_cast<SDL_Scancode>(i));
        }
    }

    Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
    for (int i = SDL_BUTTON_LEFT; i <= SDL_BUTTON_X2; ++i) {
        if (mouseState & SDL_BUTTON(i)) {
            handleMouseHeld(i);
        }
    }

    for (int i = 0; i < _controllers.size(); ++i) {
        if (_controllers[i]) {
            for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j) {
                if (_gamepadButtonStates[i][j]) {
                    handleGamepadButtonHeld(i, j);
                }
            }
        }
    }
}

void InputSystem::receive(const Message* m) {

}

bool InputSystem::isKeyPressed(int key) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    return state[key] != 0;
}

bool InputSystem::isMouseButtonPressed(uint8_t button) {

    return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
}

HPoint InputSystem::getMousePosition() {
    return _mousePosition;
}

bool InputSystem::isGamepadButtonPressed(int gamepadIndex, uint8_t button) {
    return _gamepadButtonStates[gamepadIndex][button];
}

Sint16 InputSystem::getGamepadAxis(int gamepadIndex, uint8_t axis) {
    return _gamepadAxisStates[gamepadIndex][axis];
}

void InputSystem::handleKeyDown(int key) {
    
    _keyStates[key] = true;
    Message* m = new Message;
    m->id = _m_KEY_DOWN;
    m->key.key = sdlKeycodeToKeyCode(key);
    _mngr->send(m);

    DEBUG_KEY(static_cast<SDL_Scancode>(key));
    
}

void InputSystem::handleKeyUp(int key) {
    _keyStates[key] = false;
    Message* m = new Message;
    m->id = _m_KEY_UP;
    m->key.key = sdlKeycodeToKeyCode(key);
    _mngr->send(m);
}

void InputSystem::handleKeyHeld(int key) {
    Message* m = new Message;
    m->id = _m_KEY_HELD;
    m->key.key = sdlKeycodeToKeyCode(key);
    _mngr->send(m);
}

void InputSystem::handleMouseMotion(int x, int y, float xrel, float yrel) {
    _mousePosition.x = x;
    _mousePosition.y = y;

    Message* m = new Message;
    m->id = _m_MOUSE_MOTION;
    m->mouse_motion.x = x;
    m->mouse_motion.y = y;
    m->mouse_motion.xrel = xrel;
    m->mouse_motion.yrel = yrel;
    _mngr->send(m);
}

void InputSystem::handleMouseWheel(int32_t y)
{
    _mouseWheel = y;

    Message* m = new Message;
    m->id = _m_MOUSE_WHEEL;
    m->mouse_wheel.dir = y;
    _mngr->send(m);
}

void InputSystem::handleMouseDown(uint8_t button) {
    _mouseButtonStates[button] = true;

    Message* m = new Message;
    m->id = _m_MOUSE_DOWN;
    m->mouse.button = sdlMouseButtonToMouseButton(button);
    _mngr->send(m);

    DEBUG_MOUSE_BUTTON(button);
}

void InputSystem::handleMouseUp(uint8_t button) {
    _mouseButtonStates[button] = false;
    Message* m = new Message;
    m->id = _m_MOUSE_UP;
    m->mouse.button = sdlMouseButtonToMouseButton(button);
    _mngr->send(m);
}

void InputSystem::handleMouseHeld(uint8_t button) {
    Message* m = new Message;
    m->id = _m_MOUSE_HELD;
    m->mouse.button = sdlMouseButtonToMouseButton(button);
    _mngr->send(m);
}

void InputSystem::handleGamepadButtonDown(int gamepadIndex, uint8_t button) {
    _gamepadButtonStates[gamepadIndex][button] = true;

    Message* m = new Message;
    m->id = _m_GAMEPAD_DOWN;
    m->gamepad.gamepad = gamepadIndex;
    m->gamepad.button = sdlGamepadButtonToGamepadButton(button);
    _mngr->send(m);
}

void InputSystem::handleGamepadButtonUp(int gamepadIndex, uint8_t button) {
    _gamepadButtonStates[gamepadIndex][button] = false;

    Message* m = new Message;
    m->id = _m_GAMEPAD_UP;
    m->gamepad.gamepad = gamepadIndex;
    m->gamepad.button = sdlGamepadButtonToGamepadButton(button);
    _mngr->send(m);
}

void InputSystem::handleGamepadButtonHeld(int gamepadIndex, uint8_t button) {
    Message* m = new Message;
    m->id = _m_GAMEPAD_HELD;
    m->gamepad.gamepad = gamepadIndex;
    m->gamepad.button = sdlGamepadButtonToGamepadButton(button);
    _mngr->send(m);
}

void InputSystem::handleGamepadAxisMotion(int gamepadIndex, uint8_t axis, int16_t value) {
    _gamepadAxisStates[gamepadIndex][axis] = value;

    Message* m = new Message;
    m->id = _m_GAMEPAD_MOTION;
    m->gamepad_motion.gamepad = gamepadIndex;
    m->gamepad_motion.axis = sdlGamepadAxisToGamepadAxis(axis);
    m->gamepad_motion.value = value;
    _mngr->send(m);
}

H::KeyCode InputSystem::sdlKeycodeToKeyCode(int key) {
    SDL_Scancode scancode = static_cast<SDL_Scancode>(key);
    switch (scancode) {
        case SDL_SCANCODE_A:      return H::HENGINE_A;
        case SDL_SCANCODE_B:      return H::HENGINE_B;
        case SDL_SCANCODE_C:      return H::HENGINE_C;
        case SDL_SCANCODE_D:      return H::HENGINE_D;
        case SDL_SCANCODE_E:      return H::HENGINE_E;
        case SDL_SCANCODE_F:      return H::HENGINE_F;
        case SDL_SCANCODE_G:      return H::HENGINE_G;
        case SDL_SCANCODE_H:      return H::HENGINE_H;
        case SDL_SCANCODE_I:      return H::HENGINE_I;
        case SDL_SCANCODE_J:      return H::HENGINE_J;
        case SDL_SCANCODE_K:      return H::HENGINE_K;
        case SDL_SCANCODE_L:      return H::HENGINE_L;
        case SDL_SCANCODE_M:      return H::HENGINE_M;
        case SDL_SCANCODE_N:      return H::HENGINE_N;
        case SDL_SCANCODE_O:      return H::HENGINE_O;
        case SDL_SCANCODE_P:      return H::HENGINE_P;
        case SDL_SCANCODE_Q:      return H::HENGINE_Q;
        case SDL_SCANCODE_R:      return H::HENGINE_R;
        case SDL_SCANCODE_S:      return H::HENGINE_S;
        case SDL_SCANCODE_T:      return H::HENGINE_T;
        case SDL_SCANCODE_U:      return H::HENGINE_U;
        case SDL_SCANCODE_V:      return H::HENGINE_V;
        case SDL_SCANCODE_W:      return H::HENGINE_W;
        case SDL_SCANCODE_X:      return H::HENGINE_X;
        case SDL_SCANCODE_Y:      return H::HENGINE_Y;
        case SDL_SCANCODE_Z:      return H::HENGINE_Z;
        case SDL_SCANCODE_DOWN:   return H::HENGINE_DOWN;
        case SDL_SCANCODE_UP:     return H::HENGINE_UP;
        case SDL_SCANCODE_RIGHT:  return H::HENGINE_RIGHT;
        case SDL_SCANCODE_LEFT:   return H::HENGINE_LEFT;
        case SDL_SCANCODE_SPACE:  return H::HENGINE_SPACE;
        case SDL_SCANCODE_RETURN: return H::HENGINE_RETURN;
        case SDL_SCANCODE_LSHIFT: return H::HENGINE_LSHIFT;
        case SDL_SCANCODE_LCTRL:  return H::HENGINE_LCTRL;
        case SDL_SCANCODE_TAB:    return H::HENGINE_TAB;
        case SDL_SCANCODE_ESCAPE: return H::HENGINE_ESCAPE;

        case SDL_SCANCODE_0:      return H::HENGINE_0;
        case SDL_SCANCODE_1:      return H::HENGINE_1;
        case SDL_SCANCODE_2:      return H::HENGINE_2;
        case SDL_SCANCODE_3:      return H::HENGINE_3;
        case SDL_SCANCODE_4:      return H::HENGINE_4;
        case SDL_SCANCODE_5:      return H::HENGINE_5;
        case SDL_SCANCODE_6:      return H::HENGINE_6;
        case SDL_SCANCODE_7:      return H::HENGINE_7;
        case SDL_SCANCODE_8:      return H::HENGINE_8;
        case SDL_SCANCODE_9:      return H::HENGINE_9;

        default:
            return static_cast<H::KeyCode>(0);
    }
}

H::MouseButton H::InputSystem::sdlMouseButtonToMouseButton(uint8_t sdlButton) {
    switch (sdlButton) {
        case SDL_BUTTON_LEFT:   return H::HENGINE_LEFT_BUTTON;
        case SDL_BUTTON_RIGHT:  return H::HENGINE_RIGHT_BUTTON;
        case SDL_BUTTON_MIDDLE: return H::HENGINE_MIDDLE_BUTTON;
        case SDL_BUTTON_X1:     return H::HENGINE_X1_BUTTON;
        case SDL_BUTTON_X2:     return H::HENGINE_X2_BUTTON;

        default:
            return static_cast<H::MouseButton>(0);
        }
}

H::GamepadButton InputSystem::sdlGamepadButtonToGamepadButton(uint8_t sdlButton) {
    switch (sdlButton) {
        case SDL_CONTROLLER_BUTTON_A:             return H::HENGINE_GAMEPAD_A;
        case SDL_CONTROLLER_BUTTON_B:             return H::HENGINE_GAMEPAD_B;
        case SDL_CONTROLLER_BUTTON_X:             return H::HENGINE_GAMEPAD_X;
        case SDL_CONTROLLER_BUTTON_Y:             return H::HENGINE_GAMEPAD_Y;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return H::HENGINE_GAMEPAD_LB;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return H::HENGINE_GAMEPAD_RB;
        case SDL_CONTROLLER_BUTTON_START:         return H::HENGINE_GAMEPAD_START;
        case SDL_CONTROLLER_BUTTON_BACK:          return H::HENGINE_GAMEPAD_BACK;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:     return H::HENGINE_GAMEPAD_LEFTSTICK;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    return H::HENGINE_GAMEPAD_RIGHTSTICK;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:       return H::HENGINE_GAMEPAD_DPAD_UP;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     return H::HENGINE_GAMEPAD_DPAD_DOWN;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     return H::HENGINE_GAMEPAD_DPAD_LEFT;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    return H::HENGINE_GAMEPAD_DPAD_RIGHT;
        default:
            return static_cast<H::GamepadButton>(0);
    }
}

H::GamepadAxis InputSystem::sdlGamepadAxisToGamepadAxis(uint8_t sdlAxis) {
    switch (sdlAxis) {
        case SDL_CONTROLLER_AXIS_LEFTX:        return H::HENGINE_LEFT_X;
        case SDL_CONTROLLER_AXIS_LEFTY:        return H::HENGINE_LEFT_Y;
        case SDL_CONTROLLER_AXIS_RIGHTX:       return H::HENGINE_RIGHT_X;
        case SDL_CONTROLLER_AXIS_RIGHTY:       return H::HENGINE_RIGHT_Y;
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:  return H::HENGINE_LT;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return H::HENGINE_RT;
        default:
            return static_cast<H::GamepadAxis>(0);
        }
}

