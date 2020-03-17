#include "ShpFileWriter.h"

int main() {
	ShpFileWriter s;
	char cebollas[100];
	//std::vector<std::string> headers;
	//headers.push_back("watrStress");
	//s.setHeaders(headers);
	//s.init();
	s.initAppend();
	s.writeSingleValue(27.05, -2.523420, 42.451389);

}