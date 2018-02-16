#pragma once

#include "RenderManager.h"
#include "NetworkManager.h"
#include "../GameEvent/EventManager.h"
#include "../GameObject/InputComponent.h"
#include "../GameObject/NetworkComponent/DebugNetworkComponent.h"
#include "../GameServer/CustomIdentifiers.h"

class NetworkManager;

class DebugManager{
    public: 

        //Constructor
        DebugManager() {};

        //Destructor
        ~DebugManager() {};

        //Initialization
        void init();

        //Update
        void update();

        //Shutdown
        void close();

        //Static class getter
        static DebugManager& getInstance(){
            static DebugManager instance;
            return instance;
        };

        //DebugNetworkComponent creator
	    IComponent::Pointer createDebugNetworkComponent(GameObject& newGameObject);
    
        //==============================================
        // INITIALIZE DEBUGS
        //==============================================

         //Initialize AI driving debugging
        void initDebugAI();

         //Initialize Battle behaviour debugging
        void initDebugBehaviour();

         //Initialize Camera debugging
        void initDebugCamera();

        //Initialize Network debugging
        void initDebugNetwork();


        //==============================================
        // CLEAN DEBUGS
        //==============================================

        //Erase all the elements that are part of the AI driving debug
        void cleanDebugAI();

        //Erase all the elements that are part of the Battle Behaviour debug
        void cleanDebugBehaviour();

        //Erase all the elements that are part of the Camera debug
        void cleanDebugCamera();
                
        //Erase all the elements that are part of the network debug
        void cleanDebugNetwork();

        //==============================================================
        // Getters and setters
        //==============================================================
        void setDebugAI       (bool s){ debugAI = s;           };
        void setDebugBehaviour(bool s){ debugBehaviour = s;    };
        void setDebugCamera   (bool s){ debugCamera = s;       };
        void setDebugNetwork  (bool s){ debugNetwork = s;      };
        bool getDebugAI       ()      { return debugAI;        };
        bool getDebugBehaviour()      { return debugBehaviour; };
        bool getDebugCamera   ()      { return debugCamera;    };
        bool getDebugNetwork  ()      { return debugNetwork;   };

    private:
        //==============================================================
        // Private data
        //==============================================================
        
        //VIGILS
        //Checks if debug AI is on
        bool debugAI;

        //Checks if debug of battle behaviour is on
        bool debugBehaviour;

        //Checks if camera debug is on
        bool debugCamera;

        //Checks if network debug is on
        bool debugNetwork;

        //MANAGERS
        NetworkManager* networkManager;
        RenderManager*  renderManager;

        //NetworkData
        int32_t idLastMessageBoard;                 //id of the board
        std::vector<int32_t> idLastMessageTexts;    //id of all the last messages
        int32_t idLastMessageText;                  //last sent message
        std::vector<int32_t> idLastDataTexts;       //id of all the last data of the last messages

        //==============================================
        // NETWORK PARTICULAR FUNCTIONS
        //==============================================

        //Set last active message received to green
        void setActiveLastMessage(uint32_t id);

        //Update last information received from the message with the id given
        void updateNetworkText(uint32_t id, std::string text);

        //==============================================
        // UPDATE DEBUGS
        //==============================================
        //Updates the AI debug
        void updateDebugAI();

        //Updates the battle behaviour debug
        void updateDebugBehaviour();

        //Updates the camera debug
        void updateDebugCamera();

        //Updates de network debug
        void updateDebugNetwork();
};