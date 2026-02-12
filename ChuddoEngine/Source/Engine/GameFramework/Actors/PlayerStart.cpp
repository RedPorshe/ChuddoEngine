#include "Actors/PlayerStart.h"

CPlayerStart::CPlayerStart ( CObject * inOwner, const std::string & inName )
    : Super ( inOwner, inName )
    {
    LOG_DEBUG ( "[PLAYERSTART] Created: ", GetName () );
    }

CPlayerStart::~CPlayerStart ()
    {}