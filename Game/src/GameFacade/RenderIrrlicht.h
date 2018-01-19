#pragma once

#include <irrlicht.h>
#include <map>

#include "IRenderFacade.h"

#include "../GameManager/InputManager.h"

class RenderIrrlicht : public IRenderFacade {

public:

    //==============================================================
    // Class Related functions
    //==============================================================

    //Constructor
    RenderIrrlicht() : IRenderFacade() {}

    //Destructor
    virtual ~RenderIrrlicht() {}

    //==============================================================
    // Engine Related functions
    //==============================================================

    //Creates a window depending on the engine
    virtual void openWindow();

    //Updates window info in the engine
    virtual void updateWindow();

    //Closes engine window
    virtual void closeWindow();

    //==============================================================
    // Render Related functions
    //==============================================================

    //Renders all the scene
    virtual void renderDraw();

    //Add a camera to the game
    virtual void addCamera();

    //Update the current camera
    virtual void updateCamera();

    //Add an object to the game
    virtual void addObject(IComponent::Pointer ptr);

    //Delete an object of the game
    virtual void deleteObject(IComponent::Pointer ptr);

    //Add a light to the game
    virtual void addLight();

    //Change the position of an object in-game
    virtual void updateObjectTransform(uint16_t id, GameObject::TransformationData transform);

    //Update the logo video
    virtual void updateLogo();

private: 

    //Irrlicht own window
    irr::IrrlichtDevice* device;

    //Irrlicht scene manager
    irr::scene::ISceneManager* sceneManager;

    //Irrlicht video manager
    irr::video::IVideoDriver* videoDriver;

    //Irrlicht geometry creator
    const irr::scene::IGeometryCreator* geometryCreator;

    //Irrlicht node map
    std::map<uint16_t, irr::scene::ISceneNode*> nodeMap;

    //Irrlicht camera
    irr::scene::ICameraSceneNode* camera;

    irr::gui::IGUIStaticText* pos;

    irr::gui::IGUIStaticText* lap;

    irr::gui::IGUIStaticText* item;

    irr::gui::IGUIFont* font;

};
