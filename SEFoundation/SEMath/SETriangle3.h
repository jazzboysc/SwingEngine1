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

#ifndef Swing_Triangle3_H
#define Swing_Triangle3_H

#include "SEFoundationLIB.h"
#include "SEVector3.h"

namespace Swing
{

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20080803
//----------------------------------------------------------------------------
class SE_FOUNDATION_API SETriangle3f
{
public:
    SETriangle3f(void);  // 未初始化
    SETriangle3f(const SEVector3f& rV0, const SEVector3f& rV1, const 
        SEVector3f& rV2);
    SETriangle3f(const SEVector3f aV[3]);

    // 三角形到点Q的距离.
    float GetDistance(const SEVector3f& rQ) const;

    SEVector3f V[3];
};

}

#endif
