#include "Audio.h"

#include <cassert>

Audio* Audio::GetInstance() {
	static Audio instans;
	return &instans;
}
void Audio::Initialize() {
	HRESULT result;
	// XAudioエンジンのインスタンスを作成
	result = XAudio2Create(&xAudio2,0,XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	// マスターボイスを作成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

void Audio::SoundPlayWave(size_t soundHandle) {
	HRESULT result;
	SoundData soundData = soundHandle_.at(soundHandle);
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
	assert(SUCCEEDED(result));
}

void Audio::SoundPlayLoopStart(size_t soundHandle) {
	HRESULT result;

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundHandle_.at(soundHandle).pBuffer;
	buf.AudioBytes = soundHandle_.at(soundHandle).bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	// 波形データの再生
	result = soundHandle_.at(soundHandle).pSourceVoice->SubmitSourceBuffer(&buf);
	result = soundHandle_.at(soundHandle).pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

void Audio::SoundPlayLoopEnd(size_t soundHandle) {
	// soundHandle に対応する SourceVoice を取得
	if (soundHandle_.at(soundHandle).pSourceVoice) {
		// SourceVoice の停止と再生中フラグのリセット
		soundHandle_.at(soundHandle).pSourceVoice->Stop();
	}
}

size_t Audio::SoundLoadWave(const char* filename) {
	// 既存の要素を検索し、かぶるファイル名が存在するか確認
	for (size_t i = 0; i < soundHandle_.size(); ++i) {
		if (soundHandle_.at(i).filename == filename) {
			// 既存の要素が見つかった場合、その要素のインデックスを返す
			return i;
		}
	}
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
	// LISTチャンクを検出した場合
	if (strncmp(data.id, "LIST", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	// bextチャンクを検出した場合
	if (strncmp(data.id, "bext", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	// INFOチャンクを検出した場合
	if (strncmp(data.id, "INFO", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	// REAPERチャンクを検出した場合
	if (strncmp(data.id, "REAPER", 6) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "junk", 4) == 0) {
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
	HRESULT result;
	// returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	// SourceVoice の作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));
	soundData.pSourceVoice = pSourceVoice;
	// 名前の登録
	soundData.filename = filename;
#pragma endregion
	soundHandle_.emplace_back(soundData);

	return soundHandle_.size() - 1;
}

void Audio::SoundUnload(size_t soundHandle) {
	// バッファのメモリを解放
	soundHandle_.at(soundHandle).pBuffer = 0;
	soundHandle_.at(soundHandle).bufferSize = 0;
	soundHandle_.at(soundHandle).wfex = {};
	soundHandle_.erase(soundHandle_.begin() + soundHandle);
}
