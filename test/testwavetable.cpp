
#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "wavetable.hpp"

using namespace audioelectric;

class SimpleWavetableTest : public ::testing::Test {
protected:

  Wavetable<double> wt = {0.0, 1.0, 2.0, 3.0};

  void testBasic(void) {
    EXPECT_EQ(wt.size(),4);
    //Check element access with integers
    for (int i=0;i<4;i++) {
      double d = wt[i];
      EXPECT_EQ(d,i);
      const double& dconst = wt[i];
      EXPECT_FLOAT_EQ(d,dconst);
    }
    //Check element access with doubles
    for (double j=0.0; j<3.1; j+=0.1) {
      double d = wt.waveform(j);
      if (j > 3)
        EXPECT_FLOAT_EQ(d,0);
      else
        EXPECT_FLOAT_EQ(d,j);
    }
  }
  
  /* Since the values in wt have a constant slope of 1, iterations should increment at the same rate as speed
   */
  void testIterSpeed(double speed) {
    auto interp = wt.pbegin(speed);
    auto begin = interp;
    double val = *interp;
    ASSERT_EQ(val, wt[0]);
    while (interp) {
      EXPECT_TRUE(interp);
      EXPECT_TRUE(interp>=begin);
      EXPECT_FLOAT_EQ(*(interp++),val) << "when speed = " << speed;
      val += speed;
    }
    testReverseIter(speed);
  }

  void testReverseIter(double speed) {
    auto rinterp = wt.rpbegin(speed);
    double val = *rinterp;
    //ASSERT_EQ(val,wt[wt.size()-1]);
    while (rinterp) {
      EXPECT_FLOAT_EQ(*(rinterp++),val) << "when speed = " << speed;
      val -= speed;
    }
  }

  void testCopy(int len) {
    double speed = (double)wt.size()/(double)len;
    Wavetable<double> wt_new(dynamic_cast<Wavetable<double>&>(wt), speed, len);
    EXPECT_EQ(wt_new.size(), len);  //Length should be correct
    int n = len/wt.size();
    for (int i=0;i<len;i++) {
      if (i >= n)
      break;
      EXPECT_FLOAT_EQ(wt.waveform(i*speed),wt_new[i]) << "len: " << len << " i: " << i;
    }
  }

};

TEST_F(SimpleWavetableTest, basic) {

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
                      PaStreamCallbackFlags statusFlags, void *wt );

class AudioPlaybackTest : public ::testing::Test {
protected:

  Wavetable<float> *wt;
  SF_INFO info;
  PaStream *stream;
  
  virtual void SetUp(void) {
    info.format = 0;
    SNDFILE* testfile = sf_open("testfile.wav", SFM_READ, &info);
    ASSERT_TRUE(testfile) << "unable to open testfile.wav";
    wt = new Wavetable<float>(info.frames); //We know this is just one channel
    sf_count_t nread = sf_readf_float(testfile, wt->data(), info.frames);
    ASSERT_EQ(nread,info.frames) << "Failed to read entire testfile.wav";
    sf_close(testfile);
  }

  virtual void TearDown(void) {
    delete wt;
  }

  void playBack(double speed) {
    Waveform<float>::phasor interp;
    ASSERT_FALSE(speed<0.1&&speed>-0.1) << "Speed too close to zero";
    if (speed>0) {
      interp = wt->pbegin(speed);
    }
    else {
      interp = wt->rpbegin(-speed);
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
                                info.samplerate,
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
