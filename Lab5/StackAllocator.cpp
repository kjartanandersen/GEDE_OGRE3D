#include "pch.h"
#include "StackAllocator.h"

void StackAllocator::init(size_t total_size)
{
	

	m_StartPtr = malloc(total_size);
	m_TotalSize = total_size;
	m_Offset = 0;

}

void* StackAllocator::allocate(size_t size)
{
	m_Offset++;
	return (void*)((int)m_StartPtr + m_Offset);
}

void StackAllocator::reset()
{
	m_Offset = 0;
}

StackAllocator::~StackAllocator()
{
	free(m_StartPtr);
}