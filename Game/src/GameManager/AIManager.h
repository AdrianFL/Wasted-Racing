#pragma once

#include "../GameObject/AIComponent/AIDrivingComponent.h"
#include "../GameObject/AIComponent/AIBattleComponent.h"
#include "../GameObject/AIComponent/VSensorComponent.h"
#include "../GameObject/AIComponent/MSensorComponent.h"
#include "../GameObject/AIComponent/PathPlanningComponent.h"
#include "../GameObject/AIComponent/VObject.h"
#include "../GameObject/ItemComponent/IItemComponent.h"
#include "../GameObject/PhysicsComponent/MoveComponent.h"
#include "../GameObject/GameObject.h"
#include "../GameEvent/EventManager.h"
#include <memory>
#include <iostream>
#include <vector>

class AIManager{

public: 

    //Constructor
    AIManager() {}

    //Destructor
    ~AIManager() {}

    //Initialization
    void init();

    //Update
    void update();

    //Shutdown
    void close();

    std::vector<IComponent::Pointer>& getAIDrivingComponentList() {
        return objectsAI;
    }

    std::vector<IComponent::Pointer>& getAIBattleComponentList()
    {
        return battleAI;
    }

    //Static class getter
    static AIManager& getInstance();


    //Component creators
    IComponent::Pointer createAIDrivingComponent(GameObject& newGameObject);
    IComponent::Pointer createAIBattleComponent(GameObject& newGameObject);


private:
    std::vector<IComponent::Pointer> objectsAI;
    std::vector<IComponent::Pointer> battleAI;
    bool changeAI;
};