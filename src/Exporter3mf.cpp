#include "Exporter3mf.h"



Exporter3mf::Exporter3mf()
{

}

Exporter3mf::~Exporter3mf()
{

}

void Exporter3mf::parse(NPLInterface::NPLObjectProxy renderList)
{
	for (NPLInterface::NPLTable::IndexIterator_Type itCur = renderList.index_begin(), itEnd = renderList.index_end(); itCur != itEnd; ++itCur)
	{
		NPLInterface::NPLObjectProxy& value = itCur->second;
		NPLInterface::NPLObjectProxy& vertices = value["vertices"];
		NPLInterface::NPLObjectProxy& normals = value["normals"];
		NPLInterface::NPLObjectProxy& colors = value["colors"];
		NPLInterface::NPLObjectProxy& indices = value["indices"];
		NPLInterface::NPLObjectProxy& matrix = value["world_matrix"];


		std::vector<MODELMESHVERTEX> input_vertices;
		std::vector<MODELMESHTRIANGLE> input_triangles;
		std::vector<unsigned int> input_indices;
		std::vector<MODELMESHCOLOR_SRGB> input_colors_vertices;
		std::vector<MODELMESH_TRIANGLECOLOR_SRGB> input_colors_triangles;

		for (NPLInterface::NPLTable::IndexIterator_Type vCur = vertices.index_begin(), vEnd = vertices.index_end(); vCur != vEnd; ++vCur)
		{
			NPLInterface::NPLObjectProxy& vertex = vCur->second;
			input_vertices.push_back(fnCreateVertex((float)(double)vertex[1], (float)(double)vertex[2], (float)(double)vertex[3]));
		}
		
		for (NPLInterface::NPLTable::IndexIterator_Type cCur = colors.index_begin(), cEnd = colors.index_end(); cCur != cEnd; ++cCur)
		{
			NPLInterface::NPLObjectProxy& color = cCur->second;
			input_colors_vertices.push_back(fnCreateColor((255 * (double)color[1]), (255 * (double)color[2]), (255 * (double)color[3])));
		}

		for (NPLInterface::NPLTable::IndexIterator_Type iCur = indices.index_begin(), iEnd = indices.index_end(); iCur != iEnd; ++iCur)
		{
			unsigned int index = (unsigned int)(double)iCur->second - 1;
			input_indices.push_back(index);

		}
		for (size_t i = 0; i < input_indices.size(); i+=3)
		{
			int index1 = input_indices[i];
			int index3 = input_indices[i+1];
			int index2 = input_indices[i+2];
			input_triangles.push_back(fnCreateTriangle(index1, index2, index3));

			MODELMESHCOLOR_SRGB c1 = input_colors_vertices[index1];
			MODELMESHCOLOR_SRGB c2 = input_colors_vertices[index2];
			MODELMESHCOLOR_SRGB c3 = input_colors_vertices[index3];

			MODELMESH_TRIANGLECOLOR_SRGB color;
			color.m_Colors[0] = c1;
			color.m_Colors[1] = c2;
			color.m_Colors[2] = c3;
			input_colors_triangles.push_back(color);
		}
		createModel(input_vertices,input_triangles, input_colors_triangles);
	}
}

NMR::MODELMESHVERTEX Exporter3mf::fnCreateVertex(float x, float y, float z)
{
	MODELMESHVERTEX result;
	result.m_fPosition[0] = x;
	result.m_fPosition[1] = y;
	result.m_fPosition[2] = z;
	return result;
}

NMR::MODELMESHTRIANGLE Exporter3mf::fnCreateTriangle(int v0, int v1, int v2)
{
	MODELMESHTRIANGLE result;
	result.m_nIndices[0] = v0;
	result.m_nIndices[1] = v1;
	result.m_nIndices[2] = v2;
	return result;
}

NMR::MODELMESHCOLOR_SRGB Exporter3mf::fnCreateColor(unsigned char red, unsigned char green, unsigned char blue)
{
	MODELMESHCOLOR_SRGB result;
	result.m_Red = red;
	result.m_Green = green;
	result.m_Blue = blue;
	result.m_Alpha = 255;
	return result;
}

NMR::MODELTRANSFORM Exporter3mf::createTranslationMatrix(float x, float y, float z)
{
	MODELTRANSFORM mMatrix;
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 3; j++) {
			mMatrix.m_fFields[j][i] = (i == j) ? 1.0f : 0.0f;
		}
	}

	mMatrix.m_fFields[0][3] = x;
	mMatrix.m_fFields[1][3] = y;
	mMatrix.m_fFields[2][3] = z;

	return mMatrix;
}

int Exporter3mf::createModel(std::vector<MODELMESHVERTEX>& vertices, std::vector<MODELMESHTRIANGLE>& triangles, std::vector<MODELMESH_TRIANGLECOLOR_SRGB>& colors)
{
	// General Variables
	HRESULT hResult;
	DWORD nErrorMessage;
	LPCSTR pszErrorMessage;

	// Objects
	PLib3MFModel * pModel;
	PLib3MFModelWriter * p3MFWriter;
	PLib3MFModelMeshObject * pMeshObject;
	PLib3MFModelBuildItem * pBuildItem;
	PLib3MFPropertyHandler * pPropertyHandler;

	// Create Model Instance
	hResult = lib3mf_createmodel(&pModel, true);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create model: " << std::hex << hResult << std::endl;
		return -1;
	}

	// Create Mesh Object
	hResult = lib3mf_model_addmeshobject(pModel, &pMeshObject);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not add mesh object: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pModel);
		return -1;
	}

	hResult = lib3mf_object_setnameutf8(pMeshObject, "Colored Box");
	if (hResult != LIB3MF_OK) {
		std::cout << "could not set object name: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return -1;
	}

	hResult = lib3mf_meshobject_setgeometry(pMeshObject, &vertices[0], vertices.size(), &triangles[0], triangles.size());
	if (hResult != LIB3MF_OK) {
		std::cout << "could not set mesh geometry: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return -1;
	}

	// Create color entries for cube
	hResult = lib3mf_meshobject_createpropertyhandler(pMeshObject, &pPropertyHandler);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create property handler: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return -1;
	}
	// Set colors
	for (int i = 0; i< triangles.size(); i++)
	{
		lib3mf_propertyhandler_setgradientcolor(pPropertyHandler, i, &colors[i]);
	}
	// release property handler
	lib3mf_release(pPropertyHandler);


	// Add Build Item for Mesh
	hResult = lib3mf_model_addbuilditem(pModel, pMeshObject, NULL, &pBuildItem);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create build item: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return -1;
	}

	// Output cube as STL and 3MF
	lib3mf_release(pMeshObject);
	lib3mf_release(pBuildItem);

	// Create Model Writer
	hResult = lib3mf_model_querywriter(pModel, "3mf", &p3MFWriter);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create model reader: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pModel);
		return -1;
	}

	// Export Model into File
	std::cout << "writing colorcube.3mf..." << std::endl;
	hResult = lib3mf_writer_writetofile(p3MFWriter, L"colorcube.3mf");
	if (hResult != LIB3MF_OK) {
		std::cout << "could not write file: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(p3MFWriter, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pModel);
		lib3mf_release(p3MFWriter);
		return -1;
	}

	// Release Model Writer
	lib3mf_release(p3MFWriter);

	// Release Model
	lib3mf_release(pModel);

	std::cout << "done" << std::endl;
	return 0;
}

bool Exporter3mf::check()
{
	// General Variables
	HRESULT hResult;
	DWORD nInterfaceVersion;
	// Check 3MF Library Version
	hResult = lib3mf_getinterfaceversion(&nInterfaceVersion);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not get 3MF Library version: " << std::hex << hResult << std::endl;
		return false;
	}

	if ((nInterfaceVersion != NMR_APIVERSION_INTERFACE)) {
		std::cout << "invalid 3MF Library version: " << NMR_APIVERSION_INTERFACE << std::endl;
		return false;
	}
	return true;
}
