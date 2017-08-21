
#include "stdafx.h"
#include "dbgarrow.h"

using namespace graphic;


cDbgArrow::cDbgArrow()
{
}

cDbgArrow::~cDbgArrow()
{
}


bool cDbgArrow::Create(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1
	, const float size //= 1.f
)
{
	m_head.Create(renderer, size, size, p0
		, eVertexType::POSITION | eVertexType::DIFFUSE
		, cColor::WHITE );
	m_head.SetDirection(p0, p1, p1, size*2.f);

	m_body.Create(renderer, p0, p1, size * 0.5f, cColor::WHITE);
	return true;
}


void cDbgArrow::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	m_head.Render(renderer, tm);
	m_body.Render(renderer, tm);
	renderer.GetDevContext()->RSSetState(NULL);
}


void cDbgArrow::SetDirection(const Vector3 &p0, const Vector3 &p1
	, const float size //= 1.f
)
{
	m_head.SetDirection(p0, p1, p1, size*2.f);
	m_body.SetLine(p0, p1, size * 0.5f);
}