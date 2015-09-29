// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "GameDataManager.h"
#include "CoreMisc.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

GameDataManager* GameDataManager::m_instance = nullptr;

GameDataManager* GameDataManager::Instance() {
  if (m_instance == nullptr)
    m_instance = new GameDataManager();
  return m_instance;
}

GameDataManager::GameDataManager() {
  m_data = "";
  m_filePath = FPaths::GameContentDir() + m_filePath;
  m_music = NONE;
  m_effects = NONE;
  m_language = "";
  m_pageName = "";

  FFileHelper::LoadFileToString(m_data, *m_filePath);
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (doc.HasParseError()){
    DeleteGameData();
  }
}

GameDataManager::~GameDataManager() {
  SavedGame();

  m_data = "";
  delete m_instance;
  m_instance = nullptr;
}

bool GameDataManager::SavedGame() {
  return FFileHelper::SaveStringToFile(m_data, *m_filePath);
}

void GameDataManager::DeleteGameData() {
  FString initial_content;
  FString initial_content_path = FPaths::GameContentDir() + "StorageFiles/init/initial.json";
  FFileHelper::LoadFileToString(initial_content, *initial_content_path);

  m_data = initial_content;
  SavedGame();
}

LevelData GameDataManager::ReadLevelData(FString levelName){
  LevelData ret;
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("levels"))
        if (doc["levels"].IsArray()){
          SizeType numLevels = doc["levels"].Size();
          const Value &levels = doc["levels"];
          for (SizeType i = 0; i < numLevels; i++)
            if (levels[i].IsObject())
              if (levels[i].HasMember("name"))
                if (levels[i]["name"].IsString())
                  if (FString(levels[i]["name"].GetString()) == levelName){
                    ret.name = levels[i]["name"].GetString();

                    if (levels[i].HasMember("orbs"))
                      if (levels[i]["orbs"].IsNumber())
                        ret.orbs = levels[i]["orbs"].GetDouble();

                    if (levels[i].HasMember("points"))
                      if (levels[i]["points"].IsNumber())
                        ret.points = levels[i]["points"].GetDouble();

                    return ret;
                  }
        }

  // si no lo ha encontrado en el fichero, devuelve un struct con el nombre y el resto de parámetros por defecto.
  // cuando se llame a la función WriteLevelData, si no existe el bloque, se añadirá al fichero
  ret.name = levelName;
  return ret;
}

void GameDataManager::WriteLevelData(LevelData data){
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  bool success = false;
  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("levels"))
        if (doc["levels"].IsArray()){
          SizeType numLevels = doc["levels"].Size();
          Value &levels = doc["levels"];
          for (SizeType i = 0; i < numLevels; i++){
            if (levels[i].IsObject())
              if (levels[i].HasMember("name"))
                if (levels[i]["name"].IsString())
                  if (FString(levels[i]["name"].GetString()) == data.name){

                    if (levels[i].HasMember("orbs"))
                      if (levels[i]["orbs"].IsNumber())
                        levels[i]["orbs"].SetDouble(data.orbs);

                    if (levels[i].HasMember("points"))
                      if (levels[i]["points"].IsNumber())
                        levels[i]["points"].SetDouble(data.points);

                    success = true;
                    break;
                  }
          }

          if (!success){ //añadir el nuevo bloque
            Document::AllocatorType& allocator = doc.GetAllocator();

            Value lvl(kObjectType);
            char* n = TCHAR_TO_ANSI(*data.name);
            lvl.AddMember("name", n, allocator);
            lvl.AddMember("orbs", data.orbs, allocator);
            lvl.AddMember("points", data.points, allocator);

            levels.PushBack(lvl, allocator);
          }
        }

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  m_data = buffer.GetString();

  SavedGame();
}

float GameDataManager::GetOrbsCounts() {
  float ret = 0;
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("levels"))
        if (doc["levels"].IsArray()){
          SizeType numLevels = doc["levels"].Size();
          const Value &levels = doc["levels"];
          for (SizeType i = 0; i < numLevels; i++)
            if (levels[i].IsObject())
              if (levels[i].HasMember("orbs"))
                if (levels[i]["orbs"].IsNumber())
                  ret += levels[i]["orbs"].GetDouble();
        }

  return ret;
}

float GameDataManager::GetOrbsLevel(FString levelName){
  float ret = 0.f;
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("levels"))
        if (doc["levels"].IsArray()){
          SizeType numLevels = doc["levels"].Size();
          const Value &levels = doc["levels"];
          for (SizeType i = 0; i < numLevels; i++)
            if (levels[i].IsObject())
              if (levels[i].HasMember("name"))
                if (levels[i]["name"].IsString())
                  if (FString(levels[i]["name"].GetString()) == levelName){
                    if (levels[i].HasMember("orbs"))
                      if (levels[i]["orbs"].IsNumber())
                        ret = levels[i]["orbs"].GetDouble();

                    return ret;
                  }
        }

  return ret;
}

float GameDataManager::GetPointsLevel(FString levelName){
  float ret = 0.f;
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("levels"))
        if (doc["levels"].IsArray()){
          SizeType numLevels = doc["levels"].Size();
          const Value &levels = doc["levels"];
          for (SizeType i = 0; i < numLevels; i++)
            if (levels[i].IsObject())
              if (levels[i].HasMember("name"))
                if (levels[i]["name"].IsString())
                  if (FString(levels[i]["name"].GetString()) == levelName){
                    if (levels[i].HasMember("points"))
                      if (levels[i]["points"].IsNumber())
                        ret = levels[i]["points"].GetDouble();

                    return ret;
                  }
        }

  return ret;
}

bool GameDataManager::LevelExists(FString levelName){
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("levels"))
        if (doc["levels"].IsArray()){
          SizeType numLevels = doc["levels"].Size();
          const Value &levels = doc["levels"];
          for (SizeType i = 0; i < numLevels; i++)
            if (levels[i].IsObject())
              if (levels[i].HasMember("name"))
                if (levels[i]["name"].IsString())
                  if (FString(levels[i]["name"].GetString()) == levelName)
                    return true;
        }

  return false;
}

//OPTIONS
bool GameDataManager::HasMusic(){
  if (m_music == NONE){
    Document doc;
    doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

    if (!doc.HasParseError())
      if (doc.IsObject())
        if (doc.HasMember("general"))
          if (doc["general"].IsObject())
            if (doc["general"].HasMember("music"))
              if (doc["general"]["music"].IsBool()){
                bool ret = doc["general"]["music"].GetBool();
                m_music = ret ? YES : NO;
              }
  }

  return m_music == YES;
}
void GameDataManager::SetMusic(bool enable){
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("general"))
        if (doc["general"].IsObject())
          if (doc["general"].HasMember("music"))
            if (doc["general"]["music"].IsBool()){
              doc["general"]["music"].SetBool(enable);
              m_music = enable ? YES : NO;
            }

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  m_data = buffer.GetString();

  SavedGame();
}

bool GameDataManager::HasEffects(){
  if (m_effects == NONE){
    Document doc;
    doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

    if (!doc.HasParseError())
      if (doc.IsObject())
        if (doc.HasMember("general"))
          if (doc["general"].IsObject())
            if (doc["general"].HasMember("effects"))
              if (doc["general"]["effects"].IsBool()){
                bool ret = doc["general"]["effects"].GetBool();
                m_effects = ret ? YES : NO;
              }
  }

  return m_effects == YES;
}
void GameDataManager::SetEffects(bool enable){
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("general"))
        if (doc["general"].IsObject())
          if (doc["general"].HasMember("effects"))
            if (doc["general"]["effects"].IsBool()){
              doc["general"]["effects"].SetBool(enable);
              m_effects = enable ? YES : NO;
            }

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  m_data = buffer.GetString();

  SavedGame();
}

FString GameDataManager::GetLanguage(){
  if (m_language == ""){
    Document doc;
    doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

    if (!doc.HasParseError())
      if (doc.IsObject())
        if (doc.HasMember("general"))
          if (doc["general"].IsObject())
            if (doc["general"].HasMember("language"))
              if (doc["general"]["language"].IsString())
                m_language = doc["general"]["language"].GetString();
  }

  return m_language;
}
void GameDataManager::SetLanguage(FString language){
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("general"))
        if (doc["general"].IsObject())
          if (doc["general"].HasMember("language"))
            if (doc["general"]["language"].IsString()){
              doc["general"]["language"].SetString(TCHAR_TO_ANSI(*language), language.Len());
              m_language = language;
            }

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  m_data = buffer.GetString();

  SavedGame();
}

FString GameDataManager::GetPageName(){
  if (m_pageName == ""){
    Document doc;
    doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

    if (!doc.HasParseError())
      if (doc.IsObject())
        if (doc.HasMember("general"))
          if (doc["general"].IsObject())
            if (doc["general"].HasMember("page"))
              if (doc["general"]["page"].IsString())
                m_pageName = doc["general"]["page"].GetString();
  }

  return m_pageName;
}
void GameDataManager::SetPageName(FString pageName){
  Document doc;
  doc.Parse<0>(TCHAR_TO_ANSI(*m_data));

  if (!doc.HasParseError())
    if (doc.IsObject())
      if (doc.HasMember("general"))
        if (doc["general"].IsObject())
          if (doc["general"].HasMember("page"))
            if (doc["general"]["page"].IsString()){
              doc["general"]["page"].SetString(TCHAR_TO_ANSI(*pageName), pageName.Len());
              m_pageName = pageName;
            }

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  m_data = buffer.GetString();

  SavedGame();
}