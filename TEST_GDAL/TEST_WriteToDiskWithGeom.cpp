// TEST_GDAL.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#pragma warning(disable : 4996)
#include <iostream>
#include <ogrsf_frmts/ogrsf_frmts.h>

int main3000()
{
	const char *pszDriverName = "ESRI Shapefile";
	GDALDriver *poDriver;

	GDALAllRegister();

	poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)
	{
		printf("%s driver not available.\n", pszDriverName);
		exit(1);
	}

	GDALDataset *poDataSet;

	poDataSet = poDriver->Create("coco.shp", 0, 0, 0, GDT_Unknown, NULL);
	if (poDataSet == NULL)
	{
		printf("Creation of output file failed.\n");
		exit(1);
	}
	
	OGRGeomFieldDefn oPointField("PointField", wkbPoint);
	OGRSpatialReference* poSRS = new OGRSpatialReference();
	poSRS->importFromEPSG(4326);
	oPointField.SetSpatialRef(poSRS);
	OGRLayer *poLayer;
	poLayer = poDataSet->CreateLayer("coco", poSRS, wkbPoint, NULL);
	poSRS->Release();
	
	

	OGRFeature *poFeature, *poFeature2;
	OGRGeometry *poGeometry, *poGeometry2;
	char *pszWKT, *pszWKT2;

	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
	poFeature2 = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

	OGRFieldDefn oField("e_WS", OGRFieldType::OFTReal);
	poLayer->CreateField(&oField);
	poFeature->SetField("e_WS", 522.57);
	poFeature2->SetField("e_WS", 522.57);
	printf("Number of fields: %d.\n", poFeature->GetFieldCount());


	pszWKT = (char*) "POINT (-2.514557 42.444437)";
	pszWKT2 = (char*) "POINT (-2.515 42.445)";
	OGRGeometryFactory::createFromWkt(&pszWKT, NULL, &poGeometry);
	OGRGeometryFactory::createFromWkt(&pszWKT2, NULL, &poGeometry2);
	poFeature->SetGeomFieldDirectly(0, poGeometry);
	poFeature2->SetGeomFieldDirectly(0, poGeometry2);


	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
	{
		printf("Failed to create feature.\n");
		exit(1);
	}
	if (poLayer->CreateFeature(poFeature2) != OGRERR_NONE)
	{
		printf("Failed to create feature.\n");
		exit(1);
	}

	//If Features are destroyed manually program crashes iif additional fields are present. Library bug?
	//OGRFeature::DestroyFeature(poFeature);
	//OGRFeature::DestroyFeature(poFeature2);
	GDALClose(poDataSet);
}