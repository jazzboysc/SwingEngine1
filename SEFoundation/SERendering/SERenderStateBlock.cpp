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
#include "SERenderStateBlock.h"

using namespace Swing;

SE_IMPLEMENT_RTTI(Swing, SERenderStateBlock, SEObject);
SE_IMPLEMENT_STREAM(SERenderStateBlock);
SE_IMPLEMENT_DEFAULT_NAME_ID(SERenderStateBlock, SEObject);

//SE_REGISTER_STREAM(SERenderStateBlock);

//----------------------------------------------------------------------------
SERenderStateBlock::SERenderStateBlock()
{
    memset(States, 0, SEGlobalState::MAX_STATE_TYPE*sizeof(SEGlobalState*));
}
//----------------------------------------------------------------------------
SERenderStateBlock::~SERenderStateBlock()
{
    Release();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SERenderStateBlock::Load(SEStream& rStream, SEStream::SELink* pLink)
{
    SE_BEGIN_DEBUG_STREAM_LOAD;

    SEObject::Load(rStream, pLink);

    // link data
    SEObject* pObject;
    for( int i = 0; i < SEGlobalState::MAX_STATE_TYPE; i++ )
    {
        rStream.Read(pObject);  // States[i]
        pLink->Add(pObject);
    }

    SE_END_DEBUG_STREAM_LOAD(SERenderStateBlock);
}
//----------------------------------------------------------------------------
void SERenderStateBlock::Link(SEStream& rStream, SEStream::SELink* pLink)
{
    SEObject::Link(rStream, pLink);

    SEObject* pLinkID;
    for( int i = 0; i < SEGlobalState::MAX_STATE_TYPE; i++ )
    {
        pLinkID = pLink->GetLinkID();
        States[i] = (SEGlobalState*)rStream.GetFromMap(pLinkID);
    }
}
//----------------------------------------------------------------------------
bool SERenderStateBlock::Register(SEStream& rStream) const
{
    if( !SEObject::Register(rStream) )
    {
        return false;
    }

    for( int i = 0; i < SEGlobalState::MAX_STATE_TYPE; i++ )
    {
        if( States[i] )
        {
            States[i]->Register(rStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void SERenderStateBlock::Save(SEStream& rStream) const
{
    SE_BEGIN_DEBUG_STREAM_SAVE;

    SEObject::Save(rStream);

    // link data
    for( int i = 0; i < SEGlobalState::MAX_STATE_TYPE; i++ )
    {
        rStream.Write(States[i]);
    }

    SE_END_DEBUG_STREAM_SAVE(SERenderStateBlock);
}
//----------------------------------------------------------------------------
int SERenderStateBlock::GetDiskUsed(const SEStreamVersion& rVersion) const
{
    return SEObject::GetDiskUsed(rVersion) +
        SEGlobalState::MAX_STATE_TYPE*SE_PTRSIZE(States[0]);
}
//----------------------------------------------------------------------------
SEStringTree* SERenderStateBlock::SaveStrings(const char*)
{
    SEStringTree* pTree = SE_NEW SEStringTree;

    // strings
    pTree->Append(Format(&TYPE, GetName().c_str()));

    // children
    pTree->Append(SEObject::SaveStrings());

    for( int i = 0; i < SEGlobalState::MAX_STATE_TYPE; i++ )
    {
        if( States[i] )
        {
            pTree->Append(States[i]->SaveStrings());
        }
    }

    return pTree;
}
//----------------------------------------------------------------------------
