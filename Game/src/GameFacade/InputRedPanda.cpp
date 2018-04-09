#include "InputRedPanda.h"

#include <nuklear/nuklear.h>
#include <nuklear/nuklear_sdl_gl3.h>

//Define macros
#define DetectKeyInput(TheKey,Event_Down,Event_Up) \
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_##TheKey) { \
            EventData eventData; \
            eventData.grade = -2; \
            Event event; \
            event.type = EventType::Event_Down; \
            event.data = eventData; \
            EventManager::getInstance().addEvent(event); \
    } \
    else if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_##TheKey){ \
            EventData eventData; \
            eventData.grade = -2; \
            Event event; \
            event.type = EventType::Event_Up; \
            event.data = eventData; \
            EventManager::getInstance().addEvent(event); \
    };


#define DetectButtonInput(TheKey,Event_Down,Event_Up,Mapping) \
    if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_##TheKey)) { \
            EventData eventData; \
            eventData.grade = -2; \
            Event event; \
            event.type = EventType::Event_Down; \
            event.data = eventData; \
            EventManager::getInstance().addEvent(event); \
            buttonMapping[Mapping] = true; \
    } \
    else if(buttonMapping[Mapping]){ \
            EventData eventData; \
            eventData.grade = -2; \
            Event event; \
            event.type = EventType::Event_Up; \
            event.data = eventData; \
            EventManager::getInstance().addEvent(event); \
            buttonMapping[Mapping] = false; \
    };



void InputRedPanda::openInput(uintptr_t dev) {
    device = reinterpret_cast<SDL_Window*>(dev);
    
    if(SDL_NumJoysticks() > 0)
        gamepad = SDL_GameControllerOpen(0);

}

struct nk_context *inputGUI; //:::> global variable

void InputRedPanda::updateInput() { 


    // Process events
    SDL_Event event;
    nk_input_begin(inputGUI);
    while( SDL_PollEvent( &event ) )
    {
        //Update GUI input
        nk_sdl_handle_event(&event);

        //Update input 
        DetectKeyInput(SPACE, Key_Jump_Down, Key_Jump_Up)
        DetectKeyInput(ESCAPE,Key_Back_Down,Key_Back_Up)
        DetectKeyInput(w,Key_Advance_Down,Key_Advance_Up)
        DetectKeyInput(s,Key_Brake_Down,Key_Brake_Up)
        DetectKeyInput(a,Key_TurnLeft_Down,Key_TurnLeft_Up)
        DetectKeyInput(d,Key_TurnRight_Down,Key_TurnRight_Up)
        DetectKeyInput(t,Key_Drift_Down,Key_Drift_Up)
        DetectKeyInput(q,Key_UseItem_Down,Key_UseItem_Up)
        DetectKeyInput(5,Key_Multiplayer_Down,Key_Multiplayer_Up)
        DetectKeyInput(6,Key_Singleplayer_Down,Key_Singleplayer_Up)
        DetectKeyInput(F6,Key_SlowControl_Down,Key_SlowControl_Up)
        DetectKeyInput(F7,Key_NormalControl_Down,Key_NormalControl_Up)
        DetectKeyInput(F8,Key_FastControl_Down,Key_FastControl_Up)
        DetectKeyInput(F9,Key_DebugAI_Down,Key_DebugAI_Up)
        DetectKeyInput(F10,Key_DebugBehaviour_Down,Key_DebugBehaviour_Up)
        DetectKeyInput(F11,Key_DebugCamera_Down,Key_DebugCamera_Up)

        DetectButtonInput(A, Key_Jump_Down, Key_Jump_Up, 0)
        DetectButtonInput(B, Key_Drift_Down, Key_Drift_Up, 1)
        DetectButtonInput(Y, Key_UseItem_Down, Key_UseItem_Up, 2)

        DetectAxisInput();

        //Exit game
        if (event.type == SDL_QUIT)
            EventManager::getInstance().addEvent(Event {EventType::Game_Close});
    }
    nk_input_end(inputGUI);

    if(gamepad == nullptr && SDL_NumJoysticks() > 0) {
        gamepad = SDL_GameControllerOpen(0);
    }
    if(SDL_NumJoysticks() == 0)
        gamepad = nullptr;
}

void InputRedPanda::closeInput() {

    SDL_GameControllerClose(gamepad);
       
} 

void InputRedPanda::setGUIContext(void* ctx) {
    inputGUI = (nk_context*)ctx;
}

void InputRedPanda::DetectAxisInput() {

    if(gamepad) {

        int StickX = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
        int TriggerL = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        int TriggerR = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

        if(StickX > 9000){
            EventData eventData;
            eventData.grade = StickX/32000;
            Event event;
            event.type = EventType::Key_TurnRight_Down;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[3] = true; 
        }
        else if(StickX > 0 && buttonMapping[3]){
            EventData eventData;
            eventData.grade = -2;
            Event event;
            event.type = EventType::Key_TurnRight_Up;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[3] = false; 
        }

        if(StickX < -9000){
            EventData eventData;
            eventData.grade = -StickX/32000;
            Event event;
            event.type = EventType::Key_TurnLeft_Down;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[4] = true; 
        }
        else if(StickX < 0 && buttonMapping[4]){
            EventData eventData;
            eventData.grade = -2;
            Event event;
            event.type = EventType::Key_TurnLeft_Up;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[4] = false; 
        }

        if(TriggerL > 12000){
            EventData eventData;
            eventData.grade = -2;
            Event event;
            event.type = EventType::Key_Brake_Down;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[5] = true; 
        }
        else if(buttonMapping[5]){
            EventData eventData;
            eventData.grade = -2;
            Event event;
            event.type = EventType::Key_Brake_Up;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[5] = false; 
        }

        if(TriggerR > 3000){
            EventData eventData;
            eventData.grade = TriggerR/32000;
            Event event;
            event.type = EventType::Key_Advance_Down;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[6] = true; 
        }
        else if(buttonMapping[6]){
            EventData eventData;
            eventData.grade = -2;
            Event event;
            event.type = EventType::Key_Advance_Up;
            event.data = eventData; 
            EventManager::getInstance().addEvent(event); 
            buttonMapping[6] = false; 
        }

    }

}