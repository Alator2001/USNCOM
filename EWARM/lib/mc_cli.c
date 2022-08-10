//==================================================================================================
//INCLUDE ==========================================================================================
//
#include <stdlib.h>

#include "mc_cli.h"
#include "CLI.h"
#include "ring_buffer.h"
#include "dm542_move.h"
#include "mc_model.h"
#include "usbd_cdc_if.h"


//==================================================================================================
//MACROS ===========================================================================================
//

//==================================================================================================
//TYPES ============================================================================================
//


//==================================================================================================
//LOCAL IFACE PROTOTYPES ===========================================================================
//
static int find_zero ( char *writeBuffer, size_t writeBufferLen, const char *commandString);
static int move( char *writeBuffer, size_t writeBufferLen, const char *commandString);
static int move_by_timer( char *writeBuffer, size_t writeBufferLen, const char *commandString);
static void onEvent(void*obs, McModelEvent Event);
static int get_command (Ring_type *buffer);
static int check_parameter (long delta);
static int convert_steps_to_lenght (int steps);

//==================================================================================================
//VARIABLES ========================================================================================
//

char buffer_command [256];
char buff_out [256];

int error_flag;

Ring_type ring_buf1;
//==================================================================================================
//CONSTANTS ========================================================================================
//

const Command findZeroCmd =
{
	.command = "zero",
	.helpString = "\r\nzero\r\nsearch zero\r\n\r\n",
	.commandInterpreter = find_zero,
	.expectedNumberOfParameters = 0
};

const Command moveCmd =
{
	.command = "move",
	.helpString = "\r\nmove:\r\nmove to specify distance [uM]\r\n\r\n",
	.commandInterpreter = move,
	.expectedNumberOfParameters = 1
};

const Command moveByTimerCmd =
{
	.command = "tmove",
	.helpString = "\r\ntmove\r\nmove with timer\r\n\r\n",
	.commandInterpreter = move_by_timer,
	.expectedNumberOfParameters = 3
};


//==================================================================================================
//PUBLIC FUNCTIONS =================================================================================
//
void mci_cli_process_comand (void) {
  if (get_command(&ring_buf1)) 
    {
      int len = 0;
      while (process_command(buffer_command, &buff_out[len], 256-len))
      {
        len = strlen (buff_out);
      }
    CDC_Transmit_FS((uint8_t*)buff_out, strlen(buff_out));

    }
}

void mci_cli_init (void* mcModel) {
  register_command(&findZeroCmd);
  register_command(&moveCmd);
  register_command(&moveByTimerCmd);
  mc_model_subscribe(NULL, onEvent);
  
}
//==================================================================================================
//LOCAL FUNCTIONS ==================================================================================
//
static void onEvent(void*obs, McModelEvent Event) {
    int delta = mc_model_get_delta();
    int moveDelta = mc_model_get_moveDelta();
    int moveCout = mc_model_get_moveCount();
    int moveInterval = mc_model_get_moveInterval();
    int moveCnt = mc_model_get_moveCnt();
    int position = mc_model_get_position();
    static char fake_buff [128];
    if (Event==MC_MODEL_MOVE_DONE) {
      int size = sprintf (fake_buff, "\r\nCommand MOVE complite. Delta = %d Position = %d\r\n", convert_steps_to_lenght(delta), convert_steps_to_lenght(position));
      CDC_Transmit_FS ((uint8_t*)fake_buff, size);
    }
    else if (Event==MC_MODEL_TMOVE_DONE) {
      int size = sprintf (fake_buff, "\r\nCommand TMOVE complite. Delta = %d Position = %d\r\n", moveDelta, convert_steps_to_lenght(position));
      CDC_Transmit_FS ((uint8_t*)fake_buff, size);
    }
    else if (Event==MC_MODEL_TMOVE_STEP_DONE) { 
      int size = sprintf (fake_buff, "%d MOVE DONE\r",moveCnt);
      CDC_Transmit_FS ((uint8_t*)fake_buff, size); 
    }
    else if (Event==MC_MODEL_ERROR_1) {
      int size = sprintf (fake_buff, "\r\nError, parameter LENGHT more than Range.  Range: [-1000000; +1000000]\r\n");
      CDC_Transmit_FS ((uint8_t*)fake_buff, size); 
    }
    else if (Event==MC_MODEL_ERROR_2) {
      int size = sprintf (fake_buff, "\r\nError, parameter LENGHT less than Range.  Range: [-1000000; +1000000]\r\n");
      CDC_Transmit_FS ((uint8_t*)fake_buff, size); 
    }
    else if (Event==MC_MODEL_ERROR_3) {
      int size = sprintf (fake_buff, "Error, parameter LENGHT not changed.");
      CDC_Transmit_FS ((uint8_t*)fake_buff, size); 
    }     
}

static int find_zero ( char *writeBuffer, size_t writeBufferLen, const char *commandString )
{
  CDC_Transmit_FS ("\r\nhello\r\n", 9);  	
  return false;
}

static int move ( char *writeBuffer, size_t writeBufferLen, const char *commandString )
{
  int position = mc_model_get_position();
  int parametr_srting_lenght_1;
  char *parameter_1;
  parameter_1 = get_parameter ((char*)commandString, 1, &parametr_srting_lenght_1);
  long value_parametr = strtol (parameter_1, NULL, 10);
  int position_ln = convert_steps_to_lenght(position);
  if (check_parameter(value_parametr+position_ln)==1) {
    mc_model_move(value_parametr);
  }  
  else onEvent(NULL, check_parameter(value_parametr+position_ln));
  return 0;
}

static int move_by_timer ( char *writeBuffer, size_t writeBufferLen, const char *commandString )
{
  int parametr_srting_lenght_1; // count
  int parametr_srting_lenght_2; // interval
  int parametr_srting_lenght_3; // delta
  int position = mc_model_get_position();
  char *parameter_1, *parameter_2, *parameter_3;
  parameter_1 = get_parameter ((char*)commandString, 1, &parametr_srting_lenght_1);
  parameter_2 = get_parameter ((char*)commandString, 2, &parametr_srting_lenght_2);
  parameter_3 = get_parameter ((char*)commandString, 3, &parametr_srting_lenght_3);
  long value_parametr_1 = strtol (parameter_1, NULL, 10);
  long value_parametr_2 = strtol (parameter_2, NULL, 10);
  long value_parametr_3 = strtol (parameter_3, NULL, 10);
  int position_ln = convert_steps_to_lenght(position);
  if (check_parameter((value_parametr_3+position_ln)*value_parametr_1)==1)
  {
    value_parametr_3 = convert_lenght_to_steps(value_parametr_3);
    CDC_Transmit_FS ("\n\r", 2);
//    for (mcModel.moveCnt = 0; mcModel.moveCnt < value_parametr_1; mcModel.moveCnt++) {
    //for (int i = 0; i < value_parametr_1; i++) {
      mc_model_move_by_timer(value_parametr_1, value_parametr_2, value_parametr_3);
    }
  else onEvent(NULL, check_parameter((value_parametr_3+position_ln)*value_parametr_1));
    return 0;
}


static int get_command (Ring_type *buffer) {
  uint16_t buffer_size = ring_buffer_size (buffer);
  for (int i=0; i<buffer_size; i++)  {
    if (ring_buffer_peek (buffer, i) == '\r')  {
      for (int j=0; j<=i; j++) {
        ring_buffer_pop(buffer, (uint8_t*)&buffer_command[j]);
        if (buffer_command[j] == '\b') {               
          // if \b in 0 index input
          if (j==0) {
            j=j-1;
            i=i-1;
          } else {
            j=j-2;
            i=i-2;
          }
        }
        
      }
      return 1; 
    }
  }
  return 0;
}

static int check_parameter (long delta) {
  if (delta>=MC_MODEL_MOVE_LENGHT_MAX) {
    return error_flag = MC_MODEL_ERROR_1;
  }
  else if (delta<=MC_MODEL_MOVE_LENGHT_MIN) {
    return error_flag = MC_MODEL_ERROR_2;
  }
  else if (delta == 0) {
    return error_flag = MC_MODEL_ERROR_3;
  }
  else return error_flag = 1;
}

static int convert_steps_to_lenght (int steps) {
  static const int stepsPerRef = 12800;
  static const int uMetrPerRef = 5000;
  return steps * uMetrPerRef / stepsPerRef ;
}