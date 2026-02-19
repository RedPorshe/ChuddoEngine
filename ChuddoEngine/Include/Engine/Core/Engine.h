#pragma once
#include "CoreMinimal.h"

class CActor;
class CCollisionSystem;
class CGameInstance;
class IRenderer;
class CInputSystem;


class CEngine
    {
    private:
        static CEngine * Instance;

    public:
        virtual ~CEngine ();

        // Singleton
        static CEngine & Get ();
        static bool InitializeEngine ();
        static void ShutdownEngine ();

        bool Initialize ();
        void Shutdown ();

        void Start ();
        void RequestExit ();

        // GameInstance access
        CGameInstance & GetGameInstance (); 

    protected:
        void MainLoop ();
        void Tick ( float deltaTime );
        void CalculateDeltaTime ();
        float m_DeltaTime = 0.f;
        std::chrono::steady_clock::time_point m_LastFrameTime;
        bool bIsInitialized = false;
        bool bIsRunning = false;
        void * Window = nullptr; // Окно для использования в инпуте , берется из рендера для передачи в инпут систему, чтобы не создавать отдельное окно для инпута
        void CreateTestWorld ();

        std::unique_ptr< IRenderer>  Renderer = nullptr;
        CCollisionSystem & CollisionSystem;
        CInputSystem & InputSystem;

        CEngine ();
    };