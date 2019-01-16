#define NDKLOG

#include <elf.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "android_log.h"
#include "com_andr0day_soencrypt_MainActivity.h"

__attribute__((section (".encrypt"))) jstring getString(JNIEnv* env) {
    static const char* txt __attribute__((section (".encrypt2"))) = "Str from native";
    return (*env)->NewStringUTF(env, txt);
};

void init_1() __attribute__((constructor (3)));

void init_getString() __attribute__((constructor (2)));

void init_2() __attribute__((constructor (1)));


unsigned long getLibAddr();

void init_getString(){
    char name[15];
  unsigned int nblock;
  unsigned int nsize;
  unsigned long base;
  unsigned long text_addr;
  unsigned int i;
  Elf32_Ehdr *ehdr;
  Elf32_Shdr *shdr;

  //获取so的起始地址
  base = getLibAddr();
  //获取指定section的偏移值和size
  ehdr = (Elf32_Ehdr *)base;
  text_addr = ehdr->e_shoff + base;

  nblock = ehdr->e_entry >> 16;
  nsize = ehdr->e_entry & 0xffff;

  __android_log_print(ANDROID_LOG_INFO, "JNITag", "nblock =  0x%x,nsize:%d", nblock,nsize);
  __android_log_print(ANDROID_LOG_INFO, "JNITag", "base =  0x%x", text_addr);
  printf("nblock = %d\n", nblock);

  //修改内存的操作权限
  if(mprotect((void *) (text_addr / PAGE_SIZE * PAGE_SIZE), 4096 * nsize, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
    puts("mem privilege change failed");
    __android_log_print(ANDROID_LOG_INFO, "JNITag", "mem privilege change failed");
  }
  //解密
  for(i=0;i< nblock; i++){
    char *addr = (char*)(text_addr + i);
    *addr = ~(*addr);
    __android_log_print(ANDROID_LOG_INFO, "JNITag", "decoding!!!!!!!");
  }

  if(mprotect((void *) (text_addr / PAGE_SIZE * PAGE_SIZE), 4096 * nsize, PROT_READ | PROT_EXEC) != 0){
    puts("mem privilege change failed");
     __android_log_print(ANDROID_LOG_INFO, "JNITag", "decoding!!!!!!!failed");
  }
   __android_log_print(ANDROID_LOG_INFO, "JNITag", "Decrypt success");
  puts("Decrypt success");






    /*char name[15];
    unsigned int nblock;
    unsigned int nsize;
    unsigned long base;
    unsigned long text_addr;
    unsigned int i;
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdr;

    base = getLibAddr();

    LOGE("base:%p", base);

    ehdr = (Elf32_Ehdr *)base;
    text_addr = ehdr->e_shoff + base;

    nblock = ehdr->e_entry >> 16;
    nsize = ehdr->e_entry & 0xffff;

    LOGE("nblock = %d", nblock);

    if(mprotect((void *) base, 4096 * nsize, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
        LOGE("mem privilege change failed");
    }

    for(i=0;i< nblock; i++){
        char *addr = (char*)(text_addr + i);
        *addr = ~(*addr);
    }

    if(mprotect((void *) base, 4096 * nsize, PROT_READ | PROT_EXEC) != 0){
        LOGE("mem privilege change failed");
    }
    LOGE("Decrypt success");*/
}

void init_1(){
    LOGE("init_1");
}

void init_2(){
    LOGE("init_2");
}


unsigned long getLibAddr(){
    unsigned long ret = 0;
    char name[] = "libencrypt.so";
    char buf[4096], *temp;
    int pid;
    FILE *fp;
    pid = getpid();
    sprintf(buf, "/proc/%d/maps", pid);
    fp = fopen(buf, "r");
    if(fp == NULL)
    {
        LOGE("open failed");
        goto _error;
    }
    while(fgets(buf, sizeof(buf), fp)){
        if(strstr(buf, name)){
            temp = strtok(buf, "-");
            ret = strtoul(temp, NULL, 16);
            break;
        }
    }
    _error:
    fclose(fp);
    return ret;
}

JNIEXPORT jstring JNICALL Java_com_andr0day_soencrypt_MainActivity_getStr
        (JNIEnv *env, jclass clazz){
    return getString(env);

}