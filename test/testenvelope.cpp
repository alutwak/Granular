#include <gtest/gtest.h>

#include "envelope.hpp"

using namespace audioelectric;

TEST(envelope, gate) {
  Envelope<double> env(1,1,1,1);

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

  Envelope<double> env(10, 5, 0.5, 5);
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

  Envelope<double> env(10, 5, 0 , 0);
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
  Envelope<double> env(3, 10, 3, 5, 0.5, 5);
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
  Envelope<double> env(100, 5, 0 , 0);
  printf("Testing attack reduction...\n");
  env.gate(true);
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.05);
  env.setAttack(10);
  for (int i=0; i<5; i++ )
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.8);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing attack increase...\n");
  env.gate(true);
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.5);
  env.setAttack(100);
  for (int i=0; i<95; i++)
    env.increment();
  EXPECT_NEAR(env.value(), 1, 1e-9);
  env.increment();
  EXPECT_NEAR(env.value(), 0.8, 1e-9);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing attack decrease beyond current time...\n");
  env.gate(true);
  for (int i=0; i<11; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.11);
  env.setAttack(10);
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.8);
}

TEST(envelope, dec_change) {
  Envelope<double> env(1, 100, 0 , 0);
  printf("Testing decay reduction...\n");
  env.gate(true);
  env.increment(); //Get through the attack
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1-0.05);
  env.setDecay(10);
  for (int i=0; i<5; i++ )
    env.increment();
  EXPECT_NEAR(env.value(), 0, 1e-9);
  env.increment();
  EXPECT_NEAR(env.value(), 0, 1e-9);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing decay increase...\n");
  env.gate(true);
  env.increment(); //Get through the attack  
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.5);
  env.setDecay(100);
  for (int i=0; i<95; i++)
    env.increment();
  EXPECT_NEAR(env.value(), 0, 1e-9);
  env.increment();
  EXPECT_NEAR(env.value(), 0, 1e-9);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing decay decrease beyond current time...\n");
  env.gate(true);
  env.increment(); //Get through the attack
  for (int i=0; i<11; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1-0.11);
  env.setDecay(10);
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
}

TEST(envelope, rel_change) {
  Envelope<double> env(1, 1, 1 , 100);
  printf("Testing release reduction...\n");
  env.gate(true);
  env.increment(); //Get through the attack
  env.gate(false); //Relase env
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1-0.05);
  env.setRelease(10);
  for (int i=0; i<5; i++ ) {
    EXPECT_TRUE(env);
    env.increment();
  }
  EXPECT_FALSE(env);

  printf("Testing release increase...\n");
  env.gate(true);
  env.increment(); //Get through the attack
  env.gate(false); //Release env
  for (int i=0; i<5; i++) 
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.5);
  env.setRelease(100);
  for (int i=0; i<95; i++) {
    EXPECT_TRUE(env);
    env.increment();
  }
  EXPECT_FALSE(env);

  printf("Testing release decrease beyond current time...\n");
  env.gate(true);
  env.increment(); //Get through the attack
  env.gate(false); //Release env
  for (int i=0; i<11; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1-0.11);
  env.setRelease(10);
  EXPECT_DOUBLE_EQ(env.value(), 0);
  EXPECT_FALSE(env);
}

TEST(envelope, del_change) {
  Envelope<double> env(100, 1, 0, 1, 1 , 1);
  printf("Testing delay reduction...\n");
  env.gate(true);
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.setDelay(10);
  for (int i=0; i<5; i++ )
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing delay increase...\n");
  env.gate(true);
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.setDelay(100);
  for (int i=0; i<95; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing delay decrease beyond current time...\n");
  env.gate(true);
  for (int i=0; i<11; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.setDelay(10);
  EXPECT_DOUBLE_EQ(env.value(), 0);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
}

TEST(envelope, hold_change) {
  Envelope<double> env(0, 1, 100, 1, 0, 1);
  printf("Testing hold reduction...\n");
  env.gate(true);
  env.increment(); //Get through attack
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.setHold(10);
  for (int i=0; i<5; i++ )
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing hold increase...\n");
  env.gate(true);
  env.increment(); //Get through attack  
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.setHold(100);
  for (int i=0; i<95; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing hold decrease beyond current time...\n");
  env.gate(true);
  env.increment(); //Get through attack  
  for (int i=0; i<11; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.setHold(10);
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0);
}

TEST(enveloope, sus_change) {
  Envelope<double> env(1,10, 1, 10);
  printf("Testing sustain reduction during decay...\n");
  env.gate(true);
  env.increment(); //Get through attack
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.setSustain(0.5);
  for (double i=0; i<5; i++ ) {
    EXPECT_DOUBLE_EQ(env.value(), 1 - 0.5*i/5.);
    env.increment();
  }
  EXPECT_DOUBLE_EQ(env.value(), 0.5);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.5);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing sustain increase during decay...\n");
  env.gate(true);
  env.increment(); //Get through attack
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 0.75);
  env.setSustain(1);
  for (double i=0; i<5; i++ ) {
    EXPECT_DOUBLE_EQ(env.value(), 0.75 + 0.25*i/5.);
    env.increment();
  }
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  //Close the gate and make sure the envelope completes
  env.gate(false);
  while(env)
    env.increment();

  printf("Testing sustain change during sustain...\n");
  env.gate(true);
  env.increment(); //Get through attack
  for (int i=0; i<5; i++)
    env.increment();
  EXPECT_DOUBLE_EQ(env.value(), 1);
  env.setSustain(0.5);
  EXPECT_DOUBLE_EQ(env.value(), 0.5);
  env.setSustain(0.999);
  EXPECT_DOUBLE_EQ(env.value(), 0.999);
  env.increment();
  env.setSustain(0.666);
  EXPECT_DOUBLE_EQ(env.value(), 0.666);
  //Close the gate and make sure the envelope completes
  env.setSustain(0.2);
  EXPECT_DOUBLE_EQ(env.value(), 0.2);
  env.gate(false);
  for (double i=0; i<10; i++) {
    env.increment();
    EXPECT_DOUBLE_EQ(env.value(), 0.2-0.2*i/10.);
  }
}
