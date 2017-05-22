#include "stdafx.h"
#include "UndoStack.h"

// 栈初始化
CUndoStack::CUndoStack()
{
	m_nCapacity = MAX_UNDO_NUMBER;
	for (int i = 0; i<m_nCapacity; i++)
	{
		m_Array[i] = NULL;
	}
	m_nSizeOfStack = 0;
	m_nTopOfStack = 0;
	m_nBottomOfStack = 0;
}

// 将栈摧毁
CUndoStack::~CUndoStack()
{
	while (m_nSizeOfStack > 0) Pop();
	m_nSizeOfStack = 0;
	m_nTopOfStack = 0;
	m_nBottomOfStack = 0;
}

// 判断为空
int CUndoStack::IsEmpty()
{
	return m_nSizeOfStack == 0;
}

// 判断为满
int CUndoStack::IsFull()
{
	return m_nSizeOfStack == m_nCapacity;
}

// 进栈操作，总是可以进行
void CUndoStack::Push(ElementType X)
{
	m_nTopOfStack++;
	m_nSizeOfStack++;
	// 堆栈未满
	if (m_nSizeOfStack <= m_nCapacity)
	{
		if (m_nTopOfStack == m_nCapacity)
			m_nTopOfStack = 0;
		m_Array[m_nTopOfStack] = X;
	}
	else
	{
		m_nSizeOfStack = m_nCapacity;
		if (m_nTopOfStack == m_nCapacity)
			m_nTopOfStack = 0;
		ElementType TempCell = m_Array[m_nBottomOfStack];
		m_nBottomOfStack++;
		if (m_nBottomOfStack == m_nCapacity)
			m_nBottomOfStack = 0;
		m_Array[m_nTopOfStack] = X;
		SAFE_DELETE(TempCell);
	}
}

// 出栈操作，栈总是在减小
void CUndoStack::Pop()
{
	if (IsEmpty())
		return;
	SAFE_DELETE(m_Array[m_nTopOfStack]);
	m_nTopOfStack--;
	if (m_nTopOfStack == -1)
		m_nTopOfStack = m_nCapacity-1;
	m_nSizeOfStack--;
}

// 取出栈顶元素, 栈减小
ElementType CUndoStack::Top()
{
	if(!IsEmpty())
	{
		ElementType top =  m_Array[m_nTopOfStack];
		m_nTopOfStack--;
		if (m_nTopOfStack == -1)
			m_nTopOfStack = m_nCapacity-1;
		m_nSizeOfStack--;
		return top;
	}
	return NULL;
}

// 返回栈顶元素指针
ElementType CUndoStack::GetTop()
{
	return m_Array[m_nTopOfStack];
}

// 设置栈顶元素
void CUndoStack::SetTop(ElementType X)
{
	m_Array[m_nTopOfStack] = X;
}