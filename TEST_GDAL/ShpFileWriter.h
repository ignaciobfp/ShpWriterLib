#pragma once
#include<iostream>
#include "ogrsf_frmts.h"

class ShpFileWriter{
	private:
		std::string shp_path;
		const char *pszDriverName = "ESRI Shapefile";
		GDALDriver *poDriver;
		GDALDataset *poDataSet;
		OGRLayer *poLayer;
		OGRSpatialReference* poSRS;
		std::vector<std::string> headers;
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

		void writeSingleValue(double val, double x, double y);
		void writeVector();

};

