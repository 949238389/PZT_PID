#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "SCI_queue.h"

QUEUE UARTa_queue = {0,0,0}; //���崮��1����
QUEUE UARTb_queue = {0,0,0}; //���崮��1����
SERIALCMD UARTa_cmd = {0,0,0,0};//���崮��1��ָ��ṹ��
SERIALCMD UARTb_cmd = {0,0,0,0};//���崮��1��ָ��ṹ��

/********************************
*��    �ƣ� Uart��Queue_Init
*��    �ܣ� ���ڶ��г�ʼ��
*��ڲ����� ���ڶ���
*���ڲ����� ��
*********************************/
void Uart_Queue_Init(QUEUE *UARTX_queue)
{
   UARTX_queue->_head = UARTX_queue->_tail = 0;//��ն���
}

/********************************
*��    �ƣ� Uart��Queue��pushAchar
*��    �ܣ� ���봮�ڶ���һ������
*��ڲ����� ���ڶ���,����
*���ڲ����� ��
*********************************/
void Uart_Queue_pushAchar(QUEUE *UARTX_queue,unsigned char data)
{
   UARTX_queue->_tail = (UARTX_queue->_tail+1)%QUEUE_MAX_SIZE;//��UARTX��queue.tail�����ڶ��еĳ��ȷ�Χ��
   if(UARTX_queue->_tail!=UARTX_queue->_head)//����״̬
      UARTX_queue->_data[(UARTX_queue->_tail)%QUEUE_MAX_SIZE] = data;
}

/********************************
*��    �ƣ� Uart��Queue��popAchar
*��    �ܣ� ȡ�����ڶ���һ������
*��ڲ����� ���ڶ���
*���ڲ����� ��
*********************************/
void Uart_Queue_popAchar(QUEUE *UARTX_queue,unsigned char *data)
{

   if(UARTX_queue->_tail!=UARTX_queue->_head)//�ǿ�״̬
   {
    UARTX_queue->_head = (UARTX_queue->_head+1)%QUEUE_MAX_SIZE;
    *data = UARTX_queue->_data[UARTX_queue->_head];

    }
 }

/********************************
*��    �ƣ� Uart��Queue��size
*��    �ܣ� ������ڶ��еĳ���
*��ڲ����� ���ڶ���
*���ڲ����� ���г���
*********************************/
unsigned char Uart_Queue_size(QUEUE *UARTX_queue)
{
   return ((UARTX_queue->_tail+QUEUE_MAX_SIZE-UARTX_queue->_head)%QUEUE_MAX_SIZE);
}


/********************************
*��    �ƣ� Uart��Queue��getAcmd
*��    �ܣ� ��ȡһ֡������ָ��
*��ڲ����� ����ָ�����飬���ڶ���
*���ڲ����� 0�ɹ�1ʧ��
*********************************/

unsigned char Uart_Queue_getAcmd(SERIALCMD *UARTX_cmd,QUEUE *UARTX_queue)
{
    unsigned char cmd_data;

  while(Uart_Queue_size(UARTX_queue)>0)
    {
        //ȡһ������
        Uart_Queue_popAchar(UARTX_queue,&cmd_data);

        switch(UARTX_cmd->cmd_status)
				{
					case WAIT_FRAME1:                  //�ȴ�֡��1  0x5A
						 if(cmd_data==0x5A)
						 {
						    UARTX_cmd->cmd_status = WAIT_DATA;
						 }

					break;

				    case WAIT_DATA:                  //�ҵ���֡�ף�������������
						  if(UARTX_cmd->cmd_buffer_pointer<6)
							{
                	UARTX_cmd->cmd_buffer_R[UARTX_cmd->cmd_buffer_pointer]=cmd_data;
               		UARTX_cmd->cmd_buffer_pointer++;
							}
						  if(UARTX_cmd->cmd_buffer_pointer==6)
						  {
							  UARTX_cmd->cmd_status = WAIT_OVER;

						  }
				    break;

				    case WAIT_OVER:

							  if(cmd_data==0x23)
							  {
								  UARTX_cmd->cmd_buffer_pointer=0;
								  UARTX_cmd->cmd_status=0;
								  return 1;   //�õ���һ��������֡
							  }
							  else
							  {
								 UARTX_cmd->cmd_status = WAIT_FRAME1;
								 UARTX_cmd->cmd_buffer_pointer=0;
							  }

					break;
				}
     }
		 return 0;//û�еõ�������֡
}
