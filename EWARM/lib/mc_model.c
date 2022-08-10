//==================================================================================================
//INCLUDE ==========================================================================================
//
#include "mc_model.h"
#include "mc_cli.h"
#include "CLI.h"
#include "ring_buffer.h"
#include "dm542_move.h"
#include "stm32f1xx_it.h"
//==================================================================================================
//MACROS ===========================================================================================
//

//==================================================================================================
//TYPES ============================================================================================
//

//==================================================================================================
//LOCAL IFACE PROTOTYPES ===========================================================================
//
static void onMoveDone (void *obs);
static void onTMoveDone (void *obs);



//==================================================================================================
//VARIABLES ========================================================================================
//
McModel mcModel;
//==================================================================================================
//CONSTANTS ========================================================================================
//

//==================================================================================================
//PUBLIC FUNCTIONS =================================================================================
//
void mc_model_init (void* cfg) {
  mcModel.position = 0;
  dm542_init();
}

void mc_model_subscribe (void*obs, McModelOnEvent onEvent) {
  mcModel.obs=obs;
  mcModel.onEvent=onEvent;
}

void mc_model_move (int lenght) {    //micrometr  
  int delta = convert_lenght_to_steps(lenght);
  if (delta + mcModel.position < 0) {
    delta = -mcModel.position; 
    mcModel.delta = delta;
    dm542_ccw (delta,  DM542_SPEED_25K, onMoveDone, NULL);
  }
  else if (delta + mcModel.position > MC_MODEL_POSITION_MAX) {
    delta = MC_MODEL_POSITION_MAX - mcModel.position;
    mcModel.delta = delta;
    dm542_ccw (delta,  DM542_SPEED_25K, onMoveDone, NULL);
  }
 
  else if (delta > 0) {
    mcModel.delta = delta;
    dm542_ccw (delta,  DM542_SPEED_25K, onMoveDone, NULL);
  }
  else if (delta < 0) {
    mcModel.delta = delta;
    dm542_ccw (delta,  DM542_SPEED_25K, onMoveDone, NULL);
  }
}

void mc_model_move_by_timer (int moveCount, int moveInterval, int delta) {
    uint32_t timeEnd;
    mcModel.moveCount = moveCount;
    mcModel.moveInterval = moveInterval;
//    mcModel.moveDelta = delta;
    for (mcModel.moveCnt = 0; mcModel.moveCnt < moveCount; mcModel.moveCnt++) {
    timeEnd = HAL_GetTick() + moveInterval*1000; 
    while (HAL_GetTick() != timeEnd) continue;
      if (delta + mcModel.position < 0 && mcModel.position > 0) {
        delta = -mcModel.position;
        mcModel.moveDelta = delta;
        dm542_ccw (delta,  DM542_SPEED_25K, onTMoveDone, NULL);
      }
      else if (delta + mcModel.position > MC_MODEL_POSITION_MAX && mcModel.position < MC_MODEL_POSITION_MAX) {
        delta = MC_MODEL_POSITION_MAX - mcModel.position;
        mcModel.moveDelta = delta;
        dm542_ccw (delta,  DM542_SPEED_25K, onTMoveDone, NULL);
      }
      else if (delta > 0) {
        mcModel.moveDelta = delta;
        dm542_ccw (delta,  DM542_SPEED_25K, onTMoveDone, NULL);
      }
      else if (delta < 0) {
        mcModel.moveDelta = delta;
        dm542_ccw (delta,  DM542_SPEED_25K, onTMoveDone, NULL);
      }
    }
  return;
}

int convert_lenght_to_steps (int lenght) {
  static const int stepsPerRef = 12800;
  static const int uMetrPerRef = 5000;
  return lenght * stepsPerRef / uMetrPerRef;
}
//==================================================================================================
//LOCAL FUNCTIONS ==================================================================================
//
static void onMoveDone (void *obs) {
  mcModel.position += mcModel.delta;
  mcModel.onEvent(mcModel.obs, MC_MODEL_MOVE_DONE);
}

static void onTMoveDone (void *obs) {
  if (mcModel.moveCnt == mcModel.moveCount) {
    mcModel.position += mcModel.moveDelta;
    mcModel.onEvent(mcModel.obs, MC_MODEL_TMOVE_DONE);
  }
  else {
    mcModel.position += mcModel.moveDelta;
    mcModel.onEvent(mcModel.obs, MC_MODEL_TMOVE_STEP_DONE);
  }
}




