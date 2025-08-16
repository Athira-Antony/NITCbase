#include "BlockAccess.h"
#include <iostream>
#include <cstring>

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op)
{
    RecId prevId;
    RelCacheTable::getSearchIndex(relId,&prevId);
    

    int block = -1, slot = -1;
    if(prevId.block == -1 && prevId.slot == -1)
    {
        // (no hits from previous search; search should start from the
        // first record itself)

        // get the first record block of the relation from the relation cache

        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(relId,&relCatBuf);
        // block = first record block of the relation
        // slot = 0
        block = relCatBuf.firstBlk;
        slot = 0;
    }
    else
    {
        // (there is a hit from previous search; search should start from
        // the record next to the search index record)

        // block = search index's block
        // slot = search index's slot + 1
        block = prevId.block;
        slot = prevId.slot+1;
    }


    RelCatEntry relCatBuffer;
	RelCacheTable::getRelCatEntry(relId, &relCatBuffer);

    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
           RecBuffer buffer(block);
           HeadInfo head;
           Attribute catRecord[RELCAT_NO_ATTRS];

        // get the record with id (block, slot) using RecBuffer::getRecord()
        buffer.getRecord(catRecord, slot);
        // get header of the block using RecBuffer::getHeader() function
        buffer.getHeader(&head);
        // get slot map of the block using RecBuffer::getSlotMap() function
        unsigned char * slotMap = (unsigned char*)malloc(sizeof(unsigned char) * head.numSlots);
        buffer.getSlotMap(slotMap);

        if(slot >= relCatBuffer.numSlotsPerBlk)
        {
            if(slot >= head.numSlots)
            {
                block = head.rblock;
                slot = 0;
                continue;
            }
        }

        if(slotMap[slot] == SLOT_UNOCCUPIED)
        {
            slot++;
            continue;
        }

        /*
        firstly get the attribute offset for the attrName attribute
        from the attribute cache entry of the relation using
        AttrCacheTable::getAttrCatEntry()
    */
        AttrCatEntry attrCatBuf;
        AttrCacheTable::getAttrCatEntry(relId,attrName, &attrCatBuf);

        /* use the attribute offset to get the value of the attribute from
       current record */
       Attribute *record = (Attribute*)malloc(sizeof(Attribute) * head.numAttrs);
       buffer.getRecord(record,slot);
       int attrOffset = attrCatBuf.offset;

        int cmpVal = compareAttrs(record[attrOffset], attrVal, attrCatBuf.attrType);  // will store the difference between the attributes
        // set cmpVal using compareAttrs()

       
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
           RecId searchInd;
           searchInd.block = block;
           searchInd.slot = slot;

           RelCacheTable::setSearchIndex(relId,&searchInd);

           return RecId{block, slot};
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
}
