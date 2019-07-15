#include <gtest/gtest.h>

#include "envelope.hpp"

using namespace audioelectric;

TEST(envelope, ASDR) {

  Envelope env(10, 5, 0.5, 5);

  EXPECT_FALSE(env);

  env.gate(true);
  EXPECT_EQ(env.value(), 0);

  printf("Testing attack...\n");
  for (double i=1; i<=10; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  printf("Testing decay...\n");
  for (double i=9; i>=5; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  printf("Testing sustain...\n");
  for (int i=0; i<100; i++) {
    // Sustain (should go on forever)
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), 0.5);
  }
  env.gate(false);
  printf("Testing release...\n");
  for (double i=4; i>=0; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  EXPECT_FALSE(env);
  env.increment();
  EXPECT_FALSE(env);  
}
