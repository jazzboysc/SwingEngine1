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
#include "DynamicMultiTextureEffect.h"

using namespace Swing;

SE_IMPLEMENT_RTTI(Swing, DynamicMultiTextureEffect, SEShaderEffect);
SE_IMPLEMENT_STREAM(DynamicMultiTextureEffect);
SE_IMPLEMENT_DEFAULT_NAME_ID(DynamicMultiTextureEffect, SEShaderEffect);

SE_REGISTER_STREAM(DynamicMultiTextureEffect);

SEVector3f DynamicMultiTextureEffect::ConstantColor = SEVector3f::ZERO;

//----------------------------------------------------------------------------
DynamicMultiTextureEffect::DynamicMultiTextureEffect(int iTextureCount)
    :
    SEShaderEffect(1)
{
    m_aImageName = 0;
    m_aTextureTypeName = 0;
    SetTextureCount(iTextureCount);
}
//----------------------------------------------------------------------------
DynamicMultiTextureEffect::DynamicMultiTextureEffect()
{
    m_iTextureCount = 0;
    m_aImageName = 0;
    m_aTextureTypeName = 0;
}
//----------------------------------------------------------------------------
DynamicMultiTextureEffect::~DynamicMultiTextureEffect()
{
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::SetTextureCount(int iTextureCount)
{
    SE_ASSERT( iTextureCount >= 1 );

    m_iTextureCount = iTextureCount;
    SE_DELETE[] m_aImageName;
    m_aImageName = SE_NEW std::string[m_iTextureCount];
    SE_DELETE[] m_aTextureTypeName;
    m_aTextureTypeName = SE_NEW std::string[m_iTextureCount];
}
//----------------------------------------------------------------------------
int DynamicMultiTextureEffect::GetTextureCount() const
{
    return m_iTextureCount;
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::SetImageName(int i, const std::string& 
    rImageName)
{
    SE_ASSERT( 0 <= i && i < m_iTextureCount );

    m_aImageName[i] = rImageName;
}
//----------------------------------------------------------------------------
const std::string& DynamicMultiTextureEffect::GetImageName(int i) const
{
    SE_ASSERT( 0 <= i && i < m_iTextureCount );

    return m_aImageName[i];
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::SetTextureTypeName(int i, 
    const std::string& rTextureTypeName)
{
    SE_ASSERT( 0 <= i && i < m_iTextureCount );

    m_aTextureTypeName[i] = rTextureTypeName;
}
//----------------------------------------------------------------------------
const std::string& DynamicMultiTextureEffect::GetTextureTypeName(int i) const
{
    SE_ASSERT( 0 <= i && i < m_iTextureCount );

    return m_aTextureTypeName[i];
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::Configure()
{
    m_VShader[0] = SE_NEW SEVertexShader("ITexture.v_ITexture");
    m_PShader[0] = SE_NEW SEPixelShader("ITexture.p_ITexture");

    m_PShader[0]->SetTextureCount(m_iTextureCount);
    for( int i = 0; i < m_iTextureCount; i++ )
    {
        m_PShader[0]->SetImageName(i, m_aImageName[i]);
        SETexture* pTexture = m_PShader[0]->GetTexture(i);
        pTexture->SetFilterType(SETexture::NEAREST);
    }

    SEInterfaceDescriptor* pInterfaceDesc = SE_NEW SEInterfaceDescriptor;
    m_PShader[0]->SetInterfaceDescriptor(pInterfaceDesc);

    SEDescriptorItem* pDescItem = SE_NEW SEDescriptorItem;
    pInterfaceDesc->AttachItem(pDescItem);
    pDescItem->IsArray = true;
    pDescItem->SetInstanceName("aTextures");
    for( int i = 0; i < m_iTextureCount; i++ )
        pDescItem->AttachTypeName(m_aTextureTypeName[i]);
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::OnLoadPrograms(int, SEProgram*,
    SEProgram* pPProgram, SEProgram*)
{
    SEUserConstant* pUC = pPProgram->GetUC("aTextures[3].kColor");

    if( pUC )
        pUC->SetDataSource((float*)ConstantColor);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::Load(SEStream& rStream, SEStream::SELink* 
    pLink)
{
    SE_BEGIN_DEBUG_STREAM_LOAD;

    SEShaderEffect::Load(rStream, pLink);

    // native data
    rStream.Read(m_iTextureCount);
    SetTextureCount(m_iTextureCount);
    for( int i = 0; i < m_iTextureCount; i++ )
    {
        rStream.Read(m_aImageName[i]);
        rStream.Read(m_aTextureTypeName[i]);
    }

    SE_END_DEBUG_STREAM_LOAD(DynamicMultiTextureEffect);
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::Link(SEStream& rStream, SEStream::SELink* 
    pLink)
{
    SEShaderEffect::Link(rStream, pLink);
}
//----------------------------------------------------------------------------
bool DynamicMultiTextureEffect::Register(SEStream& rStream) const
{
    return SEShaderEffect::Register(rStream);
}
//----------------------------------------------------------------------------
void DynamicMultiTextureEffect::Save(SEStream& rStream) const
{
    SE_BEGIN_DEBUG_STREAM_SAVE;

    SEShaderEffect::Save(rStream);

    // native data
    rStream.Write(m_iTextureCount);
    for( int i = 0; i < m_iTextureCount; i++ )
    {
        rStream.Write(m_aImageName[i]);
        rStream.Write(m_aTextureTypeName[i]);
    }

    SE_END_DEBUG_STREAM_SAVE(DynamicMultiTextureEffect);
}
//----------------------------------------------------------------------------
int DynamicMultiTextureEffect::GetDiskUsed(const SEStreamVersion& rVersion) 
    const
{
    int iSize = SEShaderEffect::GetDiskUsed(rVersion) +
        sizeof(m_iTextureCount);
 
    for( int i = 0; i < m_iTextureCount; i++ )
    {
        iSize += sizeof(int) + (int)m_aImageName[i].length() + 
            sizeof(int) + (int)m_aTextureTypeName[i].length();
    }

    return iSize;
}
//----------------------------------------------------------------------------
SEStringTree* DynamicMultiTextureEffect::SaveStrings(const char*)
{
    SEStringTree* pTree = SE_NEW SEStringTree;

    // strings
    pTree->Append(Format(&TYPE, GetName().c_str()));
    pTree->Append(Format("texture count =", m_iTextureCount));

    const size_t uiTitleSize = 16;
    char acTitle[uiTitleSize];
    for( int i = 0; i < m_iTextureCount; i++ )
    {
        SESystem::SE_Sprintf(acTitle, uiTitleSize, "image[%d] =", i);
        pTree->Append(Format(acTitle, m_aImageName[i].c_str()));
        SESystem::SE_Sprintf(acTitle, uiTitleSize, "type[%d] =", i);
        pTree->Append(Format(acTitle, m_aTextureTypeName[i].c_str()));
    }

    // children
    pTree->Append(SEShaderEffect::SaveStrings());

    return pTree;
}
//----------------------------------------------------------------------------
