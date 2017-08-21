
#include "stdafx.h"
#include "bonenode.h"


using namespace graphic;


cBoneNode::cBoneNode(cRenderer &renderer, const int id, vector<Matrix44> &palette, const sRawBone &rawBone) :
	cNode(id, rawBone.name)
,	m_track(NULL)
,	m_mesh(NULL)
,	m_palette(palette)
{
	m_offset = rawBone.worldTm.Inverse();
	m_localTM = rawBone.localTm;

	//m_offset = rawBone.worldTm;
	//m_localTM = Matrix44::Identity;

	m_track = new cBlendTrack();

	// debug ��
	m_mesh = new cMesh(renderer, id, rawBone);

}

cBoneNode::~cBoneNode()
{
	SAFE_DELETE( m_track );
	SAFE_DELETE( m_mesh );
}


void cBoneNode::SetAnimation( const sRawAni &rawAni, 
	const int totalAniFrame,  //= 0;
	const bool isLoop, //bLoop=false
	const bool isBlend // true
	)
{
	m_track->PlayAnimation(m_localTM, rawAni, totalAniFrame, isLoop, isBlend);
}


// �ִϸ��̼� ����.
void cBoneNode::SetAnimation( const sRawBone &rawBone, const sRawAni &rawAni, 
	const int totalAniFrame,  //= 0;
	const bool isLoop, //bLoop=false
	const bool isBlend // true
	)
{
	m_track->PlayAnimation(rawBone.localTm, rawAni, totalAniFrame, isLoop, isBlend);
}


// ���ϸ��̼�.
bool cBoneNode::Update(const float deltaSeconds)
{
	if (m_id < 0)
	{ // �ֻ��� ��Ʈ ����� ���� �ִϸ��̼� ó���� ���� �ʴ´�.
		for each (auto p in m_children)
			p->Update(deltaSeconds);
		return true;
	}

	// �ִϸ��̼� ���� �ƴϸ� ����.
	if (!m_track || (m_track && !m_track->IsAnimation()))
		return true;

	// �ִϸ��̼� ��� ���.
	if (m_track->Move(deltaSeconds, m_TM, m_aniTM, m_accTM))
	{
		if (m_parent)
			m_accTM = m_accTM * ((cBoneNode*)m_parent)->m_accTM;
		m_palette[ m_id] = m_offset * m_accTM;
	}

	// �ڽ� ��� �ִϸ��̼� ó��.
	for each (auto p in m_children)
		p->Update(deltaSeconds);

	return true;
}


// �� ��� ���
void cBoneNode::Render(cRenderer &renderer, const Matrix44 &tm)
{
	RET(!m_mesh);

	// �� ��� �޽��� world ��ǥ��� �Ǿ��ֱ� ������ �׳� �׸��� �ȴ�.
	if (m_track)
		m_mesh->Render(renderer, m_offset * m_accTM * tm);
	else
		m_mesh->Render(renderer, tm);

	for each (auto p in m_children)
		p->Render( renderer, tm);
}


void cBoneNode::SetCurrentFrame(const int curFrame)
{ 
	RET(!m_track);

	m_track->SetCurrentFrame(curFrame, m_TM, m_aniTM, m_accTM);

	// �ִϸ��̼� ��� ������Ʈ. 
	if (m_id >= 0)
	{
		if (m_parent)
			m_accTM = m_accTM * ((cBoneNode*)m_parent)->m_accTM;
		m_palette[ m_id] = m_offset * m_accTM;
	}
}


// m_accTM �� ������Ʈ �ϰ�, �ȷ�Ʈ�� ������Ʈ �Ѵ�.
void cBoneNode::UpdateAccTM()
{
	RET(!m_track);
	RET(m_id < 0);

	m_track->GetCalculateTM(m_localTM, m_aniTM, m_TM, m_accTM);

	if (m_parent)
		m_accTM = m_accTM * ((cBoneNode*)m_parent)->m_accTM;
	m_palette[ m_id] = m_offset * m_accTM;
}


Matrix44 cBoneNode::GetCalculateAniTM()
{
	RETV(!m_track, Matrix44::Identity);
	m_track->GetCalculateTM(m_localTM, m_aniTM, m_TM, m_accTM);
	return m_accTM;
}


int cBoneNode::GetCurrentFrame() const
{
	RETV(!m_track, 0);
	return m_track->GetCurrentFrame();
}


// cBlendTrack ���� localTM �� ����ϰ� �ֱ� ������
// cBlendTrack ���� �ٲ���� �Ѵ�.
void cBoneNode::UpdateLocalTM(const Matrix44 &tm)
{
	RET(!m_track);
	SetLocalTM(tm);
	m_track->UpdateLocalTM(tm);
}