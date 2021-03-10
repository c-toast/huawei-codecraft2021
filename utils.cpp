//
// Created by ctoast on 2021/3/9.
//
#include "utils.h"
#include <cstring>
#include <iostream>

char* readFile(char* filePath,int size){
    FILE* f=fopen(filePath,"w");
    if(f==NULL){
        return NULL;
    }
    char* data=(char*)malloc(sizeof(char)*size+1);
    fread(data,size,1,f);
    data[size]='\0';
    fclose(f);
    return data;
}

int writeFile(char* filePath,char* data){
    FILE* f=fopen(filePath,"w");
    if(f==NULL){
        return -1;
    }
    fwrite(data,1,strlen(data),f);
    fclose(f);
    return 0;
}