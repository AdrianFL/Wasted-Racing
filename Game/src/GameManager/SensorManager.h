#pragma once

#include "../GameObject/AIComponent/ISensorComponent.h"
#include "../GameObject/AIComponent/VSensorComponent.h"
#include "../GameObject/AIComponent/MSensorComponent.h"
#include "../GameObject/StartLineComponent.h"
#include "ObjectManager.h"
#include "PhysicsManager.h"
#include <vector>

class SensorManager{

public: 

    //Constructor
    SensorManager() {}

    //Destructor
    ~SensorManager() {}

    //Initialization
    void init();

    //Update
    void update();

    //Shutdown
    void close();

    std::vector<IComponent::Pointer>& getComponentList() { //Visual sensors
        return sensorComponentList;
    }

    std::vector<IComponent::Pointer>& getMComponentList() {//Map sensors
        return sensorMComponentList;
    }

    //Static class getter
    static SensorManager& getInstance();

    //Component creators
    IComponent::Pointer createVSensorComponent(GameObject& newGameObject, float angV, float angI, float md, float ml);
    IComponent::Pointer createMSensorComponent(GameObject& newGameObject, float angV, float angI);

private:
    std::vector<IComponent::Pointer> sensorComponentList;
    std::vector<IComponent::Pointer> sensorMComponentList;
    std::vector<GameObject> worldObjects;
};