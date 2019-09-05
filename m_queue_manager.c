#include "m_queue_manager.h"

bool m_loopbuf_push(m_loopbuf_t *str, unsigned short *data, unsigned short n)
{
    unsigned short *pbufdata;
    pbufdata = str->pbufdataadd;
    if(n > str->buflength)
       return 1;
    
    str->newdatacount += n;
    
    if(str->newdatacount >= str->buflength)
    {
        str->readptr += (str->newdatacount - str->buflength);
        if(str->readptr >= str->buflength)
        {
            str->readptr -= str->buflength; 
        }
        str->newdatacount -= (str->newdatacount - str->buflength);
    }
    
    while(n!=0)
    {
        *(pbufdata + str->writeptr) = *data++;
        str->writeptr++;
        n--;
        
        if(str->writeptr >= str->buflength)
        {
            str->writeptr=0;
        }
    }

	return 0;
}

bool m_loopbuf_pop(m_loopbuf_t *str, unsigned short *data, unsigned short n)
{
    volatile unsigned short *pbufdata;
    pbufdata = str->pbufdataadd;
    if(n > str->newdatacount)
    {
	    do
	    {
	        *data++ = 0;
	    }
	    while(n--);
	    return 1;
    }
    str->newdatacount -= n;
    
    while(n!=0)
    {
        *data=*(pbufdata + str->readptr);
        data++;
        n--;
        str->readptr++;
        if(str->readptr >= str->buflength)
        {
            str->readptr=0;
        }
    }
    return 0;
}

unsigned short m_loopbuf_is_empty(m_loopbuf_t *str)
{
    return str->newdatacount;
}

void m_loopbuf_reset(m_loopbuf_t *str)
{
    str->writeptr 		= 0;
    str->readptr 		= 0;
    str->newdatacount 	= 0;
}

static void m_loopbuf_init(m_loopbuf_t *str)
{
    unsigned short i, *pbufdata;
    
    pbufdata 			= str->pbufdataadd;
    str->writeptr 		= 0;
    str->readptr 		= 0;
    str->newdatacount 	= 0;
	
    for(i = 0; i < str->buflength; i ++)
    {
        *pbufdata = 0;
        pbufdata++;
    }
}

void m_loopbuf_setup(m_loopbuf_t *str, unsigned short *buf_add, unsigned short buf_len)
{
    str->pbufdataadd 	= buf_add;
    str->buflength 		= buf_len;
    m_loopbuf_init(str);
}

void m_blockbuf_setup(m_blockbuf_t *str, void *p_blockbuf, unsigned char blocknum, unsigned char blocksize)
{
	memset(str, 0, sizeof(m_blockbuf_t));
    str->blockbufptr 	= (uint8_t *)p_blockbuf;
    str->blocknum 		= blocknum;
    str->blocksize 		= blocksize;
}

bool m_blockbuf_write(unsigned char report_id, m_blockbuf_t *p_loopbuf, unsigned char const *data, unsigned char len)
{
    unsigned char *writeptr = &p_loopbuf->blockbufptr[(p_loopbuf->blocksize + M_QUEUE_BLOCKBUF_HEAD)*p_loopbuf->writeptr];

    //buf is full
    if(p_loopbuf->newblockcount == p_loopbuf->blocknum)
    {
        return 1;
    }
    p_loopbuf->newblockcount++;
    *writeptr++ = report_id;
    *writeptr++ = len;
    memcpy(writeptr, data, ((len > p_loopbuf->blocksize) ? p_loopbuf->blocksize : len));
    
    p_loopbuf->writeptr++;
    p_loopbuf->writeptr %= p_loopbuf->blocknum;
    
    return 0;
}

unsigned char *m_blockbuf_malloc(m_blockbuf_t *p_loopbuf)
{
    unsigned char *p_read_data; 
    if(p_loopbuf->newblockcount)
    {
        p_read_data = &p_loopbuf->blockbufptr[(p_loopbuf->blocksize + M_QUEUE_BLOCKBUF_HEAD) * p_loopbuf->readptr]; 
        return p_read_data;
    }
    
    return NULL;
}

bool m_blockbuf_free(m_blockbuf_t *p_loopbuf)
{

    if(p_loopbuf->newblockcount)
    {
        p_loopbuf->newblockcount --;
        p_loopbuf->readptr ++;
        p_loopbuf->readptr %= p_loopbuf->blocknum;

        return 0;
    }
    else
    {
		return 1;
    }
}

unsigned char m_blockbuf_check_capacity(m_blockbuf_t *p_loopbuf)
{
	return p_loopbuf->newblockcount;
}

void m_blockbuf_flush(m_blockbuf_t *p_loopbuf)
{
	p_loopbuf->writeptr = 0;
    p_loopbuf->readptr = 0;
    p_loopbuf->newblockcount = 0;
}

