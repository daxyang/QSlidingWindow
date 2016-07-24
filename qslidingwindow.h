#ifndef QSLIDINGWINDOW_H
#define QSLIDINGWINDOW_H

#include "qslidingwindow_global.h"
#include "qslidingwindowconsume.h"

#define DEBUG 1
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#define	SEQ_LT(a, b, c)		((int32_t) (((a) - (b)) << c) < 0)
#define SEQ_LEQ(a, b, c)	((int32_t) (((a) - (b)) << c) <= 0)
#define SEQ_GT(a, b, c)		((int32_t) (((a) - (b)) << c) > 0)
#define SEQ_GEQ(a, b, c)	((int32_t) (((a) - (b)) << c) >= 0)
#define SEQ_INCR(a, b, c)	(((a) + (b)) % (c))

typedef unsigned char  	    u8;	/**< UNSIGNED 8-bit data type */
typedef unsigned short 	    u16;/**< UNSIGNED 16-bit data type */
typedef unsigned int   	   	u32;/**< UNSIGNED 32-bit data type */
typedef unsigned long long 	u64;/**< UNSIGNED 64-bit data type */
typedef signed char         s8;	/**< SIGNED 8-bit data type */
typedef signed short       	s16;/**< SIGNED 16-bit data type */
typedef signed int         	s32;/**< SIGNED 32-bit data type */
typedef signed long long	s64;/**< SIGNED 64-bit data type */
typedef u8                  uint8_t;
typedef u32                 uint32_t;


struct consume_list_t
{
    int no;
    QSlidingWindowConsume *new_consume;
    struct consume_list_t *next;
};


//class QSLIDINGWINDOWSHARED_EXPORT QSlidingWindow
class QSlidingWindow
{

public:
    QSlidingWindow();
    void sliding_init(u32 len,char *buffer);
    void write_data_to_buffer(u32 data_len,char *data,struct _frame_info_t *frame);
    int consume_linklist_append(int no);
    int consume_linklist_delete(int no);
    int consume_linklist_isEmpty();
    QSlidingWindowConsume *consume_linklist_getConsume(int no);
    u32 buffer_len;


private:
    void consume_linklist_init();
    data_buf_t *dataBuffer;
    u32 wnd_size;
    int SHIFT_BITS;
    head_buf_t *head;
    char chrHead[sizeof(head_buf_t)];
    struct consume_list_t *head_node;


};

#endif // QSLIDINGWINDOW_H
