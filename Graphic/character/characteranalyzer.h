// 캐릭터 분석용 클래스
// 본을 출력하고, 선택할 수 있는 기능을 가짐.
#pragma once


namespace graphic
{
	class cRenderer;

	class cCharacterAnalyzer
	{
	public:
		cCharacterAnalyzer();
		virtual ~cCharacterAnalyzer();

		//bool Create(const string &modelName);
		cCharacter* GetCharacter();
		void SetCharacter(cCharacter *character);

		bool Update(const float deltaSeconds);
		void Render(cRenderer &renderer,  const Matrix44 &tm);

		void HighlightBone(cRenderer &renderer, const string &boneName);


	public:
		cCharacter *m_character; // reference
		cCube m_selectBoneCube;
		cBoneNode *m_selectBone;
	};


	inline cCharacter* cCharacterAnalyzer::GetCharacter() { return m_character; }
	inline void cCharacterAnalyzer::SetCharacter(cCharacter *character) { m_character = character; }
}
