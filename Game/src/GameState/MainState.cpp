#include "MainState.h"

void MainState::init() {

    audioManager    = &AudioManager::getInstance();     //Initialize true audio manager
    eventManager    = &EventManager::getInstance();     //Initilize event manager
    renderManager   = &RenderManager::getInstance();    //First we initialize renderManager, who creates a device and passes this reference to the inputManager
    inputManager    = &InputManager::getInstance();     //Once we've initialized the renderManager, we can do the same with our inputManager
    objectManager   = &ObjectManager::getInstance();    //Initialize object manager
    physicsManager  = &PhysicsManager::getInstance();   //Initialize physics manager
    waypointManager = &WaypointManager::getInstance();  //Initialize Waypoint Manager 
    aiManager       = &AIManager::getInstance();        //Initialize AI manager
    sensorManager   = &SensorManager::getInstance();    //Initialize Sensor manager
    itemManager     = &ItemManager::getInstance();      //Initialize Sensor manager
    scoreManager    = &ScoreManager::getInstance();     //Initialize Score Manager
}

void MainState::update(float &accumulatedTime) {
    //If time surpassed the loopTime
    if(accumulatedTime > loopTime){
        //Update managers
        updateManagers(accumulatedTime);

        Game::getInstance().setStay(objectManager->getGameRunning());
        accumulatedTime = 0;
    }
    //Always interpolate
        physicsManager->interpolate(accumulatedTime, loopTime);
        renderManager->getRenderFacade()->interpolateCamera(accumulatedTime, loopTime);

}

void MainState::updateManagers(float dTime){
    //Input manager has to be the first to be updated
    inputManager->update();

    physicsManager->update(dTime);

    aiManager->update();

    renderManager->update();

    waypointManager->update(dTime);

    sensorManager->update();

    itemManager->update(dTime);
    
    scoreManager->update();

    audioManager->update();

    //Event manager has to be the last to be updated
    eventManager->update();
}

void MainState::draw() {
    renderManager->draw();
}

void MainState::close() {

}

