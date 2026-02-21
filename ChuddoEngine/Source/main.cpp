#include "Core/Engine.h"
#include <iostream>
#include "CoreMinimal.h"


int main ()
    {
    setlocale ( LC_ALL, "ru" );
    setlocale ( LC_ALL, "ru_RU.UTF-8" );

    LOG_INIT ( "Engine", false, true );

#ifdef _DEBUG
    LOG_SET_LEVEL ( CE::CLogger::CLogLevel::DEBUG );
    LOG_INFO ( "DEBUG CONFIG" );
#else
    LOG_SET_LEVEL ( CE::CLogger::CLogLevel::INFO );
    LOG_INFO ( "RELEASE CONFIG" );
#endif
    FEngineInfo *engineInfo = new FEngineInfo();
    engineInfo->EngineName = "ChuddoEngine";
    engineInfo->WindowInfo.Title = "ChuddoEngine - Vulkan Test";
    engineInfo->WindowInfo.Width = 1280;
    engineInfo->WindowInfo.Height = 720;


    LOG_INFO ( "Initializing engine..." );
    if (!CEngine::InitializeEngine ( *engineInfo ))
        {
        LOG_FATAL ( "Failed to initialize engine!" );
        LOG_SHUTDOWN ();
        return EXIT_FAILURE;
        }

    auto & eng = CEngine::Get ();

    LOG_INFO ( "Starting engine..." );
    eng.Start ();

    LOG_INFO ( "Shutting down engine..." );
    CEngine::ShutdownEngine ();
    
    LOG_SHUTDOWN ();
    return EXIT_SUCCESS;
    }