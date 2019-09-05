#ifndef _M_QUEUE_MANAGER_H_
#define _M_QUEUE_MANAGER_H_

#include "gen_config.h"

#define M_QUEUE_BLOCKBUF_HEAD	2	/* one byte for report id, ont byte for the length of buffer */

typedef struct 
{
    unsigned short	buflength;
    unsigned short 	*pbufdataadd;
    unsigned short 	writeptr;
    unsigned short	readptr;
    unsigned short 	newdatacount;
} m_loopbuf_t;

typedef struct 
{
    unsigned char 			*blockbufptr;
    unsigned char 			writeptr;
    unsigned char 			readptr;
    volatile unsigned char 	newblockcount;
    unsigned char 			blocknum;
    unsigned char 			blocksize;
}m_blockbuf_t;


bool m_loopbuf_push(m_loopbuf_t *str, unsigned short *data, unsigned short n);

bool m_loopbuf_pop(m_loopbuf_t *str, unsigned short *data, unsigned short n);

unsigned short m_loopbuf_is_empty(m_loopbuf_t *str);

void m_loopbuf_reset(m_loopbuf_t *str);

void m_loopbuf_setup(m_loopbuf_t *str, unsigned short *buf_add, unsigned short buf_len);

void m_blockbuf_setup(m_blockbuf_t *str, void *p_blockbuf, unsigned char blocknum, unsigned char blocksize);

bool m_blockbuf_write(unsigned char report_id, m_blockbuf_t *p_loopbuf, unsigned char const *data, unsigned char len);

unsigned char *m_blockbuf_malloc(m_blockbuf_t *p_loopbuf);

bool m_blockbuf_free(m_blockbuf_t *p_loopbuf);

unsigned char m_blockbuf_check_capacity(m_blockbuf_t *p_loopbuf);

void m_blockbuf_flush(m_blockbuf_t *p_loopbuf);




#endif /* END */

