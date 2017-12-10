
#include "stdafx.h"
#include "navigationmesh.h"

using namespace ai;


cNavigationMesh::cNavigationMesh()
{
}

cNavigationMesh::~cNavigationMesh()
{
}


// pathFinder ������ �о, Navigation Mesh�� �����Ѵ�.
bool cNavigationMesh::ReadFromPathFile(const char *fileName)
{
	cPathFinder pathFinder;
	if (!pathFinder.Read(fileName))
		return false;

	m_vertices.clear();
	m_vertices.reserve(pathFinder.m_vertices.size());

	vector<WORD> indices;
	indices.reserve(pathFinder.m_vertices.size()*3);

	for (auto &vtx : pathFinder.m_vertices)
		m_vertices.push_back(vtx.pos);

	for (u_int i = 0; i < pathFinder.m_vertices.size(); ++i)
	{
		auto &vtx = pathFinder.m_vertices[i];
		if (vtx.edge[0] < 0)
			continue;

		// ù ��° Polygon�� ã�Ƽ�, Navigation Mesh�� �����Ѵ�.
		BuildPolygonFromPathFinder(pathFinder, i, vtx.edge[0], -1, indices);
		break;
	}

	if (indices.empty())
		return false;

	return true;
}


// �ΰ��� ���ؽ��� ���Ե� �������� ã�Ƽ�, �����Ѵ�.
// ��������� ���� �������� �˻��ؼ� �����Ѵ�.
// ������ ������ Index �� �����Ѵ�.
int cNavigationMesh::BuildPolygonFromPathFinder(const ai::cPathFinder &pathFinder
	, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3
	, INOUT vector<WORD> &indices )
{
	bool isFind = false;
	const ai::sVertex vtx1 = pathFinder.m_vertices[vtxIdx1];
	const ai::sVertex vtx2 = pathFinder.m_vertices[vtxIdx2];

	// vtxIdx1, vtxIdx2 �ΰ��� ����� ���ؽ��� ã�´�.
	// ���� ���� ���ؽ��� ����Ű��, �ϳ��� �����￡ ������ ���Ѵ�.
	for (int i = 0; i < ai::sVertex::MAX_EDGE; ++i)
	{
		if (vtx1.edge[i] < 0)
			break;

		for (int k = 0; k < ai::sVertex::MAX_EDGE; ++k)
		{
			if (vtx2.edge[k] < 0)
				break;

			if (vtx1.edge[i] == vtx2.edge[k])
			{
				const int nextIdx3 = vtx1.edge[i];
				if (vtxIdx3 == nextIdx3)
					continue; // �����ܿ��� �̹� ������ ���ؽ���.

				const int triIdx = GetExistTriangle(indices, vtxIdx1, vtxIdx2, nextIdx3);
				if (triIdx >= 0)
				{
					// vtxIdx1, vtxIdx2�� �������� �ϰ�, vtxIdx3 ���ؽ��� �ݴ����� �ִ�
					// ��带 ���Ѵ�. �̹� ������� ����̴�, node index�� �����ϰ� �����Ѵ�.
					return triIdx / 3;
				}

				// vtxIdx1, vtxIdx2�� ���Ե� ������ ������ ���ؽ��� ã��.
				// �޼� ��Ģ�� �̿��� Normal�� Vector3(0,1,0)�� �Ǵ� ������ �����Ѵ�.
				const Vector3 p1 = vtx1.pos;
				const Vector3 p2 = vtx2.pos;
				const Vector3 p3 = pathFinder.m_vertices[nextIdx3].pos;
				Triangle tri(p1, p2, p3);
				if (tri.Normal().y > 0.f)
				{
					indices.push_back(vtxIdx1);
					indices.push_back(vtxIdx2);
					indices.push_back(nextIdx3);
				}
				else
				{
					indices.push_back(vtxIdx1);
					indices.push_back(nextIdx3);
					indices.push_back(vtxIdx2);
				}

				isFind = true;
				break;
			}
		}

		if (isFind)
			break;
	}

	if (!isFind)
		return -1; // ������ ����.

	// ������ ã�� ������, �ε���
	const int polyIndices[3] = {
		indices[indices.size() - 3]
		, indices[indices.size() - 2]
		, indices[indices.size() - 1]
	};

	// ������ �׺���̼� �޽� ��� �ε����� �����Ѵ�.
	sNaviNode node;
	node.idx1 = polyIndices[0];
	node.idx2 = polyIndices[1];
	node.idx3 = polyIndices[2];
	node.center = pathFinder.m_vertices[polyIndices[0]].pos;
	node.center += pathFinder.m_vertices[polyIndices[1]].pos;
	node.center += pathFinder.m_vertices[polyIndices[2]].pos;
	node.center /= 3.f;
	m_naviNodes.push_back(node);
	const int nodeId = m_naviNodes.size() - 1;

	// ������ �������� ã�Ƽ� �����Ѵ�.
	const int idx1 = BuildPolygonFromPathFinder(pathFinder, polyIndices[0], polyIndices[1], polyIndices[2], indices);
	const int idx2 = BuildPolygonFromPathFinder(pathFinder, polyIndices[1], polyIndices[2], polyIndices[0], indices);
	const int idx3 = BuildPolygonFromPathFinder(pathFinder, polyIndices[2], polyIndices[0], polyIndices[1], indices);

	m_naviNodes[nodeId].adjacent[0] = idx1;
	m_naviNodes[nodeId].adjacent[1] = idx2;
	m_naviNodes[nodeId].adjacent[2] = idx3;

	return nodeId;
}


// ��ϵ� �������� �ִ��� �˻��Ѵ�. �ִٸ� index �� �����Ѵ�.
// ���ٸ�, -1�� �����Ѵ�.
int cNavigationMesh::GetExistTriangle(const vector<WORD> &indices
	, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3)
{
	for (u_int i = 2; i < indices.size(); i += 3)
	{
		const WORD idx1 = indices[i - 2];
		const WORD idx2 = indices[i - 1];
		const WORD idx3 = indices[i - 0];

		if (((idx1 == vtxIdx1) && (idx2 == vtxIdx2) && (idx3 == vtxIdx3))
			|| ((idx1 == vtxIdx1) && (idx2 == vtxIdx3) && (idx3 == vtxIdx2))
			|| ((idx1 == vtxIdx2) && (idx2 == vtxIdx1) && (idx3 == vtxIdx3))
			|| ((idx1 == vtxIdx2) && (idx2 == vtxIdx3) && (idx3 == vtxIdx1))
			|| ((idx1 == vtxIdx3) && (idx2 == vtxIdx1) && (idx3 == vtxIdx2))
			|| ((idx1 == vtxIdx3) && (idx2 == vtxIdx2) && (idx3 == vtxIdx1))
			)
			return (i-2);
	}
	return -1;
}

// Normal Distance
float Distance(const Vector3 &p0, const Vector3 &p1)
{
	return p0.Distance(p1);
}


// Search Navigation Mesh
bool cNavigationMesh::Find(const Vector3 &start, const Vector3 &end
	, OUT vector<Vector3> &out1
	, OUT vector<Vector3> &out2 
)
{
	const int startIdx = GetNearestNode(start);
	const int endIdx = GetNearestNode(end);

	if ((startIdx == -1) || (endIdx == -1))
		return false;

	ZeroMemory(g_edges_visit, sizeof(g_edges_visit));
	ZeroMemory(g_edges_len, sizeof(g_edges_len));

	vector<int> candidate;
	candidate.reserve(m_vertices.size());
	candidate.push_back(startIdx);

	m_naviNodes[startIdx].startLen = 0;
	m_naviNodes[startIdx].endLen = Distance(start, end);

	bool isFind = false;
	while (!candidate.empty())
	{
		const int curIdx = candidate.front();
		rotatepopvector(candidate, 0);

		sNaviNode &curNode = m_naviNodes[curIdx];

		if (endIdx == curIdx)
		{
			isFind = true;
			break;
		}

		for (int i = 0; i < 3; ++i)
		{
			if (curNode.adjacent[i] < 0)
				continue;

			const int nextIdx = curNode.adjacent[i];

			if (g_edges_visit[curIdx][nextIdx])
				continue;

			sNaviNode &nextNode = m_naviNodes[nextIdx];
			
			nextNode.startLen = curNode.startLen + Distance(curNode.center, nextNode.center) + 0.00001f;
			nextNode.endLen = Distance(end, nextNode.center);
			g_edges_visit[curIdx][nextIdx] = true;
			g_edges_visit[nextIdx][curIdx] = true;
			g_edges_len[curIdx][nextIdx] = nextNode.startLen + nextNode.endLen;
			g_edges_len[nextIdx][curIdx] = nextNode.startLen + nextNode.endLen;

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				sNaviNode &compNode = m_naviNodes[candidate[k]];
				if ((compNode.endLen + compNode.startLen) > (nextNode.endLen + nextNode.startLen))
				{
					candidate.push_back(nextIdx);
					common::rotateright2(candidate, k);
					isInsert = true;
					break;
				}
			}

			if (!isInsert)
				candidate.push_back(nextIdx);
		}
	}

	if (!isFind)
		return false;

	// tracking end point to start point
	vector<int> nodeIndices;

	out1.push_back(m_naviNodes[endIdx].center);
	nodeIndices.push_back(endIdx);

	ZeroMemory(g_edges_visit, sizeof(g_edges_visit));

	int curIdx = endIdx;
	while ((curIdx != startIdx) && (nodeIndices.size() < 1000))
	{
		float minEdge = FLT_MAX;
		int nextIdx = -1;
		sNaviNode &node = m_naviNodes[curIdx];

		for (int i = 0; i < 3; ++i)
		{
			if (node.adjacent[i] < 0)
				continue;
			if (g_edges_visit[curIdx][node.adjacent[i]])
				continue;

			const float len = g_edges_len[curIdx][node.adjacent[i]];
			if (0 == len)
				continue;

			if (minEdge > len)
			{
				minEdge = len;
				nextIdx = node.adjacent[i];
			}
		}

		if (nextIdx < 0)
		{
			assert(0);
			break; // error occur
		}

		g_edges_visit[curIdx][nextIdx] = true;
		g_edges_visit[nextIdx][curIdx] = true;
		out1.push_back(m_naviNodes[nextIdx].center);
		nodeIndices.push_back(nextIdx);
		curIdx = nextIdx;
	}

	assert(nodeIndices.size() < 1000);

	out1.push_back(start);
	std::reverse(out1.begin(), out1.end());
	std::reverse(nodeIndices.begin(), nodeIndices.end());

	out1.push_back(end);

	OptimizePath(nodeIndices, out1, out2);

	return true;
}


// Find the node nearest to pos and return the index m_naviNodes.
// If NavigationMesh is created correctly, there are no duplicated nodes in pos.
// If not found, return -1
int cNavigationMesh::GetNearestNode(const Vector3 &pos)
{
	float nearDist = FLT_MAX;
	int nearIdx = -1;

	for (u_int i=0; i < m_naviNodes.size(); ++i)
	{
		auto &node = m_naviNodes[i];

		const Triangle tri(
			m_vertices[node.idx1]
			, m_vertices[node.idx2]
			, m_vertices[node.idx3]
		);

		if (tri.Intersect(Vector3(pos.x, pos.y + 1, pos.z), Vector3(0, -1, 0)))
			return (int)i;

		const float len = node.center.LengthRoughly(pos);
		if (len < nearDist)
		{
			nearDist = len;
			nearIdx = i;
		}
	}
	
	return nearIdx;
}


// Optimize the path found in Find ().
// ���� ������ �������� ��, �浹�� �Ͼ�� ������, �� ���� ������
// ���� ������� �˻��ؼ�, �ִ��� �������� �����̰� �Ѵ�.
// �� ��� ���� �پ��ִ� �������� �˻����� �ʰ�, �������� �ʴ� ���
// �浹�� �Ͼ���� �Ǵ��Ѵ�.
void cNavigationMesh::OptimizePath(const vector<int> &nodeIndices
	, const vector<Vector3> &path
	, OUT vector<Vector3> &out)
{
	if (nodeIndices.empty() || (path.size() < 3))
		return;

	out.push_back(path[0]);

	Vector3 curPos = path[0] + Vector3(0, 1, 0); // start pos
	int curVtxIdx = -1; // ��ȸ Vertex Index

	int s = 0; // ù��° ������ �˻�
	for (int i = 0; i < (int)nodeIndices.size()-1; ++i)
	{
		const int curNodeIdx = nodeIndices[i];
		const int nextNodeIdx = nodeIndices[i+1];
		sNaviNode &curNode = m_naviNodes[curNodeIdx];
		sNaviNode &nextNode = m_naviNodes[nextNodeIdx];

		Vector3 nextPos = nextNode.center + Vector3(0,1,0);

		// Node�� �� �߿�, �������� �ʴ� ����, ����Ѵٸ�, �̵��� �� ����
		// ����̱� ������, �� �������δ� �̵��ؼ��� �ȵȴ�.
		const Vector3 dir = (nextPos - curPos).Normal();

		// ��ü ��ο��� ������ �浹 �׽�Ʈ
		for (int o = s; o <= i; ++o)
		{
			const int preTestNodeIdx = nodeIndices[o];
			const int nextTestNodeIdx = nodeIndices[o+1];
			sNaviNode &testNode = m_naviNodes[nextTestNodeIdx];
			const int idxs[3] = { testNode.idx1, testNode.idx2, testNode.idx3 };

			// �������� ������ �浹 �׽�Ʈ
			for (int k = 0; k < 3; ++k)
			{
				if (testNode.adjacent[k] >= 0)
					continue;

				// Vertex�� �߽����� ��ȸ�� ��, ���� Vertex Index�� �����ϸ�,
				// Vertex ��ġ�� dir �������� ���� �̵��Ѵ�, �׷��� ������ ���ؽ��� 
				// �鿡 �پ��ֱ� ������, �׻� �浹 ������ �Ͼ��.
				Vector3 offset;
				if ((curVtxIdx >= 0)
					&& ((idxs[k] == curVtxIdx) || (idxs[(k + 1) % 3] == curVtxIdx)))
				{
					offset = dir * 0.1f;
				}

				const Vector3 offsetY(0, 10, 0);
				const Vector3 p1 = m_vertices[idxs[k]];
				const Vector3 p2 = m_vertices[idxs[(k + 1) % 3]];
				const Vector3 p3 = p1 + offsetY;
				const Triangle tri1(p1, p2, p3);
				const Triangle tri2(p1 + offsetY, p2, p2 + offsetY);

				// �������� �ʴ� ���� �浹 (�� �� ���� ���� ����� ���)
				float t, u;
				const bool collision1 =
					tri1.Intersect(curPos + offset, dir, &t, &u)
					|| tri2.Intersect(curPos + offset, dir, &t, &u);

				const bool collision2 = tri1.Intersect(nextPos, -dir, &t, &u)
					|| tri2.Intersect(nextPos, -dir, &t, &u);

				if (!collision1 && !collision2)
					continue; // �浹�� �Ͼ�� �ʴ´ٸ�, ����

				// ���� ���� ���� ��忡�� �ߺ��� ���ؽ� 2������, (���� ���ؽ�)
				// �������� �ʴ� ���� ���ϴ� ���ؽ���, �浹�Ǵ� �������̴�.
				// �� ���ؽ��� �߽����� �ٽ� ���� ��θ� Ž���Ѵ�.
				int pointIdx = GetAdjacentCollisionVertexIdx(idxs[k], idxs[(k + 1) % 3]
					, curNodeIdx, nextNodeIdx);

				if (pointIdx < 0)
					pointIdx = GetAdjacentCollisionVertexIdx(idxs[k], idxs[(k + 1) % 3]
						, preTestNodeIdx, nextTestNodeIdx);

				if (pointIdx < 0)
				{
					assert(pointIdx >= 0);
					break;
				}

				curPos = m_vertices[pointIdx];
				curVtxIdx = pointIdx;

				s = GetNearestNodeFromVertexIdx(nodeIndices, pointIdx).second;
				out.push_back(curPos);
				break;
			} // for k
		} // for o
	} // for i

	out.push_back(path.back());
}


// nodeFromIdx ���� nodeToIdx ����� ���� ���ؽ�����
// adjVtxIdx1, adjVtxIdx2 ���ؽ��� �ߺ��� Vertex Index�� �����Ѵ�.
// ���ٸ� -1�� �����Ѵ�.
int cNavigationMesh::GetAdjacentCollisionVertexIdx(
	const int adjVtxIdx1, const int adjVtxIdx2
	, const int nodeFromIdx, const int nodeToIdx)
{
	sNaviNode &curNode = m_naviNodes[nodeFromIdx];

	const int idxs[3] = { curNode.idx1, curNode.idx2, curNode.idx3 };
	for (int i = 0; i < 3; ++i)
	{
		if (nodeToIdx == curNode.adjacent[i])
		{
			if (adjVtxIdx1 == idxs[i])
				return adjVtxIdx1;
			if (adjVtxIdx1 == idxs[(i + 1) % 3])
				return adjVtxIdx1;
			if (adjVtxIdx2 == idxs[i])
				return adjVtxIdx2;
			if (adjVtxIdx2 == idxs[(i + 1) % 3])
				return adjVtxIdx2;
		}
	}

	return -1;
}


// nodeIndices���� vtxIdx �� ���Ե� node index �� �����Ѵ�.
// �� ��, ���� ó���� node index �� �����Ѵ�.
// ���ٸ�, -1�� �����Ѵ�.
// return = {node index, order of nodeIndices}
std::pair<int, int> cNavigationMesh::GetNearestNodeFromVertexIdx(const vector<int> nodeIndices, const int vtxIdx)
{
	for (u_int i = 0; i < nodeIndices.size(); ++i)
	{
		const sNaviNode &node = m_naviNodes[nodeIndices[i]];
		if ((node.idx1 == vtxIdx)
			|| (node.idx2 == vtxIdx)
			|| (node.idx3 == vtxIdx))
		{
			return{ nodeIndices[i], i };
		}
	}

	return{ -1,-1 };
}