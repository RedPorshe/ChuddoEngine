#pragma once
#include "CoreMinimal.h"

class CActor;
class CCollisionSystem;
class CGameInstance;
class CWindow;
class CInputSystem;


class CEngine
    {
    private:
        static CEngine * Instance;

    public:
        virtual ~CEngine ();

        // Singleton
        static CEngine & Get ();
        static bool InitializeEngine ( FEngineInfo & EngineInfo );
        static void ShutdownEngine ();

        bool Initialize ();
        void Shutdown ();

        void Start ();
        void RequestExit ();
        bool IsRunning () const { return bIsRunning; }
        // GameInstance access
        CGameInstance & GetGameInstance (); 
      
    protected:
        void MainLoop ();
        void Tick ( float deltaTime );
        void CalculateDeltaTime ();
        void CreateTestWorld ();

        float m_DeltaTime = 0.f;
        std::chrono::steady_clock::time_point m_LastFrameTime;
        bool bIsInitialized = false;
        bool bIsRunning = false;
        
        std::unique_ptr<CWindow>  Window = nullptr;
        CCollisionSystem & CollisionSystem;
        CInputSystem & InputSystem;
        FEngineInfo & Info;
        CEngine (FEngineInfo& EngineInfo);
    };