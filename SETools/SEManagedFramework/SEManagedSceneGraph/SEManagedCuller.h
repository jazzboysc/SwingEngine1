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

#pragma once

#include "SEManagedCamera.h"
#include "SEINativeSpatial.h"

namespace Swing{ namespace Tools{ namespace ManagedFramework{

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20091229
//----------------------------------------------------------------------------
public ref class ManagedCuller sealed
{
public:
    ManagedCuller(void);
    ~ManagedCuller(void);

    // SECamera access.
    void SetCamera(ManagedCamera^ thCamera);
    ManagedCamera^ GetCamera(void);

    // Culling system entry point.
    void ComputeUnculledSet(INativeSpatial^ thSpatial);

internal:
    [CLSCompliant(false)]
    SECuller* GetNativeCuller(void);

private:
    SECuller* m_pCuller;
    ManagedCamera^ m_thCamera;
};

}}}