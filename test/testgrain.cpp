#include <cmath>
#include <gtest/gtest.h>

#include "grain.hpp"

using namespace audioelectric;

class GrainTest : public ::testing::Test {
protected:

  Waveform<double> carrier = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  Waveform<double> shape = {0.0, 1.0, 0.0};

  void TestGrain(double crate, double srate, double ampl) {
    ASSERT_GT(crate, 0);
    ASSERT_GT(srate, 0);
    double ccheck = 0;
    double scheck = 0;
    printf("Testing grain with crate(%f) and srate(%f)\n", crate, srate);
    Grain<double> grain(carrier, crate, shape, srate, ampl);
    int itr = 0;
    while (grain) {
      double val = grain.value();
      double check = ccheck*scheck*ampl;
      EXPECT_FLOAT_EQ(val, check) << "iter " << itr << ": ccheck=" << ccheck << ", scheck=" << scheck;
      grain.increment();
      ccheck += crate;
      if (ccheck > 9.0)
        ccheck -= 9.0;
      scheck += srate;
      if (scheck > 1.-srate) { //">1-srate" instead of ">=1." helps deal with floating point errors
        scheck -= scheck-1;
        srate = -srate;
      }
      itr++;
    }
    EXPECT_EQ(grain.value(), 0) << "finished grain should output value of 0";
  }
  
};

TEST_F(GrainTest, basic) {
  TestGrain(1, 1, 1);
  TestGrain(0.5, 1, 1);
  TestGrain(1, 0.5, 1);
  TestGrain(2, 1, 1);
  TestGrain(1, 0.1, 1);
  TestGrain(1, 1, 0.5);
}
