#pragma once
#include <msclr/marshal_cppstd.h>
#include "ShpFileWriter.h"
#include <Windows.h>

bool isinit = false;
ShpFileWriter s;
vector<string> headers;

extern "C" {
	void createWriter() {
		headers.push_back("IMG_NAME");
		//headers.push_back("DIST_CM");
		//headers.push_back("HEADING");
		//headers.push_back("FIX_STATUS");
		s.setHeaders(headers);
		//s.setFieldDatatypes("s,i,d,i");
		s.setFieldDatatypes("s");
		isinit = true;
	}

	__declspec(dllexport) int __stdcall math_add(int a, int b) {
		return a + b;
	}

	__declspec(dllexport) int __stdcall sum_test(int a, int b) {
		return a + b;
	}

	__declspec(dllexport) void __stdcall WriteImageNameDistance(char* shppath, double X, double Y, char* imagename, int distance) {
		if (!isinit)
			createWriter();
		s.initAppend(shppath);
		s.writeCStringIntEfficient(imagename, distance, X, Y);
	}

	__declspec(dllexport) void __stdcall WriteNameDistanceGPS(char* shppath, double X, double Y, char* imagename, int distance, 
		double heading, int fixStatus) {
		if (!isinit)
			createWriter();
		s.initAppend(shppath);
		s.writeCStringIntGPSEfficient(imagename, distance, X, Y, heading, fixStatus);
	}

}