//
// 2017-03-14, jjuiddong
// gloabl input manager
//
#pragma once


namespace graphic
{

	class cInputManager : public common::cSingleton<cInputManager>
	{
	public:
		cInputManager();
		virtual ~cInputManager();
		void Update(const float deltaSeconds);
		void MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		bool IsClick();


	public:
		bool m_lBtnDown;
		bool m_lBtnDbClick;
		bool m_rBtnDown;
		bool m_mBtnDown;
		POINT m_mousePt;
		int m_clickTime;
		int m_dbClickTime;
	};

}