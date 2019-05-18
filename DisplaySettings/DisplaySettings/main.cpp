#include <Windows.h>
#include <strsafe.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <thread>




LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
void ListenForKeyCode();
void MessageLoop();
bool SearchForElement(int *, int, int);
void ChangeDisplay();
double start;
DEVMODE *dev;

int pressed[4];
bool recievedInputRecently = false;

int main()
{

	HWND window;
	AllocConsole();
	window = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(window, 0);
	dev = new DEVMODE();
	MessageLoop();
	return 0;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode == HC_ACTION)
	{

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{


			PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
			//asycKey doesnt work in hook
			//if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU) && key->vkCode == 0x46)
			if (key->vkCode == 0x46 || key->vkCode == VK_LCONTROL || key->vkCode == VK_LMENU || key->vkCode == VK_LSHIFT)
			{
				//ShellExecute(nullptr, "open", "C:\\Program Files (x86)\\Windows Media Player\\wmplayer.exe", nullptr, nullptr, SW_SHOW);


				if (!SearchForElement(&pressed[0], key->vkCode, 4))
				{

					for (int i = 0; i < 4; i++)
					{
						if (pressed[i] == '\0')
						{
							pressed[i] = key->vkCode;
							ListenForKeyCode();
							break;
						}
					}

				}
			}
		}
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{


			PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
			if (key->vkCode == 0x46 || key->vkCode == VK_LCONTROL || key->vkCode == VK_LMENU || key->vkCode == VK_LSHIFT)
			{
				if (SearchForElement(&pressed[0], key->vkCode, 4))
				{
					for (int i = 0; i < 4; i++)
					{
						if (pressed[i] == key->vkCode)
						{

							pressed[i] = '\0';
							break;
						}
					}
				}
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool SearchForElement(int * arrayPointer, int searchFor, int length)
{

	for (int i = 0; i < length; i++)
	{

		if (*arrayPointer == searchFor)
			return true;

		arrayPointer++;
	}
	return false;
}


void MessageLoop()
{
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);

	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void ChangeDisplay()
{
	dev->dmSize = sizeof(DEVMODE);
	dev->dmDriverExtra = 0;
	DISPLAY_DEVICE monitor = DISPLAY_DEVICE();
	monitor.cb = sizeof(monitor);
	if (EnumDisplayDevices(0, 0, &monitor, EDD_GET_DEVICE_INTERFACE_NAME) == 0)
		std::cout << "FAILED";
	EnumDisplaySettingsEx(NULL, ENUM_CURRENT_SETTINGS, dev, 0);

	if (dev->dmDisplayOrientation == 0)
		dev->dmDisplayOrientation = 1;
	else
		dev->dmDisplayOrientation = 0;

	int tmp = dev->dmPelsWidth;
	dev->dmPelsWidth = dev->dmPelsHeight;
	dev->dmPelsHeight = tmp;

	std::cout << dev->dmPelsWidth << " , " << dev->dmPelsHeight << "  Mode: " << dev->dmDisplayOrientation << "\n";
	int result = ChangeDisplaySettingsEx(NULL, dev, NULL, 0, NULL);



}


double timeOfLastCall = -1;
void ListenForKeyCode()
{
	double timePassed;
	if (timeOfLastCall == -1)
		timePassed = 10;
	else
	{
		timePassed = (clock() - timeOfLastCall) / CLOCKS_PER_SEC;
	}

	if (timePassed > 2)
	{
		if (SearchForElement(pressed, 0x46, 4) && SearchForElement(pressed, VK_LCONTROL, 4) && SearchForElement(pressed, VK_LMENU, 4) && SearchForElement(pressed, VK_LSHIFT, 4))
		{
			ChangeDisplay();
			timeOfLastCall = clock();

		}
	}


}