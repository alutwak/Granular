#include <chrono>

#include <gtest/gtest.h>

#include "function.hpp"
#include "audioplaybacktest.hpp"

using namespace audioelectric;

void testConstant(double value) {

  auto constant = make_constant(value);
  printf("Constant = %f\n",value);
  EXPECT_EQ(*constant,value);
  EXPECT_EQ(*(++constant),value);
  EXPECT_EQ(*constant++, value);
}

TEST(Constant, basic) {
  testConstant(0);
  testConstant(1);
  testConstant(-1);
  testConstant(1059824);
  testConstant(-8e7);
}

#define PLAYTIME 5000
#define SAMPLERATE 96000

class SinPlaybackTest : public ::testing::Test {
protected:

  PaStream *stream;
  
  virtual void playBack(double freq, double ampl, double offset) {
    freq = freq/SAMPLERATE;
    auto sin = make_sinusoid(freq, ampl, offset);
    initPA(sin);
    std::chrono::system_clock::time_point tstart = std::chrono::system_clock::now();
    long playtime = 0;
    while (playtime < PLAYTIME) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
    }
    closePA();
    printf("playtime: %ld, %d\n", playtime, (bool)sin);
    EXPECT_TRUE((bool)sin);
    EXPECT_GE(playtime, PLAYTIME);
  }

  void initPA(Waveform<double>::phasor& wtiter) {
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                1,          /* mono output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLERATE,
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

TEST_F(SinPlaybackTest, freq) {
  playBack(440,0.1,0);
}

