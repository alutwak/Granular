
#include <gtest/gtest.h>
#include <sndfile.h>
#include <portaudio.h>

#include "audioplaybacktest.hpp"
#include "wavetable.hpp"

using namespace audioelectric;

class SimpleWavetableTest : public ::testing::Test {
protected:

  Wavetable<double> wt = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

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
  void testInterpolator(double speed, double start=0) {
    auto interp = wt.pbegin(speed,start);
    auto begin = interp;
    double val = *interp;
    ASSERT_EQ(val, start);
    while (interp) {
      EXPECT_TRUE(interp>=begin);
      EXPECT_FLOAT_EQ(*(interp++),val) << "when speed = " << speed << " and start = " << start;
      val += speed;
    }
    testReverseInterpolator(speed,start);
  }

  void testReverseInterpolator(double speed, double start) {
    if (start == 0) start = wt.size()-1;
    auto rinterp = wt.rpbegin(speed, start);
    auto begin = rinterp;
    double val = *rinterp;
    ASSERT_EQ(val,start);
    while (rinterp) {
      EXPECT_TRUE(rinterp>=begin);
      EXPECT_NEAR(*(rinterp++),val, 1e-3) << "when speed = " << speed << " and start = " << start;
      val -= speed;
    }
  }

  void testVariableRateInterpolator(void) {
    double rate = 1;
    auto interp = wt.pbegin(rate,0);
    auto begin = interp;
    double val = *interp;
    ASSERT_EQ(val,wt[0]);
    while(interp) {
      rate++;
      interp.setRate(rate);
      EXPECT_TRUE(interp>=begin);
      ASSERT_FLOAT_EQ(*(interp++),val) << "when rate = " << rate;
      val += rate;
    }
    rate = -rate;
    interp.setRate(rate);
    interp++;
    while(interp) {
      rate++;
      interp.setRate(rate);
      EXPECT_TRUE(interp>=begin);
      EXPECT_FLOAT_EQ(*(interp++),val) << "when rate = " << rate;
      val += rate;
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
      EXPECT_FLOAT_EQ(wt.waveform(i*speed),wt_new[i]) << "len: " << len << ", i: " << i << ", speed: " << speed;;
    }
  }

};

TEST_F(SimpleWavetableTest, basic) {

  testBasic();
  testInterpolator(0, 1);
  testInterpolator(0.5);
  testInterpolator(0.5,1.2);
  testInterpolator(1.0);
  testInterpolator(1.0, 2.1);
  testInterpolator(2.0);
  testInterpolator(4.0);
  testInterpolator(0.3428);
  testInterpolator(1.2864);
  testInterpolator(1.2864, 0.1);
  testVariableRateInterpolator();

  for (int i=0;i<10;i++)
    testCopy(i);
}


static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void *wt );

class WavetablePlaybackTest : public AudioPlaybackTest {
protected:

  
  virtual void SetUp(void) {
    SF_INFO info;
    info.format = 0;
    SNDFILE* testfile = sf_open("testfile.wav", SFM_READ, &info);
    ASSERT_TRUE(testfile) << "unable to open testfile.wav";
    wt = new Wavetable<float>(info.frames); //We know this is just one channel
    sf_count_t nread = sf_readf_float(testfile, wt->data(), info.frames);
    //wt = dynamic_cast<Waveform<float>*>(wt);
    samplerate = info.samplerate;
    ASSERT_EQ(nread,info.frames) << "Failed to read entire testfile.wav";
    sf_close(testfile);
  }

};

TEST_F(WavetablePlaybackTest, speed) {
  double end = wt->end();
  printf("Cycling over a short section at normal rate...\n");
  playBack(1, 17850, 27850, true);
  printf("Cycling backward over a short section at normal rate...\n");  
  playBack(-1, 27850, 17850, true);
  printf("Cycling over a short section at double rate...\n");  
  playBack(2, 17850, 27850, true);
  printf("Cycling backward over a short section at double rate...\n");    
  playBack(-2, 27850, 17850, true);

  printf("Cycling over an empty section. Should be silent...\n");  
  playBack(-1, 17850, 27850, true);
  printf("Playing back the whole wavetable at various speeds...\n");
  playBack(1, 0);
  playBack(0.5, 0);
  playBack(2, 0);
  playBack(1.232, 0);
  printf("Playing back the whole wavetable backward at various speeds...\n");  
  playBack(1,17850);
  playBack(4.2,17850);
  printf("Playing back the whole wavetable backward at various speeds...\n");  
  playBack(-1, end);
  playBack(-0.5, end);
  playBack(-2, end);
  playBack(-1.232, end);
  printf("Playing back part of wavetable backward at various speeds...\n");
  playBack(-1,35229);
  playBack(-4.2,35229);
  
}
