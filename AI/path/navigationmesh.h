//
// 2017-12-03, jjuiddong
// Navigation Mesh
//
#pragma once


namespace ai
{
	using namespace common;

	class cPathFinder;

	class cNavigationMesh
	{
	public:
		cNavigationMesh();
		virtual ~cNavigationMesh();

		bool ReadFromPathFile(const char *fileName);

		bool Find(const Vector3 &start, const Vector3 &end
			, OUT vector<Vector3> &out
			, OUT vector<int> *outNodePath = NULL
		);

		void GetNodesFromVertexIdx(const int vtxIdx, OUT set<int> &out);
		void GetNodesFromPosition(const Vector3 &pos, OUT set<int> &out);
		void GetWallPlane(const set<int> &nodeIndices, OUT vector<Plane> &out);
		inline int MakeEdgeKey(const int from, const int to) const;


	protected:
		int BuildPolygonFromPathFinder(const cPathFinder &pathFinder
			, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3, INOUT vector<WORD> &indices);
		int GetExistTriangle(const vector<WORD> &indices
			, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3);
		int GetNearestNode(const Vector3 &pos);
		int GetAdjacentCollisionVertexIdx(const int adjVtxIdx1, const int adjVtxIdx2
			, const int nodeFromIdx, const int nodeToIdx);
		int GetAdjacentCollisionVertexIdx(const int adjVtxIdx1, const int adjVtxIdx2
			, const vector<int> &nodeIndices, const bool isReverse=true);
		std::pair<int, int> GetAdjacentVertexIdx(const int nodeIndex1, const int nodeIndex2);

		std::pair<int,int> GetNearestNodeFromVertexIdx(const vector<int> &nodeIndices
			, const int vtxIdx);
		std::pair<int, int> GetNearestNodeFromVertexIdx(const vector<int> &nodeIndices
			, const int vtxIdx1, const int vtxIdx2);

		void OptimizePath(const vector<int> &nodeIndices
			, const vector<Vector3> &path
			, OUT vector<Vector3> &out);


	public:
		struct sNaviNode
		{
			int idx1, idx2, idx3; // Vertex Index (m_vertices)
			Vector3 center;
			int adjacent[3]; // adjacent NaviNode index (m_naviNodes)
			float startLen; // for search
			float endLen; // for search
		};

		struct sWall
		{
			uint nodeIdx;
			bool collision;			
			Plane bplane;
		};

		vector<Vector3> m_vertices;
		vector<sNaviNode> m_naviNodes;
		vector<sWall> m_walls;
		map<uint, vector<uint>> m_wallMap; // key: node index, value: m_walls indices (fast search)
	};

}
