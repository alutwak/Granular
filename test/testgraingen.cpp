#include <chrono>
#include <gtest/gtest.h>
#include <portaudio.h>

#include "graingenerator.hpp"

using namespace audioelectric;

#define BUFSIZE 256

struct paData {
  GrainGenerator<float>* graingen;
  double density;
  double length;
  double freq;
  float ampl;
};

class GrainGenTest : public ::testing::Test {
protected:
  
  PaStream *stream = nullptr;

  void TearDown(void) {
    if (stream != nullptr) {
      printf("Shutting down PA\n");
      Pa_StopStream(stream);
      Pa_CloseStream(stream);
      Pa_Terminate();
    }
  }
  
  void SimpleTest(double fs, double density, double length, double freq, float ampl) {

    printf("Playing simple grains: \n");
    printf("\tfs:      %f\n", fs);
    printf("\tdensity: %f\n", density);
    printf("\tlength:  %f\n", length);
    printf("\tfreq:    %f\n", freq);
    printf("\tampl:    %f\n", ampl);
    
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    
    GrainGenerator<float> graingen(fs);
    paData data = {
      &graingen,
      density,
      length,
      freq,
      ampl
    };
    
    auto paCallback = [] (const void *input, void *output,
                          unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void *graingen_data)
                        {
                          paData *data = static_cast<paData*>(graingen_data);
                          data->graingen->updateGrains(data->density, data->length, data->freq, data->ampl);
                          float *buffer = (float*)output;
                          for (int i=0; i<frames; i++) {
                            buffer[i] = data->graingen->value();
                            data->graingen->increment();
                          }
                          return 0;
                        };

    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                1,          /* mono output */
                                paFloat32,  /* 32 bit floating point output */
                                fs,
                                BUFSIZE,    /* frames per buffer*/
                                paCallback, /* this is your callback function */
                                &data);
    ASSERT_EQ(err, paNoError) << "PA error when opening stream: " << Pa_GetErrorText(err);
    err = Pa_StartStream(stream);
    ASSERT_EQ(err, paNoError) << "PA error when starting stream: " << Pa_GetErrorText(err);

    std::chrono::system_clock::time_point tstart = std::chrono::system_clock::now();
    long playtime = 0;
    while (playtime < 2000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
    }
  }

  void RandTest(double drand, double lrand, double arand, double frand) {
    
  }
  
};


TEST_F(GrainGenTest, simp48k10d10ms440Hz0_25a) {
  SimpleTest(48000, 10, 0.01, 440, 0.25);
}

TEST_F(GrainGenTest, simp48k10d100ms440Hz0_25a) {
  SimpleTest(48000, 10, 0.1, 440, 0.25);
}

TEST_F(GrainGenTest, simp48k100d10ms440Hz0_25a) {
  SimpleTest(48000, 100, 0.01, 440, 0.25);
}

TEST_F(GrainGenTest, simp48k100d33ms440Hz0_25a) {
  SimpleTest(48000, 100, 0.033, 440, 0.25);
}
