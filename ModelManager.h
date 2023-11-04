#pragma once

#include <string>
#include <map>
#include <initializer_list>
#include <vector>

#include "MapChip.h"
#include "Model.h"

class ModelManager {
public:
	static ModelManager* GetInstance();
	void Shutdown();
	void LoadBlockModel(std::initializer_list<std::string> modelNames, bool isLighting = false);
	void LoadModel(std::string modeName, bool isLighting=false);
	
	Model* GetModel(std::string modeName);
	Model* GetBlockModel(size_t num) {return blockModels_[num];}
	size_t GetBlockModelSize() {return blockModels_.size();}
private:
	std::map<std::string, Model*> models_;
	std::vector<Model*> blockModels_;
};

