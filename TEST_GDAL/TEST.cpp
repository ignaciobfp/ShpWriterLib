#include "ShpFileWriter.h"
#include<thread>

inline double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

int main() {
	{
		ShpFileWriter s;
		vector<string> headers;
		headers.push_back("var1");
		s.setHeaders(headers);
		s.setFieldDatatypes("d");
		s.init();
		//s.initAppend();
		for (int i = 0; i < 500; ++i) {
			s.writeSingleValue(fRand(0, 100), -2.514276 + fRand(-0.001, 0.001), 42.444090 + fRand(-0.001, 0.001));
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	string previousFile = "";
	{
		ShpFileWriter s;
		vector<string> headers;
		headers.push_back("var1");
		headers.push_back("var2");
		headers.push_back("var3");
		s.setHeaders(headers);
		s.setFieldDatatypes("d,d,d");
		s.init();
		for (int i = 0; i < 10; ++i) {
			s.writeMultiValue(to_string(fRand(0, 1))+",2,3", -2.514276 + fRand(-0.001, 0.001), 42.444090 + fRand(-0.001, 0.001));
		}
		previousFile = s.getCurrentFilePath();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	{
		ShpFileWriter s;
		s.initAppend(previousFile);
		for (int i = 0; i < 10; ++i) {
			s.writeMultiValue(to_string(fRand(1, 10)) + ",2,3", -2.514276 + fRand(-0.001, 0.001), 42.444090 + fRand(-0.001, 0.001));
		}
	}
}