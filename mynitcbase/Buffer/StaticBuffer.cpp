#include "StaticBuffer.h"
#include <iostream>

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

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

StaticBuffer::~StaticBuffer(){}

int StaticBuffer::getFreeBuffer(int blockNum)
{
    if(blockNum < 0 || blockNum > DISK_BLOCKS)
        return E_OUTOFBOUND;
    int allocatedBuffer;

    for(allocatedBuffer=0; allocatedBuffer<BUFFER_CAPACITY;allocatedBuffer++)
    {
        if(metainfo[allocatedBuffer].free) break;
    }

    // if(allocatedBuffer == BUFFER_CAPACITY) // no free buffer so do LRU
    // {
    //     int lastTimestamp = -1, bufferNum = -1;
    //     for(int bufferIndex = 0; bufferIndex<BUFFER_CAPACITY; bufferIndex++)
    //     {
    //         if(metainfo[bufferIndex].timeStamp > lastTimestamp)
    //         {
    //             lastTimestamp = metainfo[bufferIndex].timeStamp;
    //             bufferNum = bufferIndex;
    //         }
    //     }
    // }

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