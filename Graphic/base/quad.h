// 사각형을 나타내는 클래스다.
// 간단하게 텍스쳐를 입힐 때 사용할 수 있는 클래스.
#pragma once


namespace graphic
{

	class cQuad
	{
	public:
		cQuad();
		virtual ~cQuad();

		bool Create(cRenderer &renderer, const float width, const float height, const Vector3 &pos,
			const string &textureFileName = "", const bool isSizePow2 = true);

		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void RenderAlpha(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void RenderFactor(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void RenderLine(cRenderer &renderer);

		void SetTransform( const Matrix44 &tm );
		const Matrix44& GetTransform() const;
		cMaterial& GetMaterial();
		cVertexBuffer& GetVertexBuffer();
		cTexture* GetTexture();


	public:
		cVertexBuffer m_vtxBuff;
		cMaterial m_material;
		cTexture *m_texture; // reference
		Matrix44 m_tm;
	};


	inline cMaterial& cQuad::GetMaterial() { return m_material; }
	inline cVertexBuffer& cQuad::GetVertexBuffer() { return m_vtxBuff; }
	inline void cQuad::SetTransform( const Matrix44 &tm ) { m_tm = tm; }
	inline const Matrix44& cQuad::GetTransform() const { return m_tm; }
	inline cTexture* cQuad::GetTexture() { return m_texture; }
}
