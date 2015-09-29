// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class TOWARDSTHELIGHT_API LocalizationManager {
public:
  static LocalizationManager* Instance();

  bool ParseLanguage(const FString& language);
  bool SetLanguage(const FString& language);
  const FString& GetCurrentLanguage() const { return m_languages[m_currentLanguage].GetName(); }
  const FString& GetString(const FString& key, const FString& defVal = "Value not found") const { return m_languages[m_currentLanguage].GetString(key, defVal); }

protected:
	LocalizationManager();
	~LocalizationManager();

private:
  class TOWARDSTHELIGHT_API LangDef {
  public:
    LangDef() {}
    LangDef(const FString& name) { m_name = name; }
    const FString& GetName() const { return m_name; }
    void AddString(const FString& key, const FString& value) { m_keys.Add(key); m_values.Add(value); }
    const FString& GetString(const FString& key, const FString& defVal) const;

  private:
    FString m_name;
    TArray<FString> m_keys;
    TArray<FString> m_values;
  };

  static LocalizationManager* m_instance;
  TArray<LangDef> m_languages;
  unsigned m_currentLanguage;
};


