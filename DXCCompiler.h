#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>


#pragma comment(lib,"dxcompiler.lib")

class DXCCompiler {
public:
	static void Initialize();
	static IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);
private:
	// dxcCompierを初期化
	static Microsoft::WRL::ComPtr<IDxcUtils> sDxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> sDxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> sIncludeHandler_;
};

