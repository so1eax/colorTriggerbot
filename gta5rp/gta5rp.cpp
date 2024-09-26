#include <Windows.h>
#include <cstdio>
#include <iostream>
#include "wtypes.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
int triggerBot() {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();

    while (true) {
        GetWindowRect(hDesktop, &desktop);

        if (!desktop.right || !desktop.bottom)
            continue;

        HDC dc = GetDC(NULL);
        if (!dc)
            continue;

        COLORREF pxcolor = GetPixel(dc, desktop.right / 2, desktop.bottom / 2);
        ReleaseDC(NULL, dc);
        if (pxcolor == CLR_INVALID)
            continue;

        int red = GetRValue(pxcolor);
        int green = GetGValue(pxcolor);
        int blue = GetBValue(pxcolor);

        if (GetAsyncKeyState(VK_RBUTTON)) {
            std::this_thread::sleep_for(10ms);
            if (red > 150 && green < 100 && blue < 100)
            {
                INPUT input = { 0 };
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

                SendInput(1, &input, sizeof(INPUT));

                std::this_thread::sleep_for(1ms);

                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, &input, sizeof(INPUT));
            }
        }
        else
            std::this_thread::sleep_for(500ms);

    }

    return 0;
}

int main()
{
    triggerBot();
}