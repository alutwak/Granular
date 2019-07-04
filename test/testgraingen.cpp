
#include <gtest/gtest.h>
#include <sndfile.h>

#include "graingenerator.hpp"

using namespace audioelectric;

#define BUFSIZE 1024

class GrainGenTest : public ::testing::Test {
protected:

  void SimpleTest(double fs, double density, double length, double freq, double ampl) {
    GrainGenerator<double> graingen(fs);
    SF_INFO info;
    info.channels = 1;
    info.samplerate = fs;
    info.format = SF_FORMAT_PCM_16 | SF_FORMAT_WAV;
    std::stringstream fname;
    fname << "graingen-simple-" << density << "-" << length << "-" << freq << "-" << ampl << "-" << (int)fs <<"Hz.wav";
    
    SNDFILE* fout = sf_open(fname.str().c_str(), SFM_WRITE, &info);
    double buffer[BUFSIZE];

    size_t ctl_per = fs/1000;   // Give the control period 1ms
    double *b = buffer;
    for (size_t i=0; i<fs*10; i++) {
      if (i%ctl_per == 0) {
        graingen.updateGrains(density, length, freq, ampl);
      }
      *b = graingen.value();
      graingen.increment();
      b++;
      if (b >= buffer + BUFSIZE) {
        sf_write_double(fout, buffer, BUFSIZE);
        b = buffer;
      }
    }
    
    sf_close(fout);
    
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
