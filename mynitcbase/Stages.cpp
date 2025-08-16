//stage 1

// OpenRelTable cache;
  
  // unsigned char buffer[BLOCK_SIZE];
  // Disk::readBlock(buffer, 7000);
  // char message[] = "hello";
  // memcpy(buffer + 20, message, 6);
  // Disk::writeBlock(buffer, 7000);

  // unsigned char buffer2[BLOCK_SIZE];
  // char message2[6];
  // Disk::readBlock(buffer2, 7000);
  // memcpy(message2, buffer2 + 20, 6);
  // std::cout << message2;

  
  // unsigned char buffer3[BLOCK_SIZE];
  // Disk::readBlock(buffer3,0);
  // for(int i=0; i<6; i++)
  // {
  //   int message3 = buffer3[i];
  //   std::cout << message3 << " ";
  // }
  
  //return FrontendInterface::handleFrontend(argc, argv);


  // stage 2

  // RecBuffer relCatBuffer(RELCAT_BLOCK);
  // RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

  // HeadInfo relCatHeader;

  // relCatBuffer.getHeader(&relCatHeader);
  // int totRel = relCatHeader.numEntries;

  // vector<Attribute *>allAttrRecords;
  // int attrBlockNum = ATTRCAT_BLOCK;

  // while(attrBlockNum != -1)
  // {
  //   RecBuffer attrCatBuffer(attrBlockNum);
  //   HeadInfo attrCatHeader;
  //   attrCatBuffer.getHeader(&attrCatHeader);

  //   for (int j = 0; j < attrCatHeader.numEntries; ++j) 
  //   {
  //     Attribute* attrRec = new Attribute[ATTRCAT_NO_ATTRS];
  //     if (attrCatBuffer.getRecord(attrRec, j) == SUCCESS)
  //     {
  //       const char * relName = attrRec[ATTRCAT_REL_NAME_INDEX].sVal;
  //       const char * attrName = attrRec[ATTRCAT_ATTR_NAME_INDEX].sVal;

  //       if(strcmp(relName, "Students") == 0 && strcmp(attrName,"Class") == 0)
        
  //       {
  //         memset(attrRec[ATTRCAT_ATTR_NAME_INDEX].sVal,0,ATTR_SIZE);
  //         memcpy(attrRec[ATTRCAT_ATTR_NAME_INDEX].sVal,"Batch",6);

  //         attrCatBuffer.setRecord(attrRec,j);
  //       }
  //       allAttrRecords.push_back(attrRec);
  //     } 
        
  //     else 
  //       delete[] attrRec;
  //   }

  //   attrBlockNum = attrCatHeader.rblock;
  // }

  // for (int i = 0; i < totRel; ++i) {
  //   Attribute relCatRecord[RELCAT_NO_ATTRS];
  //   if (relCatBuffer.getRecord(relCatRecord, i) != SUCCESS)
  //     continue;

  //   const char *relName = relCatRecord[RELCAT_REL_NAME_INDEX].sVal;
  //   printf("Relation: %s\n", relName);

  //   for (auto attrRec : allAttrRecords) 
  //   {
  //     const char *recRelName = attrRec[ATTRCAT_REL_NAME_INDEX].sVal;
  //     if (strcmp(recRelName, relName) != 0) continue;

  //     const char *attrName = attrRec[ATTRCAT_ATTR_NAME_INDEX].sVal;
  //     int type = attrRec[ATTRCAT_ATTR_TYPE_INDEX].nVal;
  //     const char *attrType = (type == NUMBER ? "NUM" : "STR");

  //     printf("  %s : %s\n", attrName, attrType);
  //   }
  
  // }
  // printf("\n");

  //stage 3
//   RecBuffer relCatBuffer(RELCAT_BLOCK);
//   RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

//   HeadInfo relCatHeader;
//   HeadInfo attrCatHeader;

//   relCatBuffer.getHeader(&relCatHeader);
//   attrCatBuffer.getHeader(&attrCatHeader);

//   int totRel = relCatHeader.numEntries;
//   int totAttr = attrCatHeader.numEntries;

//   for(int i=0; i<totRel; i++)
//   {
//     Attribute relCatRecord[RELCAT_NO_ATTRS];
//     if(relCatBuffer.getRecord(relCatRecord, i) != SUCCESS)
//       continue;

//     const char *relName = relCatRecord[RELCAT_REL_NAME_INDEX].sVal;
//     printf("Relation: %s\n", relName);

//     for(int j=0; j<totAttr; j++)
//     {
//       Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
//       if(attrCatBuffer.getRecord(attrCatRecord, j) != SUCCESS)
//         continue;
      
//       const char * recRelName = attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal;
//       if(strcmp(recRelName, relName) != 0) continue;

//       const char *attrName = attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal;
//       int type = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal;
//       const char *attrType = (type == NUMBER ? "NUM" : "STR");

//       printf(" %s : %s\n",attrName,attrType);

//     }
    
//   }
//   printf("\n");


//exercise stage3
// RecBuffer relCatBuffer(RELCAT_BLOCK);
//   RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

//   HeadInfo relCatHeader;

//   relCatBuffer.getHeader(&relCatHeader);
//   int totRel = relCatHeader.numEntries;

//   vector<Attribute *>allAttrRecords;
//   int attrBlockNum = ATTRCAT_BLOCK;

//   while(attrBlockNum != -1)
//   {
//     RecBuffer attrCatBuffer(attrBlockNum);
//     HeadInfo attrCatHeader;
//     attrCatBuffer.getHeader(&attrCatHeader);

//     for (int j = 0; j < attrCatHeader.numSlots; ++j) 
//     {
//       Attribute* attrRec = new Attribute[ATTRCAT_NO_ATTRS];
//       if (attrCatBuffer.getRecord(attrRec, j) == SUCCESS) 
//         allAttrRecords.push_back(attrRec);
//       else 
//         delete[] attrRec;
//     }

//     attrBlockNum = attrCatHeader.rblock;
//   }

//   for (int i = 0; i < totRel; ++i) {
//     Attribute relCatRecord[RELCAT_NO_ATTRS];
//     if (relCatBuffer.getRecord(relCatRecord, i) != SUCCESS)
//       continue;

//     const char *relName = relCatRecord[RELCAT_REL_NAME_INDEX].sVal;
//     printf("Relation: %s\n", relName);

//     for (auto attrRec : allAttrRecords) 
//     {
//       const char *recRelName = attrRec[ATTRCAT_REL_NAME_INDEX].sVal;
//       if (strcmp(recRelName, relName) != 0) continue;

//       const char *attrName = attrRec[ATTRCAT_ATTR_NAME_INDEX].sVal;
//       int type = attrRec[ATTRCAT_ATTR_TYPE_INDEX].nVal;
//       const char *attrType = (type == NUMBER ? "NUM" : "STR");

//       printf("  %s : %s\n", attrName, attrType);
//     }
  
//   }
//   printf("\n");
