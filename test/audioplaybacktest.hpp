#include <chrono>

#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "wavetable.hpp"
#include "phasor.hpp"

using namespace audioelectric;

static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags, void *wtinterp_data)
{
  Phasor<float> *phs = static_cast<Phasor<float>*>(wtinterp_data);
  phs->generate((float**)&output, frames, 1);
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
  
  virtual void playBack(double rate, double start, double begin=0, double end=-1, bool cycle=false) {
    printf("\nPlayback rate: %f\n",rate);
    printf("Start position: %f\n",start);
    printf("Begin position: %f\n",begin);    
    printf("End position: %f\n",end);
    if (cycle) printf("Cycling...\n");
    //ASSERT_FALSE(rate<0.1&&rate>-0.1) << "Rate too close to zero";
    auto phs = Phasor<float>(*wt, rate, start, begin, end, cycle);
    long maxplay;
    if (cycle)
      maxplay = 1000; //just play for 1 second
    else
      maxplay = 5000; //Give 5 seconds for a slow test file
    long playtime = runPlayback(phs, maxplay);
    if (cycle && begin < end) {
      EXPECT_TRUE((bool)phs);
      EXPECT_GE(playtime, maxplay);
    }
    else {
      EXPECT_FALSE((bool)phs);
      EXPECT_LT(playtime, maxplay);
    }
  }

  virtual void playBack(Phasor<float> rate, double start, double begin=0, double end=-1, bool cycle=false) {
    printf("\nStarting playback rate: %f\n",rate.value());
    printf("Start position: %f\n",start);
    printf("End position: %f\n",end);
    if (cycle) printf("Cycling...\n");
    auto phs = ModPhasor<float>(*wt, rate, start, begin, end, cycle);
    long playtime = runPlayback(phs, 5000);
    if (cycle) {
      EXPECT_TRUE((bool)phs);
      EXPECT_GE(playtime, 5000);
    }
    else {
      EXPECT_FALSE((bool)phs);
      EXPECT_LT(playtime, 5000);
    }
  }

  virtual long runPlayback(Phasor<float>& phs, long maxplay) {
    initPA(phs);
    std::chrono::system_clock::time_point tstart = std::chrono::system_clock::now();
    long playtime = 0;
    while (phs && (playtime < maxplay)) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
    }
    closePA();
    printf("playtime: %ld, %d\n", playtime, (bool)phs);
    return playtime;
  }
  
  void initPA(Phasor<float>& wtiter) {
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
