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
#include "SEImageCatalog.h"
#include "SEImage.h"

using namespace Swing;

const std::string SEImageCatalog::ms_NullString("");
const std::string SEImageCatalog::ms_DefaultString("Default");
SEImageCatalog* SEImageCatalog::ms_pActive = 0;

//----------------------------------------------------------------------------
SEImageCatalog::SEImageCatalog(const std::string& rName)
    :
    m_Name(rName),
    m_Entry(IMAGE_MAP_SIZE)
{
    // 由于有些图形管线不支持1D image,
    // 因此创建一个默认2x2 image,用于无法找到指定资源时提醒用户.
    unsigned char* pData = SE_NEW unsigned char[16];
    pData[0] = 255;
    pData[1] = 0;
    pData[2] = 0;
    pData[3] = 0;
    pData[4] = 255;
    pData[5] = 0;
    pData[6] = 0;
    pData[7] = 0;
    pData[8] = 255;
    pData[9] = 0;
    pData[10] = 0;
    pData[11] = 0;
    pData[12] = 255;
    pData[13] = 0;
    pData[14] = 0;
    pData[15] = 0;

    m_spDefaultImage = SE_NEW SEImage(SEImage::IT_RGBA8888, 2, 2, pData,
        ms_DefaultString.c_str());
}
//----------------------------------------------------------------------------
SEImageCatalog::~SEImageCatalog()
{
}
//----------------------------------------------------------------------------
const std::string& SEImageCatalog::GetName() const
{
    return m_Name;
}
//----------------------------------------------------------------------------
bool SEImageCatalog::Insert(SEImage* pImage)
{
    if( !pImage )
    {
        SE_ASSERT( false );

        return false;
    }

    std::string StrImageName(pImage->GetName());
    if( StrImageName == ms_NullString
    ||  StrImageName == ms_DefaultString
    ||  pImage == m_spDefaultImage )
    {
        return false;
    }

    // 首先在资源目录中查找
    SEImage** ppTempImage = m_Entry.Find(StrImageName);
    if( ppTempImage )
    {
        // 该image已经存在
        return true;
    }

    // 该image不存在,则插入
    m_Entry.Insert(StrImageName, pImage);

    return true;
}
//----------------------------------------------------------------------------
bool SEImageCatalog::Remove(SEImage* pImage)
{
    if( !pImage )
    {
        SE_ASSERT( false );

        return false;
    }

    std::string StrImageName(pImage->GetName());
    if( StrImageName == ms_NullString
    ||  StrImageName == ms_DefaultString
    ||  pImage == m_spDefaultImage )
    {
        return false;
    }

    // 首先在资源目录中查找
    SEImage** ppTempImage = m_Entry.Find(StrImageName);
    if( !ppTempImage )
    {
        // 该image不存在
        return false;
    }

    // image存在,则移除
    m_Entry.Remove(StrImageName);

    return true;
}
//----------------------------------------------------------------------------
SEImage* SEImageCatalog::Find(const std::string& rImageName)
{
    if( rImageName == ms_NullString 
    ||  rImageName == ms_DefaultString )
    {
        return StaticCast<SEImage>(m_spDefaultImage);
    }

    // 首先在资源目录中查找
    SEImage** ppTempImage = m_Entry.Find(rImageName);
    if( ppTempImage )
    {
        // 找到则返回
        return *ppTempImage;
    }

    // 在磁盘中查找
    SEImage* pImage = SEImage::Load(rImageName.c_str());
    if( pImage )
    {
        // 该资源存在,且已经在Load后被加入资源目录,不用再次调用Insert函数
        return pImage;
    }

    // image不存在,则使用默认image
    return StaticCast<SEImage>(m_spDefaultImage);
}
//----------------------------------------------------------------------------
bool SEImageCatalog::PrintContents(const std::string& rFileName) const
{
    const char* pDecorated = SESystem::SE_GetPath(rFileName.c_str(), 
        SESystem::SM_WRITE);

    if( pDecorated )
    {
        std::ofstream OStream(pDecorated);

        SE_ASSERT( OStream );

        std::string StrImageName;
        SEImage** ppTempImage = m_Entry.GetFirst(&StrImageName);
        while( ppTempImage )
        {
            SEImage* pImage = *ppTempImage;
            OStream << StrImageName.c_str() << ":" << std::endl;
            OStream << "    dimension = " << pImage->GetDimension()
                << std::endl;
            for( int i = 0; i < pImage->GetDimension(); i++ )
            {
                OStream << "    bound(" << i << ") = " << pImage->GetBound(i)
                    << std::endl;
            }
            OStream << "    format = " << pImage->GetFormatName().c_str() << 
                std::endl;
            OStream << std::endl;
            ppTempImage = m_Entry.GetNext(&StrImageName);
        }
        OStream.close();

        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SEImageCatalog::SetActive(SEImageCatalog* pActive)
{
    ms_pActive = pActive;
}
//----------------------------------------------------------------------------
SEImageCatalog* SEImageCatalog::GetActive()
{
    return ms_pActive;
}
//----------------------------------------------------------------------------
