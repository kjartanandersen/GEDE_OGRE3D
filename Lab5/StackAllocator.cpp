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

	void* retVal = (void*)((int)m_StartPtr + (m_Offset));
	m_Offset += size;

	return retVal;
}

void StackAllocator::reset()
{
	m_Offset = 0;
}

StackAllocator::~StackAllocator()
{
	
	free(m_StartPtr);
	
}