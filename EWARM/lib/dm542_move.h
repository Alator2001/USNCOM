#pragma once

#include <stdint.h>


typedef enum Dm542_Speed {  // [u_Steps_Per_Sec]
  DM542_SPEED_100K,
  DM542_SPEED_25K,
  DM542_SPEED_5K,
  DM542_SPEED_1K,
  DM542_SPEED_200,
} Dm542_Speed;

typedef struct Dm542Control {
  void (*onDone) (void *obs);
  void *obs;
  int uSteps;
} Dm542Control;

void dm542_cw (long uSteps, Dm542_Speed speed, void (*onDone) (void *obs), void *obs);
void dm542_ccw (long uSteps, Dm542_Speed speed, void (*onDone) (void *obs), void *obs);
void dm542_break (void);
void dm542_init (void);


static inline int dm542_get_steps (void);

static inline int dm542_get_steps (void) {
  extern Dm542Control dm542Control;
  return dm542Control.uSteps;
}