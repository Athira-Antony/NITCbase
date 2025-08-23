#include "StaticBuffer.h"
#include <iostream>

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

StaticBuffer::StaticBuffer()
{
    for (int bufferIndex = 0; bufferIndex<BUFFER_CAPACITY; bufferIndex++)
    {
        metainfo[bufferIndex].free = true;
        metainfo[bufferIndex].dirty = false;
        metainfo[bufferIndex].blockNum = -1;
        metainfo[bufferIndex].timeStamp = -1;
    }
}

StaticBuffer::~StaticBuffer() 
{
  /*iterate through all the buffer blocks,
    write back blocks with metainfo as free=false,dirty=true
    using Disk::writeBlock()
    */
   for(int bufferIndex = 0; bufferIndex<BUFFER_CAPACITY; bufferIndex++)
   {
        if(metainfo[bufferIndex].free ==false && metainfo[bufferIndex].dirty)
            Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
   }
}

int StaticBuffer::getFreeBuffer(int blockNum)
{
    if(blockNum < 0 || blockNum > DISK_BLOCKS)
        return E_OUTOFBOUND;
    
    int allocatedBuffer = -1;
    int timeStamp = 0, maxInd = 0;

    for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++)
    {
      if(metainfo[bufferIndex].timeStamp > timeStamp)
      {
        timeStamp = metainfo[bufferIndex].timeStamp;
        maxInd = bufferIndex;
      }
      if(metainfo[bufferIndex].free)
      {
        allocatedBuffer = bufferIndex;
        break;
      }
    }

    if(allocatedBuffer == -1)
    {
      if(metainfo[maxInd].dirty)
        Disk::writeBlock(blocks[maxInd], metainfo[maxInd].blockNum);
      allocatedBuffer = maxInd;
    }

    metainfo[allocatedBuffer].free = false;
    metainfo[allocatedBuffer].dirty = false;
    metainfo[allocatedBuffer].blockNum = blockNum;
    metainfo[allocatedBuffer].timeStamp = 0;

    return allocatedBuffer;

}

int StaticBuffer::getBufferNum(int blockNum) 
{
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
  if(blockNum < 0 || blockNum >= DISK_BLOCKS)
    return E_OUTOFBOUND;

  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
  for(int bufferIndex =0; bufferIndex<BUFFER_CAPACITY; bufferIndex++)
  {
    if(metainfo[bufferIndex].free == false && metainfo[bufferIndex].blockNum == blockNum)
        return bufferIndex;
  }

  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}

int StaticBuffer::setDirtyBit(int blockNum)
{
  int bufferNum = StaticBuffer::getBufferNum(blockNum);

  if(bufferNum == E_BLOCKNOTINBUFFER) return E_BLOCKNOTINBUFFER;

  if(blockNum < 0 || blockNum >= BUFFER_CAPACITY) return E_OUTOFBOUND;

  metainfo[bufferNum].dirty = true;
  return SUCCESS;
}