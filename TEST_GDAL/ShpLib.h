#pragma once
#include <msclr/marshal_cppstd.h>
#include "ShpFileWriter.h"

bool isinit = false;
ShpFileWriter s;
vector<string> headers;

extern "C" {
	void createWriter() {
		headers.push_back("IMG_NAME");
		s.setHeaders(headers);
		s.setFieldDatatypes("s");
		isinit = true;
	}
	__declspec(dllexport) void __stdcall WriteImageNameOnly(char* shppath, double X, double Y, char* imagename) {
		if (!isinit)
			createWriter();
		s.initAppend(std::string(shppath));

		s.writeSingleValueString(imagename, X, Y);
	}

}