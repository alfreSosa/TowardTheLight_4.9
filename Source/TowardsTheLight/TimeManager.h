// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class TOWARDSTHELIGHT_API TimeManager{
public:
  static TimeManager* Instance();

  void SetPause(bool paused);
  bool IsPaused();

  float GetDeltaTime(float deltaTime);

protected:
  TimeManager();
  ~TimeManager();

private:
  static TimeManager* m_instance;

  bool m_paused;
};