//
// 2019-11-04, jjuiddong
// interpreter debugger
//
#pragma once


namespace common
{
	namespace script
	{

		class cInterpreter;

		class cDebugger
		{
		public:
			cDebugger();
			virtual ~cDebugger();

			bool Init(cInterpreter *interpreter);
			bool Process(const float deltaSeconds);
			bool OneStep();
			bool Run();
			bool Break();
			bool Terminate();
			void Clear();


		public:
			enum class eState {Stop, DebugWait, DebugStep, DebugRun };

			eState m_state;
			cInterpreter *m_interpreter; // reference
		};

	}
}
