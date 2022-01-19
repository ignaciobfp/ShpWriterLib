#include "ShpFileWriter.h"

ShpFileWriter::ShpFileWriter()
{
	shp_path = generateTimestampedFileName();
	poSRS = new OGRSpatialReference();
	poSRS->importFromEPSG(4326);
}

ShpFileWriter::ShpFileWriter(char * path)
{
	shp_path = path;
	poSRS = new OGRSpatialReference();
	poSRS->importFromEPSG(4326);
}

ShpFileWriter::~ShpFileWriter()
{
}

string ShpFileWriter::getCurrentFilePath()
{
	return string(shp_path);
}

void ShpFileWriter::init()
{
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
		OGRFieldDefn oField(headers[i].c_str(), OGRFieldType::OFTReal);
		poLayer->CreateField(&oField);
	}
}

void ShpFileWriter::init(vector<string> headers)
{
	this->setHeaders(headers);
	this->setFieldDatatypes(generateFTypesFromHeaders(headers));
	init();
}

void ShpFileWriter::init(vector<string> headers, vector<OGRFieldType> dataTypes)
{
	this->setHeaders(headers);
	this->setFieldDatatypes(dataTypes);
	init();
}

void ShpFileWriter::init(vector<string> headers, string dataTypesString)
{
	this->setHeaders(headers);
	this->setFieldDatatypes(dataTypesString);
}

void ShpFileWriter::initAppend()
{
	GDALAllRegister();
	poDataSet = (GDALDataset*)GDALOpenEx(shp_path.c_str(), GDAL_OF_VECTOR + GDAL_OF_UPDATE, NULL, NULL, NULL);
	if (poDataSet != NULL) {
		poLayer = poDataSet->GetLayer(0);
		auto *poFeature = poLayer->GetFeature(0);
		headers.clear();
		fieldTypes.clear();
		for (auto && oField : *poFeature) {
			headers.push_back(oField.GetName());
			fieldTypes.push_back(oField.GetType());
		}
	}
	else {
		init();
	}
}

void ShpFileWriter::initAppend(string path)
{
	shp_path = path;
	initAppend();
}

void ShpFileWriter::newFile()
{
	shp_path = generateTimestampedFileName();
	init();
}

void ShpFileWriter::setHeaders(std::vector<std::string> inh)
{
	headers = inh;
}

void ShpFileWriter::setFieldDatatypes(std::vector<OGRFieldType> indt)
{
	fieldTypes = indt;
}

void ShpFileWriter::setFieldDatatypes(std::string s)
{
	fieldTypes = getFieldTypesFromString(s);
}

void ShpFileWriter::writeSingleValue(double val, double x, double y)
{
	OGRFeature *poFeature;
	OGRGeometry *poGeometry;
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

void ShpFileWriter::writeSingleValueString(char* val, double x, double y)
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

void ShpFileWriter::writeMultiValue(std::string valAsCsv, double x, double y)
{
	OGRFeature *poFeature;
	OGRGeometry *poGeometry;
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
