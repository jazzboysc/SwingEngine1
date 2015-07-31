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
//
//----------------------------------------------------------------------------
// AngelCode Tool Box Library
// Copyright (c) 2007-2008 Andreas Jonsson
//
// This software is provided 'as-is', without any express or implied 
// warranty. In no event will the authors be held liable for any 
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any 
// purpose, including commercial applications, and to alter it and 
// redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you 
//    must not claim that you wrote the original software. If you use
//    this software in a product, an acknowledgment in the product 
//    documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and 
//    must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source 
//    distribution.
//
// Andreas Jonsson
// andreas@angelcode.com
//----------------------------------------------------------------------------

#ifndef Swing_BMFont_H
#define Swing_BMFont_H

#include "SEGuiLIB.h"
#include "SEObject.h"
#include "SETexture.h"
#include "SEBMFontInfoBlock.h"
#include "SEBMFontCharInfo.h"
#include "SEBMFontCommonBlock.h"
#include "SEBMFontKerningPair.h"

namespace Swing
{

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20100602
//----------------------------------------------------------------------------
class SE_GUI_API SEBMFont : public SEObject
{
    SE_DECLARE_RTTI;
    SE_DECLARE_NAME_ID;
    SE_DECLARE_STREAM;

public:
    SEBMFont(char* pRawData, int iDataSize);
    virtual ~SEBMFont(void);

    // Block access.
    SEBMFontInfoBlock* GetInfoBlock(void);
    SEBMFontCommonBlock* GetCommonBlock(void);

    // CharInfo access.
    int GetCharInfoCount(void) const;
    SEBMFontCharInfo* GetCharInfo(int i);
    SEBMFontCharInfo* GetCharInfo(wchar_t wcChar);

    // KerningPair access.
    int GetKerningPairCount(void) const;
    SEBMFontKerningPair* GetKerningPair(int i);
    SEBMFontKerningPair* GetKerningPair(wchar_t wcFirst, wchar_t wcSecond);

    // Texture page access.
    int GetTexturePageCount(void) const;
    const std::string& GetTexturePageName(int i) const;
    void SetTexturePage(int i, SETexture* pTexture);
    SETexture* GetTexturePage(int i);

    static SEBMFont* Load(const char* acBMFontName);

protected:
    // support for streaming.
    SEBMFont(void);

    void Initialize(char* pRawData, int iDataSize);

    SEBMFontInfoBlock* m_pInfoBlock;
    SEBMFontCommonBlock* m_pCommonBlock;

    SEBMFontCharInfo* m_aCharInfo;
    int m_iCharInfoCount;

    SEBMFontKerningPair* m_aKerningPair;
    int	m_ikerningPairCount;

    enum { MAX_TEXTUREPAGES = 8 };

    std::string m_aTextureNames[MAX_TEXTUREPAGES];
    SETexturePtr m_aspTextures[MAX_TEXTUREPAGES];
    int m_iTexturePageCount;
};

typedef SESmartPointer<SEBMFont> SEBMFontPtr;

}

#endif
