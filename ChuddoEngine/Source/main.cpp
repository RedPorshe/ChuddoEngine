// main.cpp
#include "Core/Engine.h"
#include <iostream>




int main ()
    {
    setlocale ( LC_ALL, "ru" );
    LOG_INIT ( "Engine", false, true );
        // Установка уровня логирования в зависимости от конфигурации сборки
#ifdef _DEBUG
    LOG_SET_LEVEL ( CE::CLogger::CLogLevel::DEBUG );
    LOG_INFO ( "DEBUG CONFIG" );
#else
    LOG_SET_LEVEL ( CE::CLogger::CLogLevel::INFO );
    LOG_INFO ( "RELEASE CONFIG" );
#endif

   
    LOG_INFO ( "Initializing engine..." );
    if (!CEngine::InitializeEngine ())
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