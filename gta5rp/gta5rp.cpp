#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <wtypes.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdlib>

using namespace std;
using namespace std::chrono_literals;

std::atomic<bool> triggerBotRunning(false);
std::atomic<bool> strafeMacroRunning(false);

void triggerBot() {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();

    while (triggerBotRunning.load()) {
        GetWindowRect(hDesktop, &desktop);

        if (!desktop.right || !desktop.bottom)
            continue;

        HDC dc = GetDC(NULL);
        if (!dc)
            continue;

        COLORREF pxcolor = GetPixel(dc, (desktop.right / 2), (desktop.bottom / 2));
        ReleaseDC(NULL, dc);
        if (pxcolor == CLR_INVALID)
            continue;

        int red = GetRValue(pxcolor);
        int green = GetGValue(pxcolor);
        int blue = GetBValue(pxcolor);

        if (GetAsyncKeyState(VK_RBUTTON)) {
            if (red > 150 && green < 100 && blue < 100) {
                INPUT input = { 0 };
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

                SendInput(1, &input, sizeof(INPUT));

                std::this_thread::sleep_for(0.5ms);

                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, &input, sizeof(INPUT));
            }
        }
    }
}



void simulateKeyPress(WORD key) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;

    SendInput(1, &input, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(90));

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}
void strafeMacro() {
    while (strafeMacroRunning.load()) {
        simulateKeyPress(0x57);
        simulateKeyPress(0x41);
        simulateKeyPress(0x53);
        simulateKeyPress(0x44);
    }
}


bool TbRunning = false;
bool SmRunning = false;
void statusThread() {
    while (true)
    {
        if (TbRunning) cout << "TriggerBot status: \033[32m running \033[0m" << endl;
        else cout << "TriggerBot status: \033[31m stopped \033[0m" << endl;

        if (SmRunning) cout << "StrafeMacro status: \033[32m running \033[0m" << endl;
        else cout << "StrafeMacro status: \033[31m stopped \033[0m" << endl;

        this_thread::sleep_for(500ms);

        system("cls");
    }
}

int main()
{
    std::thread tbThread;
    std::thread smThread;

    auto triggerbotKey = 0x58;
    auto strafemacroKey = 0x45;

    bool TbKeyPressed = false;
    bool SmKeyPressed = false;

    thread statusLoop(statusThread);

    while (true) {
        if (GetAsyncKeyState(triggerbotKey) & 0x8000) {
            if (!TbKeyPressed) {
                if (!TbRunning && !triggerBotRunning.load()) {
                    triggerBotRunning.store(true);
                    tbThread = thread(triggerBot);
                    TbRunning = true;
                }
                else {
                    triggerBotRunning.store(false);
                    if (tbThread.joinable())
                        tbThread.join();

                    TbRunning = false;
                }
                TbKeyPressed = true;
            }
        }
        else if (TbKeyPressed)
            TbKeyPressed = false;



        else if (GetAsyncKeyState(strafemacroKey) & 0x8000) {
            if (!SmKeyPressed) {
                if (!SmRunning && !strafeMacroRunning.load()) {
                    strafeMacroRunning.store(true);
                    smThread = thread(strafeMacro);
                    SmRunning = true;
                }
                else {
                    strafeMacroRunning.store(false);
                    if (smThread.joinable())
                        smThread.join();

                    SmRunning = false;
                }
                SmKeyPressed = true;
            }
        }
        else if (SmKeyPressed)
            SmKeyPressed = false;
        this_thread::sleep_for(100ms);
    }

    return 0;
}