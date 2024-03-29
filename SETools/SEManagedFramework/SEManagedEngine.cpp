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

#include "SEManagedFrameworkPCH.h"
#include "SEManagedEngine.h"

using namespace Swing;
using namespace Swing::Tools::ManagedFramework;

//---------------------------------------------------------------------------
ManagedEngine::ManagedEngine()
{
    // 运行Register.
    SE_Foundation_Register();
    SE_DX9Renderer_Register();

    // Swing Engine initialize.
    SESystem::SE_Initialize();
    std::string tempSEPath(SESystem::SE_PATH);
    SEMain::Initialize();

    // 总是检查当前工作目录.
    SESystem::SE_InsertDirectory(".");

    // scene graph文件的路径.
    std::string tempDir;
    tempDir = tempSEPath + std::string("/Data/seof");
    SESystem::SE_InsertDirectory(tempDir.c_str());

    // texture image文件的路径.
    tempDir = tempSEPath + std::string("/Data/seif");
    SESystem::SE_InsertDirectory(tempDir.c_str());

    // shader program文件的路径.
    tempDir = tempSEPath + std::string("/Data/sesp/Cg");
    SESystem::SE_InsertDirectory(tempDir.c_str());
}
//---------------------------------------------------------------------------
ManagedEngine::~ManagedEngine()
{
    // Swing Engine terminate.
    SEMain::Terminate();
    SESystem::SE_Terminate();
}
//---------------------------------------------------------------------------
void ManagedEngine::Initialize()
{
    if( !ms_thThis )
    {
        ms_thThis = gcnew ManagedEngine;
    }
}
//---------------------------------------------------------------------------
void ManagedEngine::Terminate()
{
    if( ms_thThis )
    {
        delete ms_thThis;
        ms_thThis = nullptr;
    }
}
//---------------------------------------------------------------------------
void ManagedEngine::InitializeShaderProgramCatalog(
    ManagedRenderer^ thRenderer)
{
    if( !ms_thThis )
    {
        return;
    }

    if( !thRenderer )
    {
        throw gcnew ArgumentNullException("thRenderer");
    }

    if( !SEVertexProgramCatalog::GetActive() || 
        !SEPixelProgramCatalog::GetActive() || 
        !SEDX9ProgramInterfaceCatalog::GetActive() )
    {
        throw gcnew NullReferenceException(
            "Initializing shader program catalog");
    }

    SERenderer* pRenderer = thRenderer->GetNativeRenderer();
    SEVertexProgramCatalog::GetActive()->SetRenderer(pRenderer);
    SEPixelProgramCatalog::GetActive()->SetRenderer(pRenderer);
    SEDX9ProgramInterfaceCatalog::GetActive()->SetRenderer(
        (SEDX9Renderer*)pRenderer);
}
//---------------------------------------------------------------------------
void ManagedEngine::TerminateShaderProgramCatalog()
{
    if( !ms_thThis )
    {
        return;
    }

    if( !SEVertexProgramCatalog::GetActive() || 
        !SEPixelProgramCatalog::GetActive() || 
        !SEDX9ProgramInterfaceCatalog::GetActive() )
    {
        throw gcnew NullReferenceException(
            "Terminating shader program catalog");
    }

    SEVertexProgramCatalog::GetActive()->SetRenderer(0);
    SEPixelProgramCatalog::GetActive()->SetRenderer(0);
    SEDX9ProgramInterfaceCatalog::GetActive()->SetRenderer(0);
}
//---------------------------------------------------------------------------