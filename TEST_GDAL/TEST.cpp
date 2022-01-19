#include "ShpFileWriter.h"
#include<thread>

inline double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

int main() {
	{
		//Generar un archivo nuevo con una sola variable
		ShpFileWriter s;
		vector<string> headers;
		headers.push_back("var1");
		s.setHeaders(headers);
		s.setFieldDatatypes("d");
		s.init();
		for (int i = 0; i < 1000; ++i) {
			s.writeSingleValue(fRand(0, 100), -2.514276 + fRand(-0.001, 0.001), 42.444090 + fRand(-0.001, 0.001));
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	string previousFile = "";
	{
		//Generar un archivo nuevo con varias variables
		ShpFileWriter s;
		vector<string> headers = {"var1","var2","var3"};
		s.setHeaders(headers);
		s.setFieldDatatypes("d,d,d");
		s.init();
		s.writeMultiValue("0,0,0",-2.514276, 42.444090);
		/*
		for (int i = 0; i < 10; ++i) {
			s.writeMultiValue(
				to_string(fRand(0, 1))+",2,3",
				-2.514276 + fRand(-0.001, 0.001),
				42.444090 + fRand(-0.001, 0.001)
			);
		}
		*/
		previousFile = s.getCurrentFilePath();
	}
	{
		//Añadir datos a un archivo existente
		ShpFileWriter s;
		s.initAppend(previousFile);
		for (int i = 0; i < 10; ++i) {
			s.writeMultiValue(
				to_string(fRand(10, 20)) + ",2,3",
				-2.514276 + fRand(-0.001, 0.001),
				42.444090 + fRand(-0.001, 0.001)
			);
		}
	}
}