#include "stdafx.h"
#include "Timer.h"

float Timer::GetDeltaTime( bool capAt60FPS )
{
    m_bCapAt60FPS = capAt60FPS;

	// Update our time
    static float deltaTime = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if ( timeStart == 0 )
        timeStart = timeCur;
    deltaTime = ( timeCur - timeStart ) / 1000.0f;
    timeStart = timeCur;

    // Cap the framerate at 60 fps
    if ( capAt60FPS )
    {
        float FPS60 = 1.0f / 60.0f;
        static float cummulativeTime = 0;
        cummulativeTime += deltaTime;
        if ( cummulativeTime >= FPS60 )
            cummulativeTime = cummulativeTime - FPS60;
        else
            return 0;
    }

    return deltaTime;
}