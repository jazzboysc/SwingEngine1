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
#include "SERenderer.h"
#include "SECamera.h"
#include "SEEffect.h"
#include "SEGeometry.h"
#include "SELight.h"
#include "SENode.h"
#include "SEShaderEffect.h"

using namespace Swing;

SERenderer::SetConstantFunction SERenderer::ms_aoSCFunction[SC_COUNT] =
{
    &SERenderer::SetConstantWMatrix,
    &SERenderer::SetConstantVMatrix,
    &SERenderer::SetConstantPMatrix,
    &SERenderer::SetConstantWVMatrix,
    &SERenderer::SetConstantVPMatrix,
    &SERenderer::SetConstantWVPMatrix,

    &SERenderer::SetConstantMaterialEmissive,
    &SERenderer::SetConstantMaterialAmbient,
    &SERenderer::SetConstantMaterialDiffuse,
    &SERenderer::SetConstantMaterialSpecular,

    &SERenderer::SetConstantCameraModelPosition,
    &SERenderer::SetConstantCameraModelRight,
    &SERenderer::SetConstantCameraModelUp,
    &SERenderer::SetConstantCameraModelDirection,
    &SERenderer::SetConstantCameraWorldPosition,
    &SERenderer::SetConstantCameraWorldRight,
    &SERenderer::SetConstantCameraWorldUp,
    &SERenderer::SetConstantCameraWorldDirection,

    &SERenderer::SetConstantProjectorModelPosition,
    &SERenderer::SetConstantProjectorModelRight,
    &SERenderer::SetConstantProjectorModelUp,
    &SERenderer::SetConstantProjectorModelDirection,
    &SERenderer::SetConstantProjectorWorldPosition,
    &SERenderer::SetConstantProjectorWorldRight,
    &SERenderer::SetConstantProjectorWorldUp,
    &SERenderer::SetConstantProjectorWorldDirection,
    &SERenderer::SetConstantProjectorMatrix,

    &SERenderer::SetConstantLightModelPosition,
    &SERenderer::SetConstantLightModelDirection,
    &SERenderer::SetConstantLightWorldPosition,
    &SERenderer::SetConstantLightWorldDirection,
    &SERenderer::SetConstantLightAmbient,
    &SERenderer::SetConstantLightDiffuse,
    &SERenderer::SetConstantLightSpecular,
    &SERenderer::SetConstantLightSpotCutoff,
    &SERenderer::SetConstantLightAttenuation,
    &SERenderer::SetConstantLightCount
};

//----------------------------------------------------------------------------
SERenderer::SERenderer(SEFrameBuffer::FormatType eFormat,
    SEFrameBuffer::DepthType eDepth, SEFrameBuffer::StencilType eStencil,
    SEFrameBuffer::BufferingType eBuffering,
    SEFrameBuffer::MultisamplingType eMultisampling,
    int iWidth, int iHeight)
    :
    m_eFormat(eFormat),
    m_eDepth(eDepth),
    m_eStencil(eStencil),
    m_eBuffering(eBuffering),
    m_eMultisampling(eMultisampling),
    m_iWidth(iWidth),
    m_iHeight(iHeight),
    m_ClearColor(SEColorRGBA::SE_RGBA_WHITE),
    m_fClearDepth(1.0f),
    m_uiClearStencil(0),
    m_WorldMatrix(SEMatrix4f::IDENTITY),
    m_SaveWorldMatrix(SEMatrix4f::IDENTITY),
    m_ViewMatrix(SEMatrix4f::IDENTITY),
    m_SaveViewMatrix(SEMatrix4f::IDENTITY),
    m_ProjectionMatrix(SEMatrix4f::IDENTITY),
    m_SaveProjectionMatrix(SEMatrix4f::IDENTITY)
{
    // 派生类应该检测并设置这些参数,并且创建m_aspLight数组.
    m_iMaxLights = 0;
    m_iMaxColors = 0;
    m_iMaxTCoords = 0;
    m_iMaxVShaderImages = 0;
    m_iMaxGShaderImages = 0;
    m_iMaxPShaderImages = 0;
    m_iMaxStencilIndices = 0;
    m_iMaxUserClipPlanes = 0;
    m_iMaxVShaderProfile = -1;
    m_iMaxGShaderProfile = -1;
    m_iMaxPShaderProfile = -1;
    m_iMaxRenderTargets = 0;
    m_iNumActiveLights = 0;
    m_aspLight = 0;

    // Derived classes should allocate the m_apActiveSamplers.
    m_iNumActiveSamplers = 0;
    m_iCurrentSampler = 0;
    m_iMaxActiveSamplerCount = 0;
    m_apActiveSamplers = 0;

    // 如果有必要,派生类应该检测并设置这个参数.
    m_bHasShaderCompiler = true;

    m_pCamera = 0;
    m_pProjector = 0;
    m_pGeometry = 0;
    m_bReverseCullFace = false;
    m_bFullscreen = false;
    m_iFontID = 0;
}
//----------------------------------------------------------------------------
SERenderer::~SERenderer()
{
    SE_DELETE[] m_apActiveSamplers;
    SE_DELETE[] m_aspLight;
    SetCamera(0);
}
//----------------------------------------------------------------------------
void SERenderer::SetCamera(SECamera* pCamera)
{
    if( m_pCamera )
    {
        m_pCamera->m_pRenderer = 0;
    }

    if( pCamera )
    {
        pCamera->m_pRenderer = this;
    }

    m_pCamera = pCamera;

    if( m_pCamera )
    {
        OnFrustumChange();
        OnViewportChange();
        OnFrameChange();
    }
}
//----------------------------------------------------------------------------
void SERenderer::OnFrameChange()
{
    const SEVector3f& rEye = m_pCamera->GetLocation();
    const SEVector3f& rRVector = m_pCamera->GetRVector();
    const SEVector3f& rUVector = m_pCamera->GetUVector();
    const SEVector3f& rDVector = m_pCamera->GetDVector();

    // 更新摄像机矩阵
    m_ViewMatrix[0][0] = rRVector[0];
    m_ViewMatrix[0][1] = rUVector[0];
    m_ViewMatrix[0][2] = rDVector[0];
    m_ViewMatrix[0][3] = 0.0f;
    m_ViewMatrix[1][0] = rRVector[1];
    m_ViewMatrix[1][1] = rUVector[1];
    m_ViewMatrix[1][2] = rDVector[1];
    m_ViewMatrix[1][3] = 0.0f;
    m_ViewMatrix[2][0] = rRVector[2];
    m_ViewMatrix[2][1] = rUVector[2];
    m_ViewMatrix[2][2] = rDVector[2];
    m_ViewMatrix[2][3] = 0.0f;
    m_ViewMatrix[3][0] = -rRVector.Dot(rEye);
    m_ViewMatrix[3][1] = -rUVector.Dot(rEye);
    m_ViewMatrix[3][2] = -rDVector.Dot(rEye);
    m_ViewMatrix[3][3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::OnFrustumChange()
{
    if( !m_pCamera )
    {
        return;
    }

    float fRMin, fRMax, fUMin, fUMax, fDMin, fDMax;
    m_pCamera->GetFrustum(fRMin, fRMax, fUMin, fUMax, fDMin, fDMax);

    float fInvRDiff = 1.0f / (fRMax - fRMin);
    float fInvUDiff = 1.0f / (fUMax - fUMin);
    float fInvDDiff = 1.0f / (fDMax - fDMin);

    // Update projection matrix.
    if( m_pCamera->IsPerspective() )
    {
        // Perspective projection.
        m_ProjectionMatrix[0][0] = 2.0f*fDMin*fInvRDiff;
        m_ProjectionMatrix[0][1] = 0.0f;
        m_ProjectionMatrix[0][2] = 0.0f;
        m_ProjectionMatrix[0][3] = 0.0f;
        m_ProjectionMatrix[1][0] = 0.0f;
        m_ProjectionMatrix[1][1] = 2.0f*fDMin*fInvUDiff;
        m_ProjectionMatrix[1][2] = 0.0f;
        m_ProjectionMatrix[1][3] = 0.0f;
        m_ProjectionMatrix[2][0] = -(fRMin + fRMax)*fInvRDiff;
        m_ProjectionMatrix[2][1] = -(fUMin + fUMax)*fInvUDiff;
        m_ProjectionMatrix[2][2] = fDMax*fInvDDiff;
        m_ProjectionMatrix[2][3] = 1.0f;
        m_ProjectionMatrix[3][0] = 0.0f;
        m_ProjectionMatrix[3][1] = 0.0f;
        m_ProjectionMatrix[3][2] = -fDMax*fDMin*fInvDDiff;
        m_ProjectionMatrix[3][3] = 0.0f;
    }
    else
    {
        // Orthogonal projection.
        m_ProjectionMatrix[0][0] = 2.0f*fInvRDiff;
        m_ProjectionMatrix[0][1] = 0.0f;
        m_ProjectionMatrix[0][2] = 0.0f;
        m_ProjectionMatrix[0][3] = 0.0f;
        m_ProjectionMatrix[1][0] = 0.0f;
        m_ProjectionMatrix[1][1] = 2.0f*fInvUDiff;
        m_ProjectionMatrix[1][2] = 0.0f;
        m_ProjectionMatrix[1][3] = 0.0f;
        m_ProjectionMatrix[2][0] = 0.0f;
        m_ProjectionMatrix[2][1] = 0.0f;
        m_ProjectionMatrix[2][2] = fInvDDiff;
        m_ProjectionMatrix[2][3] = 0.0f;
        m_ProjectionMatrix[3][0] = -(fRMin + fRMax)*fInvRDiff;
        m_ProjectionMatrix[3][1] = -(fUMin + fUMax)*fInvUDiff;
        m_ProjectionMatrix[3][2] = -fDMin*fInvDDiff;
        m_ProjectionMatrix[3][3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::Resize(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    OnViewportChange();
}
//----------------------------------------------------------------------------
void SERenderer::SetGlobalState(SEGlobalStatePtr aspState[])
{
    SEGlobalState* pState = aspState[SEGlobalState::ALPHA];
    if( pState )
    {
        SetAlphaState((SEAlphaState*)pState);
    }

    pState = aspState[SEGlobalState::CULL];
    if( pState )
    {
        SetCullState((SECullState*)pState);
    }

    pState = aspState[SEGlobalState::MATERIAL];
    if( pState )
    {
        SetMaterialState((SEMaterialState*)pState);
    }

    pState = aspState[SEGlobalState::POLYGONOFFSET];
    if( pState )
    {
        SetPolygonOffsetState((SEPolygonOffsetState*)pState);
    }

    pState = aspState[SEGlobalState::STENCIL];
    if( pState )
    {
        SetStencilState((SEStencilState*)pState);
    }

    pState = aspState[SEGlobalState::WIREFRAME];
    if( pState )
    {
        SetWireframeState((SEWireframeState*)pState);
    }

    pState = aspState[SEGlobalState::ZBUFFER];
    if( pState )
    {
        SetZBufferState((SEZBufferState*)pState);
    }
}
//----------------------------------------------------------------------------
void SERenderer::RestoreGlobalState(SEGlobalStatePtr aspState[])
{
    SEGlobalState* pState;
    
    if( aspState[SEGlobalState::ALPHA] )
    {
        pState = SEGlobalState::Default[SEGlobalState::ALPHA];
        SetAlphaState((SEAlphaState*)pState);
    }

    if( aspState[SEGlobalState::CULL] )
    {
        pState = SEGlobalState::Default[SEGlobalState::CULL];
        SetCullState((SECullState*)pState);
    }

    if( aspState[SEGlobalState::MATERIAL] )
    {
        pState = SEGlobalState::Default[SEGlobalState::MATERIAL];
        SetMaterialState((SEMaterialState*)pState);
    }

    if( aspState[SEGlobalState::POLYGONOFFSET] )
    {
        pState = SEGlobalState::Default[SEGlobalState::POLYGONOFFSET];
        SetPolygonOffsetState((SEPolygonOffsetState*)pState);
    }

    if( aspState[SEGlobalState::STENCIL] )
    {
        pState = SEGlobalState::Default[SEGlobalState::STENCIL];
        SetStencilState((SEStencilState*)pState);
    }

    if( aspState[SEGlobalState::WIREFRAME] )
    {
        pState = SEGlobalState::Default[SEGlobalState::WIREFRAME];
        SetWireframeState((SEWireframeState*)pState);
    }

    if( aspState[SEGlobalState::ZBUFFER] )
    {
        pState = SEGlobalState::Default[SEGlobalState::ZBUFFER];
        SetZBufferState((SEZBufferState*)pState);
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetPostWorldTransformation(const SEMatrix4f& rMatrix)
{
    m_SaveViewMatrix = m_ViewMatrix;
    rMatrix.GetTransposeTimes(m_SaveViewMatrix, m_ViewMatrix);
}
//----------------------------------------------------------------------------
void SERenderer::RestorePostWorldTransformation()
{
    m_ViewMatrix = m_SaveViewMatrix;
}
//----------------------------------------------------------------------------
void SERenderer::SetWorldTransformation()
{
    m_pGeometry->World.GetHomogeneous(m_WorldMatrix);
}
//----------------------------------------------------------------------------
void SERenderer::RestoreWorldTransformation()
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------
void SERenderer::SetLights()
{
    int iLCount = (int)m_pGeometry->Lights.size();
    for( int i = 0; i < iLCount; i++ )
    {
        SetLight(i, m_pGeometry->Lights[i]);
    }

    SetLightCount(iLCount);
}
//----------------------------------------------------------------------------
void SERenderer::RestoreLights()
{
    int iLCount = (int)m_pGeometry->Lights.size();
    for( int i = 0; i < iLCount; i++ )
    {
        SetLight(i, 0);
    }

    SetLightCount(0);
}
//----------------------------------------------------------------------------
void SERenderer::SetColorMask(bool bAllowRed, bool bAllowGreen,
    bool bAllowBlue, bool bAllowAlpha)
{
    m_bAllowRed = bAllowRed;
    m_bAllowGreen = bAllowGreen;
    m_bAllowBlue = bAllowBlue;
    m_bAllowAlpha = bAllowAlpha;
}
//----------------------------------------------------------------------------
void SERenderer::GetColorMask(bool& rbAllowRed, bool& rbAllowGreen,
    bool& rbAllowBlue, bool& rbAllowAlpha)
{
    rbAllowRed = m_bAllowRed;
    rbAllowGreen = m_bAllowGreen;
    rbAllowBlue = m_bAllowBlue;
    rbAllowAlpha = m_bAllowAlpha;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 可选geometry shader stage的资源装载与释放.
//----------------------------------------------------------------------------
void SERenderer::OnLoadGProgram(SEResourceIdentifier*&, SEGeometryProgram*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------
void SERenderer::OnReleaseGProgram(SEResourceIdentifier*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 可选render state block的资源装载与释放.
//----------------------------------------------------------------------------
void SERenderer::OnLoadRenderStateBlock(SEResourceIdentifier*&, 
    SERenderStateBlock*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------
void SERenderer::OnReleaseRenderStateBlock(SEResourceIdentifier*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 可选geometry shader stage的资源开启与关闭.
//----------------------------------------------------------------------------
void SERenderer::OnEnableGProgram(SEResourceIdentifier*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------
void SERenderer::OnDisableGProgram(SEResourceIdentifier*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 可选render state block的资源开启与关闭.
//----------------------------------------------------------------------------
void SERenderer::OnEnableRenderStateBlock(SEResourceIdentifier*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------
void SERenderer::OnDisableRenderStateBlock(SEResourceIdentifier*)
{
    // 由派生类负责实现.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 可选shader程序连接.
//----------------------------------------------------------------------------
bool SERenderer::OnLinkPrograms(SEVertexProgram*, SEGeometryProgram*, 
    SEPixelProgram*)
{
    // 由派生类负责实现.
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 设置shader程序所需的渲染器常量.
//----------------------------------------------------------------------------
void SERenderer::GetTransform(const SEMatrix4f& rMat, int iOperation,
    float* afData)
{
    const size_t uiSize = 16 * sizeof(float);

    if( iOperation == 0 )
    {
        // matrix
        SESystem::SE_Memcpy(afData, uiSize, (const float*)rMat, uiSize);
    }
    else if( iOperation == 1 )
    {
        // transpose of matrix
        SEMatrix4f mat4fMT;
        rMat.GetTranspose(mat4fMT);
        SESystem::SE_Memcpy(afData, uiSize, (const float*)mat4fMT, uiSize);
    }
    else if( iOperation == 2 )
    {
        // inverse of matrix
        SEMatrix4f mat4fMI;
        rMat.GetInverse(mat4fMI);
        SESystem::SE_Memcpy(afData, uiSize, (const float*)mat4fMI, uiSize);
    }
    else
    {
        // inverse-transpose of matrix
        SEMatrix4f mat4fMIT;
        rMat.GetInverse(mat4fMIT);
        mat4fMIT.Transpose();
        SESystem::SE_Memcpy(afData, uiSize, (const float*)mat4fMIT, uiSize);
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantWMatrix(int iOperation, float* afData)
{
    GetTransform(m_WorldMatrix, iOperation, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantVMatrix(int iOperation, float* afData)
{
    GetTransform(m_ViewMatrix, iOperation, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantPMatrix(int iOperation, float* afData)
{
    GetTransform(m_ProjectionMatrix, iOperation, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantWVMatrix(int iOperation, float* afData)
{
    SEMatrix4f mat4fWV = m_WorldMatrix * m_ViewMatrix;
    GetTransform(mat4fWV, iOperation, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantVPMatrix(int iOperation, float* afData)
{
    SEMatrix4f mat4fVP = m_ViewMatrix * m_ProjectionMatrix;
    GetTransform(mat4fVP, iOperation, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantWVPMatrix(int iOperation, float* afData)
{
    SEMatrix4f mat4fWVP = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
    GetTransform(mat4fWVP, iOperation, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorMatrix(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    // 为投影器建立view matrix.
    const SEVector3f& rEye = m_pProjector->GetLocation();
    const SEVector3f& rRVector = m_pProjector->GetRVector();
    const SEVector3f& rUVector = m_pProjector->GetUVector();
    const SEVector3f& rDVector = m_pProjector->GetDVector();
    float fRdE = rRVector.Dot(rEye);
    float fUdE = rUVector.Dot(rEye);
    float fDdE = rDVector.Dot(rEye);
    SEMatrix4f mat4fProjVMatrix(
        rRVector[0], rUVector[0], rDVector[0], 0.0f,
        rRVector[1], rUVector[1], rDVector[1], 0.0f,
        rRVector[2], rUVector[2], rDVector[2], 0.0f,
        -fRdE,        -fUdE,        -fDdE,     1.0f);

    // 为投影器建立projection matrix.
    float fRMin, fRMax, fUMin, fUMax, fDMin, fDMax;
    m_pProjector->GetFrustum(fRMin, fRMax, fUMin, fUMax, fDMin, fDMax);
    float fInvRDiff = 1.0f / (fRMax - fRMin);
    float fInvUDiff = 1.0f / (fUMax - fUMin);
    float fInvDDiff = 1.0f / (fDMax - fDMin);
    float fRTerm0 = fDMin * fInvRDiff;
    float fUTerm0 = fDMin * fInvUDiff;
    float fDTerm0 = fDMin * fInvDDiff;
    float fRTerm1 = -(fRMin + fRMax) * fInvRDiff;
    float fUTerm1 = -(fUMin + fUMax) * fInvUDiff;
    float fDTerm1 = fDMax * fInvDDiff;
    SEMatrix4f mat4fProjPMatrix(
        2.0f*fRTerm0, 0.0f,         0.0f,           0.0f,
        0.0f,         2.0f*fUTerm0, 0.0f,           0.0f,
        fRTerm1,      fUTerm1,      fDTerm1,        1.0f,
        0.0f,         0.0f,         -fDMax*fDTerm0, 0.0f);

    // 为投影器建立bias和scale matrix.
    SEMatrix4f mat4fProjBSMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    // 最终投影器matrix.
    SEMatrix4f mat4fProjectorMatrix =
        m_WorldMatrix * mat4fProjVMatrix * mat4fProjPMatrix * 
        mat4fProjBSMatrix;

    GetTransform(mat4fProjectorMatrix, 0, afData);
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantMaterialEmissive(int, float* afData)
{
    SEMaterialState* pMaterial = StaticCast<SEMaterialState>(
        m_aspState[SEGlobalState::MATERIAL]);
    afData[0] = pMaterial->Emissive.R;
    afData[1] = pMaterial->Emissive.G;
    afData[2] = pMaterial->Emissive.B;
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantMaterialAmbient(int, float* afData)
{
    SEMaterialState* pMaterial = StaticCast<SEMaterialState>(
        m_aspState[SEGlobalState::MATERIAL]);
    afData[0] = pMaterial->Ambient.R;
    afData[1] = pMaterial->Ambient.G;
    afData[2] = pMaterial->Ambient.B;
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantMaterialDiffuse(int, float* afData)
{
    SEMaterialState* pMaterial = StaticCast<SEMaterialState>(
        m_aspState[SEGlobalState::MATERIAL]);
    afData[0] = pMaterial->Diffuse.R;
    afData[1] = pMaterial->Diffuse.G;
    afData[2] = pMaterial->Diffuse.B;
    afData[3] = pMaterial->Alpha;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantMaterialSpecular(int, float* afData)
{
    SEMaterialState* pMaterial = StaticCast<SEMaterialState>(
        m_aspState[SEGlobalState::MATERIAL]);
    afData[0] = pMaterial->Specular.R;
    afData[1] = pMaterial->Specular.G;
    afData[2] = pMaterial->Specular.B;
    afData[3] = pMaterial->Shininess;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraModelPosition(int, float* afData)
{
    SEVector3f vec3fMLocation;
    m_pGeometry->World.ApplyInverse(m_pCamera->GetLocation(), vec3fMLocation);

    afData[0] = vec3fMLocation.X;
    afData[1] = vec3fMLocation.Y;
    afData[2] = vec3fMLocation.Z;
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraModelRight(int, float* afData)
{
    SEVector3f vec3fMRVector;
    m_pGeometry->World.InvertVector(m_pCamera->GetRVector(), vec3fMRVector);
    vec3fMRVector.Normalize();

    afData[0] = vec3fMRVector.X;
    afData[1] = vec3fMRVector.Y;
    afData[2] = vec3fMRVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraModelUp(int, float* afData)
{
    SEVector3f vec3fMUVector;
    m_pGeometry->World.InvertVector(m_pCamera->GetUVector(), vec3fMUVector);
    vec3fMUVector.Normalize();

    afData[0] = vec3fMUVector.X;
    afData[1] = vec3fMUVector.Y;
    afData[2] = vec3fMUVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraModelDirection(int, float* afData)
{
    SEVector3f vec3fMDVector;
    m_pGeometry->World.InvertVector(m_pCamera->GetDVector(), vec3fMDVector);
    vec3fMDVector.Normalize();

    afData[0] = vec3fMDVector.X;
    afData[1] = vec3fMDVector.Y;
    afData[2] = vec3fMDVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraWorldPosition(int, float* afData)
{
    SEVector3f vec3fWLocation = m_pCamera->GetLocation();

    afData[0] = vec3fWLocation.X;
    afData[1] = vec3fWLocation.Y;
    afData[2] = vec3fWLocation.Z;
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraWorldRight(int, float* afData)
{
    SEVector3f vec3fWRVector = m_pCamera->GetRVector();

    afData[0] = vec3fWRVector.X;
    afData[1] = vec3fWRVector.Y;
    afData[2] = vec3fWRVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraWorldUp(int, float* afData)
{
    SEVector3f vec3fWUVector = m_pCamera->GetUVector();

    afData[0] = vec3fWUVector.X;
    afData[1] = vec3fWUVector.Y;
    afData[2] = vec3fWUVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantCameraWorldDirection(int, float* afData)
{
    SEVector3f vec3fWDVector = m_pCamera->GetDVector();

    afData[0] = vec3fWDVector.X;
    afData[1] = vec3fWDVector.Y;
    afData[2] = vec3fWDVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorModelPosition(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fMLocation;
    m_pGeometry->World.ApplyInverse(m_pProjector->GetLocation(), 
        vec3fMLocation);

    afData[0] = vec3fMLocation.X;
    afData[1] = vec3fMLocation.Y;
    afData[2] = vec3fMLocation.Z;
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorModelRight(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fMRVector;
    m_pGeometry->World.InvertVector(m_pProjector->GetRVector(), vec3fMRVector);
    vec3fMRVector.Normalize();

    afData[0] = vec3fMRVector.X;
    afData[1] = vec3fMRVector.Y;
    afData[2] = vec3fMRVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorModelUp(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fMUVector;
    m_pGeometry->World.InvertVector(m_pProjector->GetUVector(), vec3fMUVector);
    vec3fMUVector.Normalize();

    afData[0] = vec3fMUVector.X;
    afData[1] = vec3fMUVector.Y;
    afData[2] = vec3fMUVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorModelDirection(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fMDVector;
    m_pGeometry->World.InvertVector(m_pProjector->GetDVector(), vec3fMDVector);
    vec3fMDVector.Normalize();

    afData[0] = vec3fMDVector.X;
    afData[1] = vec3fMDVector.Y;
    afData[2] = vec3fMDVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorWorldPosition(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fWLocation = m_pProjector->GetLocation();

    afData[0] = vec3fWLocation.X;
    afData[1] = vec3fWLocation.Y;
    afData[2] = vec3fWLocation.Z;
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorWorldRight(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fWRVector = m_pProjector->GetRVector();

    afData[0] = vec3fWRVector.X;
    afData[1] = vec3fWRVector.Y;
    afData[2] = vec3fWRVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorWorldUp(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fWUVector = m_pProjector->GetUVector();

    afData[0] = vec3fWUVector.X;
    afData[1] = vec3fWUVector.Y;
    afData[2] = vec3fWUVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantProjectorWorldDirection(int, float* afData)
{
    SE_ASSERT( m_pProjector );

    SEVector3f vec3fWDVector = m_pProjector->GetDVector();

    afData[0] = vec3fWDVector.X;
    afData[1] = vec3fWDVector.Y;
    afData[2] = vec3fWDVector.Z;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightModelPosition(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        SEVector3f vec3fMPosition;
        m_pGeometry->World.ApplyInverse(pLight->Position, vec3fMPosition);

        afData[0] = vec3fMPosition[0];
        afData[1] = vec3fMPosition[1];
        afData[2] = vec3fMPosition[2];
        afData[3] = (int)pLight->Type < (int)SELight::LT_POINT ? 0.0f : 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightModelDirection(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        SEVector3f vec3fMDVector;
        m_pGeometry->World.InvertVector(pLight->DVector, vec3fMDVector);
        vec3fMDVector.Normalize();

        afData[0] = vec3fMDVector[0];
        afData[1] = vec3fMDVector[1];
        afData[2] = vec3fMDVector[2];
        afData[3] = 0.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightWorldPosition(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->Position[0];
        afData[1] = pLight->Position[1];
        afData[2] = pLight->Position[2];
        afData[3] = (int)pLight->Type < (int)SELight::LT_POINT ? 0.0f : 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightWorldDirection(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->DVector[0];
        afData[1] = pLight->DVector[1];
        afData[2] = pLight->DVector[2];
        afData[3] = 0.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightAmbient(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->Ambient.R;
        afData[1] = pLight->Ambient.G;
        afData[2] = pLight->Ambient.B;
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
 }
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightDiffuse(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->Diffuse.R;
        afData[1] = pLight->Diffuse.G;
        afData[2] = pLight->Diffuse.B;
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightSpecular(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->Specular.R;
        afData[1] = pLight->Specular.G;
        afData[2] = pLight->Specular.B;
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightSpotCutoff(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->Angle;
        afData[1] = pLight->CosAngle;
        afData[2] = pLight->SinAngle;
        afData[3] = pLight->Exponent;
    }
    else
    {
        afData[0] = SEMathf::PI;
        afData[1] = -1.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightAttenuation(int iLight, float* afData)
{
    SELight* pLight = GetLight(iLight);
    if( pLight )
    {
        afData[0] = pLight->Constant;
        afData[1] = pLight->Linear;
        afData[2] = pLight->Quadratic;
        afData[3] = pLight->Intensity;
    }
    else
    {
        afData[0] = 1.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SERenderer::SetConstantLightCount(int, float* afData)
{
    afData[0] = (float)m_iNumActiveLights;
}
//----------------------------------------------------------------------------
void SERenderer::SetRendererConstant(SERendererConstant::Type eRCType,
    float* afData)
{
    int iRCType = (int)eRCType;
    int iFunction;

    if( iRCType <= (int)SERendererConstant::WVP_MATRIX_INVERSE_TRANSPOSE )
    {
        // 常量与矩阵数据有关.
        // 这些数据以6个为一组:
        // matrix, transpose, inverse, inverse-transpose.
        int iOperation = iRCType / 6;
        iFunction = iRCType % 6;
        (this->*ms_aoSCFunction[iFunction])(iOperation, afData);
    }
    else if( iRCType <= (int)SERendererConstant::PROJECTOR_MATRIX )
    {
        // 常量与material,camera,projector数据有关.
        // 第一参数iOperation被忽略掉,因此设置为0.
        // iFunction中的"6"用于索引ms_aoSCFunction中的
        // SetConstantMaterialEmissive.
        // 其他的iFunction是一个相对偏移量,用于定位material,fog,camera,
        // projector等的SetConstant*函数.iFunction最大值为26.
        iFunction = 6 + iRCType - (int)SERendererConstant::MATERIAL_EMISSIVE;
        (this->*ms_aoSCFunction[iFunction])(0, afData);
    }
    else if( iRCType < (int)SERendererConstant::LIGHT_COUNT )
    {
        // 常量与light有关.
        // 这些数据以9个为一组:
        // model position, model direction, world position, world direction,
        // ambient, diffuse, specular, spotcutoff, shininess.
        // iFunction中的"27"用于索引ms_aoSCFunction中的
        // SetConstantLightModelPosition.
        // 其他的iFunction是一个相对偏移量,用于定位light的SetConstant*函数.
        // iFunction最大值为35.
        // 从LIGHT_ARRAY0_MODEL_POSITION开始,
        // 是light0到light7对应的shader interface数组实例中的renderer 
        // constant.
        int iDiff = iRCType - (int)SERendererConstant::LIGHT0_MODEL_POSITION;
        int iLight = iDiff / 9;
        iLight -= (iLight >= 8 ? 8 : 0);
        int iAttribute = iDiff % 9;
        iFunction = 27 + iAttribute;
        (this->*ms_aoSCFunction[iFunction])(iLight, afData);
    }
    else
    {
        // 常量是light count.
        iFunction = 36;
        (this->*ms_aoSCFunction[iFunction])(0, afData);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 装载与释放资源
//----------------------------------------------------------------------------
void SERenderer::LoadAllResources(SESpatial* pScene)
{
    SEGeometry* pGeometry = DynamicCast<SEGeometry>(pScene);
    if( pGeometry )
    {
        LoadResources(pGeometry);
    }

    SENode* pNode = DynamicCast<SENode>(pScene);
    if( pNode )
    {
        for( int i = 0; i < pNode->GetCount(); i++ )
        {
            SESpatial* pChild = pNode->GetChild(i);
            if( pChild )
            {
                LoadAllResources(pChild);
            }
        }
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseAllResources(SESpatial* pScene)
{
    SEGeometry* pGeometry = DynamicCast<SEGeometry>(pScene);
    if( pGeometry )
    {
        ReleaseResources(pGeometry);
    }

    SENode* pNode = DynamicCast<SENode>(pScene);
    if( pNode )
    {
        for( int i = 0; i < pNode->GetCount(); i++ )
        {
            SESpatial* pChild = pNode->GetChild(i);
            if( pChild )
            {
                ReleaseAllResources(pChild);
            }
        }
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadResources(SEGeometry* pGeometry)
{
    SE_ASSERT( pGeometry );
    SE_ASSERT( pGeometry->VBuffer );
    SE_ASSERT( pGeometry->IBuffer );

    // 把index buffer装入显存.
    LoadIBuffer(pGeometry->IBuffer);

    // 把vertex buffer(s)和effects装入显存.
    const int iEffectCount = pGeometry->GetEffectCount();
    for( int i = 0; i < iEffectCount; i++ )
    {
        SEEffect* pEffect = pGeometry->GetEffect(i);
        pEffect->LoadResources(this, pGeometry);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseResources(SEGeometry* pGeometry)
{
    SE_ASSERT( pGeometry );
    SE_ASSERT( pGeometry->VBuffer );
    SE_ASSERT( pGeometry->IBuffer );

    // 从显存释放index buffer.
    ReleaseIBuffer(pGeometry->IBuffer);

    // 从显存释放vertex buffer(s).
    while( pGeometry->VBuffer->GetInfoCount() > 0 )
    {
        ReleaseVBuffer(pGeometry->VBuffer);
    }

    // 从显存释放effects.
    const int iEffectCount = pGeometry->GetEffectCount();
    for( int i = 0; i < iEffectCount; i++ )
    {
        SEEffect* pEffect = pGeometry->GetEffect(i);
        pEffect->ReleaseResources(this, pGeometry);
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadResources(SEEffect* pEffect)
{
    SE_ASSERT( pEffect );

    pEffect->LoadResources(this, 0);
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseResources(SEEffect* pEffect)
{
    SE_ASSERT( pEffect );

    pEffect->ReleaseResources(this, 0);
}
//----------------------------------------------------------------------------
void SERenderer::LoadVProgram(SEVertexProgram* pVProgram)
{
    if( !pVProgram )
    {
        return;
    }

    SEResourceIdentifier* pID = pVProgram->GetIdentifier(this);
    if( !pID )
    {
        OnLoadVProgram(pID, pVProgram);
        pVProgram->OnLoad(this, &SERenderer::ReleaseVProgram, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseVProgram(SEBindable* pVProgram)
{
    if( !pVProgram )
    {
        return;
    }

    SEResourceIdentifier* pID = pVProgram->GetIdentifier(this);
    if( pID )
    {
        OnReleaseVProgram(pID);
        pVProgram->OnRelease(this, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadGProgram(SEGeometryProgram* pGProgram)
{
    if( !pGProgram )
    {
        return;
    }

    SEResourceIdentifier* pID = pGProgram->GetIdentifier(this);
    if( !pID )
    {
        OnLoadGProgram(pID, pGProgram);
        pGProgram->OnLoad(this, &SERenderer::ReleaseGProgram, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseGProgram(SEBindable* pGProgram)
{
    if( !pGProgram )
    {
        return;
    }

    SEResourceIdentifier* pID = pGProgram->GetIdentifier(this);
    if( pID )
    {
        OnReleaseGProgram(pID);
        pGProgram->OnRelease(this, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadPProgram(SEPixelProgram* pPProgram)
{
    if( !pPProgram )
    {
        return;
    }

    SEResourceIdentifier* pID = pPProgram->GetIdentifier(this);
    if( !pID )
    {
        OnLoadPProgram(pID, pPProgram);
        pPProgram->OnLoad(this, &SERenderer::ReleasePProgram, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleasePProgram(SEBindable* pPProgram)
{
    if( !pPProgram )
    {
        return;
    }

    SEResourceIdentifier* pID = pPProgram->GetIdentifier(this);
    if( pID )
    {
        OnReleasePProgram(pID);
        pPProgram->OnRelease(this, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadTexture(SETexture* pTexture)
{
    if( !pTexture )
    {
        return;
    }

    SEResourceIdentifier* pID = pTexture->GetIdentifier(this);
    if( !pID )
    {
        OnLoadTexture(pID, pTexture);
        pTexture->OnLoad(this, &SERenderer::ReleaseTexture, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseTexture(SEBindable* pTexture)
{
    if( !pTexture )
    {
        return;
    }

    SEResourceIdentifier* pID = pTexture->GetIdentifier(this);
    if( pID )
    {
        OnReleaseTexture(pID);
        pTexture->OnRelease(this, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadVBuffer(const SEAttributes& rIAttributes,
    const SEAttributes& rOAttributes, SEVertexBuffer* pVBuffer,
    SEVertexProgram* pVProgram)
{
    if( !pVBuffer )
    {
        return;
    }

    // 试图在之前passes使用过的VB显存实例中查找匹配当前需求的VB子集.
    SEResourceIdentifier* pID = 0;
    for( int i = 0; i < pVBuffer->GetInfoCount(); i++ )
    {
        pID = pVBuffer->GetIdentifier(i, this);
        if( pID )
        {
            if( rIAttributes == *(SEAttributes*)pID )
            {
                // 如果找到则不需重新装载该VB子集.
                return;
            }
        }
    }

    // 第一次遇到该VB子集,
    // 待实现:当前只支持3-tuples顶点和法线.
    const SEAttributes& rVBAttributes = pVBuffer->GetAttributes();
    SE_ASSERT( rIAttributes.GetPositionChannels() == 3 && 
        rVBAttributes.GetPositionChannels() == 3 );

    if( rIAttributes.HasNormal() )
    {
        SE_ASSERT( rIAttributes.GetNormalChannels() == 3 && 
            rVBAttributes.GetNormalChannels() == 3 );
    }
    (void)rVBAttributes;  // 避免Release版本编译器警告

    OnLoadVBuffer(pID, rIAttributes, rOAttributes, pVBuffer, pVProgram);
    pVBuffer->OnLoad(this, &SERenderer::ReleaseVBuffer, pID);
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseVBuffer(SEBindable* pVBuffer)
{
    if( !pVBuffer )
    {
        return;
    }

    for( int i = 0; i < pVBuffer->GetInfoCount(); i++ )
    {
        SEResourceIdentifier* pID = pVBuffer->GetIdentifier(i, this);
        if( pID )
        {
            OnReleaseVBuffer(pID);
            pVBuffer->OnRelease(this, pID);

            return;
        }
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadIBuffer(SEIndexBuffer* pIBuffer)
{
    if( !pIBuffer )
    {
        return;
    }

    SEResourceIdentifier* pID = pIBuffer->GetIdentifier(this);
    if( !pID )
    {
        OnLoadIBuffer(pID, pIBuffer);
        pIBuffer->OnLoad(this, &SERenderer::ReleaseIBuffer, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseIBuffer(SEBindable* pIBuffer)
{
    if( !pIBuffer )
    {
        return;
    }

    SEResourceIdentifier* pID = pIBuffer->GetIdentifier(this);
    if( pID )
    {
        OnReleaseIBuffer(pID);
        pIBuffer->OnRelease(this, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::LoadRenderStateBlock(SERenderStateBlock* pRStateBlock)
{
    if( !pRStateBlock )
    {
        return;
    }

    SEResourceIdentifier* pID = pRStateBlock->GetIdentifier(this);
    if( !pID )
    {
        OnLoadRenderStateBlock(pID, pRStateBlock);
        pRStateBlock->OnLoad(this, &SERenderer::ReleaseRenderStateBlock, pID);
    }
}
//----------------------------------------------------------------------------
void SERenderer::ReleaseRenderStateBlock(SEBindable* pRStateBlock)
{
    if( !pRStateBlock )
    {
        return;
    }

    SEResourceIdentifier* pID = pRStateBlock->GetIdentifier(this);
    if( pID )
    {
        OnReleaseRenderStateBlock(pID);
        pRStateBlock->OnRelease(this, pID);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 资源开启与关闭
//----------------------------------------------------------------------------
void SERenderer::EnableVProgram(SEVertexProgram* pVProgram)
{
    SE_ASSERT( pVProgram );

    // Ensure that the shader program is loaded to VRAM.
    LoadVProgram(pVProgram);
    SEResourceIdentifier* pID = pVProgram->GetIdentifier(this);
    SE_ASSERT( pID );

    OnEnableVProgram(pID);

    // Process the sampler information.
    int i, iCount;
    iCount = pVProgram->GetSICount();
    for( i = 0; i < iCount; i++ )
    {
        SE_ASSERT( m_iNumActiveSamplers < m_iMaxActiveSamplerCount );
        m_apActiveSamplers[m_iNumActiveSamplers++] =
            pVProgram->GetSI(i);
    }

    // Process the renderer constants.
    iCount = pVProgram->GetRCCount();
    for( i = 0; i < iCount; i++ )
    {
        SERendererConstant* pRC = pVProgram->GetRC(i);
        SE_ASSERT(pRC);

        // Set pipeline's T&L date to system RAM.
        SetRendererConstant(pRC->GetType(), pRC->GetData());

        // Set the data to VRAM.
        SetVProgramRC(pRC);
    }

    // Process the user-defined constants.
    iCount = pVProgram->GetUCCount();
    for( i = 0; i < iCount; i++ )
    {
        SEUserConstant* pUC = pVProgram->GetUC(i);
        SE_ASSERT( pUC );

        // Set the data to VRAM.
        if( pUC->Active )
        {
            SetVProgramUC(pUC);
        }
    }

    UpdateVProgramConstants(pVProgram);
}
//----------------------------------------------------------------------------
void SERenderer::DisableVProgram(SEVertexProgram* pVProgram)
{
    SE_ASSERT( pVProgram );

    SEResourceIdentifier* pID = pVProgram->GetIdentifier(this);
    SE_ASSERT( pID );

    OnDisableVProgram(pID);
}
//----------------------------------------------------------------------------
void SERenderer::EnableGProgram(SEGeometryProgram* pGProgram)
{
    SE_ASSERT( pGProgram );

    // Ensure that the shader program is loaded to VRAM.
    LoadGProgram(pGProgram);
    SEResourceIdentifier* pID = pGProgram->GetIdentifier(this);
    SE_ASSERT( pID );

    OnEnableGProgram(pID);

    // Process the sampler information.
    int i, iCount;
    iCount = pGProgram->GetSICount();
    for( i = 0; i < iCount; i++ )
    {
        SE_ASSERT( m_iNumActiveSamplers < m_iMaxActiveSamplerCount );
        m_apActiveSamplers[m_iNumActiveSamplers++] =
            pGProgram->GetSI(i);
    }

    // Process the renderer constants.
    iCount = pGProgram->GetRCCount();
    for( i = 0; i < iCount; i++ )
    {
        SERendererConstant* pRC = pGProgram->GetRC(i);
        SE_ASSERT(pRC);

        // Set pipeline's T&L date to system RAM.
        SetRendererConstant(pRC->GetType(), pRC->GetData());

        // Set the data to VRAM.
        SetGProgramRC(pRC);
    }

    // Process the user-defined constants.
    iCount = pGProgram->GetUCCount();
    for( i = 0; i < iCount; i++ )
    {
        SEUserConstant* pUC = pGProgram->GetUC(i);
        SE_ASSERT( pUC );

        // Set the data to VRAM.
        if( pUC->Active )
        {
            SetGProgramUC(pUC);
        }
    }

    UpdateGProgramConstants(pGProgram);
}
//----------------------------------------------------------------------------
void SERenderer::DisableGProgram(SEGeometryProgram* pGProgram)
{
    SE_ASSERT( pGProgram );

    SEResourceIdentifier* pID = pGProgram->GetIdentifier(this);
    SE_ASSERT( pID );

    OnDisableGProgram(pID);
}
//----------------------------------------------------------------------------
void SERenderer::EnablePProgram(SEPixelProgram* pPProgram)
{
    SE_ASSERT( pPProgram );

    // Ensure that the shader program is loaded to VRAM.
    LoadPProgram(pPProgram);
    SEResourceIdentifier* pID = pPProgram->GetIdentifier(this);
    SE_ASSERT( pID );

    OnEnablePProgram(pID);

    // Process the sampler information.
    int i, iCount;
    iCount = pPProgram->GetSICount();
    for( i = 0; i < iCount; i++ )
    {
        SE_ASSERT( m_iNumActiveSamplers < m_iMaxActiveSamplerCount );
        m_apActiveSamplers[m_iNumActiveSamplers++] =
            pPProgram->GetSI(i);
    }

    // Process the renderer constants.
    iCount = pPProgram->GetRCCount();
    for( i = 0; i < iCount; i++ )
    {
        SERendererConstant* pRC = pPProgram->GetRC(i);
        SE_ASSERT( pRC );

        // Set pipeline's T&L date to system RAM.
        SetRendererConstant(pRC->GetType(), pRC->GetData());

        // Set the data to VRAM.
        SetPProgramRC(pRC);
    }

    // Process the user-defined constants.
    iCount = pPProgram->GetUCCount();
    for( i = 0; i < iCount; i++ )
    {
        SEUserConstant* pUC = pPProgram->GetUC(i);
        SE_ASSERT( pUC );

        // Set the data to VRAM.
        if( pUC->Active )
        {
            SetPProgramUC(pUC);
        }
    }

    UpdatePProgramConstants(pPProgram);
}
//----------------------------------------------------------------------------
void SERenderer::DisablePProgram(SEPixelProgram* pPProgram)
{
    SE_ASSERT( pPProgram );

    SEResourceIdentifier* pID = pPProgram->GetIdentifier(this);
    SE_ASSERT( pID );

    OnDisablePProgram(pID);
}
//----------------------------------------------------------------------------
void SERenderer::EnableTexture(SETexture* pTexture)
{
    SE_ASSERT( pTexture );

    // 确保texture装载入显存.
    LoadTexture(pTexture);
    SEResourceIdentifier* pID = pTexture->GetIdentifier(this);
    SE_ASSERT( pID );

    OnEnableTexture(pID);
}
//----------------------------------------------------------------------------
void SERenderer::DisableTexture(SETexture* pTexture)
{
    SEResourceIdentifier* pID = pTexture->GetIdentifier(this);
    SE_ASSERT( pID );

    OnDisableTexture(pID);
}
//----------------------------------------------------------------------------
SEResourceIdentifier* SERenderer::EnableVBuffer(const SEAttributes& 
    rIAttributes, const SEAttributes& rOAttributes, SEVertexProgram* 
    pVProgram)
{
    // 确保当前要渲染的geometry的VB子集装载入显存.
    // 由rIAttributes确定当前VB子集需要哪些数据.
    SEVertexBuffer* pVBuffer = m_pGeometry->VBuffer;
    LoadVBuffer(rIAttributes, rOAttributes, pVBuffer, pVProgram);

    // 待实现.
    // LoadVBuffer直接返回一个匹配当前需求的VB子集,避免下面的循环查找.

    SEResourceIdentifier* pID = 0;
    for( int i = 0; i < pVBuffer->GetInfoCount(); i++ )
    {
        pID = pVBuffer->GetIdentifier(i, this);
        if( pID )
        {
            if( rIAttributes == *(SEAttributes*)pID )
            {
                // 在显存中找到一个匹配当前需求的VB子集.
                break;
            }
        }
    }
    SE_ASSERT( pID );

    OnEnableVBuffer(pID, pVProgram);

    return pID;
}
//----------------------------------------------------------------------------
void SERenderer::DisableVBuffer(SEResourceIdentifier* pID, 
    SEVertexProgram* pVProgram)
{
    OnDisableVBuffer(pID, pVProgram);
}
//----------------------------------------------------------------------------
void SERenderer::EnableIBuffer()
{
    // 确保当前要渲染的geometry的IB装载入显存.
    SEIndexBuffer* pIBuffer = m_pGeometry->IBuffer;
    LoadIBuffer(pIBuffer);
    SEResourceIdentifier* pID = pIBuffer->GetIdentifier(this);
    SE_ASSERT( pID );

    OnEnableIBuffer(pID);
}
//----------------------------------------------------------------------------
void SERenderer::DisableIBuffer()
{
    SEIndexBuffer* pIBuffer = m_pGeometry->IBuffer;
    SEResourceIdentifier* pID = pIBuffer->GetIdentifier(this);
    SE_ASSERT( pID );

    OnDisableIBuffer(pID);
}
//----------------------------------------------------------------------------
void SERenderer::EnableRenderStateBlock(SERenderStateBlock* pRStateBlock)
{
    SE_ASSERT( pRStateBlock );

    // 确保render state block装载入设备.
    LoadRenderStateBlock(pRStateBlock);
    SEResourceIdentifier* pID = pRStateBlock->GetIdentifier(this);
    SE_ASSERT( pID );

    OnEnableRenderStateBlock(pID);
}
//----------------------------------------------------------------------------
void SERenderer::DisableRenderStateBlock(SERenderStateBlock* pRStateBlock)
{
    SEResourceIdentifier* pID = pRStateBlock->GetIdentifier(this);
    SE_ASSERT( pID );

    OnDisableRenderStateBlock(pID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 对象渲染入口
//----------------------------------------------------------------------------
void SERenderer::DrawScene(SEUnculledSet& rVisibleSet)
{
    // 注意: 2-tuples堆栈最大元素数被限制为64.
    // 理论上应该足够了,因为如果在同一个树型路径上收集到64个以上的global 
    // effect将会带来过多的渲染工作量.如果应用程序真有这种需求,则修改
    // iMaxTuples.
    const int iMaxTuples = 64;    // 堆栈元素最大允许值
    int aaiStack[iMaxTuples][2];  // 堆栈元素(startIndex,finalIndex)
    int iTop = -1;                // 堆栈初始为空

    const int iVisibleCount = rVisibleSet.GetCount();
    SEUnculledObject* pVisibleSet = rVisibleSet.GetUnculled();
    for( int i = 0; i < iVisibleCount; i++ )
    {
        if( pVisibleSet[i].Object )
        {
            if( pVisibleSet[i].GlobalEffect )
            {
                // 遇到一个global effect作用域的开始标志.
                // 该作用域压入堆栈.
                iTop++;
                SE_ASSERT( iTop < iMaxTuples );

                // 更新该作用域起始作用范围索引,结束作用范围索引.
                aaiStack[iTop][0] = i;
                aaiStack[iTop][1] = i;
            }
            else
            {
                // 遇到一个geometry叶子节点对象.
                if( iTop == -1 )
                {
                    // 没有处在global effect作用域中,则直接渲染.
                    Draw((SEGeometry*)pVisibleSet[i].Object);
                }
                else
                {
                    // 处在某global effect作用域中,则更新该作用域作用范围.
                    aaiStack[iTop][1]++;
                }
            }
        }
        else
        {
            // 遇到一个global effect作用域的结束标志.
            SE_ASSERT( !pVisibleSet[i].GlobalEffect );

            // 取出栈顶global effect作用域的起始作用范围索引,结束作用范围索引.
            int jMin = aaiStack[iTop][0];
            int jMax = aaiStack[iTop][1];

            // 用栈顶global effect渲染作用域范围内的所有可渲染节点对象.
            pVisibleSet[jMin].GlobalEffect->Draw(this, 
                pVisibleSet[jMin].Object, jMin+1, jMax, pVisibleSet);

            // 当前作用域弹出堆栈.
            if( --iTop >= 0 )
            {
                // 如果堆栈中还有待渲染作用域存在,则更新该作用域的结束作用范
                // 围索引.
                aaiStack[iTop][1] = jMax + 1;
            }
        }
    }
}
//----------------------------------------------------------------------------
void SERenderer::ApplyEffect(SEShaderEffect* pEffect, bool& rbPrimaryEffect)
{
    // 允许SEShaderEffect派生类自定义行为.
    pEffect->OnPreApplyEffect(this, rbPrimaryEffect);

    // 每个shader effect可以有多个pass.
    // 每个pass使用一对vertex/pixel程序进行一次针对当前geometry对象的渲染.
    const int iPassCount = pEffect->GetPassCount();
    for( int iPass = 0; iPass < iPassCount; iPass++ )
    {
        // 确保pass所需shader程序及其纹理资源已装载到系统内存.
        pEffect->LoadPrograms(iPass, this);

        // 允许SEShaderEffect派生类自定义行为.
        // global state必须在开启shader程序之前被设置,
        // 因为shader程序将设置纹理采样器的采样状态.
        pEffect->OnPreApplyPass(iPass, this, rbPrimaryEffect);

        // Keep track of the current number of active samplers.
        m_iNumActiveSamplers = 0;

        // 开启vertex shader程序,设置shader常量.
        SEVertexProgram* pVProgram = pEffect->GetVProgram(iPass);
        EnableVProgram(pVProgram);

        // 可选geometry shader stage.开启geometry shader程序,设置shader常量.
        int iGTCount = 0;
        SEGeometryShader* pGShader = pEffect->GetGShader(iPass);
        if( pGShader )
        {
            SEGeometryProgram* pGProgram = pEffect->GetGProgram(iPass);
            EnableGProgram(pGProgram);
        }

        // 开启pixel shader程序,设置shader常量.
        SEPixelProgram* pPProgram = pEffect->GetPProgram(iPass);
        EnablePProgram(pPProgram);

        // Keep track of the current sampler to be used in enabling the
        // textures.
        m_iCurrentSampler = 0;

        // 开启texture供vertex shader程序使用.
        int iVTCount = pEffect->GetVTextureCount(iPass);
        int iTexture;
        for( iTexture = 0; iTexture < iVTCount; iTexture++ )
        {
            EnableTexture(pEffect->GetVTexture(iPass, iTexture));
            m_iCurrentSampler++;
        }

        // 可选geometry shader stage.开启texture供geometry shader程序使用.
        if( pGShader )
        {
            iGTCount = pEffect->GetGTextureCount(iPass);
            for( iTexture = 0; iTexture < iGTCount; iTexture++ )
            {
                EnableTexture(pEffect->GetGTexture(iPass, iTexture));
                m_iCurrentSampler++;
            }
        }

        // 开启texture供pixel shader程序使用.
        int iPTCount = pEffect->GetPTextureCount(iPass);
        for( iTexture = 0; iTexture < iPTCount; iTexture++ )
        {
            EnableTexture(pEffect->GetPTexture(iPass, iTexture));
            m_iCurrentSampler++;
        }

        // 根据vertex shader程序的输入输出需求,新建或找到一个VB子集并开启.
        const SEAttributes& rIAttributes = pVProgram->GetInputAttributes();
        const SEAttributes& rOAttributes = pVProgram->GetOutputAttributes();
        SEResourceIdentifier* pID = EnableVBuffer(rIAttributes, rOAttributes,
            pVProgram);

        // 一切准备就绪,渲染由具体图形API负责实现.
        DrawElements();

        // 关闭vertex shader程序使用的VB子集.
        DisableVBuffer(pID, pVProgram);

        // Keep track of the current sampler to be used in disabling the
        // textures.
        m_iCurrentSampler = 0;

        // 关闭vertex shader程序使用的纹理.
        for( iTexture = 0; iTexture < iVTCount; iTexture++ )
        {
            DisableTexture(pEffect->GetVTexture(iPass, iTexture));
            m_iCurrentSampler++;
        }

        // 可选geometry shader stage.关闭geometry shader程序使用的纹理.
        if( pGShader )
        {
            for( iTexture = 0; iTexture < iGTCount; iTexture++ )
            {
                DisableTexture(pEffect->GetGTexture(iPass, iTexture));
                m_iCurrentSampler++;
            }
        }

        // 关闭pixel shader程序使用的纹理.
        for( iTexture = 0; iTexture < iPTCount; iTexture++ )
        {
            DisableTexture(pEffect->GetPTexture(iPass, iTexture));
            m_iCurrentSampler++;
        }

        // 关闭pixel shader程序.
        DisablePProgram(pPProgram);

        // 可选geometry shader stage.关闭geometry shader程序.
        if( pGShader )
        {
            DisableGProgram(pEffect->GetGProgram(iPass));
        }

        // 关闭vertex shader程序.
        DisableVProgram(pVProgram);

        // 允许SEShaderEffect派生类自定义行为.
        // 恢复当前pass之前的global state.
        pEffect->OnPostApplyPass(iPass, this, rbPrimaryEffect);
    }

    // 允许SEShaderEffect派生类自定义行为.
    pEffect->OnPostApplyEffect(this, rbPrimaryEffect);

    // 首个effect渲染之后,color buffer上已经存在有效颜色值.
    // 之后的effect渲染都将使用alpha混合.
    rbPrimaryEffect = false;
}
//----------------------------------------------------------------------------
void SERenderer::Draw(SEGeometry* pGeometry)
{
    SE_ASSERT( pGeometry );

    // 引用当前可渲染对象,供其它函数使用.
    m_pGeometry = pGeometry;

    // per-geometry pre-draw,渲染器派生类自定义行为入口.
    OnPreDrawGeometry();

    // 更新世界变换矩阵.
    SetWorldTransformation();

    // 更新灯光数组.
    SetLights();

    // 开启当前可渲染对象的IB.
    // 所有应用于当前可渲染对象的effect和每个effect的所有pass都将使用该IB.
    EnableIBuffer();

    // 默认情况下,首先应用光照effect,然后应用其他effect.
    bool bPrimaryEffect = true;
    const int iMin = m_pGeometry->GetStartEffect();
    const int iMax = m_pGeometry->GetEffectCount();
    for( int i = iMin; i < iMax; i++ )
    {
        // effect必须是shader effect.
        // DrawScene函数把受到同一个global effect影响的对象收集在一起.
        // global effect的Draw函数应该注意到只能调用渲染器的Draw(SEGeometry*)
        // 函数, 不可调用渲染器的DrawScene函数,从而避免无限AB递归的产生.
        SEShaderEffect* pEffect = DynamicCast<SEShaderEffect>(
            m_pGeometry->GetEffect(i));
        SE_ASSERT( pEffect );

        ApplyEffect(pEffect, bPrimaryEffect);
    }

    // 关闭当前可渲染对象的IB.
    DisableIBuffer();

    // 目前只是用于清空灯光数组.
    RestoreLights();

    // 恢复之前的世界变换矩阵.
    RestoreWorldTransformation();

    // per-geometry post-draw,渲染器派生类自定义行为入口.
    OnPostDrawGeometry();

    // 渲染完毕,解除引用当前可渲染对象.
    m_pGeometry = 0;
}
//----------------------------------------------------------------------------