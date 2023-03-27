// CVector3D - a fast inline class

#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

class CVector3D
{
public:
	float m_x;
	float m_y;
	float m_z;

public:
	// Constructor
	CVector3D(float x, float y, float z)		{ Set(x, y, z); }
	CVector3D()									{ Set(0, 0, 0); }
	CVector3D(CVector3D &pt)					{ Set(pt); }

	// Destructor
	~CVector3D()								{ }

	// Convertion to 2D
	CVector vec2D()								{ return CVector(m_x, m_y); }

	// Getters & Setters
	float &X()									{ return m_x; }
	float &Y()									{ return m_y; }
	float &Z()									{ return m_z; }
	float GetX() const							{ return m_x; }
	float GetY() const							{ return m_y; }
	float GetZ() const							{ return m_z; }

	void SetX(float x)							{ m_x = x; }
	void SetY(float y)							{ m_y = y; }
	void SetZ(float z)							{ m_z = z; }
	CVector3D &Set(float x, float y, float z)	{ m_x = x; m_y = y; m_z = z; return(*this); }
	CVector3D &Set(CVector3D &pt)				{ m_x = pt.m_x; m_y = pt.m_y; m_z = pt.m_z; return(*this); }

	// Operations - Add, Subtract, Scale
	CVector3D &Add(CVector3D &pt)				{ m_x += pt.m_x; m_y += pt.m_y; m_z += pt.m_z; return *this; }
	CVector3D &Subtract(CVector3D &pt)			{ m_x -= pt.m_x; m_y -= pt.m_y; m_z -= pt.m_z; return *this; }
	CVector3D &Scale(CVector3D &pt)				{ m_x *= pt.m_x; m_y *= pt.m_y; m_z *= pt.m_z; return *this; }
	
	CVector3D &Scale(float scalar)				{ m_x *= scalar; m_y *= scalar; m_z *= scalar; return *this; }
	CVector3D &ScaleUp(float scalar)			{ m_x *= scalar; m_y *= scalar; m_z *= scalar; return *this; }
	CVector3D &ScaleDown(float scalar)			{ m_x /= scalar; m_y /= scalar; m_z /= scalar; return *this; }

	friend CVector3D AddVectors(CVector3D &pt1, CVector3D &pt2)			{ return CVector3D(pt1.m_x + pt2.m_x, pt1.m_y + pt2.m_y, pt1.m_z + pt2.m_z); }
	friend CVector3D SubtracCVector3Ds(CVector3D &pt1, CVector3D &pt2)	{ return CVector3D(pt1.m_x - pt2.m_x, pt1.m_y - pt2.m_y, pt1.m_z - pt2.m_z); }
	friend CVector3D ScaleVectors(CVector3D &pt1, CVector3D &pt2)		{ return CVector3D(pt1.m_x * pt2.m_x, pt1.m_y * pt2.m_y, pt1.m_z * pt2.m_z); }

	friend CVector3D ScaleVector(CVector3D &pt, float scalar)			{ return CVector3D(pt.m_x * scalar, pt.m_y * scalar, pt.m_z * scalar); }
	friend CVector3D ScaleVectorUp(CVector3D &pt, float scalar)			{ return CVector3D(pt.m_x * scalar, pt.m_y * scalar, pt.m_z * scalar); }
	friend CVector3D ScaleVectorDown(CVector3D &pt, float scalar)		{ return CVector3D(pt.m_x / scalar, pt.m_y / scalar, pt.m_z / scalar); }

	// Normalization
	CVector3D &Normalize()												{ float l = Length(); if (l > 0) ScaleDown(l); return *this; }
	friend CVector3D Normalize(CVector3D v)								{ CVector3D v1(v); v1.Normalize(); return v1; }
	CVector3D &Normalise()												{ float l = Length(); if (l > 0) ScaleDown(l); return *this; }
	friend CVector3D Normalise(CVector3D v)								{ CVector3D v1(v); v1.Normalize(); return v1; }

	// Dot & Cross Products
	friend float Dot(CVector3D &p, CVector3D &q)						{ return p.m_x * q.m_x + p.m_y * q.m_y + p.m_z * q.m_z; }

	// Cross Product
	friend CVector3D Cross(CVector3D &p, CVector3D &q)					{ return CVector3D(p.m_y * q.m_z - p.m_z * q.m_y, p.m_z * q.m_x - p.m_x * q.m_z, p.m_x * q.m_y - p.m_y * q.m_x); }

	// Reflection
	friend CVector3D Reflect(CVector3D &vec, CVector3D &normal)			{ return vec - 2 * Dot(vec, normal) * normal; }

	// Length and Distance
	float SqrLength()													{ return (float)(m_x * m_x + m_y * m_y + m_z * m_z); }
	float Length()														{ return (float)sqrt((float)(m_x * m_x + m_y * m_y + m_z * m_z)); }
	friend float Length(CVector3D &p)									{ return p.Length(); }
	float Distance(CVector3D &pt)										{ return Subtract(pt).Length(); }
	friend float Distance(CVector3D &p, CVector3D &q)					{ return p.Distance(q); }

	
	
	// Operators

	// unary
	CVector3D operator -()						{ return CVector3D(-m_x, -m_y, -m_z); } 
	CVector3D operator +()						{ return CVector3D(m_x, m_y, m_z); } 

	// binary
	friend CVector3D operator +(CVector3D &pt1, CVector3D &pt2)			{ return AddVectors(pt1, pt2); }
	friend CVector3D operator -(CVector3D &pt1, CVector3D &pt2)			{ return SubtracCVector3Ds(pt1, pt2); }
	friend CVector3D operator *(CVector3D &pt1, CVector3D &pt2)			{ return ScaleVectors(pt1, pt2); }

	friend CVector3D operator *(CVector3D &pt, float scalar)			{ return ScaleVector(pt, scalar); }
	friend CVector3D operator *(float scalar, CVector3D &pt)			{ return ScaleVector(pt, scalar); }
	friend CVector3D operator /(CVector3D &pt, float scalar)			{ return ScaleVectorDown(pt, scalar); }
	
	// assignment
	CVector3D &operator =(CVector3D &pt)		{ return Set(pt); }
	CVector3D &operator +=(CVector3D &pt)		{ return Add(pt); }
	CVector3D &operator -=(CVector3D &pt)		{ return Subtract(pt); }
	CVector3D &operator *=(CVector3D &pt)		{ return Scale(pt); }

	CVector3D &operator *=(float scalar)		{ return Scale(scalar); }
	CVector3D &operator /=(float scalar)		{ return ScaleDown(scalar); }

	// comparison
	bool operator ==(CVector3D pt)				{ return (m_x == pt.m_x && m_y == pt.m_y && m_z == pt.m_z); }
	bool operator !=(CVector3D pt)				{ return (m_x != pt.m_x || m_y != pt.m_y || m_z != pt.m_z); }
};	

#endif
