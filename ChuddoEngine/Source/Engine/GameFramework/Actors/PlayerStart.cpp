#include "Actors/PlayerStart.h"
#include "Components/GravityComponent.h"

CPlayerStart::CPlayerStart ( CObject * inOwner, const std::string & inName )
	: Super ( inOwner, inName )
	{
	LOG_DEBUG ( "[PLAYERSTART] Created: ", GetName () );
    if (m_Gravity)
        {
            
        auto it = std::find ( ActorComponents.begin (), ActorComponents.end (), m_Gravity );
        if (it != ActorComponents.end ())
            {
            ActorComponents.erase ( it );
            }

        
        RemoveOwnedObject ( m_Gravity->GetName () );

        
        m_Gravity = nullptr;
        }
	}

CPlayerStart::~CPlayerStart ()
	{}