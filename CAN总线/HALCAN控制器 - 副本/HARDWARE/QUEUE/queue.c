#include "stdio.h"
#include "queue.h"

// ˳��ѭ������
// ����1������һ����Ԫ�����ֶӿպͶ���
// ��βָ��ָ�� ��βԪ�صĺ�һ��λ�ã���һ��Ӧ�ò����λ�ã�



// ��ʼ������
void InitQueue(SeqQueue& q)
{
	q.front = q.rear = 0;
}

// �ж϶����Ƿ�Ϊ��
bool QueueEmpty(SeqQueue& q)
{
	if (q.rear == q.front)	//�ӿ�����
		return true;
	else
		return false;
}

// ���
bool EnQueue(SeqQueue& q, ElemType x)
{
	if ((q.rear+1) % MaxSize == q.front)
		return false;		//�������򱨴�

	q.data[q.rear] = x;		//��x�����β
	q.rear = (q.rear + 1) % MaxSize;    //��βָ�����
	return true;
}

// ����
bool DeQueue(SeqQueue& q, ElemType& x)
{
	if (q.rear == q.front)
		return false;	//�ӿ��򱨴�

	x = q.data[q.front];
	q.front = (q.front + 1) % MaxSize; //��ͷָ�����
	return true;
}

// ��ȡ��ͷԪ��
bool GetHead(SeqQueue& q, ElemType& x)
{
	if (q.rear == q.front)
		return false;	//�ӿ��򱨴�
	
	x = q.data[q.front];
	return true;
}

// ������Ԫ�صĸ���
int QueueNum(SeqQueue& q)
{
	return (q.rear - q.front + MaxSize) % MaxSize;
}
