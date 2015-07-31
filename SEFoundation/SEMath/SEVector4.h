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

#ifndef Swing_Vector4_H
#define Swing_Vector4_H

#include "SEFoundationLIB.h"
#include "SEMath.h"

namespace Swing
{

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20070512
//----------------------------------------------------------------------------
class SE_FOUNDATION_API SEVector4f
{
public:
    union
    {
        struct
        {
            float X;
            float Y;
            float Z;
            float W;
        };
        float m_fData[4];
    };

public:
    SEVector4f(void);
    SEVector4f(float fX, float fY, float fZ, float fW);
    SEVector4f(const SEVector4f& rVec);

	// ת��Ϊ����ָ��ռ�
    operator const float* (void) const;
    operator float* (void);
	// ȡ��iԪ����ʽ
    float operator [] (int i) const;
    float& operator [] (int i);

    SEVector4f& operator = (const SEVector4f& rVec);

    bool operator == (const SEVector4f& rVec) const;
    bool operator != (const SEVector4f& rVec) const;
    bool operator <  (const SEVector4f& rVec) const;
    bool operator <= (const SEVector4f& rVec) const;
    bool operator >  (const SEVector4f& rVec) const;
    bool operator >= (const SEVector4f& rVec) const;

    SEVector4f operator + (const SEVector4f& rRhsVec) const;
    SEVector4f operator - (const SEVector4f& rRhsVec) const;
    SEVector4f operator * (float fScalar) const;
    SEVector4f operator / (float fScalar) const;
    SEVector4f operator - (void) const;

    SEVector4f& operator += (const SEVector4f& rRhsVec);
    SEVector4f& operator += (float fScalar);
    SEVector4f& operator -= (const SEVector4f& rRhsVec);
    SEVector4f& operator -= (float fScalar);
    SEVector4f& operator *= (float fScalar);
    SEVector4f& operator /= (float fScalar);

    SE_FOUNDATION_API friend SEVector4f operator * (float fLhsScalar, 
        const SEVector4f& rRhsVec);
	
    // ����������
    inline float GetLength(void) const;
    // ����������ƽ��
    inline float GetSquaredLength(void) const;
    // ����
    inline float Dot(const SEVector4f& rRhsVec) const;
	// �淶��
	inline float Normalize(void);

    static const SEVector4f ZERO;
    static const SEVector4f UNIT_X;
    static const SEVector4f UNIT_Y;
    static const SEVector4f UNIT_Z;
    static const SEVector4f UNIT_W;

private:
    inline int CompareData(const SEVector4f& rVec) const;
};

//----------------------------------------------------------------------------
// Description:
// Author:Sun Che
// Date:20090515
//----------------------------------------------------------------------------
class SE_FOUNDATION_API SEVector4d
{
public:
    union
    {
        struct
        {
            double X;
            double Y;
            double Z;
            double W;
        };
        double m_dData[4];
    };

public:
    SEVector4d(void);
    SEVector4d(double dX, double dY, double dZ, double dW);
    SEVector4d(const SEVector4d& rVec);

	// ת��Ϊ����ָ��ռ�
    operator const double* (void) const;
    operator double* (void);
	// ȡ��iԪ����ʽ
    double operator [] (int i) const;
    double& operator [] (int i);

    SEVector4d& operator = (const SEVector4d& rVec);

    bool operator == (const SEVector4d& rVec) const;
    bool operator != (const SEVector4d& rVec) const;
    bool operator <  (const SEVector4d& rVec) const;
    bool operator <= (const SEVector4d& rVec) const;
    bool operator >  (const SEVector4d& rVec) const;
    bool operator >= (const SEVector4d& rVec) const;

    SEVector4d operator + (const SEVector4d& rRhsVec) const;
    SEVector4d operator - (const SEVector4d& rRhsVec) const;
    SEVector4d operator * (double dScalar) const;
    SEVector4d operator / (double dScalar) const;
    SEVector4d operator - (void) const;

    SEVector4d& operator += (const SEVector4d& rRhsVec);
    SEVector4d& operator += (double dScalar);
    SEVector4d& operator -= (const SEVector4d& rRhsVec);
    SEVector4d& operator -= (double dScalar);
    SEVector4d& operator *= (double dScalar);
    SEVector4d& operator /= (double dScalar);

    SE_FOUNDATION_API friend SEVector4d operator * (double dLhsScalar, 
        const SEVector4d& rRhsVec);
	
    // ����������
    inline double GetLength(void) const;
    // ����������ƽ��
    inline double GetSquaredLength(void) const;
    // ����
    inline double Dot(const SEVector4d& rRhsVec) const;
	// �淶��
	inline double Normalize(void);

    static const SEVector4d ZERO;
    static const SEVector4d UNIT_X;
    static const SEVector4d UNIT_Y;
    static const SEVector4d UNIT_Z;
    static const SEVector4d UNIT_W;

private:
    inline int CompareData(const SEVector4d& rVec) const;
};

typedef SE_ALIGN16 SEVector4f SEVector4fA16;
typedef SE_ALIGN16 SEVector4d SEVector4dA16;

#include "SEVector4.inl"

}

#endif