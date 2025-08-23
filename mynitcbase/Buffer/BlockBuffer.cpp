#include "BlockBuffer.h"
#include <cstdlib>
#include <cstring>
#include<iostream>

BlockBuffer::BlockBuffer(int blockNum){
    this->blockNum = blockNum;
}

RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum){}

int BlockBuffer::getHeader(struct HeadInfo *head)
{
    unsigned char *buffer;
    // Disk::readBlock(buffer, this->blockNum);
    int ret = loadBlockAndGetBufferPtr(&buffer);
    if(ret != SUCCESS) return ret;

    memcpy(&head->pblock, buffer+4,4);
    memcpy(&head->lblock,buffer+8,4);
    memcpy(&head->rblock,buffer+12,4);
    memcpy(&head->numEntries,buffer+16,4);
    memcpy(&head->numAttrs,buffer+20,4);
    memcpy(&head->numSlots,buffer+24,4);

    return SUCCESS;
}

int RecBuffer::getRecord(union Attribute *rec, int slotNum)
{
    struct HeadInfo head;
    BlockBuffer::getHeader(&head);

    int attrCnt = head.numAttrs;
    int slotCnt = head.numSlots;

    if(slotNum >= slotCnt) return E_OUTOFBOUND;

    unsigned char *buffer;

    // Disk::readBlock(buffer, this->blockNum);
    int ret = loadBlockAndGetBufferPtr(&buffer);
    if(ret != SUCCESS)  return ret;

    int recordSize = attrCnt * ATTR_SIZE;
    unsigned char *slotPointer = buffer + (HEADER_SIZE+slotCnt+(recordSize * slotNum));

    memcpy(rec, slotPointer, recordSize);

    return SUCCESS;
}

int RecBuffer::setRecord(union Attribute *rec, int slotNum)
{
    unsigned char *buffer;
    int ret = loadBlockAndGetBufferPtr(&buffer);

    if(ret != SUCCESS) return ret;

    struct HeadInfo head;
    BlockBuffer::getHeader(&head);

    int attrCnt = head.numAttrs;
    int slotCnt = head.numSlots;

    if(slotNum<0 || slotNum>= slotCnt)
        return E_OUTOFBOUND;
    
    int recordSize = attrCnt * ATTR_SIZE;
    unsigned char *slotPointer = buffer + (HEADER_SIZE+slotCnt+slotNum*recordSize);
    memcpy(slotPointer,rec,recordSize);
    // Disk::writeBlock(buffer, this->blockNum);

    int retn = StaticBuffer::setDirtyBit(this->blockNum);

    if(retn != SUCCESS) 
        std::cout <<"Error in setDirty function"<<std::endl;
        
    return SUCCESS;
}

int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **bufferPtr)
{
    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

    if(bufferNum == E_BLOCKNOTINBUFFER)
    {
        bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);
    
        if(blockNum == E_OUTOFBOUND)
            return blockNum;
        
        Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);    
    }
    else
    {
        for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++)
        {
            if(bufferIndex == bufferNum)
                StaticBuffer::metainfo[bufferIndex].timeStamp = 0;
            else
                StaticBuffer::metainfo[bufferIndex].timeStamp++;
        }
    }

    *bufferPtr = StaticBuffer::blocks[bufferNum];

    return SUCCESS;
}

int RecBuffer::getSlotMap(unsigned char *slotMap)
{
    unsigned char *bufferPtr;
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret != SUCCESS) return ret;

    struct HeadInfo head;
    BlockBuffer::getHeader(&head);

    int slotCount = head.numSlots;
    unsigned char *slotMapInBuffer = bufferPtr+HEADER_SIZE;
    memcpy(slotMap,slotMapInBuffer,slotCount);

    return SUCCESS;
}

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType)
{
    double diff;
    if(attrType == 1)
        diff = strcmp(attr1.sVal, attr2.sVal);
    else    
        diff = attr1.nVal - attr2.nVal;
    
    if (diff > 0) return 1;
    if(diff < 0) return -1;
    if (diff == 0) return 0;
    return 0;
}