#include"AddProjEnvironmentVar.h"

int main()
{
	//setenv("PROJ_LIB", "C:\\Users\\Televitis\\source\\repos\\ShpLib_x86\\Debug", true);
	
	//char* pathToCurrentFolder = (char*) malloc(MAX_PATH);
	//strcpy(pathToCurrentFolder,getCurrentProcessDirectory().u8string().c_str());
	//printf(pathToCurrentFolder);
	//setenv("PROJ_LIB", "C:\\Users\\Televitis\\source\\repos\\ShpLib_x86\\Debug", true);
	//setenv("PROJ_LIB","",true);
	const char* ret = getenv("PROJ_LIB");
	if (ret == NULL || true) { //set environment variable even if currently set
		string text = getCurrentProcessDirectory().string();
		wchar_t wtext[MAX_PATH];
		mbstowcs(wtext, text.c_str(), text.length() + 1);//plus null
		LPWSTR ptr = wtext;

		SetPermanentEnvironmentVariable(L"PROJ_LIB", ptr);
	}
	//free(pathToCurrentFolder);
}