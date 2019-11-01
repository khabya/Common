//
// 2019-10-30, jjuiddong
// Visual Programming File Read/Write
//	- converting to intermediate code
//	- common::script::cIntermediateCode
//
// read/write node, pin, link information
// ASCII format
//
// node
//	- name string 
//	- rect num num num num
//	- input
//		- type string
//		- id num
//		- name string
//		- value string
//		- links num num num...
//	- output pin
//		- type string
//		- id num
//		- name string
//		- value string
//		- links num num num...
// symbol
//	- id num
//	- value string
//
//
#pragma once

#include "vprog.h"


namespace vprog
{

	class cVProgFile
	{
	public:
		struct sNode;
		struct sPin {
			int id;
			StrId name;
			ePinType::Enum type;
			ePinKind::Enum kind;
			vector<int> links; // pin id array
		};

		struct sNode {
			int id;
			eNodeType::Enum type;
			StrId name;
			StrId varName; // if variable type, variable name
			vector<sPin> inputs;
			vector<sPin> outputs;
		};

		struct sLink {
			int from; // from pin id
			int to; // to pin id
		};

		cVProgFile();
		virtual ~cVProgFile();

		bool Read(const StrPath &fileName);
		bool Write(const StrPath &fileName);
		bool GenerateIntermediateCode(OUT common::script::cIntermediateCode &out);
		void Clear();


	protected:
		bool GenerateCode_Event(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Node(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Function(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Pin(const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_TemporalPin(const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);

		bool AddPin(const int parseState, sNode &node, const sPin &pin);
		std::pair<sNode*,sPin*> FindContainPin(const int pinId);


	public:
		vector<sNode> m_nodes;
		vector<sLink> m_links;
		set<int> m_visit; // visit node, use generate intermediate code
	};

}

