//
// 2017-05-26, jjuiddong
// Bounding Sphere
//
#pragma once


namespace graphic
{

	class cBoundingSphere
	{
	public:
		cBoundingSphere() {
		}
		cBoundingSphere(const Vector3 &center, const float radius) :
			m_bsphere(*(XMFLOAT3*)&center, radius) {
		}
		virtual ~cBoundingSphere() {
		}

		void SetBoundingSphere(const Vector3 &center, const float radius) {
			m_bsphere.Center = *(XMFLOAT3*)&center;
			m_bsphere.Radius = radius;
		}

		void SetBoundingSphere(const cBoundingBox &bbox, const Matrix44 &tm=Matrix44::Identity) {
			BoundingSphere::CreateFromBoundingBox(m_bsphere, bbox.m_bbox);
		}

		void SetPos(const Vector3 &pos) {
			m_bsphere.Center = *(XMFLOAT3*)&pos;
		}

		cBoundingSphere operator * (const Matrix44 &rhs) {
			return cBoundingSphere(*(Vector3*)&m_bsphere.Center * rhs, m_bsphere.Radius);
		}

		cBoundingSphere operator * (const XMMATRIX &rhs) {
			const XMVECTOR center = XMLoadFloat3(&m_bsphere.Center);
			const XMVECTOR pos = XMVector3Transform(center, rhs);
			XMFLOAT3 f3;
			XMStoreFloat3(&f3, pos);
			return cBoundingSphere(*(Vector3*)&f3, m_bsphere.Radius);
		}

		const cBoundingSphere& operator *= (const Matrix44 &rhs) {
			*(Vector3*)&m_bsphere.Center *= rhs;
			return *this;
		}

		XMMATRIX GetTransform() const {
			XMMATRIX tm = XMMatrixScaling(m_bsphere.Radius, m_bsphere.Radius, m_bsphere.Radius)
				* XMMatrixTranslation(m_bsphere.Center.x, m_bsphere.Center.y, m_bsphere.Center.z);
			return tm;
		}


	public:
		BoundingSphere m_bsphere;
	};

}