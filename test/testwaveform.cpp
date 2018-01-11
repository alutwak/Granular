
#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "waveform.hpp"

using namespace audioelectric;

class SimpleWaveformTest : public ::testing::Test {
protected:

  Waveform<double> wf = {0.0, 1.0, 2.0, 3.0};

  void testBasic(void) {
    EXPECT_EQ(wf.size(),4);
    //Check element access with integers
    for (int i=0;i<4;i++) {
      double d = wf[i];
      EXPECT_EQ(d,i);
      const double& dconst = wf[i];
      EXPECT_FLOAT_EQ(d,dconst);
    }
    //Check element access with doubles
    for (double j=0.0; j<3.1; j+=0.1) {
      double d = wf.interpolate(j);
      if (j > 3)
        EXPECT_FLOAT_EQ(d,0);
      else
        EXPECT_FLOAT_EQ(d,j);
    }
  }
  
  /* Since the values in wf have a constant slope of 1, iterations should increment at the same rate as speed
   */
  void testIterSpeed(double speed) {
    auto wfiter = wf.ibegin(speed);
    double val = *wfiter;
    ASSERT_EQ(val, wf[0]);
    while (wfiter != wf.iend(speed)) {
      EXPECT_FLOAT_EQ(val,*(wfiter++)) << "when speed = " << speed;
      val += speed;
    }
    testReverseIter(speed);
  }

  void testReverseIter(double speed) {
    auto riter = wf.ribegin(speed);
    double val = *riter;
    //ASSERT_EQ(val,wf[wf.size()-1]);
    while (riter != wf.riend(speed)) {
      EXPECT_FLOAT_EQ(val,*(riter++)) << "when speed = " << speed;
      val -= speed;
    }
  }

  void testCopy(int len) {
    Waveform<double> wf_new(wf,len);
    EXPECT_EQ(wf_new.size(), len);  //Length should be correct
    double speed = (double)4/(double)len;
    int n = len/4;
    for (int i=0;i<4;i++) {
      if (i >= n)
        break;
      EXPECT_FLOAT_EQ(wf.interpolate(i*speed),wf_new[i]);
    }
  }

};

TEST_F(SimpleWaveformTest, basic) {

  testBasic();
  testIterSpeed(0.5);
  testIterSpeed(1.0);
  testIterSpeed(2.0);
  testIterSpeed(4.0);
  testIterSpeed(0.3428);
  testIterSpeed(1.2864);

  for (int i=0;i<10;i++)
    testCopy(i);
}


static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void *wf );

class AudioPlaybackTest : public ::testing::Test {
protected:

  Waveform<float> *wf;
  SF_INFO info;
  PaStream *stream;
  
  virtual void SetUp(void) {
    info.format = 0;
    SNDFILE* testfile = sf_open("testfile.wav", SFM_READ, &info);
    ASSERT_TRUE(testfile) << "unable to open testfile.wav";
    wf = new Waveform<float>(info.frames); //We know this is just one channel
    sf_count_t nread = sf_readf_float(testfile, wf->data(), info.frames);
    ASSERT_EQ(nread,info.frames) << "Failed to read entire testfile.wav";
    sf_close(testfile);
  }

  virtual void TearDown(void) {
    delete wf;
  }

  void playBack(double speed) {
    Waveform<float>::interpolator interp, interp_end;
    ASSERT_FALSE(speed<0.1&&speed>-0.1) << "Speed too close to zero";
    if (speed>0) {
      interp = wf->ibegin(speed);
      interp_end = wf->iend(speed);
    }
    else {
      interp = wf->ribegin(-speed);
      interp_end = wf->riend(-speed);
    }
    initPA(interp);
    while (interp<interp_end) {}
    closePA();
  }

private:

  void initPA(Waveform<float>::interpolator& wfiter) {
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                1,          /* mono output */
                                paFloat32,  /* 32 bit floating point output */
                                info.samplerate,
                                256,        /* frames per buffer*/
                                paCallback, /* this is your callback function */
                                &wfiter);
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

static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags, void *wfinterp_data)
{
  Waveform<float>::interpolator *interp = static_cast<Waveform<float>::interpolator*>(wfinterp_data);
  float *out = (float*)output;
  while (frames--) {
    *out++ = *(*interp)++;
  }
  return 0;
}

TEST_F(AudioPlaybackTest, speed) {
  playBack(1);
  playBack(0.5);
  playBack(2);
  playBack(1.232);
  playBack(-1);
  playBack(-0.5);
  playBack(-2);
  playBack(-1.232);
}
