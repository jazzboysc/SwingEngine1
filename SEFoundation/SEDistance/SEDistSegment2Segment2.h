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

#ifndef Swing_DistSegment2Segment2_H
#define Swing_DistSegment2Segment2_H

#include "SEFoundationLIB.h"
#include "SEDistance.h"
#include "SESegment2.h"

namespace Swing
{

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20090119
//----------------------------------------------------------------------------
class SE_FOUNDATION_API SEDistSegment2Segment2f : public SEDistance<float, 
    SEVector2f>
{
public:
    SEDistSegment2Segment2f(const SESegment2f& rSegment0,
        const SESegment2f& rSegment1);

    // 对象访问.
    const SESegment2f& GetSegment0(void) const;
    const SESegment2f& GetSegment1(void) const;

    // static distance查询.
    virtual float Get(void);
    virtual float GetSquared(void);

    // 用于dynamic distance查询的convex function计算.
    virtual float Get(float fT, const SEVector2f& rVelocity0,
        const SEVector2f& rVelocity1);
    virtual float GetSquared(float fT, const SEVector2f& rVelocity0,
        const SEVector2f& rVelocity1);

private:
    const SESegment2f* m_pSegment0;
    const SESegment2f* m_pSegment1;
};

}

#endif
