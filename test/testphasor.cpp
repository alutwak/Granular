
#include <gtest/gtest.h>

#include "phasor.hpp"

using namespace audioelectric;

class PhasorTest : public ::testing::Test {
protected:
  
    Waveform<double> wt = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  double *test;

  virtual void SetUp(void) override {
    test = new double[wt.size()];
  }

  virtual void TearDown(void) override {
    delete[] test;
  }

  /* Since the values in wt have a constant slope of 1, iterations should increment at the same rate as rate
   */
  void testPhasor(double rate, double start=0) {
    auto phs = Phasor<double>(wt,rate,start);
    auto begin = phs;
    double check = phs.value();
    ASSERT_EQ(check, start) << "when rate = " << rate << " and start = " << start;
    if (rate == 0) {
      EXPECT_TRUE(phs.generate(&test,3)); //generate 3 frames
      EXPECT_TRUE(phs==begin);            //Should still be at the beginning
      for (int i=0;i<3;i++) {
        EXPECT_FLOAT_EQ(test[i],check) << "when rate = " << rate << " and start = " << start;
        check += rate;
      }
    }
    else {
      int iters = 0;
      while (phs) {
        if (rate > 0)
          EXPECT_TRUE(phs>=begin) << "when rate = " << rate << " and start = " << start;
        else
          EXPECT_TRUE(phs<=begin) << "when rate = " << rate << " and start = " << start;
        double val = phs.value();
        EXPECT_FLOAT_EQ(val,check) << "when rate = " << rate << " and start = " << start;
        phs.increment();
        check += rate;
        iters++;
      }
      EXPECT_EQ(phs.value(), 0) << "finished phasor should output value of 0";
    }
  }

  void testVariableRatePhasor(void) {
    double rate = 1;
    auto phs = Phasor<double>(wt,rate,0);
    auto begin = phs;
    double val = phs.value();
    ASSERT_EQ(val,wt[0]);
    while(phs) {
      rate++;
      phs.setRate(rate);
      EXPECT_TRUE(phs>=begin);
      phs.generate(&test,1);
      ASSERT_FLOAT_EQ(test[0],val) << "when rate = " << rate;
      val += rate;
    }
    rate = -rate;
    phs.setRate(rate);
    phs.increment();
    val += rate;
    while(phs) {
      rate++;
      phs.setRate(rate);
      EXPECT_TRUE(phs>=begin);
      phs.generate(&test,1);      
      EXPECT_FLOAT_EQ(test[0],val) << "when rate = " << rate;
      val += rate;
    }
  }

};


TEST_F(PhasorTest, basic) {
  testPhasor(0, 1);
  testPhasor(0.5);
  testPhasor(0.5,1.2);
  testPhasor(1.0);
  testPhasor(1.0, 2.1);
  testPhasor(2.0);
  testPhasor(4.0);
  testPhasor(0.3428);
  testPhasor(1.2864);
  testPhasor(1.2864, 0.1);
  testPhasor(-0.5);
  testPhasor(-0.5,1.2);
  testPhasor(-1.0);
  testPhasor(-1.0, 2.1);
  testPhasor(-2.0);
  testPhasor(-4.0);
  testPhasor(-0.3428);
  testPhasor(-1.2864);
  testPhasor(-1.2864, 0.1);
  testVariableRatePhasor();
}
