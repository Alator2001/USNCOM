#include "dm542_move.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_gpio.h"

#define MAX 3


static void set_dir_ccw (void);
static void set_dir_cw(void);
static void rotate (int uSteps, void (*onDone) (void *obs), void *obs);
static void set_speed (Dm542_Speed speed);



Dm542Control dm542Control;

void TIM1_UP_IRQHandler (void) { //функция прерыания
  LL_TIM_ClearFlag_UPDATE(TIM1);
  dm542Control.uSteps -= MAX;
  if (MAX*2 > dm542Control.uSteps && MAX < dm542Control.uSteps) {
    LL_TIM_SetRepetitionCounter (TIM1, dm542Control.uSteps-MAX-1);
  }
  else if (dm542Control.uSteps <= 0) {
    LL_TIM_DisableCounter(TIM1); //off counter
    LL_TIM_DisableIT_UPDATE(TIM1); //off counter inter gen
    dm542Control.uSteps = 0;
    dm542Control.onDone(dm542Control.obs);
  }
}

void dm542_break (void) {
  LL_TIM_DisableCounter(TIM1);
  LL_TIM_DisableIT_UPDATE(TIM1);
  LL_TIM_ClearFlag_UPDATE(TIM1);
  LL_TIM_SetCounter (TIM1, 0);
}

void dm542_init (void) {
  NVIC_EnableIRQ (TIM1_UP_IRQn);
}

void dm542_ccw (long uSteps,  Dm542_Speed speed, void (*onDone) (void *obs), void *obs) {
  set_dir_ccw ();
  set_speed (speed);
  rotate (uSteps, onDone, obs);
}

void dm542_cw (long uSteps,  Dm542_Speed speed, void (*onDone) (void *obs), void *obs) {
  set_dir_cw ();
  set_speed (speed);
  rotate (uSteps, onDone, obs);
}

static void set_dir_ccw (void) {
  LL_GPIO_ResetOutputPin (GPIOA, LL_GPIO_PIN_8);
  }

static void set_dir_cw (void) {
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
} 

static void set_speed (Dm542_Speed speed) {
  static const uint16_t speedmode[] = {
                                 [DM542_SPEED_100K] = 48-1, 
                                 [DM542_SPEED_25K] = 48*4-1, 
                                 [DM542_SPEED_5K] = 48*20-1,
                                 [DM542_SPEED_1K] = 48*100-1,
                                 [DM542_SPEED_200] = 48*500-1
                                };
  LL_TIM_SetAutoReload (TIM1, speedmode[speed]);
}

static void rotate (int uSteps, void (*onDone) (void *obs), void *obs) { //add persec
  dm542Control.onDone = onDone;
  dm542Control.obs = obs;
  dm542Control.uSteps = uSteps;
  LL_TIM_ClearFlag_UPDATE(TIM1);
  if (MAX >= dm542Control.uSteps) {
     LL_TIM_SetRepetitionCounter (TIM1, dm542Control.uSteps-1);
  }
  else LL_TIM_SetRepetitionCounter (TIM1, MAX-1);
  LL_TIM_GenerateEvent_UPDATE (TIM1);
  while (!LL_TIM_IsActiveFlag_UPDATE(TIM1)) continue;
  LL_TIM_ClearFlag_UPDATE(TIM1);
  if (MAX*2 > dm542Control.uSteps && MAX < dm542Control.uSteps) {
    LL_TIM_SetRepetitionCounter (TIM1, dm542Control.uSteps-MAX-1);
  }
  LL_TIM_CC_EnableChannel (TIM1, LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableAllOutputs (TIM1);
  LL_TIM_EnableIT_UPDATE(TIM1);
  LL_TIM_EnableCounter(TIM1);
 }
