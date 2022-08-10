#include "ring_buffer.h"

void ring_buffer_put (Ring_type *buffer, uint8_t *str) {
  uint16_t i = (uint16_t)(buffer->head + 1) % UART_RX_BUFFER_SIZE;
  if(i != buffer->tail)
  {
    buffer->buf_arr[buffer->head] = *str;
    buffer->head = i;
  }
  return;
}

void ring_buffer_pop (Ring_type *buffer, uint8_t *str) {
  uint16_t i = (uint16_t)(buffer->tail + 1) % UART_RX_BUFFER_SIZE;
  if(buffer->tail != buffer->head)
  {
    *str = buffer->buf_arr[buffer->tail];
    buffer->tail = i;
  }
  return;
}

uint8_t ring_buffer_peek (Ring_type *buffer, uint16_t indx) {
   return buffer->buf_arr[buffer->tail+indx]%UART_RX_BUFFER_SIZE;
}

uint16_t ring_buffer_size (Ring_type *buffer) {
  return buffer->head-buffer->tail;
}
 

//void ring_buffer_peek (Ring_type *buffer, uint8_t *str) {
//  while (buffer->tail != buffer->head) {
//    uint16_t i = (uint16_t)(buffer->tail) % UART_RX_BUFFER_SIZE;
//      if (buffer->buf_arr[i] != '\r')
//      {
//        *str = buffer->buf_arr[buffer->tail];
//        buffer->tail = i;
//        i++;
//      }  
//  }
//}