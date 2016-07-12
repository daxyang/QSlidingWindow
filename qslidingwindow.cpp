#include "qslidingwindow.h"
#include "math.h"

QSlidingWindow::QSlidingWindow()
{
}
/*******************************************************
 * 初始化功能
 *     输入：
 *        u32 len:开设缓冲区的长度 一般为2^n的空间大小
 *        char *buffer:缓冲区的指针
 *******************************************************
*/
void QSlidingWindow::sliding_init(u32 len, char *buffer)
{
    dataBuffer = new data_buf_t;
    dataBuffer->buffer = buffer;
    dataBuffer->size = len;
    buffer_len = len;
    dataBuffer->write_next_seq = 0;

    wnd_size = len >> 2;     //滑窗大小，不能超过缓冲区的一半；

    SHIFT_BITS = 32 - int(log(len) / log(2));  //因为缓冲区大小不是2^32,所以最高位需要移动第32位进行判定大小，SHIFT_BITS即移位位数
    printf("[SligWnd] all_size:%d,wnd_size:%d,SHIFT:%d %f\n",len,wnd_size,SHIFT_BITS,log(len)/log(2));
    head = (head_buf_t *)chrHead;  //保存数据的头格式
    memcpy(head->flag,__HEAD,HEAD_LEN); //将固定头写入
    consume_linklist_init(); //初始化用户链表
    head->rev1 = 0;
}
/*******************************************************
 * 将数据写入缓冲区
 *    输入:
 *         u32 data_len:数据写入长度
 *         char *data: 写入数据的指针
 ********************************************************
*/
void QSlidingWindow::write_data_to_buffer(u32 data_len, char *data,struct _frame_info_t *frame)
{
    head->pkg_len = data_len;//向头中添加包的长度
    head->frame.frame_type = frame->frame_type; //向头中添加帧类型
    if((dataBuffer->write_next_seq + HEAD_SIZE + data_len) <= dataBuffer->size)
    {
        memcpy(dataBuffer->buffer + dataBuffer->write_next_seq,chrHead,HEAD_SIZE);  //将数据头写入缓冲区
        memcpy(dataBuffer->buffer + dataBuffer->write_next_seq + HEAD_SIZE,data,data_len);//写入数据
        dataBuffer->write_next_seq = SEQ_INCR(dataBuffer->write_next_seq,HEAD_SIZE+data_len,dataBuffer->size);//移动指针
    }
    else if((dataBuffer->write_next_seq + HEAD_SIZE) <= dataBuffer->size)
    {
        memcpy(dataBuffer->buffer+dataBuffer->write_next_seq,chrHead,HEAD_SIZE);
        memcpy(dataBuffer->buffer,data,data_len);
        dataBuffer->write_next_seq = data_len;
    }
    else
    {
        memcpy(dataBuffer->buffer,chrHead,HEAD_SIZE);
        memcpy(dataBuffer->buffer+HEAD_SIZE,data,data_len);
        dataBuffer->write_next_seq = HEAD_SIZE+data_len;
    }
    head->rev1++;

   // printf("write seq:%d,\n",head->rev1);
   // printf("(SW)write seq: %d \n",dataBuffer->write_next_seq);
}
/**************************************************************
 * 以下是创建一个多用户(消费)的管理链表
 * 链表的结构:
 * struct consume_list_t
 * {
 *   int no;
 *   QSlidingWindowConsume *new_consume;
 *   struct consume_list_t *next;
 * };
 * 其链表格式:
 * head_node                   list1                     ....................... listn
 *    .no = count                 .no = n                                           .no = m
 *    .next  =>  list1 / NULL     .next = list2 / NULL                              .next = NULL
 *                                .new_consume = new QSlidingWindowConsume          .new_consume = new QSlidingWindowConsume
 ****************************************************************
*/
/****************************************************************
 * 在未尾追加一个用户
 *     输入:
 *       int no:新增用户的编号
 *     返回:
 *       当前所有用户数； －1:出错
 ****************************************************************
*/
int QSlidingWindow::consume_linklist_append(int no)
{
    consume_list_t *newConsume_node = new consume_list_t;
    if(NULL == newConsume_node)
    {
        printf("append consume failed. no space malloc.\n");
        return -1;
    }
    QSlidingWindowConsume *newConsume;
    newConsume = new QSlidingWindowConsume(dataBuffer,no);

    newConsume_node->no = no;
    newConsume_node->new_consume = newConsume;
    newConsume_node->next = NULL;

    if(head_node == NULL)
    {
        printf("consume link list head error.\n");
        return -1;
    }
    consume_list_t *p1 = head_node;
    while (p1->next != NULL)
    {
        if(p1->no == no)
        {
            printf("consume %d is exist. \n",no);
            return -1;
        }
        p1 = p1->next;
    }

    if(p1->next == NULL)
    {
        p1->next = newConsume_node;
        printf("append consume is %d .\n",p1->next->no);
        head_node->no++;
    }
    return head_node->no;
}
/*********************************************
 * 初始化链表
 *********************************************
*/
void QSlidingWindow::consume_linklist_init()
{
    head_node = new consume_list_t;
    head_node->next = NULL;
    head_node->no = 0;
}
/*************************************************
 * 从链表中册除一个用户
 *    输入:
 *      int no:被删的用户号
 *************************************************
*/
int QSlidingWindow::consume_linklist_delete(int no)
{
    if((head_node == NULL) || (head_node->next == NULL))
    {
        printf(" the consume link list is empty.\n");
        return 0;
    }
    struct consume_list_t *p1,*p2,*p3;
    p1 = head_node;
    p2 = p1;
    p1 = p1->next;
    while((p1->next != NULL) && (p1->no != no))
    {
        p2 = p1;  //保存当前节点
        p1 = p1->next;  //移动下一节点
    }
    if(p1->no == no)
    {
        {
            p3 = p1;
            p1 = p1->next;
            p2->next = p1;
        }
        free(p3->new_consume);
        free(p3);
        p3 = NULL;
        head_node->no--;
    }
    return head_node->no;
}
/*********************************************************
 * 获取用户
 *    输入：
 *      int no:用户号
 *********************************************************
*/
QSlidingWindowConsume *QSlidingWindow::consume_linklist_getConsume(int no)
{
    consume_list_t *p1;
    if((head_node == NULL) || (head_node->next == NULL))
    {
        printf("the consume link list is empty. \n");
        return NULL;
    }
    p1 = head_node->next;
    while((p1->next != NULL) && (p1->no != no))
        p1 = p1->next;
    if(p1->no == no)
        return p1->new_consume;
    else
        return NULL;
}

/**************************************************
 * 判定链表是否为空
 * head_node
 *     head_node->next => p1
 *                        p1->next => p2
 ***************************************************
*/
int QSlidingWindow::consume_linklist_isEmpty()
{
    int count  = 0;
    if((head_node == NULL) || (head_node->next == NULL))
    {
        printf("consume_list is empty. \n");
        return 0;
    }
    consume_list_t *p1;
    p1 = head_node->next;
    while(p1 != NULL)
    {
        count++;
        printf("consume no is: %d .\n",p1->no);
        p1 = p1->next;
    }
    return count;
}
