#pragma once

#include "Vector3D.h"

struct PARTICLE
{
	static const int N = 8;		// length of a "tail"
	CVector3D pos[N];			// position
	CVector3D vel;				// velocity
};

class CSplash
{
	Uint32 m_time;			// reference time

	CVector m_v;			// starting position

	int m_n;				// number of particles
	PARTICLE *m_particles;	// velocity - for each particle
	int m_i;				// current index to m_particles

	CColor m_color;			// colour
	float m_factor;			// fading factor;
	float m_delta;			// fading delta

	bool m_bDeleted;		// Deleted flag

public:
	CSplash(CVector v, CColor color, float density, float vmax, int frames, long time);
	virtual ~CSplash();

	void Delete() { m_bDeleted = true; }
	void UnDelete() { m_bDeleted = false; }
	bool IsDeleted() { return m_bDeleted; }

	void Update(Uint32 time);
	void Draw(CGraphics *g);
};

typedef std::list<CSplash*> CSplashList;
typedef std::vector<CSplash*> CSplashVector;
typedef std::list<CSplash*>::iterator CSplashIter;
