#pragma once
#include "Core/Object.h"

class CWorld;
class CLevel;

class CActor : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CActor, CObject )
    public:
        CActor ( CObject * owner = nullptr, const std::string & inName = "Actor" );
           

        virtual ~CActor ();
           

        virtual void BeginPlay ();
           

        virtual void Tick ( float deltaTime );


        virtual void EndPlay ();

        CLevel * GetLevel () const;          

        CWorld * GetWorld () const;
           
    };