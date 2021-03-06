//
// 2019-10-29, jjuiddong
// excute script intermediate code
//
#pragma once


namespace common
{
	namespace script
	{

		class cVirtualMachine
		{
		public:
			struct sRegister
			{
				uint idx; // instruction index (next execute instruction)
				uint exeIdx; // executed instruction index
				bool cmp; // compare output flag
				float tim; // delay timer (milliseconds unit)
				variant_t val[10]; // register
			};

			struct sTimer
			{
				StrId name;
				float interval; // seconds unit
				float t; // decrease time, seconds unit
			};

			cVirtualMachine(const StrId &name);
			virtual ~cVirtualMachine();

			bool Init(const cIntermediateCode &code, iFunctionCallback *callback
				, void *arg = nullptr);
			bool Process(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			void Clear();


		protected:
			bool ProcessEvent(const float deltaSeconds);
			bool ProcessTimer(const float deltaSeconds);
			bool ExecuteInstruction(const float deltaSeconds, sRegister &reg);


		public:
			enum class eState { 
				Stop  // waitting for start
				, Run // execute instruction
				, Wait // no operation state, wait until event trigger
			};

			eState m_state;
			StrId m_name;
			sRegister m_reg;
			cSymbolTable m_symbTable;
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			vector<sTimer> m_timers;
			iFunctionCallback *m_callback;
			void *m_callbackArgPtr;
		};

	}
}
