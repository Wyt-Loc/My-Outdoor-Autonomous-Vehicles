//#include "myqueue.h"


//Queue *InitQueue() // ��ʼ��ѭ������
//{
//    Queue *Q = (Queue *)malloc(sizeof(Queue));
//    Q->front = Q->rear = 0;
//    return Q;
//}

//int isfun(Queue *Q) // ����
//{
//    if ((Q->rear + 1) % MAXSIZE == Q->front)
//        return 1;
//    else
//        return 0;
//}

//int isEmpty(Queue *Q) // �п�
//{
//    if (Q->front == Q->rear)
//        return 1;
//    else
//        return 0;
//}

//int EnQueue(Queue *Q, int data) // ����
//{
//    if (isfun(Q))
//        return 0;
//    else
//    {
//        Q->data[Q->rear] = data;
//        Q->rear = (Q->rear + 1) % MAXSIZE;
//        return 1;
//    }
//}

//uint8_t deQueue(Queue *Q) // ����
//{
//    if (isEmpty(Q))
//        return -1;
//    else
//    {
//        int data = Q->data[Q->front]; // ͷָ��
//        Q->front = (Q->front + 1) % MAXSIZE;
//        return data;
//    }
//}


//void printQueue(Queue *Q)
//{
//		int i = 0;
//    int len = (Q->rear - Q->front + MAXSIZE) % MAXSIZE;
//    int index = Q->front;
//    for (i = 0; i < len; i++)
//    {
//        printf("%d", Q->data[index]);
//        index = (index + 1) % MAXSIZE;
//    }
//}


