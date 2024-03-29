// Swing Engine Version 1 Source Code 
// Most of techniques in the engine are mainly based on David Eberly's
// Wild Magic 4 open-source code.The author of Swing Engine learned a lot
// from Eberly's experience of architecture and algorithm.
// Several sub-systems are totally new,and others are re-implimented or
// re-organized based on Wild Magic 4's sub-systems.
// Copyright (c) 2007-2010.  All Rights Reserved
//
// Eberly's permission:
// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// the location:
// http://www.gnu.org/copyleft/lgpl.html

#ifndef Swing_System_H
#define Swing_System_H

// C++标准库
#include <cassert>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>

// STL 常用数据结构
#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "SEFoundationLIB.h"
#include "SEPlatforms.h"
#include "SEAssert.h"
#include "SEMemory.h"

namespace Swing
{

//----------------------------------------------------------------------------
// 说明:
// Author:Sun Che
// Date:20080202
//----------------------------------------------------------------------------
class SE_FOUNDATION_API SESystem
{
public:
    // 交换一个C内置类型数据的字节内存排放位置(iSize = 2,4,8)
    static void SE_SwapBytes(int iSize, void* pValue);
	// 交换一组C内置类型数据的字节内存排放位置(iSize = 2,4,8)
    static void SE_SwapBytes(int iSize, int iCount, void* pValue);

    // 统一输出数据为little endian形式
    static bool SE_IsBigEndian(void);
    static void SE_EndianCopy(int iSize, const void* pSrc, void* pDst);
    static void SE_EndianCopy(int iSize, int iCount, const void* pSrc, void* 
        pDst);

    // 返回单位为秒
    static double SE_GetTime(void);

    // 支持文件的load,save
    static bool SE_Load(const char* pFileName, char*& rDstBuffer, int& riSize);
    static bool SE_Save(const char* pFileName, const char* pSrcBuffer, int 
        iSize);
    static bool SE_Append(const char* pFileName, const char* pSrcBuffer, int 
        iSize);

    // buffer读写,文件读写,单字节数据流,endianness无关
    static int SE_Read1(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Write1(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Read1(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Write1(FILE* pDstFile, int iCount, const void* pSrcData);

    // buffer读写,文件读写,2,4,8字节数据流,源数据为little endian
    static int SE_Read2le(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Read4le(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Read8le(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Write2le(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Write4le(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Write8le(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Read2le(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Read4le(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Read8le(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Write2le(FILE* pDstFile, int iCount, const void* pSrcData);
    static int SE_Write4le(FILE* pDstFile, int iCount, const void* pSrcData);
    static int SE_Write8le(FILE* pDstFile, int iCount, const void* pSrcData);

    // buffer读写,文件读写,2,4,8字节数据流,源数据为big endian
    static int SE_Read2be(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Read4be(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Read8be(const char* pSrcBuffer, int iCount, void* pDstData);
    static int SE_Write2be(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Write4be(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Write8be(char* pDstBuffer, int iCount, const void* pSrcData);
    static int SE_Read2be(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Read4be(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Read8be(FILE* pSrcFile, int iCount, void* pDstData);
    static int SE_Write2be(FILE* pDstFile, int iCount, const void* pSrcData);
    static int SE_Write4be(FILE* pDstFile, int iCount, const void* pSrcData);
    static int SE_Write8be(FILE* pDstFile, int iCount, const void* pSrcData);

    // Macintosh平台需要此函数是由于其复杂的应用程序结构,
    // 特别的,此函数用于Xcode项目并且忽略目录项,但假设应用程序所需
    // 的数据文件在项目的资源文件夹下,
    // 其他的平台连接目录名与文件名,结果值储存在类静态全局内存中(多线程时需
    // 注意).
    static const char* SE_GetPath(const char* pDirectory, const char* 
        pFileName);

    // 维护一个目录列表,供应用程序查询,列表初始为空,
    // 由应用程序插入若干期望目录,目录名不能用目录分隔符结尾(比如"/"),
    // SE_InsertDirectory和SE_RemoveDirectory将会自动管理该字符
    static void SE_Initialize(void);
    static void SE_Terminate(void);
    static int SE_GetDirectoryCount(void);
    static const char* SE_GetDirectory(int i);
    static bool SE_InsertDirectory(const char* pDirectory);
    static bool SE_RemoveDirectory(const char* pDirectory);
    static void SE_RemoveAllDirectories(void);

    // SE_GetPath搜索目录列表,如果指定文件名存在,则返回其完整路径名称,否则返回
    // null,eMode以下是枚举值.
    enum
    {
        SM_READ,
        SM_WRITE,
        SM_READ_WRITE
    };
    static const char* SE_GetPath(const char* pFileName, int eMode);

    // 创建一个颜色值,隐藏endian细节
    static unsigned int SE_MakeRGB(unsigned char ucR, unsigned char ucG, 
        unsigned char ucB);
    static unsigned int SE_MakeRGBA(unsigned char ucR, unsigned char ucG, 
        unsigned char ucB, unsigned char ucA);

    // 封装标准I/O函数从而解决buffer读写溢出等问题,提高安全性能,
    // 目前MSVC8.0实现的方式好于早先的标准I/O库
    static FILE* SE_Fopen(const char* pFileName, const char* pMode);
    static int SE_Fprintf(FILE* pFile, const char* pFormat, ...);
    static int SE_Fclose(FILE* pFile);
    static const char* SE_GetEnv(const char* pEnvVarName); 
    static void* SE_Memcpy(void* pDst, size_t uiDstSize, const void* pSrc, 
        size_t uiSrcSize);
    static int SE_Sprintf(char* pDst, size_t uiDstSize, const char* pFormat, 
        ...);
    static char* SE_Strcpy(char* pDst, size_t uiDstSize, const char* pSrc);
    static char* SE_Strcat(char* pDst, size_t uiDstSize, const char* pSrc);
    static char* SE_Strncpy(char* pDst, size_t uiDstSize, const char* pSrc, 
        size_t uiSrcSize);
    static char* SE_Strtok(char* pToken, const char* pDelimiters, char*& 
        rpNextToken);

private:
    enum { SYSTEM_MAX_PATH = 1024, SYSTEM_MAX_ENVVAR = 1024 };
    static char Path[SYSTEM_MAX_PATH];
    static char EnvVar[SYSTEM_MAX_ENVVAR];
    static std::vector<std::string>* Directories;   // 系统维护的目录列表

public:
    static char SE_PATH[SYSTEM_MAX_ENVVAR];
};

// 2D数组的分配与释放,释放时数组指针被设为null
template <class T> void Allocate(int iCols, int iRows, T**& raatArray);
template <class T> static void Deallocate(T**& raatArray);

// 3D数组的分配与释放,释放时数组指针被设为null
template <class T> void Allocate(int iCols, int iRows, int iSlices, T***& 
    raaatArray);
template <class T> void Deallocate(T***& raaatArray);

#include "SESystem.inl"

}

#endif