#pragma once
#pragma warning(disable:4996)
#include<iostream>
#include<sstream>
#include <iomanip>      // std::put_time
#include <ctime>        // std::time_t, struct std::tm, std::localtime
#include <chrono>       // std::chrono::system_clock
#include <gdal.h>
#include <ogrsf_frmts.h>
#include<thread>
#include<cstdlib>       // setenv()
#include<filesystem>   
using namespace std;

std::string shp_path;
const char* pszDriverName = "ESRI Shapefile";
GDALDriver* poDriver;
GDALDataset* poDataSet;
OGRLayer* poLayer;
OGRSpatialReference* poSRS;
std::vector<std::string> headers;
std::vector<OGRFieldType> fieldTypes;

inline bool SetPermanentEnvironmentVariable(LPCTSTR value, LPCTSTR data)
{
	HKEY hKey;
	LPCTSTR keyPath = TEXT("System\\CurrentControlSet\\Control\\Session Manager\\Environment");
	LSTATUS lOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_ALL_ACCESS, &hKey);
	if (lOpenStatus == ERROR_SUCCESS)
	{
		//Set RegKey
		LSTATUS lSetStatus = RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)data, lstrlen(data)*sizeof(char16_t) + 1);
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

inline void updateEnvironmentVariablesNow() {
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
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

inline void generateWKT(double x, double y, char* sWKT) {
	//Example WKT string: POINT(-2.514557 42.444437)
	//Expected max size 51: 6 "point(" + 1 sign + 2 digits + 1 dot + 17 decimals + 1 space + 1 sign + 2 digits + 1 dot + 17 decimals + 1 ")" + 1 \0
	snprintf(sWKT, 55, "POINT(%.17f %.17f)", x, y);
	return;
}

void initShp();
void initFromHeaders(vector<string> headers); //If no types are specified, we will assume every field is a double
void initFromHeadersDT(vector<string> headers, vector<OGRFieldType> dataTypes);
void initFromHeadersDTstring(vector<string> headers, string dataTypesString);

void initShpAppend();

void newFile();

void setHeaders(vector<string>);
void setFieldDatatypes(vector<OGRFieldType>);
void setFieldDatatypes(string);

void writeSingleValue(double val, double x, double y);
void writeSingleValueString(const char* val, double x, double y);
void writeCStringIntEfficient(const char* curr_string, int curr_int, double x, double y);
void writeCStringIntGPSEfficient(const char* curr_string, int dist, double x, double y, double heading, int fixStatus);
void writeMultiValue(std::string valAsCsv, double x, double y);

inline vector<string> tokenize(string s) {
	//stringstream ss("1,1,1,1, or something else ,1,1,1,0");
	stringstream ss(s);
	vector<string> result;

	while (ss.good())
	{
		string substr;
		getline(ss, substr, ',');
		result.push_back(substr);
	}
	return result;
}

inline vector<OGRFieldType> getFieldTypesFromString(string s) {
	vector<string> svec = tokenize(s);
	vector<OGRFieldType> result;
	for (unsigned int i = 0; i < svec.size(); ++i) {
		if (svec[i] == "d" || svec[i] == "D" || svec[i] == "r" || svec[i] == "R") {
			result.push_back(OGRFieldType::OFTReal);
		}
		else if (svec[i] == "i" || svec[i] == "I") {
			result.push_back(OGRFieldType::OFTInteger);
		}
		else {
			result.push_back(OGRFieldType::OFTString);
		}
	}
	return result;
}

inline vector<OGRFieldType> generateFTypesFromHeaders(vector<string> headers) {
	vector<OGRFieldType> dtypes;
	for (int i = 0; i < headers.size(); ++i)
		dtypes.push_back(OGRFieldType::OFTReal);
	return dtypes;
}

inline std::string generateTimestampedFileName() {
	std::stringstream buf;
	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(p);
	std::tm tm = *std::localtime(&t);
	buf << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S.shp");
	return buf.str();
}

