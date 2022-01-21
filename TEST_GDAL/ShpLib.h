#pragma once
//#include <msclr/marshal_cppstd.h>
#include "ShpFileWriter.h"

bool isinit = false;
ShpFileWriter s;
vector<string> headers;

extern "C" {
	void createWriter() {
		headers.push_back("IMG_NAME");
		headers.push_back("DIST_CM");
		headers.push_back("HEADING");
		headers.push_back("FIX_STATUS");
		s.setHeaders(headers);
		s.setFieldDatatypes("s,i,d,i");
		isinit = true;
	}
	__declspec(dllexport) void __stdcall WriteImageNameDistance(std::string shppath, double X, double Y, std::string imagename, int distance) {
		if (!isinit)
			createWriter();
		s.initAppend(shppath);
		s.writeCStringIntEfficient(imagename.c_str(), distance, X, Y);
	}

	__declspec(dllexport) void __stdcall WriteNameDistanceGPS(std::string shppath, double X, double Y, std::string imagename, int distance, 
		double heading, unsigned int fixStatus) {
		if (!isinit)
			createWriter();
		s.initAppend(shppath);
		s.writeCStringIntGPSEfficient(imagename.c_str(), distance, X, Y, heading, fixStatus);
	}

}