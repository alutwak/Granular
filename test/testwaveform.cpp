
#include <gtest/gtest.h>

#include "waveform.hpp"

using namespace audioelectric;

class SimpleWaveformTest : public ::testing::Test {
protected:

  Waveform<double> wf = {0.0, 1.0, 2.0, 3.0};

  void testBasic(void) {
    EXPECT_EQ(wf.size(),4);
    //Check element access with integers
    for (int i=0;i<4;i++) {
      double d = wf[i];
      EXPECT_EQ(d,i);
      const double& dconst = wf[i];
      EXPECT_FLOAT_EQ(d,dconst);
    }
    //Check element access with doubles
    for (double j=0.0; j<3.1; j+=0.1) {
      double d = wf.interpolate(j);
      if (j > 3)
        EXPECT_FLOAT_EQ(d,0);
      else
        EXPECT_FLOAT_EQ(d,j);
    }
  }
  
  /* Since the values in wf have a constant slope of 1, iterations should increment at the same rate as speed
   */
  void testIterSpeed(double speed) {
    auto wfiter = wf.ibegin(speed);
    double val = *wfiter;
    ASSERT_EQ(val, wf[0]);
    while (wfiter != wf.iend(speed)) {
      EXPECT_FLOAT_EQ(val,*(wfiter++)) << "when speed = " << speed;
      val += speed;
    }
  }

  void testReverseIter(double speed) {
    auto riter = wf.ribegin(speed);
    double val = *riter;
    ASSERT_EQ(val,wf[wf.size()-1]);
    while (riter != wf.riend(speed)) {
      EXPECT_FLOAT_EQ(val,*(riter++)) << "when speed = " << speed;
      val -= speed;
    }
  }

  void testCopy(int len) {
    Waveform<double> wf_new(wf,len);
    EXPECT_EQ(wf_new.size(), len);  //Length should be correct
    double speed = (double)4/(double)len;
    int n = len/4;
    for (int i=0;i<4;i++) {
      if (i >= n)
        break;
      EXPECT_FLOAT_EQ(wf.interpolate(i*speed),wf_new[i]);
    }
  }

};

TEST_F(SimpleWaveformTest, basic) {

  testBasic();
  testIterSpeed(0.5);
  testIterSpeed(1.0);
  testIterSpeed(2.0);
  testIterSpeed(4.0);
  testIterSpeed(0.3428);
  testIterSpeed(1.2864);
  testIterSpeed(0);
  testIterSpeed(-1);

  for (int i=0;i<10;i++)
    testCopy(i);
}
