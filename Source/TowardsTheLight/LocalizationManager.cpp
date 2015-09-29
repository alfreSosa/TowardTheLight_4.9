// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "LocalizationManager.h"
#include "GameDataManager.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

/////////
// # -> á
// $ -> é
// % -> í
// / -> ó
// * -> ú
// + -> ñ
// { -> ¡
// } -> ¿
// ^ -> Á
// ; -> É
// < -> Í
// = -> Ó
// > -> Ú
// [ -> Ñ
/////////

//LangDef
const FString& LocalizationManager::LangDef::GetString(const FString& key, const FString& defVal) const {
  unsigned size = m_keys.Num();
  for (unsigned i = 0; i < size; i++)
    if (m_keys[i] == key)
      return m_values[i];
  return defVal;
}

//LocalizationManager
LocalizationManager* LocalizationManager::m_instance = nullptr;

LocalizationManager* LocalizationManager::Instance() {
  if (m_instance == nullptr)
    m_instance = new LocalizationManager();
  return m_instance;
}

LocalizationManager::LocalizationManager() {
  ParseLanguage("en");
  ParseLanguage("es");

  SetLanguage(GameDataManager::Instance()->GetLanguage());
}

LocalizationManager::~LocalizationManager() {
  delete m_instance;
  m_instance = nullptr;
}

bool LocalizationManager::ParseLanguage(const FString& language) {
  FString path = FPaths::GameContentDir() + "Languages/" + language + ".json";
  FString buffer;
  FFileHelper::LoadFileToString(buffer, *path);

  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*buffer));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("language"))
        if (doc["language"].IsString()){
          FString langName = doc["language"].GetString();
          LangDef langDef(langName);

          if (doc.HasMember("keys")){
            if (doc["keys"].IsArray()){
              SizeType numKeys = doc["keys"].Size();
              const Value &keys = doc["keys"];
              for (SizeType i = 0; i < numKeys; i++)
                if (keys[i].IsObject())
                  if (keys[i].HasMember("key"))
                    if (keys[i]["key"].IsString()){
                      FString k = keys[i]["key"].GetString();
                      if (keys[i].HasMember("value"))
                        if (keys[i]["value"].IsString()){
                          FString v = keys[i]["value"].GetString();

                          langDef.AddString(k, v);
                        }
                    }
            }
            m_languages.Add(langDef);

            return true;
          }
        }

  return false;
}

bool LocalizationManager::SetLanguage(const FString& language) {
  unsigned size = m_languages.Num();
  for (unsigned i = 0; i < size; i++)
    if (m_languages[i].GetName() == language) {
      m_currentLanguage = i;
      return true;
    }
  return false;
}
