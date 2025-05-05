#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "FS.h"
#include "SPIFFS.h"

AudioGeneratorWAV *wav;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS failed to mount");
    return;
  }

  file = new AudioFileSourceSPIFFS("/hello.wav");
  out = new AudioOutputI2S();
  out->SetOutputModeMono(true);  // Optional: mono mode
  wav = new AudioGeneratorWAV();

  if (!wav->begin(file, out)) {
    Serial.println("Failed to start WAV playback");
  } else {
    Serial.println("Audio playback started");
  }
}

void loop() {
  if (wav && wav->isRunning()) {
    wav->loop();
  }
}
