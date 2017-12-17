 #pragma once

#include "IComponent.h"
#include "GameObject.h"

class IItemComponent : public IComponent {



    public:

        enum ItemType{

            redShell = 0,      
            blueShell = 1,      
            banana = 2,      
            mushroom = 3,      
            star = 4
        
        };


    IItemComponent(GameObject& newGameObject) : IComponent(newGameObject) {};

    virtual ~IItemComponent() {};

    virtual void init() = 0;

    virtual void update(float dTime) = 0;

    virtual void close() = 0;

};
