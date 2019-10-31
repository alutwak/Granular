
#include <cstdlib>
#include <ctime>

#include <gtest/gtest.h>
#include <portaudio.h>

#include "audioplaybacktest.hpp"
#include "waveform.hpp"

using namespace audioelectric;

#define TESTFILE_LEN 441365

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
  EXPECT_EQ(wf.waveform(-0.0000000000001), 0);
  EXPECT_EQ(wf.waveform(49000), 0);
  EXPECT_EQ(wf.waveform(48000.000000000001), 0);
}

TEST(waveform, fromfile_double)
{
  try {
    Waveform<double> wf("testfile.wav");
    EXPECT_EQ(wf.size(), TESTFILE_LEN);
  }
  catch (WaveformError e) {
    FAIL() << "Exception thrown: " << e.what();
  }
  
}

TEST(waveform, fromfile_float)
{
  try {
    Waveform<float> wf("testfile.wav");
    EXPECT_EQ(wf.size(), TESTFILE_LEN);
  }
  catch (WaveformError e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(waveform, fromfile_endsection)
{
  try {
    Waveform<double> wf("testfile.wav", 1000);
    EXPECT_EQ(wf.size(), TESTFILE_LEN - 1000);
  }
  catch (WaveformError e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(waveform, fromfile_beginsection)
{
  try {
    Waveform<double> wf("testfile.wav", 0, 1000);
    EXPECT_EQ(wf.size(), 1000);
  }
  catch (WaveformError e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(waveform, fromfile_midsection)
{
  try {
    Waveform<double> wf("testfile.wav", 1000, 2000);
    EXPECT_EQ(wf.size(), 1000);
  }
  catch (WaveformError e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(waveform, fromfile_zerolen)
{
  ASSERT_THROW(Waveform<double> wf("testfile.wav", 1000, 1000), WaveformError);
}

TEST(waveform, fromfile_negativelen)
{
  ASSERT_THROW(Waveform<double> wf("testfile.wav", 2000, 1000), WaveformError);
}

TEST(waveform, fromfile_longend)
{
  try {
    Waveform<double> wf("testfile.wav", 1000, TESTFILE_LEN + 10);
    EXPECT_EQ(wf.size(), TESTFILE_LEN - 1000);
  }
  catch (WaveformError e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(waveform, fromfile_longbegin)
{
  ASSERT_THROW(Waveform<double> wf("testfile.wav", TESTFILE_LEN, TESTFILE_LEN+1), WaveformError);
}

TEST(waveform, fromfile_noexist)
{
  ASSERT_THROW(Waveform<double> wf("doesnt_exist.wav"), WaveformError);
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
  
  void testCopy(int len) {
    double rate = (double)wt.size()/(double)len;
    Waveform<double> wt_new(dynamic_cast<Waveform<double>&>(wt), rate, len);
    EXPECT_EQ(wt_new.size(), len);  //Length should be correct
    int n = len/wt.size();
    for (int i=0;i<len;i++) {
      if (i >= n)
      break;
      EXPECT_FLOAT_EQ(wt.waveform(i*rate),wt_new[i]) << "len: " << len << ", i: " << i << ", rate: " << rate;;
    }
  }

};

TEST_F(SimpleWaveformTest, basic) {

  testBasic();

  for (int i=0;i<10;i++)
    testCopy(i);
}


static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void *wt );

class WaveformPlaybackTest : public AudioPlaybackTest {
protected:

  virtual void SetUp(void) {
    wt = new Waveform<float>("testfile.wav"); //We know this is just one channel
    samplerate = wt->samplerate();
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
  printf("Playing back the whole waveform at various rates...\n");
  playBack(1, 0);
  playBack(0.5, 0);
  playBack(2, 0);
  playBack(1.232, 0);
  printf("Playing back the whole waveform backward at various rates...\n");  
  playBack(1,17850);
  playBack(4.2,17850);
  printf("Playing back the whole waveform backward at various rates...\n");  
  playBack(-1, end);
  playBack(-0.5, end);
  playBack(-2, end);
  playBack(-1.232, end);
  printf("Playing back part of waveform backward at various rates...\n");
  playBack(-1,35229);
  playBack(-4.2,35229);
}

class WaveformPlaybackTestStereo : public AudioPlaybackTest {
protected:

  virtual void SetUp(void) {
    wt = new Waveform<float>("testfilestereo.wav"); //We know this is just one channel
    samplerate = wt->samplerate();
  }

};

TEST_F(WaveformPlaybackTestStereo, simple) {
  double end = wt->end();  
  printf("Cycling over a short section at normal rate...\n");
  playBack(1, 17850, 17850, 27850, true);
  printf("Cycling backward over a short section at normal rate...\n");  
  playBack(-1, 27850, 17850, 27850, true);
  printf("Cycling over a short section at double rate...\n");  
  playBack(2, 17850, 17850, 27850, true);
  printf("Cycling backward over a short section at double rate...\n");    
  playBack(-2, 27850, 17850, 27850, true);
  printf("Playing back the whole waveform...\n");
  playBack(1, 0);
  
}
