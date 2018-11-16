#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define ERRBUFLEN 1024

#ifdef DEBUG_ON
#define ERR_PRINT(str) \
    do \
    { \
        char errbuf[ERRBUFLEN] = {'\0'}; \
        snprintf(errbuf, ERRBUFLEN, "[file: %s line: %d] %s", \
                                    __FILE__, __LINE__, str); \
        fprintf(stderr, "\033[31m"); \
        perror(errbuf); \
        fprintf(stderr, "\033[0m"); \
    } while (0)
#define INFO_PRINT(str) \
    do \
    { \
        printf("\033[31m"); \
        printf("[file: %s line: %d] %s\n", __FILE__, __LINE__, str); \
        printf("\033[0m"); \
    } while(0)
#else
#define ERR_PRINT(str)
#define INFO_PRINT(str)
#endif

//////////////
/*
 * 打印调试信息
 */
#define DEBUG_SWITCH
#ifdef  DEBUG_SWITCH
#define pr_debug(fmt,args...) printf(fmt, ##args)
#else
#define pr_debug(fmt,args...) /* do nothing */
#endif

/*
 * 打印错误信息
 * 自动打印发生错误时代码所在的位置
 */
#define  ERR_DEBUG_SWITCH
#ifdef   ERR_DEBUG_SWITCH
#define pr_err(fmt,args...) printf("File:<%s> Fun:[%s] Line:%d\n "fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
#else
#define pr_err(fmt,args...) /* do nothing */
#endif


#endif



// #include "Debug.h"

// int
// main()
// {
        // printf("test\n");

        // FILE *fp = NULL;

        // fp = fopen("./none.txt", "r");
        // if (fp == NULL)
        // {
                // ERR_PRINT("fopen error");
        // }

        // int i = 1;

        // if (i < 2)
        // {
                // INFO_PRINT("i < 2");
        // }

        // return 0;
// }

// $ gcc test.c
// $ ./a.out
// $ test
// $ gcc test.c -DDEBUG_ON
// $ ./a.out
// $ test
// $ fopen error: No such file or directory
// $ i < 2