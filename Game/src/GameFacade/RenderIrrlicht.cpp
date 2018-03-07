#include "RenderIrrlicht.h"
#include "InputIrrlicht.h"
#include "../GameEvent/EventManager.h"
#include "../GameManager/ObjectManager.h"
#include "../GameObject/RenderComponent/ObjectRenderComponent.h"
#include "../GameObject/RenderComponent/CameraRenderComponent.h"
#include <cmath>
#include <string>

void RenderIrrlicht::openWindow(){

    irr::video::E_DRIVER_TYPE driverType = irr::video::E_DRIVER_TYPE::EDT_OPENGL;

    InputIrrlicht* receiver = new InputIrrlicht();

    device = irr::createDevice(driverType, irr::core::dimension2d<irr::u32>(window.size.x, window.size.y),
        16, window.fullscreen, false, window.vsync, receiver);

    videoDriver = device->getVideoDriver();
    sceneManager = device->getSceneManager();
    geometryCreator = sceneManager->getGeometryCreator();

    font = sceneManager->getGUIEnvironment()->getFont("../media/img/fontcourier.bmp");
    pos = sceneManager->getGUIEnvironment()->addStaticText(L"Position: ", irr::core::recti(0,0, 200,50));
    lap = sceneManager->getGUIEnvironment()->addStaticText(L"Lap: ", irr::core::recti(0, 20, 200, 50));
    item = sceneManager->getGUIEnvironment()->addStaticText(L"Item: ", irr::core::recti(0, 40, 200, 50));
    pos->setOverrideFont(font);

    createItemIcon(glm::vec2(50,50), "media/img/emptymini.png");

    addCamera();
    //sceneManager->setAmbientLight(irr::video::SColorf(0.8,0.8,0.8,1));

    uintptr_t aux = reinterpret_cast<uintptr_t>(device);
    InputManager::getInstance().setDevice(aux);
    InputManager::getInstance().setInputFacade(receiver);

}

void RenderIrrlicht::updateWindow() {

    //updateCamera();
    auto score = ObjectManager::getInstance().getObject(cameraTarget->getId()).get()->getComponent<ScoreComponent>();
    if(score != nullptr)
    {
        int oM = ObjectManager::getInstance().getObject(cameraTarget->getId()).get()->getComponent<ScoreComponent>().get()->getPosition();
        int oL = ObjectManager::getInstance().getObject(cameraTarget->getId()).get()->getComponent<ScoreComponent>().get()->getLap();
        int ML = ScoreManager::getInstance().getMaxLaps();
        int iT = ObjectManager::getInstance().getObject(cameraTarget->getId()).get()->getComponent<ItemHolderComponent>().get()->getItemType();
        irr::core::stringw stringLap = L"  LAP:";
        irr::core::stringw stringItm = L"  ITEM:";
        irr::core::stringw stringPos = L"  POSITION:";
        stringLap += oL;
        stringLap += " / ";
        stringLap += ML;
        switch(iT)
        {
            case -1: stringItm+="EMPTY";
                    break;
            case 0: stringItm+="RED SHELL";
                    break;
            case 1: stringItm+="BLUE SHELL";
                    break;
            case 2: stringItm+="BANANA";
                    break;
            case 3: stringItm+="MUSHROOM";
                    break;
            case 4: stringItm+="STAR";
                    break;
        }
        stringPos += oM;
        pos->setText(stringPos.c_str());
        lap->setText(stringLap.c_str());
        item->setText(stringItm.c_str());
        updateItemIcon();
    }
    else
    {
        irr::core::stringw stringLap = L"  LAP:";
        irr::core::stringw stringItm = L"  ITEM:";
        irr::core::stringw stringPos = L"  POSITION:";
        
        pos->setText(stringPos.c_str());
        lap->setText(stringLap.c_str());
        item->setText(stringItm.c_str());
        updateItemIcon();
    }
}

void RenderIrrlicht::closeWindow() {

    device->drop();

}

void RenderIrrlicht::renderDraw() {

    videoDriver->beginScene(true, true, irr::video::SColor(255,150,150,255));
    sceneManager->drawAll();
    sceneManager->getGUIEnvironment()->drawAll();
    videoDriver->endScene();
 
}

void RenderIrrlicht::drawGUI(){
    
    videoDriver->beginScene(true, true, irr::video::SColor(255,150,150,255));
    sceneManager->getGUIEnvironment()->drawAll();
    videoDriver->endScene();
 
}

void RenderIrrlicht::addCamera() {
    camera = sceneManager->addCameraSceneNode();
    camera->setPosition(irr::core::vector3df(0,0,0));
}

void RenderIrrlicht::interpolateCamera(float accTime, float maxTime) {
    //Get target position
    auto pos = cameraTarget->getTransformData().position;

    //Get target y angle
    float radianAngle = cameraTarget->getTransformData().rotation.y;

    //Get interpolated distance to the player
    float oldD = cameraTarget->getComponent<CameraRenderComponent>().get()->getOldDistance();
    float newD = cameraTarget->getComponent<CameraRenderComponent>().get()->getDistance();

    float distance = oldD + (accTime * (newD - oldD))/maxTime;

    camera->setTarget(irr::core::vector3df(pos.x, pos.y, pos.z));
    camera->setPosition(irr::core::vector3df(pos.x - distance * cos(radianAngle), pos.y + distance * 0.4, pos.z + distance * sin(radianAngle)));
}

void RenderIrrlicht::addObject(IComponent* ptr) {

    ObjectRenderComponent* cmp = dynamic_cast<ObjectRenderComponent*>(ptr);

    if(cmp != nullptr){

        auto shape = cmp->getObjectShape();
        auto obj = cmp->getGameObject();
        //Transform the data to irrlicht type
        auto pos = obj.getTransformData().position;
        auto rot = obj.getTransformData().rotation;
        auto sca = obj.getTransformData().scale;
        irr::core::vector3df irrPos = irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z);
        irr::core::vector3df irrRot = irr::core::vector3df((float)rot.x,(float)rot.y, (float)rot.z);
        irr::core::vector3df irrSca = irr::core::vector3df((float)sca.x,(float)sca.y, (float)sca.z);

        irr::scene::ISceneNode * node;

        //Initialize the node
        switch(shape){

            case ObjectRenderComponent::Shape::Cube: {
                node = sceneManager->addCubeSceneNode();
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
            }
            break;
            case ObjectRenderComponent::Shape::Sphere: {
                node = sceneManager->addSphereSceneNode();
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
            }
            break;
            case ObjectRenderComponent::Shape::Plane: {
                auto plane = geometryCreator->createPlaneMesh(irr::core::dimension2d<irr::f32>(1,1));
                node = sceneManager->addMeshSceneNode(plane);
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
            }
            break;
            case ObjectRenderComponent::Shape::Mesh: {
                auto plane = sceneManager->getMesh(cmp->getMesh().c_str());
                node = sceneManager->addMeshSceneNode(plane);
            }
            break;
            case ObjectRenderComponent::Shape::Arrow: {
                auto plane = geometryCreator->createArrowMesh(4,8,10,6,1,3);
                node = sceneManager->addMeshSceneNode(plane);
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
            }
            break;
            case ObjectRenderComponent::Shape::Portion: {
                auto plane = sceneManager->getMesh("media/mesh/portion/portion.obj");
                node = sceneManager->addMeshSceneNode(plane);
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
                node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
            }
            break;
            case ObjectRenderComponent::Shape::Pyramid: {
                auto plane = sceneManager->getMesh("media/mesh/pyramid/pyramid.obj");
                node = sceneManager->addMeshSceneNode(plane);
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
                node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
            }
            break;
            default:
            break;
        }

        //Set node transformation
        node->setPosition(irrPos);
        node->setRotation(irrRot);
        node->setScale(irrSca);

    
        nodeMap.insert(std::pair<uint16_t, irr::scene::ISceneNode*>(obj.getId(), node));
    }
}

void RenderIrrlicht::addObject(IComponent* ptr, float radius, float length, int tesselation, bool transparency) {

    ObjectRenderComponent* cmp = dynamic_cast<ObjectRenderComponent*>(ptr);

    if(cmp != nullptr){

        auto shape = cmp->getObjectShape();
        auto obj = cmp->getGameObject();
        //Transform the data to irrlicht type
        auto pos = obj.getTransformData().position;
        auto rot = obj.getTransformData().rotation;
        auto sca = obj.getTransformData().scale;
        irr::core::vector3df irrPos = irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z);
        irr::core::vector3df irrRot = irr::core::vector3df((float)rot.x,(float)rot.y, (float)rot.z);
        irr::core::vector3df irrSca = irr::core::vector3df((float)sca.x,(float)sca.y, (float)sca.z);

        irr::scene::ISceneNode * node;

        //Initialize the node
        switch(shape){

            case ObjectRenderComponent::Shape::Cylinder: {
                auto plane = geometryCreator->createCylinderMesh(radius,length,tesselation);
                node = sceneManager->addMeshSceneNode(plane);
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
                if(transparency == true)
                {
                    node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
                }
            }
            break;
            case ObjectRenderComponent::Shape::Cone: {
                auto plane = geometryCreator->createConeMesh(radius,length,tesselation);
                node = sceneManager->addMeshSceneNode(plane);
                auto var = videoDriver->getTexture(cmp->getImg().c_str());
                node->setMaterialTexture(0, var);
                if(transparency == true)
                {
                    node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
                }
            }
            break;
            default:
            break;
        }

        //Set node transformation
        node->setPosition(irrPos);
        node->setRotation(irrRot);
        node->setScale(irrSca);

        nodeMap.insert(std::pair<uint16_t, irr::scene::ISceneNode*>(obj.getId(), node));
    }
}

void RenderIrrlicht::addLight(IComponent* ptr) {

    LightRenderComponent* cmp = dynamic_cast<LightRenderComponent*>(ptr);

    if(cmp != nullptr){

        auto obj = cmp->getGameObject();
        auto type = cmp->getLightType();
        auto rad = cmp->getLightRadius();
        //Transform the data to irrlicht type
        auto pos = obj.getTransformData().position;
        auto rot = obj.getTransformData().rotation;
        auto sca = obj.getTransformData().scale;
        irr::core::vector3df irrPos = irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z);
        irr::core::vector3df irrRot = irr::core::vector3df((float)rot.x,(float)rot.y, (float)rot.z);
        irr::core::vector3df irrSca = irr::core::vector3df((float)sca.x,(float)sca.y, (float)sca.z);

        irr::scene::ISceneNode * node;

        //Initialize the node
        switch(type){

            case LightRenderComponent::Type::Point: {
                auto light = sceneManager->addLightSceneNode(0, irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z), irr::video::SColorf(1.0,1.0,1.0), rad); 
                auto & type = light->getLightData();
                node = light;
                type.Type = irr::video::E_LIGHT_TYPE::ELT_POINT;

            }
            break;
            case LightRenderComponent::Type::Directional: {
                auto light = sceneManager->addLightSceneNode(0, irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z), irr::video::SColorf(1.0,1.0,1.0), rad); 
                auto & type = light->getLightData();
                node = light;
                type.Type = irr::video::E_LIGHT_TYPE::ELT_DIRECTIONAL;
            }
            default:
            break;
        }

        //Set node transformation
        node->setPosition(irrPos);
        node->setRotation(irrRot);
        node->setScale(irrSca);

        nodeMap.insert(std::pair<uint16_t, irr::scene::ISceneNode*>(obj.getId(), node));
    }
}

void RenderIrrlicht::deleteObject(IComponent* ptr) {
    
    auto id = ptr->getGameObject().getId();
    auto itr = nodeMap.find(id);

    if(itr != nodeMap.end()){
        auto node = itr->second;
        node->remove();
        nodeMap.erase(id);
    }
}

void RenderIrrlicht::updateObjectTransform(uint16_t id, GameObject::TransformationData transform) {
    auto pos = transform.position;
    auto rot = transform.rotation;
    auto sca = transform.scale;
    irr::core::vector3df irrPos = irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z);
    irr::core::vector3df irrSca = irr::core::vector3df((float)sca.x,(float)sca.y, (float)sca.z);

    auto iterator = nodeMap.find(id);
    if(iterator != nodeMap.end()){

        irr::core::quaternion quadX = irr::core::vector3df(rot.x,0,0);
        irr::core::quaternion quadY = irr::core::vector3df(0,rot.y,0);
        irr::core::quaternion quadZ = irr::core::vector3df(0,0,rot.z);

        irr::core::quaternion quadRes = quadY*quadX*quadZ;

        irr::core::vector3df vec;
        quadRes.toEuler(vec);

        irr::core::vector3df vecRes;

        vecRes.X = vec.X*180/M_PI;
        vecRes.Y = vec.Y*180/M_PI;
        vecRes.Z = vec.Z*180/M_PI;

        auto node = iterator->second;
        node->setPosition(irrPos);
        node->setRotation(vecRes);
        node->setScale(irrSca);
    }
}

void RenderIrrlicht::addSkybox(IComponent* ptr)
{
    ObjectRenderComponent* cmp = dynamic_cast<ObjectRenderComponent*>(ptr);

    if(cmp != nullptr){

        auto shape = cmp->getObjectShape();
        auto obj = cmp->getGameObject();
        //Transform the data to irrlicht type
        auto pos = obj.getTransformData().position;
        auto rot = obj.getTransformData().rotation;
        auto sca = obj.getTransformData().scale;
        irr::core::vector3df irrPos = irr::core::vector3df((float)pos.x,(float)pos.y, (float)pos.z);
        irr::core::vector3df irrRot = irr::core::vector3df((float)rot.x,(float)rot.y, (float)rot.z);
        irr::core::vector3df irrSca = irr::core::vector3df((float)sca.x,(float)sca.y, (float)sca.z);

        irr::scene::ISceneNode * node;

        //Initialize the node
        std::cout<<"Hola"<<"\n";
        auto var = videoDriver->getTexture(cmp->getImg().c_str());
        node = sceneManager->addSkyBoxSceneNode(var,var,var,var,var,var);

        //Set node transformation
        node->setPosition(irrPos);
        node->setRotation(irrRot);
        node->setScale(irrSca);

    
        nodeMap.insert(std::pair<uint16_t, irr::scene::ISceneNode*>(obj.getId(), node));
    }
}

    ///////////////////////////////
    ///////      DEBUG      ///////    
    ///////////////////////////////

void RenderIrrlicht::updateLogo(){
    
}


void RenderIrrlicht::createRectangle2D(glm::vec2 pos, std::string img)
{
    irr::core::position2d<irr::s32> position(pos.x, pos.y);
    auto image = videoDriver->getTexture(img.c_str());
    rectangle = sceneManager->getGUIEnvironment()->addImage(image, position);
}

void RenderIrrlicht::deleteRectangle2D()
{
    if(rectangle != nullptr)
        rectangle->remove();
}

void RenderIrrlicht::createRectangleCol2D(glm::vec2 pos, std::string img)
{
    irr::core::position2d<irr::s32> position(pos.x, pos.y);
    auto image = videoDriver->getTexture(img.c_str());
    rectangleCol = sceneManager->getGUIEnvironment()->addImage(image, position);
}

void RenderIrrlicht::deleteRectangleCol2D()
{
    rectangleCol->remove();
}

///////////////////
/// TITLE
///////////////////

void RenderIrrlicht::createTitleText(glm::vec2 pos, std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    title = sceneManager->getGUIEnvironment()->addStaticText(txt, irr::core::recti(pos.x, pos.y, pos.x+200,pos.y+50));
    title->setText(txt);
}

void RenderIrrlicht::deleteTitleText()
{
    title->remove();
}


void RenderIrrlicht::setTitleText(std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    title->setText(txt);
}

///////////////////
/// DESCRIPTION
///////////////////

void RenderIrrlicht::createDescriptionText(glm::vec2 pos, std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    description = sceneManager->getGUIEnvironment()->addStaticText(txt, irr::core::recti(pos.x, pos.y, pos.x+320,pos.y+100));
    description->setText(txt);
}

void RenderIrrlicht::deleteDescriptionText()
{
    description->remove();
}

void RenderIrrlicht::setDescriptionText(std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    description->setText(txt);
}

///////////////////
/// SUBTITLE
///////////////////

void RenderIrrlicht::createSubTitleText(glm::vec2 pos, std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    subTitle = sceneManager->getGUIEnvironment()->addStaticText(txt, irr::core::recti(pos.x, pos.y, pos.x+200,pos.y+50));
    subTitle->setText(txt);
}

void RenderIrrlicht::deleteSubTitleText()
{
    subTitle->remove();
}


void RenderIrrlicht::setSubTitleText(std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    subTitle->setText(txt);
}

///////////////////
/// SUBDESCRIPTION
///////////////////

void RenderIrrlicht::createSubDescriptionText(glm::vec2 pos, std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    subDescription = sceneManager->getGUIEnvironment()->addStaticText(txt, irr::core::recti(pos.x, pos.y, pos.x+180,pos.y+50));
    subDescription->setText(txt);
}

void RenderIrrlicht::deleteSubDescriptionText()
{
    subDescription->remove();
}

void RenderIrrlicht::setSubDescriptionText(std::string text)
{
    std::wstring text_aux;
    for(unsigned int i = 0; i < text.length(); ++i)
    text_aux += wchar_t( text[i] );

    const wchar_t* txt = text_aux.c_str();

    subDescription->setText(txt);
}


void RenderIrrlicht::createItemIcon(glm::vec2 pos, std::string img)
{
    irr::core::position2d<irr::s32> position(pos.x, pos.y);
    auto image = videoDriver->getTexture(img.c_str());
    itemIMG = sceneManager->getGUIEnvironment()->addImage(image, position);
}

void RenderIrrlicht::deleteItemIcon()
{
    if(itemIMG != nullptr)
        itemIMG->remove();
}

void RenderIrrlicht::updateItemIcon(){

    int itemID = ObjectManager::getInstance().getObject(cameraTarget->getId()).get()->getComponent<ItemHolderComponent>().get()->getItemType();


    switch(itemID){

        case -1: //EMPTY
                 deleteItemIcon();
                 createItemIcon(glm::vec2(50,50), "media/img/emptymini.png");
                 break;

        case 0: //RED SHELL
                deleteItemIcon();
                createItemIcon(glm::vec2(50,50), "media/img/iconoRuedamini.png");
                 break;

        case 1: //BLUE SHELL
                deleteItemIcon();
                createItemIcon(glm::vec2(50,50), "media/img/iconoBombamini.png");
                 break;

        case 2: //BANANA
                deleteItemIcon();
                createItemIcon(glm::vec2(50,50), "media/img/iconoTrampamini.png");
                 break;

        case 3: //MUSHROOM
                deleteItemIcon();
                createItemIcon(glm::vec2(50,50), "media/img/iconoSetamini.png");
                 break;

        case 4: //STAR
                deleteItemIcon();
                createItemIcon(glm::vec2(50,50), "media/img/iconoBotellamini.png");
                 break;
    }
}
