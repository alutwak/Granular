
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

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
  Waveform<double> wf(wave.data(), wave.size());
  auto granulator = grain.gmake(0, gspeed, wf.ibegin(wspeed));
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
