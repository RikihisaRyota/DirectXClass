#include "CompileShader.h"

#include "ConvertString.h"

using namespace Microsoft::WRL;

Microsoft::WRL::ComPtr<IDxcUtils> CompileShader::dxcUtils_;
Microsoft::WRL::ComPtr<IDxcCompiler3> CompileShader::dxcCompiler_;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> CompileShader::includeHandler_;

void CompileShader::Initialize() {
	HRESULT hr = S_FALSE;
	// dxcCompiler��������
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// �����_��include�͂��Ȃ����Ainclude�ɑΉ����邽�߂̐ݒ���s���Ă���
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

IDxcBlob* CompileShader::Compile(const std::wstring& filePath, const wchar_t* profile) {
	//���ꂩ��V�F�[�_�[���R���p�C������|�����O�ɏo��
	Log(ConvertString(std::format(L"Begin CompilerShader, Path:{},profile:{}\n", filePath, profile)));
	//hlsl�t�@�C����ǂݍ���
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//�ǂ߂Ȃ�������~�߂�
	assert(SUCCEEDED(hr));
	//�ǂݍ��񂾃t�@�C���̓��e��ݒ肷��
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8�̕����R�[�h

	LPCWSTR arguments[] = {
		filePath.c_str(),//�R���p�C���Ώۂ�hlsl�t�@�C����
		L"-E",L"main",//�G���g���[�|�C���g�̎w��B��{�I��main�ȊO�͂��Ȃ�
		L"-T",profile,//ShaderProfile�̐ݒ�
		L"-Zi",L"-Qembed_debug",//�f�o�b�N�p�̏��𖄂ߍ���
		L"-Od",//�œK�����O���Ă���
		L"-Zpr",//���������C�A�E�g�͍s�D��
	};
	//���ۂ�Shader���R���p�C������
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,//�ǂݍ��񂾃t�@�C��
		arguments,//�R���p�C���I�v�V����
		_countof(arguments),//�R���p�C���I�v�V�����̐�
		includeHandler_.Get(),//include���܂܂ꂽ���X
		IID_PPV_ARGS(&shaderResult)//�R���p�C������
	);
	//�R���p�C���G���[�ł͂Ȃ�dxc���N���ł��Ȃ��Ȃǒv���I�ȏ�
	assert(SUCCEEDED(hr));

	//�x���A�G���[���o���烍�O���o���Ď~�߂�
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//�x���A�G���[�_��
		assert(false);
	}
	//�R���p�C�����ʂ�����s�p�̃o�C�i���������擾
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//�����������O���o��
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//�����g��Ȃ����\�[�X�����
	shaderSource->Release();
	shaderResult->Release();
	//���s�p�̃o�C�i����ԋp
	return shaderBlob;
}