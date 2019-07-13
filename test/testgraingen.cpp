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

class StreamingGrainGenTest : public ::testing::Test {
protected:
  
  PaStream *stream = nullptr;

  void TearDown(void) {
    killStream();
    Pa_Terminate();
  }

  void killStream(void) {
    if (stream != nullptr) {
      printf("Shutting down PA\n");
      Pa_StopStream(stream);
      Pa_CloseStream(stream);
      stream = nullptr;
    }
  }
  
  virtual void runTest(double fs, double density, double length, double freq, float ampl,
                       double drand=0, double lrand=0, double arand=0, double frand=0) {

    printf("Playing simple grains: \n");
    printf("\tfs\tdensity\tlength\tfreq\tampl\n");
    printf("\t%0.0f\t%0.0f\t%0.2f\t%0.0f\t%0.2f\n\n", fs, density, length, freq, ampl);
    printf("\tdrand\tlrand\tarand\tfrand\n");
    printf("\t%0.3f\t%0.3f\t%0.3f\t%0.3f\n", drand, lrand, arand, frand);
    
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    
    GrainGenerator<float> graingen(Shapes::Gaussian, Carriers::Triangle, fs);
    graingen.setDensityRand(drand);
    graingen.setLengthRand(lrand);
    graingen.setAmplRand(arand);
    graingen.setFreqRand(frand);
    graingen.applyInputs(density, length, freq, ampl);

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
                          //data->graingen->applyInputs(data->density, data->length, data->freq, data->ampl);
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
    killStream();
  }

  void runTestSuite(double fs, double density, double length, double freq, float ampl) {
    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> rand(0,1);
  
    runTest(fs, density, length, freq, ampl);
    runTest(fs, density, length, freq, ampl, rand(gen), 0, 0, 0);
    runTest(fs, density, length, freq, ampl, 0, rand(gen), 0, 0);
    runTest(fs, density, length, freq, ampl, 0, 0, rand(gen), 0);
    runTest(fs, density, length, freq, ampl, 0, 0, 0, rand(gen));
    runTest(fs, density, length, freq, ampl, rand(gen), rand(gen), rand(gen), rand(gen));
  }
  
};


TEST_F(StreamingGrainGenTest, sparseShort) {
  runTestSuite(48000, 10, 0.01, 440, 0.25);
}

TEST_F(StreamingGrainGenTest, sparseLong) {
  runTestSuite(48000, 10, 0.1, 440, 0.25);
}

TEST_F(StreamingGrainGenTest, denseShort) {
  runTestSuite(48000, 100, 0.01, 440, 0.25);
}

TEST_F(StreamingGrainGenTest, denseLong) {
  runTestSuite(48000, 100, 0.033, 440, 0.25);
}

TEST_F(StreamingGrainGenTest, veryDenseShort) {
  runTestSuite(48000, 1000, 0.01, 440, 0.25);
}

class SweepingGrainGenTest : public StreamingGrainGenTest {

public:
  
  void runTest(double fs, double density, double length, double freq, float ampl,
               double drand=0, double lrand=0, double arand=0, double frand=0) {

    printf("Sweeping grains: \n");
    printf("\tfs\tdensity\tlength\tfreq\tampl\n");
    printf("\t%0.0f\t%0.0f\t%0.2f\t%0.0f\t%0.2f\n\n", fs, density, length, freq, ampl);
    printf("\tdrand\tlrand\tarand\tfrand\n");
    printf("\t%0.3f\t%0.3f\t%0.3f\t%0.3f\n", drand, lrand, arand, frand);
    
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    
    GrainGenerator<float> graingen(Shapes::Gaussian, Carriers::Triangle, fs);
    graingen.setDensityRand(drand);
    graingen.setLengthRand(lrand);
    graingen.setAmplRand(arand);
    graingen.setFreqRand(frand);
    graingen.applyInputs(density, length, freq, ampl);

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
                          data->graingen->applyInputs(data->density, data->length, data->freq, data->ampl);
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
    printf("Sweeping density...\n");
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.density = density*(double)playtime/2500;
    }
    data.density = density;
    playtime = 0;
    printf("Sweeping length...\n");
    tstart = std::chrono::system_clock::now();
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.length = length*(double)playtime/2500;
    }
    data.length = length;
    playtime = 0;
    printf("Sweeping frequency...\n");
    tstart = std::chrono::system_clock::now();
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.freq = freq*(double)playtime/2500;
    }
    data.freq = freq;
    playtime = 0;
    printf("Sweeping amplitude...\n");
    tstart = std::chrono::system_clock::now();
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.ampl = ampl*(double)playtime/2500;
    }
    data.ampl = ampl;
    killStream();
  }
    
};

TEST_F(SweepingGrainGenTest, denseLong) {
  runTest(48000, 100, 0.033, 440, 0.25);
}

TEST_F(SweepingGrainGenTest, sparseLong) {
  runTestSuite(48000, 10, 0.1, 440, 0.25);
}

TEST_F(SweepingGrainGenTest, denseLongRand) {
  runTest(48000, 100, 0.033, 440, 0.25, 0.1, 0.1, 0.1, 0.1);
}


