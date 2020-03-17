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
		void initAppend();

		void setHeaders(std::vector<std::string>);
		void setFieldDatatypes(std::vector<OGRFieldType>);

		void writeSingleValue(double val, double x, double y);
		void writeMultiValue(std::string valAsCsv, double x, double y);

		static std::vector<std::string> tokenize(std::string s) {
			//stringstream ss("1,1,1,1, or something else ,1,1,1,0");
			stringstream ss(s);
			vector<string> result;

			while (ss.good())
			{
				string substr;
				getline(ss, substr, ',');
				result.push_back(substr);
			}
		}

};

