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
	uint32_t GetBlockTextureHandle(uint32_t num) { blockModels_.at(num)->GetMaterial(0)->GetTextureHandle(); }
private:
	std::map<std::string, Model*> models_;
	std::vector<Model*> blockModels_;
};

