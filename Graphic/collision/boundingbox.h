// 
// OBB (Oriendted Bounding Box)
//
#pragma once


namespace graphic
{
	class cCube;

	class cBoundingBox
	{
	public:
		cBoundingBox();
		cBoundingBox(const cCube &cube);
		void SetBoundingBox(const Vector3 &vMin, const Vector3 &vMax );
		void SetTransform( const Matrix44 &tm );
		bool Collision( cBoundingBox &box );
		bool Pick(const Vector3 &orig, const Vector3 &dir, const Matrix44 &tm = Matrix44::Identity);
		bool Pick2(const Vector3 &orig, const Vector3 &dir, float *pfT, float *pfU, float *pfV);
		bool Pick3(const Vector3 &orig, const Vector3 &dir, float *pDistance, const Matrix44 &tm = Matrix44::Identity);
		float Length() const;
		Vector3 Center() const;
		void Scale(const Vector3 &scale);

		cBoundingBox& operator=(const cCube &cube);


	public:
		Vector3 m_min;
		Vector3 m_max;
		Matrix44 m_tm;
	};


	// length(m_min, m_max)
	inline float cBoundingBox::Length() const { return (m_min - m_max).Length(); }
}
