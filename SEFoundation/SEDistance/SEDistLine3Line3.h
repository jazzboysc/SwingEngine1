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

#ifndef Swing_DistLine3Line3_H
#define Swing_DistLine3Line3_H

#include "SEFoundationLIB.h"
#include "SEDistance.h"
#include "SELine3.h"

namespace Swing
{

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20090113
//----------------------------------------------------------------------------
class SE_FOUNDATION_API SEDistLine3Line3f : public SEDistance<float, 
    SEVector3f>
{
public:
    SEDistLine3Line3f(const SELine3f& rLine0, const SELine3f& rLine1);

    // 对象访问.
    const SELine3f& GetLine0(void) const;
    const SELine3f& GetLine1(void) const;

    // static distance查询.
    virtual float Get(void);
    virtual float GetSquared(void);

    // 用于dynamic distance查询的convex function计算.
    virtual float Get(float fT, const SEVector3f& rVelocity0,
        const SEVector3f& rVelocity1);
    virtual float GetSquared(float fT, const SEVector3f& rVelocity0,
        const SEVector3f& rVelocity1);

    // 最近点相关信息.
    float GetLine0Parameter(void) const;
    float GetLine1Parameter(void) const;

private:
    const SELine3f* m_pLine0;
    const SELine3f* m_pLine1;

    // 最近点相关信息.
    float m_fLine0Parameter;  // closest0 = line0.origin+param*line0.direction
    float m_fLine1Parameter;  // closest1 = line1.origin+param*line1.direction
};

}

#endif
