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
using namespace std;

std::string shp_path;
const char* pszDriverName = "ESRI Shapefile";
GDALDriver* poDriver;
GDALDataset* poDataSet;
OGRLayer* poLayer;
OGRSpatialReference* poSRS;
std::vector<std::string> headers;
std::vector<OGRFieldType> fieldTypes;

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

