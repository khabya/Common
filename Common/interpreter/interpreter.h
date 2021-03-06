//
// 2019-10-29, jjuiddong
// script interpreter main header
//
#pragma once


namespace common
{
	namespace script
	{

		class cVirtualMachine;

		class cInterpreter
		{
		public:
			cInterpreter();
			virtual ~cInterpreter();

			bool Init(iFunctionCallback *callback, void *arg = nullptr);
			bool ReadIntermediateCode(const StrPath &icodeFileName);
			bool Process(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			bool IsRun() const;
			bool IsStop() const;
			void Clear();


		public:
			enum class eState {Stop, Run,};

			eState m_state;
			StrPath m_fileName; // intermediate code filename
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			vector<cVirtualMachine*> m_vms;
			iFunctionCallback *m_callback;
			void *m_callbackArgPtr;
		};

	}
}
