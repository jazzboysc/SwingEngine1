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

#include "SEFoundationPCH.h"
#include "SEStringTree.h"
#include "SEBoundingVolume.h"
#include "SEColorRGB.h"
#include "SEColorRGBA.h"

using namespace Swing;

//----------------------------------------------------------------------------
SEStringTree::SEStringTree()
{
}
//----------------------------------------------------------------------------
SEStringTree::~SEStringTree()
{
    int i;
    for( i = 0; i < (int)m_Strings.size(); i++ )
    {
        SE_DELETE[] m_Strings[i];
    }

    for( i = 0; i < (int)m_Children.size(); i++ )
    {
        SE_DELETE m_Children[i];
    }
}
//----------------------------------------------------------------------------
bool SEStringTree::Save(const char* pFileName, int iTabSize)
{
    FILE* pFile = SESystem::SE_Fopen(pFileName, "wt");
    if( !pFile )
    {
        return false;
    }

    Save(pFile, 0, iTabSize);

    return SESystem::SE_Fclose(pFile) == 0;
}
//----------------------------------------------------------------------------
void SEStringTree::Save(FILE* pFile, int iLevel, int iTabSize)
{
    // 缩进到适当位置
    int i, iIndent = iLevel*iTabSize;
    if( iIndent > 0 )
    {
        for( i = 0; i < iIndent; i++ )
        {
            SESystem::SE_Fprintf(pFile, "%c", ' ');
        }
    }

    if( iLevel < 10 )
    {
        SESystem::SE_Fprintf(pFile, "%d:  ", iLevel);
    }
    else
    {
        SESystem::SE_Fprintf(pFile, "%d: ", iLevel);
    }

    // header string
    if( m_Strings.size() > 0 )
    {
        SESystem::SE_Fprintf(pFile, "%s\n", m_Strings[0]);
    }
    else
    {
        SESystem::SE_Fprintf(pFile, "<no header>\n");
    }

    // body strings
    iIndent += 4;

    int j;
    for( j = 1; j < (int)m_Strings.size(); j++ )
    {
        for( i = 0; i < iIndent; i++ )
        {
            SESystem::SE_Fprintf(pFile, "%c", ' ');
        }
        SESystem::SE_Fprintf(pFile, "%s\n", m_Strings[j]);
    }

    iLevel++;
    for( j = 0; j < (int)m_Children.size(); j++ )
    {
        m_Children[j]->Save(pFile, iLevel, iTabSize);
    }
}
//----------------------------------------------------------------------------
void SEStringTree::FormatFloat(float fValue, size_t uiStringSize, 
    char* pString)
{
    // 是否为有效区间内的浮点数
    if( fValue > -SEMathf::MAX_REAL )
    {
        if( fValue < SEMathf::MAX_REAL )
        {
            SESystem::SE_Sprintf(pString, uiStringSize, "%f", fValue);
        }
        else
        {
            SESystem::SE_Strcpy(pString, uiStringSize, "INFINITY");
        }
    }
    else
    {
        SESystem::SE_Strcpy(pString, uiStringSize, "-INFINITY");
    }
}
//----------------------------------------------------------------------------
void SEStringTree::FormatDouble(double dValue, size_t uiStringSize, 
    char* pString)
{
    // 是否为有效区间内的双精度浮点数
    if( dValue > -SEMathd::MAX_REAL )
    {
        if( dValue < SEMathd::MAX_REAL )
        {
            SESystem::SE_Sprintf(pString, uiStringSize, "%lf", dValue);
        }
        else
        {
            SESystem::SE_Strcpy(pString, uiStringSize, "INFINITY");
        }
    }
    else
    {
        SESystem::SE_Strcpy(pString, uiStringSize, "-INFINITY");
    }
}
//----------------------------------------------------------------------------
char* Swing::Format(const SERTTI* pRTTI, const char* pName)
{
    SE_ASSERT( pRTTI );

    const char* pRTTIName = pRTTI->GetName();

    size_t uiLength;
    char* pString;
    if( pName )
    {
        uiLength = strlen(pRTTIName) + strlen(pName) + 4;
        pString = SE_NEW char[uiLength];
        SESystem::SE_Sprintf(pString, uiLength, "%s <%s>", pRTTIName, pName);
    }
    else
    {
        uiLength = strlen(pRTTIName) + 2;
        pString = SE_NEW char[uiLength];
        SESystem::SE_Sprintf(pString, uiLength, "%s", pRTTIName);
    }

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pString)
{
    size_t uiSize;
    char* pDuplicate;
    if( pString )
    {
        uiSize = strlen(pString) + 1;
        pDuplicate = SE_NEW char[uiSize];
        SESystem::SE_Strcpy(pDuplicate, uiSize, pString);
    }
    else
    {
        uiSize = strlen("<no title>") + 1;
        pDuplicate = SE_NEW char[uiSize];
        SESystem::SE_Strcpy(pDuplicate, uiSize, "<no title>");
    }

    return pDuplicate;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, bool bValue)
{
    SE_ASSERT( pPrefix );

    size_t uiLength = strlen(pPrefix) + 2 + (bValue ? 4 : 5);
    char* pString = SE_NEW char[uiLength];
    if( bValue )
    {
        SESystem::SE_Sprintf(pString, uiLength, "%s true", pPrefix);
    }
    else
    {
        SESystem::SE_Sprintf(pString, uiLength, "%s false", pPrefix);
    }

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, char cValue)
{
    SE_ASSERT( pPrefix );

    size_t uiLength = strlen(pPrefix) + 2 + 1;
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %c", pPrefix, cValue);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, unsigned char ucValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%u", (unsigned int)ucValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, short sValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%hd", sValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, unsigned short usValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%hu", usValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, int iValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%d", iValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, unsigned int uiValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%u", uiValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, long lValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%ld", lValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, unsigned long ulValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%lu", ulValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, float fValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiTempBufferSize = 256;
    char TempBuffer[uiTempBufferSize];
    SEStringTree::FormatFloat(fValue, uiTempBufferSize, TempBuffer);

    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, double dValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SEStringTree::FormatDouble(dValue, uiSize, TempBuffer);

    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, void* pValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 32;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "%p", pValue);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const char* pValue)
{
    SE_ASSERT( pPrefix && pValue );

    size_t uiLength = strlen(pPrefix) + 2 + strlen(pValue);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, pValue);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEBoundingVolume* pBV)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize], acR[uiSize];
    SEStringTree::FormatFloat(pBV->GetCenter().X, uiSize, acX);
    SEStringTree::FormatFloat(pBV->GetCenter().Y, uiSize, acY);
    SEStringTree::FormatFloat(pBV->GetCenter().Z, uiSize, acZ);
    SEStringTree::FormatFloat(pBV->GetRadius(), uiSize, acR);

    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "(x: %s, y: %s, z: %s, r: %s)", 
        acX, acY, acZ, acR);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEColorRGBA& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "(r: %f, g: %f, b: %f, a: %f)", 
        rValue.R, rValue.G, rValue.B, rValue.A);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEColorRGB& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "(r: %f, g: %f, b: %f)", 
        rValue.R, rValue.G, rValue.B);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SELine3f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize,
        "(px: %f, py: %f, pz: %f, dx: %f, dy: %f, dz: %f)",
        rValue.Origin.X, rValue.Origin.Y, rValue.Origin.Z,
        rValue.Direction.X, rValue.Direction.Y, rValue.Direction.Z);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEMatrix2f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "[c0:(%f,%f),c1:(%f,%f)]",
        rValue[0][0], rValue[1][0],
        rValue[0][1], rValue[1][1]);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEMatrix3f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize,
        "[c0:(%f,%f,%f),c1:(%f,%f,%f),c2:(%f,%f,%f)]",
        rValue[0][0], rValue[1][0], rValue[2][0],
        rValue[0][1], rValue[1][1], rValue[2][1],
        rValue[0][2], rValue[1][2], rValue[2][2]);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEMatrix4f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[256];
    SESystem::SE_Sprintf(TempBuffer, uiSize,
    "[c0:(%f,%f,%f,%f),c1:(%f,%f,%f,%f),c2:(%f,%f,%f,%f),c3(%f,%f,%f,%f)]",
        rValue[0][0], rValue[1][0], rValue[2][0], rValue[3][0],
        rValue[0][1], rValue[1][1], rValue[2][1], rValue[3][1],
        rValue[0][2], rValue[1][2], rValue[2][2], rValue[3][2],
        rValue[0][3], rValue[1][3], rValue[2][3], rValue[3][3]);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEPlane3f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize,
        "(nx: %f, ny: %f, nz: %f, c: %f)", rValue.Normal.X,
        rValue.Normal.Y, rValue.Normal.Z, rValue.Constant);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEQuaternionf& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize,
        "(w: %.3f, x: %.3f, y: %.3f, z: %.3f)", rValue.W,
        rValue.X, rValue.Y, rValue.Z);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEVector2f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize];
    SEStringTree::FormatFloat(rValue.X, uiSize, acX);
    SEStringTree::FormatFloat(rValue.Y, uiSize, acY);

    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "(x: %s, y: %s)", acX, acY);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEVector3f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize];
    SEStringTree::FormatFloat(rValue.X, uiSize, acX);
    SEStringTree::FormatFloat(rValue.Y, uiSize, acY);
    SEStringTree::FormatFloat(rValue.Z, uiSize, acZ);

    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "(x: %s, y: %s, z: %s)", acX, 
        acY, acZ);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------
char* Swing::Format(const char* pPrefix, const SEVector4f& rValue)
{
    SE_ASSERT( pPrefix );

    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize], acW[uiSize];
    SEStringTree::FormatFloat(rValue.X, uiSize, acX);
    SEStringTree::FormatFloat(rValue.Y, uiSize, acY);
    SEStringTree::FormatFloat(rValue.Z, uiSize, acZ);
    SEStringTree::FormatFloat(rValue.W, uiSize, acW);

    char TempBuffer[uiSize];
    SESystem::SE_Sprintf(TempBuffer, uiSize, "(x: %s, y: %s, z: %s, w: %s)", 
        acX, acY, acZ, acW);
    size_t uiLength = strlen(pPrefix) + 2 + strlen(TempBuffer);
    char* pString = SE_NEW char[uiLength];
    SESystem::SE_Sprintf(pString, uiLength, "%s %s", pPrefix, TempBuffer);

    return pString;
}
//----------------------------------------------------------------------------