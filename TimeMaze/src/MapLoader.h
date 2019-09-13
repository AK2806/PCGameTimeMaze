#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <memory>
#include <rapidjson/document.h>
#include "Utilities.h"

class MapLoader final {
public:
	MapLoader();
	~MapLoader();

	bool loadMapJSON(const char* filename);
	bool initScene(const std::shared_ptr<Scene>& scene);
private:
	rapidjson::Document* m_document;
};

#endif