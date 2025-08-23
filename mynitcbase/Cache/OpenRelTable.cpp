#include "OpenRelTable.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable()
{
    for(int i=0; i<MAX_OPEN; i++)
    {
        RelCacheTable::relCache[i] = nullptr;
        AttrCacheTable::attrCache[i] = nullptr;
    }

    // relation catalog block to relation cache
    RecBuffer relCatBlock(RELCAT_BLOCK);
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);
    

    struct RelCacheEntry relCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
    relCacheEntry.recId.block = RELCAT_BLOCK;
    relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

    RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*) malloc (sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

    //attribute catalog block to relation cache
    relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);

    struct RelCacheEntry attrCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord, &attrCacheEntry.relCatEntry);
    attrCacheEntry.recId.block = RELCAT_BLOCK;
    attrCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

    RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry *) malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrCacheEntry;

    //relation catalog to attribute cache
    RecBuffer attrCatBlock(ATTRCAT_BLOCK);
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

    AttrCacheEntry *head = nullptr;
    AttrCacheEntry *prev = nullptr;

    for(int i=0; i<RELCAT_NO_ATTRS; i++)
    {
        attrCatBlock.getRecord(attrCatRecord,i);
        AttrCacheEntry * curr = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &curr->attrCatEntry);
        curr->recId.block = ATTRCAT_BLOCK;  //CONFUSION
        curr->recId.slot = i;
        curr->next = nullptr;

        if(prev)
            prev->next = curr;
        else
            head = curr;
       
         prev = curr;
    }
    AttrCacheTable::attrCache[RELCAT_RELID] = head;

    //attribute catalog to attribute cache
    AttrCacheEntry *head2 = nullptr;
    AttrCacheEntry *prev2 = nullptr;

    for (int i = 0; i < ATTRCAT_NO_ATTRS; i++) 
    {
        int slotNum = i + RELCAT_NO_ATTRS;
        attrCatBlock.getRecord(attrCatRecord, slotNum);
        AttrCacheEntry* curr = (AttrCacheEntry*) malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &curr->attrCatEntry);
        curr->recId.block = ATTRCAT_BLOCK;
        curr->recId.slot = slotNum;
        curr->next = nullptr;

        if (prev2) 
            prev2->next = curr;
        else 
            head2 = curr;
        if(curr)
        prev2 = curr;
    }
    AttrCacheTable::attrCache[ATTRCAT_RELID] = head2;

    for(int i=0; i<MAX_OPEN; i++)
        tableMetaInfo[i].free = true;

    OpenRelTable::tableMetaInfo[RELCAT_RELID].free = false;
    OpenRelTable::tableMetaInfo[ATTRCAT_RELID].free = false;
    strcpy(OpenRelTable::tableMetaInfo[RELCAT_RELID].relName,"RELATIONCAT");
    strcpy(OpenRelTable::tableMetaInfo[ATTRCAT_RELID].relName,"ATTRIBUTECAT");
    
}

OpenRelTable::~OpenRelTable() 
{
    for(int i=2; i<MAX_OPEN; i++)
    {
        if(!tableMetaInfo[i].free)
            OpenRelTable::closeRel(i);
    }

}

int OpenRelTable::getRelId(char relName[ATTR_SIZE]) 
{

    for(int i=0; i<MAX_OPEN; i++)
    {
        if(strcmp(relName, tableMetaInfo[i].relName) == 0 && tableMetaInfo[i].free == false)
            return i;
    }
    return E_RELNOTOPEN;
}

int OpenRelTable::getFreeOpenRelTableEntry() 
{

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/
    int i=0;
    for(i; i<MAX_OPEN; i++)
    {
        if(tableMetaInfo[i].free)
            return i;
    }
    return E_CACHEFULL;
}

int OpenRelTable::openRel(char relName[ATTR_SIZE]) 
{
    int existRelId = OpenRelTable::getRelId(relName);
    if (existRelId >= 0) {
        return existRelId;
    }

    int slotAvailable = OpenRelTable::getFreeOpenRelTableEntry();
    if (slotAvailable == E_CACHEFULL) {
        return E_CACHEFULL;
    }

    int relId = slotAvailable;
    RelCacheTable::relCache[relId] = (RelCacheEntry *) malloc(sizeof(RelCacheEntry));
    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    union Attribute relNameAttribute;
    strcpy(relNameAttribute.sVal, relName);
    char relCatAttrRelName[ATTR_SIZE];
    strcpy(relCatAttrRelName, RELCAT_ATTR_RELNAME);

    RecId relCatRecId = BlockAccess::linearSearch(RELCAT_RELID, relCatAttrRelName, relNameAttribute, EQ);
    if (relCatRecId.block == -1 || relCatRecId.slot == -1) {
        return E_RELNOTEXIST;
    }

    // add the relation catalog entry for the opening table into the relCache as a relCacheEntry
    struct RelCacheEntry relCacheEntry;
    RecBuffer relCatBlock(RELCAT_BLOCK);
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    relCatBlock.getRecord(relCatRecord, relCatRecId.slot);
    RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);

    RelCacheTable::relCache[relId] = (struct RelCacheEntry*) malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[relId]) = relCacheEntry;

    // add the attributes of the corresponding relation into the attrCache as attrCacheEntries
    AttrCacheEntry *head= nullptr;
    int numOfAttrs = relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
    
    AttrCacheEntry *prev = nullptr;

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    for (int i = 0; i < numOfAttrs; i++) {
        RecId attrCatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, relCatAttrRelName, relNameAttribute, EQ);
        if (attrCatRecId.block == -1 || attrCatRecId.slot == -1) {
            return E_ATTRNOTEXIST;
        }

        struct AttrCacheEntry *curr = (AttrCacheEntry *) malloc(sizeof(AttrCacheEntry));
        RecBuffer attrCatBlock(attrCatRecId.block);
        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        attrCatBlock.getRecord(attrCatRecord, attrCatRecId.slot);
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &curr->attrCatEntry);
        curr->next = nullptr;
        curr->recId.block = attrCatRecId.block;
        curr->recId.slot = attrCatRecId.slot;
        
        if (prev) prev->next = curr;
        else head= curr;

        prev = curr;
    }

    AttrCacheTable::attrCache[relId] = head;
    OpenRelTable::tableMetaInfo[relId].free = false;
    strcpy(OpenRelTable::tableMetaInfo[relId].relName, relName);


    if (AttrCacheTable::attrCache[relId] == nullptr) {
        return E_ATTRNOTEXIST;
    }

    return relId;
}

int OpenRelTable::closeRel(int relId) {
    if (relId == RELCAT_RELID || relId == ATTRCAT_RELID) {
        return E_NOTPERMITTED;
    }

    if (relId < 0 || relId >= MAX_OPEN) {
        return E_OUTOFBOUND;
    }

    if (AttrCacheTable::attrCache[relId] == nullptr) {
        return E_RELNOTOPEN;
    }

    free(RelCacheTable::relCache[relId]);
    RelCacheTable::relCache[relId] = nullptr;

    AttrCacheEntry *entry = AttrCacheTable::attrCache[relId];
    while (entry) {
        AttrCacheEntry *next = entry->next;
        free(entry);
        entry = next;
    }
    AttrCacheTable::attrCache[relId] = nullptr;

    OpenRelTable::tableMetaInfo[relId].free = true;
    return SUCCESS;
}
