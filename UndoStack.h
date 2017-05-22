#pragma once

//
// 存放CImage指针的栈
//

#include "afxwin.h"

#define MAX_UNDO_NUMBER 16

typedef CImage* ElementType;

class CUndoStack
{

public:
	CUndoStack();
	~CUndoStack();

	int IsEmpty();											//判断为空
	int IsFull();											//判断为满
	void Push(ElementType X);								//进行进栈操作
	void Pop();												//进行出栈操作
	ElementType Top();										//取出栈顶元素
	ElementType GetTop();									//获取栈顶元素
	void SetTop(ElementType X);								//设置栈顶元素

	int                                   m_nCapacity;       //容量
	int                                m_nSizeOfStack;       //大小
	int                                 m_nTopOfStack;       //栈顶
	int                              m_nBottomOfStack;       //栈底
	ElementType              m_Array[MAX_UNDO_NUMBER];       //元素
};
