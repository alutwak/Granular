#include <chrono>

#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "wavetable.hpp"

using namespace audioelectric;

static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags, void *wtinterp_data)
{
  Waveform<float>::phasor *interp = static_cast<Waveform<float>::phasor*>(wtinterp_data);
  float *out = (float*)output;
  while (frames--) {
    *out++ = *(*interp)++;
  }
  return 0;
}

class AudioPlaybackTest : public ::testing::Test {
protected:

  Wavetable<float> *wt = nullptr;
  double samplerate;
  PaStream *stream;
  
  virtual void TearDown(void) {
    if (wt != nullptr)
      delete wt;
  }

  virtual void playBack(double speed, double start, double end=-1, bool cycle=false) {
    Waveform<float>::phasor interp;
    printf("\nPlayback speed: %f\n",speed);
    printf("Start position: %f\n",start);
    printf("End position: %f\n",end);
    if (cycle) printf("Cycling...");
    ASSERT_FALSE(speed<0.1&&speed>-0.1) << "Speed too close to zero";
    if (speed>0) {
      interp = wt->pbegin(speed, start, end, cycle);
    }
    else {
      interp = wt->rpbegin(-speed, start, end, cycle);
    }
    initPA(interp);
    std::chrono::system_clock::time_point tstart = std::chrono::system_clock::now();
    long playtime = 0;
    while (interp && (playtime < 5000)) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
    }
    closePA();
    printf("playtime: %ld, %d\n", playtime, (bool)interp);
    if (cycle && speed*start < speed*end) {
      EXPECT_TRUE((bool)interp);
      EXPECT_GE(playtime, 5000);
    }
    else {
      EXPECT_FALSE((bool)interp);
      EXPECT_LT(playtime, 5000);
    }
  }

protected:

  void initPA(Waveform<float>::phasor& wtiter) {
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                1,          /* mono output */
                                paFloat32,  /* 32 bit floating point output */
                                samplerate,
                                256,        /* frames per buffer*/
                                paCallback, /* this is your callback function */
                                &wtiter);
    ASSERT_EQ(err, paNoError) << "PA error when opening stream: " << Pa_GetErrorText(err);
    err = Pa_StartStream(stream);
    ASSERT_EQ(err, paNoError) << "PA error when starting stream: " << Pa_GetErrorText(err);
    //printf("Playback sample rate: %.0fHz\n", samplerate);
  }

  void closePA(void) {
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
  }
  
};
