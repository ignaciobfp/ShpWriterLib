#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include<cstdlib>       // setenv()
#include<filesystem>   
#include <wtypes.h>
using namespace std;

inline bool SetPermanentEnvironmentVariable(LPCTSTR value, LPCTSTR data)
{
	HKEY hKey;
	LPCTSTR keyPath = TEXT("System\\CurrentControlSet\\Control\\Session Manager\\Environment");
	LSTATUS lOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_ALL_ACCESS, &hKey);
	if (lOpenStatus == ERROR_SUCCESS)
	{
		//Set RegKey
		LSTATUS lSetStatus = RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)data, lstrlen(data) * sizeof(char16_t) + 1);
		RegCloseKey(hKey);

		//Force update of changed environment variables
		if (lSetStatus == ERROR_SUCCESS)
		{
			//SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK, 100, NULL);
			SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
			return true;
		}
	}

	return false;
}

// Wrapper to allow seeting environment variables in a portable manner, since windows uses _putenv(x), opposed to unix setenv(x,y,z)
inline int setenv(const char* name, const char* value, int overwrite)
{
	int errcode = 0;
	if (!overwrite) {
		size_t envsize = 0;
		errcode = getenv_s(&envsize, NULL, 0, name);
		if (errcode || envsize) return errcode;
	}
	return _putenv_s(name, value);
}

inline std::filesystem::path getCurrentProcessDirectory() {
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, sizeof(buffer));
	return std::filesystem::path(buffer).parent_path();
}