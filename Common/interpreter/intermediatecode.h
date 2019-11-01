//
// 2019-10-30, jjuiddong
// script intermediate code
//
//
// ---------------------------------------------------------
// Sample
//
// # script intermediate language sample
//
// main:
// nop
//
// Event_SeqWork:
// geti Event_SeqWork, Loading, val0
// seti GetFrontNode, node, val0
// call GetFrontNode
// geti GetFrontNode, front, val0
// seti ReqMove, Dest, val0
// call ReqMove
// geti ReqMove, Result, val0
// eqic val0, 0
// jnz jump1
// jmp jump2
// nop
//
// jump1:
// geti Event_Seqwork, Loading, val0
// seti ReqLoading, station, val0
// geti Event_Seqwork, lLayer, val1
// seti ReqLoading, lLayer, val1
// call ReqLoading
// nop
//
// jump2:
// geti ReqMove, Result, val0
// seti ErrorState, errorCode, val0
// call ErrorState
// nop
// ---------------------------------------------------------
//
#pragma once


namespace common
{
	namespace script
	{

		class cIntermediateCode
		{
		public:
			cIntermediateCode();
			virtual ~cIntermediateCode();

			bool Read(const StrPath &fileName);
			bool Write(const StrPath &fileName);
			bool IsLoaded();
			void Clear();


		protected:
			uint GetRegisterIndex(const string &regName);
			const char* GetRegisterName(const uint regIdx);


		public:
			vector<sCommandSet> m_codes;
			map<StrId, uint> m_jmpMap; //key: jump name, data: code line
		};

	}
}
