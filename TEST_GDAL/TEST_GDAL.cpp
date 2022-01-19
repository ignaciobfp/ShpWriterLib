// TEST_GDAL.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#pragma warning(disable : 4996)
#include <iostream>
#include <ogrsf_frmts/ogrsf_frmts.h>

int main500()
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

	poDataSet = poDriver->Create("point_out.shp", 0, 0, 0, GDT_Unknown, NULL);
	if (poDataSet == NULL)
	{
		printf("Creation of output file failed.\n");
		exit(1);
	}

	OGRLayer *poLayer;

	poLayer = poDataSet->CreateLayer("point_out", NULL, wkbPoint, NULL);
	if (poLayer == NULL)
	{
		printf("Layer creation failed.\n");
		exit(1);
	}

	OGRFieldDefn oField("Name", OFTString);

	oField.SetWidth(32);

	if (poLayer->CreateField(&oField) != OGRERR_NONE)
	{
		printf("Creating Name field failed.\n");
		exit(1);
	}

	double x, y;
	char szName[33];

	while (!feof(stdin)
		&& fscanf(stdin, "%lf,%lf,%32s", &x, &y, szName) == 3)
	{
		OGRFeature *poFeature;

		poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		poFeature->SetField("Name", szName);

		OGRPoint pt;

		pt.setX(x);
		pt.setY(y);

		poFeature->SetGeometry(&pt);

		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
		{
			printf("Failed to create feature in shapefile.\n");
			exit(1);
		}

		OGRFeature::DestroyFeature(poFeature);
	}

	GDALClose(poDataSet);
}