#include "Audio.h"

#include <cassert>

void Audio::Initialize() {
	HRESULT result;
	// XAudioエンジンのインスタンスを作成
	result = XAudio2Create(&xAudio2,0,XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	// マスターボイスを作成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

void Audio::SoundPlayWave(const SoundData& soundData) {
	HRESULT result;

	// 波形フォーマットを先にSourceVoiceの作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice,&soundData.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

Audio::SoundData Audio::SoundLoadWave(const char* filename) {
#pragma region ファイルオープン
	// ファイル入出ストリームのインスタンス
	std::ifstream file;
	// wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());
#pragma endregion
#pragma region wavデータ読み込み
	// RIFFヘッターの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFがチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEがチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッターの確認
	file.read((char*)&format, sizeof(ChunkHearder));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size<=sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Dataチャンクの読み込み
	ChunkHearder data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();
#pragma endregion
#pragma region 読み込んだ音声データのreturn
	// returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
#pragma endregion
	return soundData;
}

void Audio::SoundUnload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}
