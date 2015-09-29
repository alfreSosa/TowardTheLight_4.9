// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class TOWARDSTHELIGHT_API SoundManager {
public:
  static SoundManager* Instance();

  void SetMusic(bool enable);
  void SetEffects(bool enable);

protected:
	SoundManager();
	~SoundManager();

private:
  static SoundManager* m_instance;
};
