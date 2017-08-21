#include "stdafx.h"
#include "skybox.h"

using namespace graphic;


cSkyBox::cSkyBox()
{
	ZeroMemory(m_textures, sizeof(m_textures));
}

cSkyBox::~cSkyBox()
{
}


// textureFilePath : �� ���� ��ο� skybox_top, skybox_front, skybox_back, 
//				skybox_left, skybox_right, skybox_bottom.jpg ������ �־�� �Ѵ�.
bool cSkyBox::Create(cRenderer &renderer, const StrPath &textureFilePath)
{
	char *textureFileName2[] = 
	{
		//"skybox_front.jpg", "skybox_back.jpg", "skybox_left.jpg", 
		//"skybox_right.jpg", "skybox_top.jpg", "skybox_bottom.jpg"
		//"blueclear2_front.png", 
		//"blueclear2_back.png",
		//"blueclear2_left.png",
		//"blueclear2_right.png",
		//"blueclear2_top.png",
		//"blueclear2_bottom.png",

		"cloud161_front.dds", 
		"cloud161_back.dds",
		"cloud161_left.dds",
		"cloud161_right.dds",
		"cloud161_top.dds",
		"cloud161_bottom.dds",
	};

	return Create(renderer, textureFilePath, textureFileName2);
}


bool cSkyBox::Create2(cRenderer &renderer, const StrPath &textureFilePath)
{	
	char *textureFileName1[] =
	{
		"ThickCloudsWaterFront2048.png", 
		"ThickCloudsWaterBack2048.png",
		"ThickCloudsWaterRight2048.png",
		"ThickCloudsWaterLeft2048.png",
		"ThickCloudsWaterUp2048.png",
		"ThickCloudsWaterDown2048.png",
	};
	return Create(renderer, textureFilePath, textureFileName1);
}


bool cSkyBox::Create(cRenderer &renderer, const StrPath &textureFilePath,
	char *skyboxTextureNames[6])
{
	for (int i = 0; i < MAX_FACE; ++i)
	{
		const StrPath fileName = textureFilePath + "/" + skyboxTextureNames[i];
		m_textures[i] = cResourceManager::Get()->LoadTextureParallel(renderer, fileName);
		cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
			, fileName, (void**)&m_textures[i]));
	}

	if (!CreateVertexBuffer(renderer))
		return false;

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool  cSkyBox::CreateVertexBuffer(cRenderer &renderer)
{
	// Example diagram of "front" quad
	// The numbers are vertices
	// 
	// 2  __________ 4
	//   |\         |
	//	 |  \       |
	//   |    \     |
	//   |      \   |
	// 1 |        \ | 3
	//	  ----------	 
	const float size = 300;
	sVertexTex SkyboxMesh[24] =
	{
		// Front quad, NOTE: All quads face inward
		sVertexTex(-size, -size,  size,  0.0f, 1.0f ),
		sVertexTex(-size,  size,  size,  0.0f, 0.0f ),
		sVertexTex( size, -size,  size,  1.0f, 1.0f ),
		sVertexTex( size,  size,  size,  1.0f, 0.0f ),

		// Back quad
		sVertexTex( size, -size, -size,  0.0f, 1.0f ),
		sVertexTex( size,  size, -size,  0.0f, 0.0f ),
		sVertexTex(-size, -size, -size,  1.0f, 1.0f ),
		sVertexTex(-size,  size, -size,  1.0f, 0.0f ),

		// Left quad
		sVertexTex(-size, -size, -size,  0.0f, 1.0f ),
		sVertexTex(-size,  size, -size,  0.0f, 0.0f ),
		sVertexTex(-size, -size,  size,  1.0f, 1.0f ),
		sVertexTex(-size,  size,  size,  1.0f, 0.0f ),

		// Right quad
		sVertexTex( size, -size,  size,  0.0f, 1.0f ),
		sVertexTex( size,  size,  size,  0.0f, 0.0f ),
		sVertexTex( size, -size, -size,  1.0f, 1.0f ),
		sVertexTex( size,  size, -size,  1.0f, 0.0f ),

		// Top quad
		sVertexTex(-size,  size,  size,  0.0f, 1.0f ),
		sVertexTex(-size,  size, -size,  0.0f, 0.0f ),
		sVertexTex( size,  size,  size,  1.0f, 1.0f ),
		sVertexTex( size,  size, -size,  1.0f, 0.0f ),

		// Bottom quad
		sVertexTex(-size, -size, -size,  0.0f, 1.0f ),
		sVertexTex(-size, -size,  size,  0.0f, 0.0f ),
		sVertexTex( size, -size, -size,  1.0f, 1.0f ),
		sVertexTex( size, -size,  size,  1.0f, 0.0f ),
	};

	const int vtxSize = 24;
	m_vtxBuff.Create(renderer, vtxSize, sizeof(sVertexTex), SkyboxMesh);

	return true;
}


void cSkyBox::Render(cRenderer &renderer
	, const XMMATRIX &tm //=XMIdentity
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::TEXTURE);
	assert(shader);
	shader->SetTechnique("Skybox");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(XMIdentity);
	renderer.m_cbPerFrame.Update(renderer);

	m_vtxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthNone(), 0);
	for (int i = 0 ; i < MAX_FACE; i++)
	{
		if (m_textures[i])
			m_textures[i]->Bind(renderer, 0);
		renderer.GetDevContext()->DrawInstanced(4, 1, i * 4, 0);
	}
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
}