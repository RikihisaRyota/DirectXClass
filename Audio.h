#pragma once

#include <fstream>
#include <xaudio2.h>
#include <wrl.h>
#include <vector>

#pragma comment(lib,"xaudio2.lib")

class Audio {
public:
	// チャンクヘッダ
	struct ChunkHearder {
		char id[4]; // チャンク毎のID
		int32_t size; // チャンクファイル
	};
	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHearder chunk; // "RIFF"
		char type[4]; // "WAVE"
	};
	// FMTチャンク
	struct FormatChunk {
		ChunkHearder chunk; // "fmt"
		WAVEFORMATEX fmt; // 波形フォーマット
	};
	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		uint32_t bufferSize;
		// 音声再生用の SourceVoice
		IXAudio2SourceVoice* pSourceVoice;
	};
public:
	static Audio* GetInstance();
public:
	void Initialize();
	/// <summary>
	/// 音声データ解放
	/// </summary>
	/// <param name="soundData"></param>
	void SoundUnload(size_t soundHandle);
	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	void SoundPlayWave(size_t soundHandle);

	void SoundPlayLoopStart(size_t soundHandle);
	void SoundPlayLoopEnd(size_t soundHandle);
	/// <summary>
	/// 音声ロード
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	size_t SoundLoadWave(const char* filename);
private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	std::vector<SoundData> soundHandle_;
};

