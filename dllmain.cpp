// dllmain.cpp : Define el punto de entrada de la aplicación DLL.
#include <windows.h>
#include "ShpFileWriter.h"
using namespace System;

void initShp()
{
	//if ((path == NULL)|| (path[0] == '\0'))
	
	//char* pathToCurrentFolder = (char*) malloc(MAX_PATH);
	//strcpy(pathToCurrentFolder,getCurrentProcessDirectory().u8string().c_str());
	//printf(pathToCurrentFolder);
	//setenv("PROJ_LIB", "C:\\Users\\Televitis\\source\\repos\\ShpLib_x86\\Debug", true);
	//setenv("PROJ_LIB","",true);
	const char *ret = getenv("PROJ_LIB");
	if (ret == NULL) {
		string text = getCurrentProcessDirectory().string();
		wchar_t wtext[MAX_PATH];
		mbstowcs(wtext, text.c_str(), text.length()+1);//plus null
		LPWSTR ptr = wtext;

		SetPermanentEnvironmentVariable(L"PROJ_LIB", ptr);
	}
	//free(pathToCurrentFolder);
	if (shp_path.empty()) {
		shp_path = generateTimestampedFileName();
	}
	poSRS = new OGRSpatialReference();
	poSRS->importFromEPSG(4326);
	GDALAllRegister();
	poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)
	{
		printf("%s driver not available.\n", pszDriverName);
		exit(1);
	}

	poDataSet = poDriver->Create(shp_path.c_str(), 0, 0, 0, GDT_Unknown, NULL);
	if (poDataSet == NULL)
	{
		printf("Creation of output file failed.\n");
		exit(1);
	}

	OGRGeomFieldDefn oPointField("PointField", wkbPoint);
	oPointField.SetSpatialRef(poSRS);
	poLayer = poDataSet->CreateLayer("points", poSRS, wkbPoint, NULL);

	if (fieldTypes.size() != headers.size()) {
		setFieldDatatypes(generateFTypesFromHeaders(headers));
	}

	for (int i = 0; i < headers.size(); ++i) {
		OGRFieldDefn oField(headers[i].c_str(), fieldTypes[i]);
		poLayer->CreateField(&oField);
	}
}

void initFromHeaders(vector<string> headers)
{
	setHeaders(headers);
	setFieldDatatypes(generateFTypesFromHeaders(headers));
	initShp();
}

void initFromHeadersDT(vector<string> headers, vector<OGRFieldType> dataTypes)
{
	setHeaders(headers);
	setFieldDatatypes(dataTypes);
	initShp();
}

void initFromHeadersDTstring(vector<string> headers, string dataTypesString)
{
	setHeaders(headers);
	setFieldDatatypes(dataTypesString);
	initShp();
}

void initShpAppend()
{
	poSRS = new OGRSpatialReference();
	poSRS->importFromEPSG(4326);
	GDALAllRegister();
	poDataSet = (GDALDataset*)GDALOpenEx(shp_path.c_str(), GDAL_OF_VECTOR + GDAL_OF_UPDATE, NULL, NULL, NULL);
	if (poDataSet != NULL) {
		poLayer = poDataSet->GetLayer(0);
		auto* poFeature = poLayer->GetFeature(0);
		headers.clear();
		fieldTypes.clear();
		for (auto&& oField : *poFeature) {
			headers.push_back(oField.GetName());
			fieldTypes.push_back(oField.GetType());
		}
	}
	else {
		initShp();
	}
}

void newFile()
{
	shp_path = generateTimestampedFileName();
	initShp();
}

void setHeaders(std::vector<std::string> inh)
{
	headers = inh;
}

void setFieldDatatypes(std::vector<OGRFieldType> indt)
{
	fieldTypes = indt;
}

void setFieldDatatypes(std::string s)
{
	fieldTypes = getFieldTypesFromString(s);
}

void writeSingleValue(double val, double x, double y)
{
	OGRFeature* poFeature;
	OGRGeometry* poGeometry;
	char sWKT[60];

	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

	for (auto i = 0; i < headers.size(); ++i) {
		poFeature->SetField(headers[i].c_str(), val);
	}

	generateWKT(x, y, sWKT);
	OGRGeometryFactory::createFromWkt(sWKT, NULL, &poGeometry);
	poFeature->SetGeomFieldDirectly(0, poGeometry);


	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
		printf("Failed to create feature.\n");
	}

	poDataSet->FlushCache();
}

void writeSingleValueString(const char* val, double x, double y)
{
	OGRFeature* poFeature;
	OGRGeometry* poGeometry;
	char sWKT[60];

	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

	for (auto i = 0; i < headers.size(); ++i) {
		poFeature->SetField(headers[i].c_str(), val);
	}

	generateWKT(x, y, sWKT);
	OGRGeometryFactory::createFromWkt(sWKT, NULL, &poGeometry);
	poFeature->SetGeomFieldDirectly(0, poGeometry);


	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
		printf("Failed to create feature.\n");
	}

	poDataSet->FlushCache();
}

void writeCStringIntEfficient(const char* curr_string, int curr_int, double x, double y)
{
	OGRFeature* poFeature;
	OGRGeometry* poGeometry;
	char sWKT[60];

	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

	if (fieldTypes.size() != 2)
		return;

	for (auto i = 0; i < fieldTypes.size(); ++i) {
		switch (fieldTypes[i]) {
		case OGRFieldType::OFTInteger:
			poFeature->SetField(headers[i].c_str(), curr_int);
			break;
		case OGRFieldType::OFTReal:
			poFeature->SetField(headers[i].c_str(), curr_int);
			break;
		default:
			//If no compatible type is available, try to store the field as string
			poFeature->SetField(headers[i].c_str(), curr_string);
			break;
		}
	}

	generateWKT(x, y, sWKT);
	OGRGeometryFactory::createFromWkt(sWKT, NULL, &poGeometry);
	poFeature->SetGeomFieldDirectly(0, poGeometry);


	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
		printf("Failed to create feature.\n");
	}

	poDataSet->FlushCache();
}

void writeCStringIntGPSEfficient(const char* curr_string, int dist, double x, double y, double heading, int fixStatus)
{
	OGRFeature* poFeature;
	OGRGeometry* poGeometry;
	char sWKT[60];

	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

	if (fieldTypes.size() != 4)
		return;

	poFeature->SetField(headers[0].c_str(), curr_string);
	poFeature->SetField(headers[1].c_str(), dist);
	poFeature->SetField(headers[2].c_str(), heading);
	poFeature->SetField(headers[3].c_str(), fixStatus);

	generateWKT(x, y, sWKT);
	OGRGeometryFactory::createFromWkt(sWKT, NULL, &poGeometry);
	poFeature->SetGeomFieldDirectly(0, poGeometry);


	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
		printf("Failed to create feature.\n");
	}

	poDataSet->FlushCache();
}

void writeMultiValue(std::string valAsCsv, double x, double y)
{
	OGRFeature* poFeature;
	OGRGeometry* poGeometry;
	char sWKT[60];

	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

	vector<string> valAsVector = tokenize(valAsCsv);

	if (fieldTypes.size() != valAsVector.size())
		return;

	for (auto i = 0; i < headers.size(); ++i) {
		switch (fieldTypes[i]) {
		case OGRFieldType::OFTInteger:
			poFeature->SetField(headers[i].c_str(), std::stoi(valAsVector[i]));
			break;
		case OGRFieldType::OFTReal:
			poFeature->SetField(headers[i].c_str(), std::stod(valAsVector[i]));
			break;
		default:
			//If no compatible type is available, try to store the field as string
			poFeature->SetField(headers[i].c_str(), valAsVector[i].c_str());
			break;
		}
	}

	generateWKT(x, y, sWKT);
	OGRGeometryFactory::createFromWkt(sWKT, NULL, &poGeometry);
	poFeature->SetGeomFieldDirectly(0, poGeometry);


	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
		printf("Failed to create feature.\n");
	}

	poDataSet->FlushCache();
}


extern "C" {

	__declspec(dllexport) int __stdcall custom_sum_test(int a, int b) {
		return a + b;
	}

	__declspec(dllexport) void __stdcall initWriter() {
		//Inicializar con malloc:
		//void* a = malloc(sizeof(ShpFileWriter));
		//ShpFileWriter* obj = new(a) ShpFileWriter();


	}

	__declspec(dllexport) void __stdcall WriteImageNameDistance(char* shppath, double X, double Y, char* imagename, int distance) {
		vector<string> headers = { "IMG_NAME", "DIST_CM"};
		setHeaders(headers);
		vector<OGRFieldType> dt = { OGRFieldType::OFTString,OGRFieldType::OFTInteger };
		setFieldDatatypes(dt);

		shp_path = shppath;
		initShpAppend();
		writeCStringIntEfficient(imagename, distance, X, Y);
	}

	__declspec(dllexport) void __stdcall WriteNameDistanceGPS(char* shppath, double X, double Y, char* imagename, int distance,
		double heading, int fixStatus) {
		vector<string> headers = { "IMG_NAME", "DIST_CM", "HEADING", "FIX_STATUS" };
		setHeaders(headers);
		//setFieldDatatypes("s,i,d,i");
		vector<OGRFieldType> dt = { OGRFieldType::OFTString,OGRFieldType::OFTInteger,OGRFieldType::OFTReal,OGRFieldType::OFTInteger };
		setFieldDatatypes(dt);

		shp_path = shppath;
		initShpAppend();
		writeCStringIntGPSEfficient(imagename, distance, X, Y, heading, fixStatus);
	}

}

inline double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

int main() {
	setenv("PROJ_LIB", "C:\\Users\\Televitis\\source\\repos\\ShpLib_x86\\Debug", true);
#if _DEBUG
	{
		//Generar un archivo nuevo con una sola variable
		vector<string> headers;
		headers.push_back("var1");
		setHeaders(headers);
		setFieldDatatypes("d");
		initShp();
		for (int i = 0; i < 1000; ++i) {
			writeSingleValue(fRand(0, 100), -2.514276 + fRand(-0.001, 0.001), 42.444090 + fRand(-0.001, 0.001));
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	string previousFile = "";
	{
		//Generar un archivo nuevo con varias variables
		vector<string> headers = { "var1","var2","var3" };
		setHeaders(headers);
		setFieldDatatypes("d,d,d");
		shp_path.clear();
		initShp();
		for (int i = 0; i < 10; ++i) {
			writeMultiValue(
				to_string(fRand(0, 1)) + ",2,3",
				-2.514276 + fRand(-0.001, 0.001),
				42.444090 + fRand(-0.001, 0.001)
			);
		}
		previousFile = shp_path;
	}
	{
		//Añadir datos a un archivo existente
		initShpAppend();
		for (int i = 0; i < 10; ++i) {
			writeMultiValue(
				to_string(fRand(10, 20)) + ",2,3",
				-2.514276 + fRand(-0.001, 0.001),
				42.444090 + fRand(-0.001, 0.001)
			);
		}
	}
	//Test escribir con funciones de GPS (ligeramente más eficiente para su caso de uso particular)
	for (int i = 0; i < 100; ++i) {
		WriteNameDistanceGPS("test.shp", -2.514276 + fRand(-0.001, 0.001), 42.444090 + fRand(-0.001, 0.001), "coco.png", fRand(90, 150), fRand(0, 2 * M_PI), 1);
	}


#endif

	return 0;
}