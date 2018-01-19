#pragma once

#include "IGameState.h"
#include "MainState.h"
#include "../Game.h"

class IntroState : public IGameState {

public: 

    //Constructor
    IntroState (){ type = IGameState::INTRO; };

    //Destructor
    virtual ~IntroState() {}

    //Initialization
    virtual void init();

    //Updater
    virtual void update(float &accumulatedTime);

    //Drawer
    virtual void draw();

    //Shutdown
    virtual void close();

    //Static class getter
    static IntroState& getInstance() {
        static IntroState instance;
        return instance;
    };
private:

    //Update's loop time
    const float loopTime = 1.0f/30.0f;
};