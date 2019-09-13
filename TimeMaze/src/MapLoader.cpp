#include "MapLoader.h"
#include <cstdio>

using namespace std;
using namespace rapidjson;

MapLoader::MapLoader() :
	m_document(nullptr)
{
}

MapLoader::~MapLoader()
{
	if (this->m_document)
		delete this->m_document;
}

bool MapLoader::loadMapJSON(const char * filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp) return false;
	fseek(fp, 0, SEEK_END);
	size_t length = ftell(fp);
	char* json = new char[length + 1];
	rewind(fp);
	fread(json, 1, length, fp);
	json[length] = '\0';
	fclose(fp);
	this->m_document = new Document;
	this->m_document->Parse(json);
	delete[] json;
	return true;
}

bool MapLoader::initScene(const shared_ptr<Scene>& scene)
{
	if (!this->m_document) return false;
	if (!this->m_document->IsArray()) return false;

	return true;
}
