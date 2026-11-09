/*
 * buffers.h
 *
 *  Created on: Oct 15, 2026
 *      Author: Hamza
 */

#ifndef INC_BUFFERS_H_
#define INC_BUFFERS_H_

#include <stdint.h>
#include <string.h>


//status enum

typedef enum {
  BUF_OK = 0,
  BUF_ERROR = 1,
  BUF_UNAVAILABLE = 2,
  BUF_EMPTY = 3,
  BUF_FULL = 4
}BUFFER_Status;

//----------------------------------------------------------------------------------------------
//fifo

typedef struct{
  volatile uint32_t _write_p;
  volatile uint32_t _read_p;
  uint32_t _size;
  uint8_t* _buf;
}FIFO_t;



void FIFO_Init(FIFO_t* fifo, uint8_t* fifo_buf, uint32_t fifo_size);
BUFFER_Status FIFO_Append(FIFO_t* fifo, uint8_t* buf_in, uint32_t len);
BUFFER_Status FIFO_Pop_G(FIFO_t* fifo, uint8_t* buf_out, uint32_t len);
uint32_t FIFO_Pop(FIFO_t* fifo, uint8_t* buf_out, uint32_t len);
uint32_t FIFO_Peek(FIFO_t* fifo, uint8_t* buf_out, uint32_t len);
uint32_t FIFO_Discard(FIFO_t* fifo, uint32_t len);
uint32_t FIFO_GetEmpty(FIFO_t* fifo);
uint32_t FIFO_GetCount(FIFO_t* fifo);
void FIFO_Clear(FIFO_t* fifo);


#endif /* INC_BUFFERS_H_ */
