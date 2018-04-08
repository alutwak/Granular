#include <chrono>
#include <stdio.h>

#include <gtest/gtest.h>
#include <portaudio.h>

#include "function.hpp"

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

void testLine(double slope, double start) {
  auto line = make_line(slope, start);
  double val = start;
  for (int i=0;i<3;i++) {
    EXPECT_EQ(*line,val);
    ++line;
    val+=slope;
  }
}

TEST(Line, basic) {
  testLine(1,0);
  testLine(0,2.4);
  testLine(-115.5,-15);
  testLine(-0.01,-0.01);
}

TEST(Sinusoid, basic) {
  auto sin = make_sinusoid(1.0/1000,1,0);
  FILE* f = fopen("sinwave", "w");
  for (int i=0;i<1000;i++) {
    fprintf(f,"%f\n", *(sin++));
  }
  fclose(f);
  sin = make_sinusoid(
    make_sinusoid(2./1000, 10./1000, 20./1000),
    make_constant(1)
    );
  f = fopen("modsine","w");
  for (int i=0;i<1000;i++) {
    fprintf(f,"%f\n", *(sin++));
  }
  fclose(f);
}

#define PLAYTIME 5000
#define SAMPLERATE 96000

static int paCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags, void *wtinterp_data)
{
  Waveform<double>::phasor *interp = static_cast<Waveform<double>::phasor*>(wtinterp_data);
  float *out = (float*)output;
  while (frames--) {
    *out++ = *(*interp)++;
  }
  return 0;
}

class SinPlaybackTest : public ::testing::Test {
protected:

  PaStream *stream;
  
  virtual void playBack(double freq, double ampl, double offset) {
    printf("Playing %0.1fHz tone\n",freq);
    freq = freq/SAMPLERATE;
    auto sin = make_sinusoid(freq, ampl, offset);
    runPlayback(sin);
  }

  virtual void playBack(dphasor fmod, dphasor amod, double offset) {
    auto sin = make_sinusoid(fmod, amod, offset);
    runPlayback(sin);    
  }

private:

  void runPlayback(dphasor sin) {
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

  auto amod_1Hz = make_sinusoid(1./SAMPLERATE, 0.5, 0.5); //1 Hz amplitude modulator
  auto fmod_4Hz = make_sinusoid(4./SAMPLERATE, 10./SAMPLERATE, 440./SAMPLERATE); //4Hz freq modulator of 10Hz around 440 Hz

  playBack(make_constant(440./SAMPLERATE),
           amod_1Hz,0);
  playBack(fmod_4Hz,make_constant(1),0);
  playBack(fmod_4Hz, amod_1Hz, 0);
}

