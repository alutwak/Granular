
#include <cstdlib>
#include <ctime>

#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "audioplaybacktest.hpp"
#include "waveform.hpp"

using namespace audioelectric;

TEST(waveform, requirements)
{
  printf("Testing that Waveform can store data...\n");
  Waveform<double> wf([](size_t i) {return i/48000.;}, 48000);

  printf("Testing that the data was stored correctly...\n");
  for (int i=0; i<48000; i+=1000)
    EXPECT_EQ(wf[i], i/48000.);

  printf("Testing that the waveform interpolates correctly...\n");
  std::srand(std::time(nullptr));
  for (int i=0; i<10; i++) {
    double testpos = (48000-1)*(double)std::rand()/RAND_MAX;
    EXPECT_FLOAT_EQ(wf.waveform(testpos), testpos/48000);
  }

  printf("Verifying the Waveform size...\n");
  EXPECT_EQ(wf.size(), 48000);

  printf("Verifying that the Waveform is 0 outside of its bounds...\n");
  EXPECT_EQ(wf.waveform(-10), 0);
  EXPECT_EQ(wf.waveform(49000), 0);
}


class SimpleWaveformTest : public ::testing::Test {
protected:

  Waveform<double> wt = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  double *test;

  virtual void SetUp(void) override {
    test = new double[wt.size()];
  }

  virtual void TearDown(void) override {
    delete[] test;
  }

  void testBasic(void) {
    EXPECT_EQ(wt.size(),10);
    //Check element access with integers
    for (int i=0;i<10;i++) {
      double d = wt[i];
      EXPECT_EQ(d,i);
      const double& dconst = wt[i];
      EXPECT_FLOAT_EQ(d,dconst);
    }
    //Check element access with doubles
    for (double j=0.0; j<9.1; j+=0.1) {
      double d = wt.waveform(j);
      if (j > 9)
        EXPECT_FLOAT_EQ(d,0);
      else
        EXPECT_FLOAT_EQ(d,j);
    }
  }
  
  /* Since the values in wt have a constant slope of 1, iterations should increment at the same rate as speed
   */
  void testPhasor(double speed, double start=0) {
    auto phs = Phasor<double>(wt,speed,start);
    auto begin = phs;
    double check = phs.value();
    ASSERT_EQ(check, start) << "when speed = " << speed << " and start = " << start;
    if (speed == 0) {
      EXPECT_TRUE(phs.generate(&test,3)); //generate 3 frames
      EXPECT_TRUE(phs==begin);            //Should still be at the beginning
      for (int i=0;i<3;i++) {
        EXPECT_FLOAT_EQ(test[i],check) << "when speed = " << speed << " and start = " << start;
        check += speed;
      }
    }
    else {
      int iters = 0;
      while (phs) {
        if (speed > 0)
          EXPECT_TRUE(phs>=begin) << "when speed = " << speed << " and start = " << start;
        else
          EXPECT_TRUE(phs<=begin) << "when speed = " << speed << " and start = " << start;
        phs.generate(&test,1);
        EXPECT_FLOAT_EQ(test[0],check) << "when speed = " << speed << " and start = " << start;
        check += speed;
        iters++;
      }
    }
  }

  void testVariableRatePhasor(void) {
    double rate = 1;
    auto phs = Phasor<double>(wt,rate,0);
    auto begin = phs;
    double val = phs.value();
    ASSERT_EQ(val,wt[0]);
    while(phs) {
      rate++;
      phs.setRate(rate);
      EXPECT_TRUE(phs>=begin);
      phs.generate(&test,1);
      ASSERT_FLOAT_EQ(test[0],val) << "when rate = " << rate;
      val += rate;
    }
    rate = -rate;
    phs.setRate(rate);
    phs.increment();
    val += rate;
    while(phs) {
      rate++;
      phs.setRate(rate);
      EXPECT_TRUE(phs>=begin);
      phs.generate(&test,1);      
      EXPECT_FLOAT_EQ(test[0],val) << "when rate = " << rate;
      val += rate;
    }
  }

  void testCopy(int len) {
    double speed = (double)wt.size()/(double)len;
    Waveform<double> wt_new(dynamic_cast<Waveform<double>&>(wt), speed, len);
    EXPECT_EQ(wt_new.size(), len);  //Length should be correct
    int n = len/wt.size();
    for (int i=0;i<len;i++) {
      if (i >= n)
      break;
      EXPECT_FLOAT_EQ(wt.waveform(i*speed),wt_new[i]) << "len: " << len << ", i: " << i << ", speed: " << speed;;
    }
  }

};

TEST_F(SimpleWaveformTest, basic) {

  testBasic();
  testPhasor(0, 1);
  testPhasor(0.5);
  testPhasor(0.5,1.2);
  testPhasor(1.0);
  testPhasor(1.0, 2.1);
  testPhasor(2.0);
  testPhasor(4.0);
  testPhasor(0.3428);
  testPhasor(1.2864);
  testPhasor(1.2864, 0.1);
  testPhasor(-0.5);
  testPhasor(-0.5,1.2);
  testPhasor(-1.0);
  testPhasor(-1.0, 2.1);
  testPhasor(-2.0);
  testPhasor(-4.0);
  testPhasor(-0.3428);
  testPhasor(-1.2864);
  testPhasor(-1.2864, 0.1);
  testVariableRatePhasor();

  for (int i=0;i<10;i++)
    testCopy(i);
}


static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void *wt );

class WaveformPlaybackTest : public AudioPlaybackTest {
protected:

  virtual void SetUp(void) {
    SF_INFO info;
    info.format = 0;
    SNDFILE* testfile = sf_open("testfile.wav", SFM_READ, &info);
    ASSERT_TRUE(testfile) << "unable to open testfile.wav";
    wt = new Waveform<float>(info.frames); //We know this is just one channel
    sf_count_t nread = sf_readf_float(testfile, wt->data(), info.frames);
    samplerate = info.samplerate;
    ASSERT_EQ(nread,info.frames) << "Failed to read entire testfile.wav";
    sf_close(testfile);
  }

};

TEST_F(WaveformPlaybackTest, simple) {
  double end = wt->end();  
  printf("Cycling over a short section at normal rate...\n");
  playBack(1, 17850, 17850, 27850, true);
  printf("Cycling backward over a short section at normal rate...\n");  
  playBack(-1, 27850, 17850, 27850, true);
  printf("Cycling over a short section at double rate...\n");  
  playBack(2, 17850, 17850, 27850, true);
  printf("Cycling backward over a short section at double rate...\n");    
  playBack(-2, 27850, 17850, 27850, true);

  printf("Cycling over an empty section. Should be silent...\n");  
  playBack(-1, 17850, 27850, 17850, true);
  printf("Playing back the whole waveform at various speeds...\n");
  playBack(1, 0);
  playBack(0.5, 0);
  playBack(2, 0);
  playBack(1.232, 0);
  printf("Playing back the whole waveform backward at various speeds...\n");  
  playBack(1,17850);
  playBack(4.2,17850);
  printf("Playing back the whole waveform backward at various speeds...\n");  
  playBack(-1, end);
  playBack(-0.5, end);
  playBack(-2, end);
  playBack(-1.232, end);
  printf("Playing back part of waveform backward at various speeds...\n");
  playBack(-1,35229);
  playBack(-4.2,35229);
}
