#pragma once
#include "stdafx.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"
#include "NMR_DLLInterfaces.h"
using namespace ParaEngine;
using namespace NMR;
class Exporter3mf {
public:
	Exporter3mf();
	~Exporter3mf();
	bool check();
	int save(NPLInterface::NPLObjectProxy& renderList, std::string filename);
private:
	MODELMESHVERTEX fnCreateVertex(float x, float y, float z);
	MODELMESHTRIANGLE fnCreateTriangle(int v0, int v1, int v2);
	MODELMESHCOLOR_SRGB fnCreateColor(unsigned char red, unsigned char green, unsigned char blue);
	MODELTRANSFORM createTranslationMatrix(float x, float y, float z);

	void read(NPLInterface::NPLObjectProxy& renderList, std::vector<MODELMESHVERTEX>& vertices, std::vector<MODELMESHTRIANGLE>& triangles, std::vector<MODELMESH_TRIANGLECOLOR_SRGB>& colors);
	int createMesh(PLib3MFModel * model, std::vector<MODELMESHVERTEX>& vertices, std::vector<MODELMESHTRIANGLE>& triangles, std::vector<MODELMESH_TRIANGLECOLOR_SRGB>& colors);
};