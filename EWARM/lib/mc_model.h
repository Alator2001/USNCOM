#define MC_MODEL_POSITION_MAX 1000000
#define MC_MODEL_MOVE_LENGHT_MIN (-MC_MODEL_POSITION_MAX)
#define MC_MODEL_MOVE_LENGHT_MAX MC_MODEL_POSITION_MAX

#include <stdint.h>

typedef enum McModelEvent{
  MC_MODEL_MOVE_DONE,
  MC_MODEL_TMOVE_DONE,
  MC_MODEL_TMOVE_STEP_DONE,
  MC_MODEL_ERROR_1,
  MC_MODEL_ERROR_2,
  MC_MODEL_ERROR_3, 
} McModelEvent;


typedef void(*McModelOnEvent)(void*obs, McModelEvent event);
int convert_lenght_to_steps (int lenght);


typedef struct McModel {
  int position; //[uSteps]
  void *obs;
  McModelOnEvent onEvent;
  int delta;
  
  
  int moveCnt;
  int moveCount;
  int moveInterval; //[sec]
  int moveDelta;
}McModel;


void mc_model_init (void* );
void mc_model_move (int);
void mc_model_subscribe (void*obs, McModelOnEvent onEvent);
void mc_model_move_by_timer (int moveCount, int moveInterval, int moveDelta);

static inline int mc_model_get_delta (void);
static inline int mc_model_get_moveInterval (void);
static inline int mc_model_get_moveCount (void);
static inline int mc_model_get_moveDelta (void);
static inline int mc_model_get_moveCnt (void);
static inline int mc_model_get_position (void);

uint32_t HAL_GetTick(void);


static inline int mc_model_get_delta (void) {
  extern McModel mcModel;
  return mcModel.delta;
}

static inline int mc_model_get_moveInterval (void) {
  extern McModel mcModel;
  return mcModel.moveInterval;
}


static inline int mc_model_get_moveCount (void) {
  extern McModel mcModel;
  return mcModel.moveCount;
}

static inline int mc_model_get_moveDelta (void) {
  extern McModel mcModel;
  return mcModel.moveDelta;
}

static inline int mc_model_get_moveCnt (void) {
  extern McModel mcModel;
  return mcModel.moveCnt;
}

static inline int mc_model_get_position (void) {
  extern McModel mcModel;
  return mcModel.position;
}

