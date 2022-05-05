#include"AddProjEnvironmentVar.h"

int main()
{
	int returnvalue = 0;
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

		if (SetPermanentEnvironmentVariable(L"PROJ_LIB", ptr)) {
			cout << "Environment variable succesfully set" << endl;
			returnvalue = 0;
		}
		else {
			cout << "There was an error setting the environment variable. Was program run as admin?" << endl;
			returnvalue = 1;
		};
	}
	else {
		cout << "No changes to the environment variables were performed" << endl;
	}
	return returnvalue;
	//free(pathToCurrentFolder);
}