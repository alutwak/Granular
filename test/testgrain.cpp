
#include <gtest/gtest.h>
#include <vector>
#include <queue>
#include <cmath>
#include <sndfile.h>
#include <portaudio.h>

#include "audioplaybacktest.hpp"
#include "grain.hpp"

using namespace audioelectric;

double interp(std::vector<double>& v, double speed, int idx) {
  if (idx < 0 || idx*speed > v.size()-1)
    return 0;
  int lidx = speed*idx;
  double diff = fmod(speed*idx, 1.0);
  return (v[lidx+1] - v[lidx])*diff + v[lidx];
}

void TestGrain(std::vector<double>& grn, double gspeed, std::vector<double>& wave, double wspeed) {
  Grain<double> grain(grn.data(), grn.size());
  Wavetable<double> wt(wave.data(), wave.size());
  auto granulator = grain.gmake(0, gspeed, wt.pbegin(wspeed));
  int i=0;
  while (granulator) {
    double gval = interp(grn,gspeed,i);
    double wval = interp(wave,wspeed,i);
    //EXPECT_TRUE(granulator) << "i=" << i << ", gspeed=" << gspeed << ", wspeed=" <<wspeed;
    EXPECT_FLOAT_EQ(*granulator,gval*wval) << "i=" << i << ", gspeed=" << gspeed << ", wspeed=" <<wspeed;
    granulator++;
    i++;
  }
  EXPECT_FALSE(granulator) << "End: gspeed=" << gspeed << ", wspeed=" <<wspeed;
}
  
TEST(graintest, basic) {

  std::vector<double> grn = {0, 0.5, 1, 0.5, 0};
  std::vector<double> wave = {-2, -1, 0, 1, 2};
  TestGrain(grn, 1, wave, 1);
  TestGrain(grn, 0.5, wave, 0.5);
  TestGrain(grn, 2, wave, 2);
  TestGrain(grn, 0.5, wave, 2);
  TestGrain(grn, 2, wave, 0.5);  
}

static int grainPlayCallback(const void *input, void *output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags, void *wt );

class GrainPlaybackTest : public AudioPlaybackTest {
protected:
  
  Wavetable<float> *source;
  Grain<float> *grain;
  SF_INFO info;
#define VOC_START 6900
#define VOC_END 240000

  virtual void SetUp(void) {

    //Read in the sample we're granulating
    info.format = 0;
    SNDFILE* testfile = sf_open("testfile.wav", SFM_READ, &info);
    ASSERT_TRUE(testfile) << "unable to open testfile.wav";
    source = new Wavetable<float>(info.frames); //We know this is just one channel
    sf_count_t nread = sf_readf_float(testfile, source->data(), info.frames);
    samplerate = info.samplerate;
    ASSERT_EQ(nread,info.frames) << "Failed to read entire testfile.wav";
    sf_close(testfile);

    //Create the grain
    Wavetable<float> gaussian;
    GenerateGaussian<float>(&gaussian, info.frames-VOC_START, 1);
    grain = new Grain<float>(dynamic_cast<Waveform<float>&>(gaussian), 1, gaussian.size());
  }

  /*!\param g_len the grain length, in milliseconds
   * \param wf_speed The waveform speed
   */
  virtual void playBack(double g_len, double wf_speed) {
    //Create the grains
    Waveform<float>::phasor phs;
    double g_speed = 1000*(double)grain->size()/info.samplerate/g_len;
    printf("Grain length: %fms\n", g_len);
    phs = source->pbegin(wf_speed, VOC_START);
    auto granu = grain->gmake(0, g_speed, phs);
    initPA(granu);
    while (granu) {}
    closePA();
  }
  
};

TEST_F(GrainPlaybackTest, play_grains) {

  playBack(1000, 1);
  playBack(500, 1);
  playBack(250, 1);
  playBack(100, 1);
  playBack(50, 1);
  playBack(25,1);
  playBack(10,1);
  playBack(5,1);
  playBack(1,1);
  playBack(0,1);
  playBack(1000,2.3);
  playBack(1000,-2.3);
  playBack(5000,1);
  playBack(100,-2);
  
}
