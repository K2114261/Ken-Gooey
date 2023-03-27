#include "stdafx.h"
#include "Splash.h"

CSplash::CSplash(CVector v, CColor color, float density, float vmax, int frames, long time)
{
	m_v = v;
	m_color = color;
	m_time = time;

	// create particles...
	m_n = (int)(density * vmax);
	m_particles = new PARTICLE[m_n];
	memset(m_particles, 0, m_n * sizeof(PARTICLE));

	// initialisation of the individual particles
	for (int i = 0; i < m_n; i++)
	{
		// calculate runaway velocity
		float phi = acos(2 * (float)rand()/(float)RAND_MAX - 1);
		if (phi < 180)
			phi = phi + 180;
		float lambda = (float)M_PI * 2.f * (float)rand()/(float)RAND_MAX;
		float vvar = 0.1f;
		float v = vmax * (1 - vvar + vvar * (float)rand()/(float)RAND_MAX);

		m_particles[i].vel = CVector3D(
			v * sin(phi) * cos(lambda),
			v * cos(phi),
			v * sin(phi) * sin(lambda));
	}

	// initialisation of the tail
	m_i = 0;

	// colour fading factor
	m_factor = 1.0;
	m_delta = m_factor / (float)frames;
}

CSplash::~CSplash()
{
	delete [] m_particles;
}

void CSplash::Update(Uint32 time)
{
	Uint32 deltaTime = time - m_time;
	m_time = time;

	if (IsDeleted()) return;

	// Fade all particles (decrease brightness)
	m_factor -= m_delta;
	if (m_factor <= 0)
	{
		m_factor = 0;
		Delete();
		return;
	}

	// update particles
	m_i = (m_i + 1) % PARTICLE::N;
	for (int i = 0; i < m_n; i++)
	{
		CVector3D vel = m_particles[i].vel;

		// apply air resistance
//		float v = Length(vel);
//		v -= v * v * 0.00001f;
//		vel = Normalise(vel) * v;

		// apply gravitation
		vel += (float)deltaTime * CVector3D(0.f, -0.05f, 0.f);

		// update velocity
		m_particles[i].vel = vel;

		// update position
		int prev_i = (m_i + PARTICLE::N - 1) % PARTICLE::N;
		m_particles[i].pos[m_i] = m_particles[i].pos[prev_i] + vel * (float)deltaTime / 500.f;
	}
}

void CSplash::Draw(CGraphics *g)
{
	if (IsDeleted()) return;

	// draw particles
	for (int i = 0; i < m_n; i++)
	{
		// draw the forehead
		m_color.SetA((Uint8)(255.f * sqrt(m_factor)));
		g->FillCircle(m_v + m_particles[i].pos[m_i].vec2D(), 1, m_color);

		// draw the tail
		int n = PARTICLE::N;
		for (int j = 1; j < n; j++)
		{
			m_color.SetA((Uint8)(255.f * sqrt(m_factor * j / n)));
			g->DrawLine(
				m_v + m_particles[i].pos[(j + m_i) % n].vec2D(),
				m_v + m_particles[i].pos[(j + 1 + m_i) % n].vec2D(),
				m_color);
		}
	}
}
