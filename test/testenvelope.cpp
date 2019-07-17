#include <gtest/gtest.h>

#include "envelope.hpp"

using namespace audioelectric;

TEST(envelope, gate) {
  Envelope env(1,1,1,1);

  EXPECT_FALSE(env) << "Ungated envelope should be false";
  EXPECT_EQ(env.value(), 0);
  env.increment();
  EXPECT_EQ(env.value(), 0);  
  env.gate(false);
  EXPECT_FALSE(env) << "Sending false gate when gate is off should still be false";
  EXPECT_EQ(env.value(), 0);
  env.gate(true);
  EXPECT_TRUE(env) << "Envelope should be true after gating";
  env.gate(true);
  EXPECT_TRUE(env) << "Applying a true gate to envelope when it's running should not change its bool value";
  env.increment();
  EXPECT_EQ(env.value(), 1);
  env.gate(false);
  EXPECT_TRUE(env) << "After the gate closes, envelope should remain open until release phase finishes";
  EXPECT_EQ(env.value(), 1);
  env.increment();
  EXPECT_FALSE(env);
  EXPECT_EQ(env.value(), 0);
}

TEST(envelope, ADSR) {

  Envelope env(10, 5, 0.5, 5);
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

  env.gate(true);
  printf("Testing release during attack...\n");
  for (double i=1; i<=7; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  env.gate(false);
  for (double i=4; i>=0; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), .14*i);
  }
  EXPECT_FALSE(env);

  env.gate(true);
  printf("Testing release during decay...\n");
  for (double i=1; i<=10; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  for (double i=9; i>=7; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  env.gate(false);
  for (double i=4; i>=0; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), .14*i);
  }
  EXPECT_FALSE(env);
}

TEST(envelope, AD) {

  Envelope env(10, 5, 0 , 0);
  env.gate(true);
  printf("Testing attack...\n");
  for (double i=1; i<=10; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  printf("Testing decay...\n");
  for (double i=8; i>=0; i-=2) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_NEAR(env.value(), i/10, 1e-12);
  }
  for (int i=0; i<100; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_NEAR(env.value(), 0, 1e-12);
  }
  env.gate(false);
  EXPECT_FALSE(env);
  EXPECT_EQ(env.value(), 0);
}

TEST(envelope, DAHDSR) {
  Envelope env(3, 10, 3, 5, 0.5, 5);
  env.gate(true);
  EXPECT_EQ(env.value(), 0);

  printf("Testing delay...\n");
  for (int i=0; i<3; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_EQ(env.value(), 0);
  }
  printf("Testing attack...\n");
  for (double i=1; i<=10; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  printf("Testing hold...\n");
  for (int i=0; i<3; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), 1);
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

  env.gate(true);
  printf("Testing release during delay...\n");
  for (double i=1; i<=2; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_EQ(env.value(), 0);
  }
  env.gate(false);
  for (double i=4; i>=0; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), 0);
  }
  EXPECT_FALSE(env);

  env.gate(true);
  printf("Testing release during hold...\n");
  for (int i=0; i<3; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_EQ(env.value(), 0);
  }
  for (double i=1; i<=10; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/10);
  }
  for (int i=0; i<2; i++) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), 1);
  }
  env.gate(false);
  for (double i=4; i>=0; i--) {
    EXPECT_TRUE(env);
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), i/5);
  }
}

TEST(envelope, att_change) {
  FAIL() << "This test is not complete yet";
  Envelope env(100, 5, 0 , 0);
  env.gate(true);
  for (double i=1; i<2; i++) {
    env.increment();
  }
  EXPECT_DOUBLE_EQ(env.value(), 0.02);
  env.setAttack(10);
}
