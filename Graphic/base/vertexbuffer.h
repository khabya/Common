// 버텍스 버퍼 클래스.
// LPDIRECT3DVERTEXBUFFER9 인터페이스를 쉽게 쓰기위해 만들어짐.
#pragma once


namespace graphic
{
	class cRenderer;

	class cVertexBuffer
	{
	public:
		cVertexBuffer();
		virtual ~cVertexBuffer();

		bool Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex, DWORD fvf);
		bool CreateVMem(cRenderer &renderer, const int vertexCount, const int sizeofVertex, DWORD fvf);
		bool Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex, const cVertexDeclaration &decl);

		void* Lock();
		void* LockDiscard(const int idx=0, const int size=0);
		void* LockNooverwrite(const int idx=0, const int size=0);
		void Unlock();
		void Bind(cRenderer &renderer) const;
		void Clear();

		void RenderTriangleList(cRenderer &renderer);
		void RenderTriangleStrip(cRenderer &renderer );
		void RenderLineStrip(cRenderer &renderer );
		void RenderLineList(cRenderer &renderer);
		void RenderLineList2(cRenderer &renderer);
		void RenderPointList(cRenderer &renderer, const int count = 0);
		void RenderPointList2(cRenderer &renderer, const int count = 0);

		DWORD GetFVF() const;
		int GetSizeOfVertex() const;
		int GetVertexCount() const;
		int GetOffset( const BYTE usage, const BYTE usageIndex=0 );

		void Set(cRenderer &renderer, cVertexBuffer &rhs);
		//cVertexBuffer& operator=(cVertexBuffer &rhs);


	private:
		LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
		LPDIRECT3DVERTEXDECLARATION9 m_pVtxDecl;
		cVertexDeclaration m_vtxDecl;
		DWORD m_fvf; // m_pVtxDecl 를 사용할 때는 fvf 를 사용하지 않는다.
		int m_sizeOfVertex; // stride
		int m_vertexCount;
		bool m_isManagedPool;
	};

	
	inline DWORD cVertexBuffer::GetFVF() const { return m_fvf; }
	inline int cVertexBuffer::GetSizeOfVertex() const { return m_sizeOfVertex; }
	inline int cVertexBuffer::GetVertexCount() const { return m_vertexCount; }
	inline int cVertexBuffer::GetOffset( const BYTE usage, const BYTE usageIndex ) { return m_vtxDecl.GetOffset(usage, usageIndex); }
}
