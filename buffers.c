/*
 * buffers.c
 *
 *  Created on: Oct 15, 2026
 *      Author: Hamza
 */

#include "buffers.h"



void FIFO_Init(FIFO_t* fifo, uint8_t* fifo_buf, uint32_t fifo_size)
{
  //initializes 'fifo' by setting 'fifo_buf' as the used buffer with 'fifo_size' size
  //this means that 'fifo_buf' should stay in scope as long as 'fifo' is in use
  fifo->_buf = fifo_buf;
  fifo->_size = fifo_size;
  fifo->_read_p = 0;
  fifo->_write_p = 0;
}


BUFFER_Status FIFO_Append(FIFO_t* fifo, uint8_t* buf_in, uint32_t len)
{
  //writes 'len' amount of bytes from 'buf_in' to the end of 'fifo'
  //returns BUF_OK if the bytes write correctly
  //returns BUF_ERROR if there is not enough space in 'fifo'

  if(FIFO_GetEmpty(fifo) < len) return BUF_ERROR;

  uint32_t wp_to_end = fifo->_size - fifo->_write_p; //amount of bytes between end-of-buffer and write_p
  uint32_t loc_w_p = fifo->_write_p;

  //wrap-around path
  if(wp_to_end <= len)
    {
      memcpy(fifo->_buf+loc_w_p, buf_in, wp_to_end);
      loc_w_p = 0;
      memcpy(fifo->_buf+loc_w_p, buf_in+wp_to_end, len-wp_to_end);
      loc_w_p += (len-wp_to_end);
    }
  //no wrap-around path
  else
    {
      memcpy(fifo->_buf+loc_w_p, buf_in, len);
      loc_w_p += len;
    }

  fifo->_write_p = loc_w_p;

  return BUF_OK;
}


BUFFER_Status FIFO_Pop_G(FIFO_t* fifo, uint8_t* buf_out, uint32_t len)
{
  //guarded fifo pop
  //reads 'len' amount of bytes from the start of 'fifo' to 'buf_out'
  //returns BUF_OK if bytes were read correctly
  //returns BUF_ERROR if the number of requested bytes, 'len', is greater than the available bytes
  if(FIFO_GetCount(fifo) < len) return BUF_ERROR;

  uint32_t rp_to_end = fifo->_size - fifo->_read_p; //amount of bytes between end-of-buffer and read_p
  uint32_t loc_r_p = fifo->_read_p;

  //wrap-around path
  if(rp_to_end <= len)
    {
      memcpy(buf_out, fifo->_buf+loc_r_p, rp_to_end);
      loc_r_p = 0;
      memcpy(buf_out+rp_to_end, fifo->_buf+loc_r_p, act_len-rp_to_end);
      loc_r_p += (len-rp_to_end);
    }
  ///no wrap-around path
  else
    {
      memcpy(buf_out, fifo->_buf+loc_r_p, len);
      loc_r_p += len;
    }

  fifo->_read_p = loc_r_p;

  return BUF_OK;
}


uint32_t FIFO_Pop(FIFO_t* fifo, uint8_t* buf_out, uint32_t len)
{
  //unguarded fifo pop
  //reads 'len' amount of bytes from the start of 'fifo' to 'buf_out'
  //returns number of bytes read

  uint32_t fifo_cnt = FIFO_GetCount(fifo);
  uint32_t act_len = fifo_cnt < len ? fifo_cnt : len;
  uint32_t rp_to_end = fifo->_size - fifo->_read_p; //amount of bytes between end-of-buffer and read_p
  uint32_t loc_r_p = fifo->_read_p;

  //wrap-around path
  if(rp_to_end <= act_len)
    {
      memcpy(buf_out, fifo->_buf+loc_r_p, rp_to_end);
      loc_r_p = 0;
      memcpy(buf_out+rp_to_end, fifo->_buf+loc_r_p, act_len-rp_to_end);
      loc_r_p += (act_len-rp_to_end);
    }
  ///no wrap-around path
  else
    {
      memcpy(buf_out, fifo->_buf+loc_r_p, act_len);
      loc_r_p += act_len;
    }

  fifo->_read_p = loc_r_p;

  return act_len;
}


uint32_t FIFO_Peek(FIFO_t* fifo, uint8_t* buf_out, uint32_t len)
{
  //fifo peek
  //reads 'len' amount of bytes from the start of 'fifo' into 'buf_out' without removing them
  //returns number of bytes peeked at

  uint32_t fifo_cnt = FIFO_GetCount(fifo);
  uint32_t act_len = fifo_cnt < len ? fifo_cnt : len;
  uint32_t rp_to_end = fifo->_size - fifo->_read_p; //amount of bytes between end-of-buffer and read_p
  uint32_t temp_read_p = fifo->_read_p;

  //wrap-around path
  if(rp_to_end <= act_len)
    {
      memcpy(buf_out, fifo->_buf+temp_read_p, rp_to_end);
      temp_read_p = 0;
      memcpy(buf_out+rp_to_end, fifo->_buf+temp_read_p, act_len-rp_to_end);
    }
  ///no wrap-around path
  else
    {
      memcpy(buf_out, fifo->_buf+temp_read_p, act_len);
    }

  return act_len;
}


uint32_t FIFO_Discard(FIFO_t* fifo, uint32_t len)
{
  //discards the first 'len' bytes in fifo
  //returns the number of bytes discarded

  uint32_t fifo_cnt = FIFO_GetCount(fifo);
  uint32_t disc_len = fifo_cnt < len ? fifo_cnt : len;
  uint32_t loc_disc_p = fifo->_read_p;

  loc_disc_p += disc_len;
  if(loc_disc_p >= fifo->_size) loc_disc_p -= fifo->_size;
  fifo->_read_p = loc_disc_p;

  return disc_len;
}


uint32_t FIFO_GetEmpty(FIFO_t* fifo)
{
  //returns the number of available bytes in the fifo
  //size - 1 creates a buffer byte between the read and write pointers so they never catch up and read as 0 when they are full
  //it also effectively makes the max size of the fifo 1 less than what is stored in the '_size' variable
  return (fifo->_size - 1) - FIFO_GetCount(fifo);
}


uint32_t FIFO_GetCount(FIFO_t* fifo)
{
  //returns the number of filled bytes in the fifo
  uint32_t fifo_r = fifo->_read_p;
  uint32_t fifo_w = fifo->_write_p;


  if(fifo_r > fifo_w)
    {
      return fifo->_size - (fifo_r - fifo_w);
    }
  else
    {
      return fifo_w - fifo_r;
    }

  return 0;
}


void FIFO_Clear(FIFO_t* fifo)
{
  //resets the fifo variables
  //beware that this does not remove the old data, it only makes it able to be written over

  fifo->_write_p = 0;
  fifo->_read_p = 0;
}
