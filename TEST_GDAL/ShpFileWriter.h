#pragma once
#include<iostream>
#include<sstream>
#include "ogrsf_frmts.h"
using namespace std;

class ShpFileWriter{
	private:
		std::string shp_path;
		const char *pszDriverName = "ESRI Shapefile";
		GDALDriver *poDriver;
		GDALDataset *poDataSet;
		OGRLayer *poLayer;
		OGRSpatialReference *poSRS;
		std::vector<std::string> headers;
		std::vector<OGRFieldType> fieldTypes;
		//std::vector<char> datatypes;
		int numberofvars;

		inline void generateWKT(double x, double y, char *sWKT) {
			//Example WKT string: POINT(-2.514557 42.444437)
			//Expected max size 51: 6 "point(" + 1 sign + 2 digits + 1 dot + 17 decimals + 1 space + 1 sign + 2 digits + 1 dot + 17 decimals + 1 ")" + 1 \0
			snprintf(sWKT, 55, "POINT(%.17f %.17f)", x, y);
			return;
		}

	public:
		ShpFileWriter();
		ShpFileWriter(char* path);
		~ShpFileWriter();

		void init();
		void init(vector<string> headers); //If no types are specified, we will assume every field is a double
		void init(vector<string> headers, vector<OGRFieldType> dataTypes);
		void init(vector<string> headers, string dataTypesString);
		void initAppend();

		void setHeaders(std::vector<std::string>);
		void setFieldDatatypes(std::vector<OGRFieldType>);
		void setFieldDatatypes(std::string);

		void writeSingleValue(double val, double x, double y);
		void writeMultiValue(std::string valAsCsv, double x, double y);

		static vector<string> tokenize(string s) {
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

		static vector<OGRFieldType> getFieldTypesFromString(string s) {
			vector<string> s = tokenize(s);
			vector<OGRFieldType> result;
			for (int i = 0; i < s.size(); ++i) {
				if (s == "d" || s == "D" || s == "r" || s == "R") {
					result.push_back(OGRFieldType::OFTReal);
				}
				else if (s == "i" || s == "I") {
					result.push_back(OGRFieldType::OFTInteger);
				}
				else {
					result.push_back(OGRFieldType::OFTString);
				}
			}
			return result;
		}

};

