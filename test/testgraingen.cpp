#include <chrono>
#include <gtest/gtest.h>
#include <portaudio.h>

#include "graingenerator.hpp"

using namespace audioelectric;

#define BUFSIZE 256

struct paData {
  GrainGenerator<float>* graingen;
  GrainParams<float> params;
};

class StreamingGrainGenTest : public ::testing::Test {
protected:
  
  PaStream *stream = nullptr;
  Waveform<float> shape;
  Waveform<float> carrier;

  void SetUp(void) {
    GenerateGaussian(shape, 48000, (float)0.15);
    GenerateTriangle(carrier, 48000, (float)0);
  }

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
  
  virtual void runTest(double fs, GrainParams<float> params, GrainParams<float> rand ={0,0,0,0}) {

    printf("Playing simple grains: \n");
    printf("\tfs\tdensity\tlength\tfreq\tampl\n");
    printf("\t%0.0f\t%0.0f\t%0.2f\t%0.0f\t%0.2f\n\n", fs, params.density, params.length, params.freq, params.ampl);
    printf("\tdrand\tlrand\tarand\tfrand\n");
    printf("\t%0.3f\t%0.3f\t%0.3f\t%0.3f\n", rand.density, rand.length, rand.freq, rand.ampl);
    
    // Normalize parameters
    params.density /= fs;
    params.length *= fs;
    params.freq /= fs;

    GrainGenerator<float> graingen(shape, carrier);
    graingen.setDensityRand(rand.density);
    graingen.setLengthRand(rand.length);
    graingen.setAmplRand(rand.ampl);
    graingen.setFreqRand(rand.freq);
    graingen.applyInputs(params);

    paData data = {
      &graingen,
      params
    };
    
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    
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

  void runTestSuite(double fs, GrainParams<float> params) {
    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0,1);
  
    runTest(fs, params);
    
    runTest(fs, params, GrainParams<float>(dist(gen),0,0,0));
    runTest(fs, params, GrainParams<float>(0, dist(gen),0,0));
    runTest(fs, params, GrainParams<float>(0,0,dist(gen),0));
    runTest(fs, params, GrainParams<float>(0,0,0, dist(gen)));
    runTest(fs, params, GrainParams<float>(dist(gen),dist(gen),dist(gen),dist(gen)));
  }
  
};


TEST_F(StreamingGrainGenTest, sparseShort) {
  runTestSuite(48000, GrainParams<float>(10, 0.01, 440, 0.25));
}

TEST_F(StreamingGrainGenTest, sparseLong) {
  runTestSuite(48000, GrainParams<float>(10, 0.1, 440, 0.25));
}

TEST_F(StreamingGrainGenTest, denseShort) {
  runTestSuite(48000, GrainParams<float>(100, 0.01, 440, 0.25));
}

TEST_F(StreamingGrainGenTest, denseLong) {
  runTestSuite(48000, GrainParams<float>(100, 0.033, 440, 0.25));
}

TEST_F(StreamingGrainGenTest, veryDenseShort) {
  runTestSuite(48000, GrainParams<float>(1000, 0.01, 440, 0.25));
}

class SweepingGrainGenTest : public StreamingGrainGenTest {

public:
  
  virtual void runTest(double fs, GrainParams<float> params, GrainParams<float> rand ={0,0,0,0}) {

    printf("Playing simple grains: \n");
    printf("\tfs\tdensity\tlength\tfreq\tampl\n");
    printf("\t%0.0f\t%0.0f\t%0.2f\t%0.0f\t%0.2f\n\n", fs, params.density, params.length, params.freq, params.ampl);
    printf("\tdrand\tlrand\tarand\tfrand\n");
    printf("\t%0.3f\t%0.3f\t%0.3f\t%0.3f\n", rand.density, rand.length, rand.freq, rand.ampl);

    // Normalize parameters
    params.density /= fs;
    params.length *= fs;
    params.freq /= fs;
    
    GrainGenerator<float> graingen(shape, carrier);
    graingen.setDensityRand(rand.density);
    graingen.setLengthRand(rand.length);
    graingen.setAmplRand(rand.ampl);
    graingen.setFreqRand(rand.freq);
    graingen.applyInputs(params);

    paData data = {
      &graingen,
      params
    };
    
    PaError err = Pa_Initialize();
    ASSERT_EQ(err, paNoError) << "PA error during init: " << Pa_GetErrorText(err);
    
    auto paCallback = [] (const void *input, void *output,
                          unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void *graingen_data)
                        {
                          paData *data = static_cast<paData*>(graingen_data);
                          data->graingen->applyInputs(data->params);
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
      data.params.density = params.density*(double)playtime/2500;
    }
    data.params.density = params.density;
    playtime = 0;
    printf("Sweeping length...\n");
    tstart = std::chrono::system_clock::now();
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.params.length = params.length*(double)playtime/2500;
    }
    data.params.length = params.length;
    playtime = 0;
    printf("Sweeping frequency...\n");
    tstart = std::chrono::system_clock::now();
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.params.freq = params.freq*(double)playtime/2500;
    }
    data.params.freq = params.freq;
    playtime = 0;
    printf("Sweeping amplitude...\n");
    tstart = std::chrono::system_clock::now();
    while (playtime < 5000) {
      playtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tstart).count();
      data.params.ampl = params.ampl*(double)playtime/2500;
    }
    data.params.ampl = params.ampl;
    killStream();
  }
    
};

TEST_F(SweepingGrainGenTest, denseLong) {
  runTest(48000, GrainParams<float>(100, 0.033, 440, 0.25));
}

TEST_F(SweepingGrainGenTest, sparseLong) {
  runTestSuite(48000, GrainParams<float>(10, 0.1, 440, 0.25));
}

TEST_F(SweepingGrainGenTest, denseLongRand) {
  runTest(48000, GrainParams<float>(100, 0.033, 440, 0.25), GrainParams<float>(0.1, 0.1, 0.1, 0.1));
}


