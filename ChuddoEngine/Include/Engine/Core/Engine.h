#pragma once
#include "CoreMinimal.h"

class CActor;

class CEngine
    {
    private:
        static CEngine * Instance;
        CEngine () { };

    public:
        virtual ~CEngine ();

        // Singleton
        static CEngine & Get ();
        static bool InitializeEngine ();  // Статический метод для инициализации
        static void ShutdownEngine ();    // Статический метод для очистки

        bool Initialize ();  // Нестатическая инициализация
        void Shutdown ();    // Нестатическая очистка

        void Start ();

    protected:
        void MainLoop ();
        void Tick ( float deltaTime );

        bool bIsInitialized = false;
        bool bIsRunning = false;

        void CreateTestWorld ();
        
    };

#include "Core/Object.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/World.h"
#include "GameFramework/World/Level.h"
#include "GameFramework/Actors/Actor.h"
#include <iostream>
#include <fstream>


   