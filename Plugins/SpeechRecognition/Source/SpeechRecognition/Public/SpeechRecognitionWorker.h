#pragma once

#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <pocketsphinx.h>
#include <stdio.h>
#include <time.h>
#include <regex>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <locale>
#include <cstdio>
#include <vector>
#include <utility>

#include "Chaos/AABB.h"

//General Log
DECLARE_LOG_CATEGORY_EXTERN(SpeechRecognitionPlugin, Log, All);

#define SENSCR_SHIFT 10

class USpeechRecognitionSubsystem;

using namespace std;

//Common structures and enumerations
struct FSpeechRecognitionParam
{
	char* name;
	ESpeechRecognitionParamType type;
	char* value;

	// constructor
	FSpeechRecognitionParam(char* name, ESpeechRecognitionParamType type, char* value) {
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
		this->type = type;
		this->value = new char[strlen(value) + 1];
		strcpy(this->value, value);
	}
};

class FSpeechRecognitionWorker :public FRunnable
{

private:
	// Sphinx
	ps_decoder_t *ps = NULL;
	cmd_ln_t *config = NULL;
	ad_rec_t *ad;
	int16 adbuf[1024];
	uint8 utt_started, in_speech;
	int32 k;
	bool initRequired = false;
	bool wordsAdded = false;

	//Stores the current, peak volume
	int16 currentVolume = 0;

	//A set of params to apply to Sphinx initialisation
	TArray<FSpeechRecognitionParam> sphinxParams;

	//Speech detection mode
	ESpeechRecognitionMode detectionMode;
	
	//Language
	ESpeechRecognitionLanguage language;

	//Thread
	FRunnableThread* Thread;

	//Pointer to our manager
	USpeechRecognitionSubsystem* Manager;

	//Thread safe counter 
	FThreadSafeCounter StopTaskCounter;

	//Language
	const char* langStr = NULL;

	//Paths
	std::string argFilePath;
	std::string contentPath_str;
	std::string logPath;
	std::string modelPath;
	std::string languageModel;
	std::string dictionaryPath;

	//Stores the recognition keywords, along with their tolerances
	std::map <string , char*> keywords;

	//Dictionary
	std::map <string, set<string>> dictionary;

	//Splits a string by whitespace
	vector<string> Split(string s);
	//Removes brackets, and 1-9 characters, from a string
	string GetOriginalString(string s) const;

public:
	FSpeechRecognitionWorker();
	virtual ~FSpeechRecognitionWorker();

	//FRunnable interface
	virtual void Stop();
	virtual uint32 Run();

	//Methods to switch recognition modes
	bool EnableKeywordMode(const TArray<FRecognitionPhrase>& wordList);
	bool EnableGrammarMode(FString grammarName);
	bool EnableLanguageModel(FString InLanguageModel);

	//Action methods
	void AddWords(TArray<FRecognitionPhrase> InKeywords);
	int16 GetCurrentVolume() const;
	void InitConfig();
	bool SetConfigParam(FString param, ESpeechRecognitionParamType type, FString value);
	void SetLanguage(ESpeechRecognitionLanguage InLanguage);
	bool StartThread(USpeechRecognitionSubsystem* manager);
	void ShutDown();

	// Print Debug Text
	static void ClientMessage(FString txt);

};

