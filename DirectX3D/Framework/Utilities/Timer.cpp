#include "Framework.h"

Timer::Timer()
{
    //1초동안 CPU진동수를 반환하는 함수
    QueryPerformanceFrequency((LARGE_INTEGER*)&periodFrequency);

    //현재 CPU진동수를 반환하는 함수
    QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);

    timeScale = 1.0f / (float)periodFrequency;
}

Timer::~Timer()
{
}

void Timer::Update()
{
    QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
    elapsedTime = (float)(curTime - lastTime) * timeScale;

    if (lockFPS != 0)
    {
        while (elapsedTime < (1.0f / lockFPS))
        {
            QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
            elapsedTime = (float)(curTime - lastTime) * timeScale;
        }
    }

    lastTime = curTime;

    frameCount++;
    oneSecCount += elapsedTime;

    if (oneSecCount >= 1.0f)
    {
        frameRate = frameCount;
        frameCount = 0;
        oneSecCount = 0.0f;
    }    
}

void Timer::GUIRender()
{
    
    //if (ImGui::TreeNode("Clock"))
    //{
        ImGui::Text("Clock");

        ImGui::Text("startTime : %lf", startTime);
        ImGui::Text("finishTime : %lf", finishTime);
        ImGui::Text("duration : %lf", finishTime - startTime); // 걸린시간
    //}
}

void Timer::SaveStartTime()
{
    startTime = clock();
}

void Timer::SaveFinishTime()
{
    finishTime = clock();
}
