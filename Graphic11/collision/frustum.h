//
// 2017-08-03, jjuiddong
// Upgrade DX9 - DX11
// 
#pragma once


namespace graphic
{
	class cRenderer;

	class cFrustum
	{
	public:
		cFrustum();
		virtual ~cFrustum();

		//virtual bool SetFrustum(const Matrix44 &matViewProj);
		virtual bool SetFrustum(const Vector3 &pos, const Vector3 &direction, const Matrix44 &maProj);
		virtual bool SetFrustum(const cCamera &camera);
		bool IsIn( const Vector3 &point ) const;
		//bool IsInSphere( const Vector3 &point, float radius, const Matrix44 &tm = Matrix44::Identity ) const;
		bool IsInSphere(const cBoundingSphere &sphere) const;
		bool IsInBox(const cBoundingBox &bbox) const;
		float LengthRoughly(const Vector3 &pos) const;
		//const Vector3& GetPos() const;
		//vector<Plane>& GetPlanes();
		
		static void Split2(cCamera &cam, const float f1, const float f2
			, cFrustum &out1, cFrustum &out2);
		static void Split3(cCamera &cam, const float f1, const float f2, const float f3
			, cFrustum &out1, cFrustum &out2, cFrustum &out3);

		void GetGroundPlaneVertices(const Plane &plane, OUT Vector3 outVertices[4]) const;

		cFrustum& operator=(const cFrustum &rhs);


	public:
		//bool m_fullCheck;	// IsIn, IsInSphere �Լ� ȣ��� üũ���� default : false
		//vector<Plane> m_plane; // frustum�� 6�� ���
		//Vector3 m_pos; // Position, near plane center
		//float m_epsilon; // �������ҿ� ��Ȯ�ϰ� ���Ե��� �ʴ���, �ణ�� ������ �־ �������ҿ� ���Խ�Ű�� ���� ��, default : 5 

		Matrix44 m_viewProj;
		BoundingFrustum m_frustum;
	};


//	inline const Vector3& cFrustum::GetPos() const { return m_pos; }
	//inline vector<Plane>& cFrustum::GetPlanes() { return m_plane; }
}