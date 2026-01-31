#pragma once
#include "CoreMinimal.h"

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