#include "ItemRedShellComponent.h"



ItemRedShellComponent::ItemRedShellComponent(GameObject& newGameObject, GameObject& obj) : IItemComponent(newGameObject), player(obj)
{
    speed = 1.f;
    consTime = 0.1f;
    decTime = 1.f;
}

ItemRedShellComponent::~ItemRedShellComponent()
{

}

void ItemRedShellComponent::init()
{
    lastVector = player.getComponent<PathPlanningComponent>()->getLastPosVector();
    myPos = player.getComponent<ScoreComponent>()->getPosition(); //= listNodes[lastVector]->getTransformData().position;
    
    if(myPos > 1)
    {
        enemy = ScoreManager::getInstance().getPlayers()[myPos-2];
    }
}

void ItemRedShellComponent::update(float dTime)
{
        auto listNodes = WaypointManager::getInstance().getWaypoints();
        auto vSensorComponent = getGameObject().getComponent<VSensorComponent>().get();
        auto moveComponent = getGameObject().getComponent<MoveComponent>().get();
        
    if(myPos > 1)
    {
        auto aiDrivingComponent = getGameObject().getComponent<AIDrivingComponent>().get();

        auto pos =getGameObject().getTransformData().position;

        auto posWay = listNodes[lastVector].get()->getTransformData().position;

        float distaneActualWay = (posWay.x - pos.x) * (posWay.x - pos.x) +
                            (posWay.y - pos.y) * (posWay.y - pos.y) +
                            (posWay.z - pos.z) * (posWay.z - pos.z);
        float radius = listNodes[lastVector].get()->getComponent<WaypointComponent>()->getRadius();
        
        if(distaneActualWay <= (radius*radius)/2)
        {
            if(lastVector < listNodes.size()-1)
            {
                lastVector++;
            }
            else if(lastVector == listNodes.size()-1)
            {
                lastVector = 0;
            }
        }
        
        
        vSensorComponent->setAngleInitial(moveComponent->getMovemententData().angle);

        objective = enemy.get()->getGameObject().getTransformData().position; //= listNodes[lastVector]->getTransformData().position;

        float distancePlayer = (objective.x - pos.x) * (objective.x - pos.x) +
                            (objective.y - pos.y) * (objective.y - pos.y) +
                            (objective.z - pos.z) * (objective.z - pos.z);
        
        int posVectorEnemy = enemy.get()->getGameObject().getComponent<PathPlanningComponent>()->getLastPosVector();
        if(distaneActualWay < distancePlayer || lastVector < posVectorEnemy)
        {
            objective = posWay;
        }
        
        float a = 0.f,b = 0.f;
        vSensorComponent->calculateAB(objective, a, b);
        std::vector<VObject::Pointer> seenObjects;
        //DECIDE 
        float turnValue = aiDrivingComponent->girar(seenObjects, seenObjects, objective, a, b);

        moveComponent->changeSpin(turnValue);
    }
    //Accelerate and brake
    moveComponent->isMoving(true);
    moveComponent->changeAcc(1);
    
}

void ItemRedShellComponent::close()
{
    
} 
 
 
