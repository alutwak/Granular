#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "waveform.hpp"

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

  Waveform<float> *wf = nullptr;
  double samplerate;
  PaStream *stream;
  
  virtual void TearDown(void) {
    if (wf != nullptr)
      delete wf;
  }

  virtual void playBack(double speed, double start) {
    Waveform<float>::phasor interp;
    ASSERT_FALSE(speed<0.1&&speed>-0.1) << "Speed too close to zero";
    if (speed>0) {
      interp = wf->pbegin(speed, start);
    }
    else {
      interp = wf->rpbegin(-speed, start);
    }
    initPA(interp);
    while (interp) {}
    closePA();
  }

private:

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
  }

  void closePA(void) {
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
  }
  
};
