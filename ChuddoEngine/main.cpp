#include "CoreMinimal.h"

bool GIsRequestingExit = false;
FEngineLoop GEngine;



int main()
{
	if(GEngine.Init())
	{
	GEngine.Start();
	}

    return 0;
}