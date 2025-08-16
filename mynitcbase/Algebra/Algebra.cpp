#include "Algebra.h"
#include <iostream>
#include <cstring>

using namespace std;


//  will return if a string can be parsed as a floating point number
bool isNumber(char *str) 
{
  int len;
  float ignore;
  /*
    sscanf returns the number of elements read, so if there is no float matching
    the first %f, ret will be 0, else it'll be 1

    %n gets the number of characters read. this scanf sequence will read the
    first float ignoring all the whitespace before and after. and the number of
    characters read that far will be stored in len. if len == strlen(str), then
    the string only contains a float with/without whitespace. else, there's other
    characters.
  */
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}

int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) 
{
  int srcRelId = OpenRelTable::getRelId(srcRel);

  if (srcRelId == E_RELNOTOPEN) return srcRelId;

  AttrCatEntry attrCatEntry;

  if(AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry) == E_ATTRNOTEXIST) 
    return E_ATTRNOTEXIST;

  int type = attrCatEntry.attrType;
  Attribute attrVal;
  
  if (type == NUMBER) 
  {
    if (isNumber(strVal))  
      attrVal.nVal = atof(strVal);     
    else 
      return E_ATTRTYPEMISMATCH;      
  } 

  else if (type == STRING) 
    strcpy(attrVal.sVal, strVal);

  /*** Selecting records from the source relation ***/

  // Before calling the search function, reset the search to start from the first hit
  RelCacheTable::resetSearchIndex(srcRelId);

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

  /************************
  The following code prints the contents of a relation directly to the output
  console. Direct console output is not permitted by the actual the NITCbase
  specification and the output can only be inserted into a new relation. We will
  be modifying it in the later stages to match the specification.
  ************************/

  printf("|");
  for (int i = 0; i < relCatEntry.numAttrs; ++i) 
  {
    AttrCatEntry attrCatEntry;
    // get attrCatEntry at offset i using AttrCacheTable::getAttrCatEntry()
    AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrCatEntry);
    printf(" %s |", attrCatEntry.attrName);
  }
  printf("\n");

  while (true) {
    RecId searchRes = BlockAccess::linearSearch(srcRelId, attr, attrVal, op);

    if (searchRes.block != -1 && searchRes.slot != -1) {

      // get the record at searchRes using BlockBuffer.getRecord
      // print the attribute values in the same format as above
      AttrCatEntry attrCatEntry;
      RecBuffer BlockBuffer(searchRes.block);
      HeadInfo blockHeader;
      BlockBuffer.getHeader(&blockHeader);
    
      Attribute recordBuffer[blockHeader.numAttrs];
      BlockBuffer.getRecord(recordBuffer, searchRes.slot);

      for(int i=0; i<relCatEntry.numAttrs; i++)
      {
        AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);
        if(attrCatEntry.attrType == NUMBER)
            printf(" %d |",(int)recordBuffer[i].nVal);
        else
            printf(" %s |", recordBuffer[i].sVal);
      }
      printf("\n");
    }
     else 
      break;
  }

  return SUCCESS;
}



