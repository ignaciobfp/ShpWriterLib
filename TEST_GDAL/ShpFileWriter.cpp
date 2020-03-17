#include "ShpFileWriter.h"

ShpFileWriter::ShpFileWriter()
{
	shp_path = "test.shp";
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
	poSRS->Release();
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

	for (int i = 0; i < headers.size(); ++i) {
		OGRFieldDefn oField(headers[i].c_str(), OGRFieldType::OFTReal);
		poLayer->CreateField(&oField);
	}
}

void ShpFileWriter::initAppend()
{
	GDALAllRegister();
	poDataSet = (GDALDataset*)GDALOpenEx(shp_path.c_str(), GDAL_OF_VECTOR + GDAL_OF_UPDATE, NULL, NULL, NULL);
	poLayer = poDataSet->GetLayer(0); 
	auto *poFeature = poLayer->GetFeature(0);
	headers.clear();
	for (auto && oField: *poFeature) {
		headers.push_back(oField.GetName());
	}
}

void ShpFileWriter::setHeaders(std::vector<std::string> inh)
{
	headers = inh;
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

}

void ShpFileWriter::writeVector()
{
}
