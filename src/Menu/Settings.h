#ifndef SETTINGS_H
#define SETTINGS_H

#include <raylib.h> // Corrected from #incl as per "syntactically correct" instruction

// Draw the settings popup. Returns true if the popup should stay open, false if it should close.
bool DrawSettingsPopup(Music musicMenu);

// Get current volume settings (0.0 to 1.0)
float GetMusicVolumeValue();
float GetSFXVolumeValue();

#endif
