
#include "stdafx.h"
#include "vprogfile.h"

using namespace vprog;
namespace script = common::script;

const static cVProgFile::sNode nullNode = { 0, };


cVProgFile::cVProgFile()
{
}

cVProgFile::~cVProgFile()
{
	Clear();
}


bool cVProgFile::Read(const StrPath &fileName)
{
	Clear();

	m_fileName = fileName.GetFullFileName();

	vector<common::cSimpleData2::sRule> rules;
	rules.push_back({ 0, "node", 1, -1 });
	rules.push_back({ 1, "output", 2, -1 });
	rules.push_back({ 2, "output", 2, 1 });
	rules.push_back({ 3, "output", 2, 1 });
	rules.push_back({ 1, "input", 3, -1 });
	rules.push_back({ 3, "input", 3, 1 });
	rules.push_back({ 2, "input", 3, 1 });
	rules.push_back({ 1, "node", 1, 0 });
	rules.push_back({ 2, "node", 1, 0 });
	rules.push_back({ 3, "node", 1, 0 });
	rules.push_back({ 4, "node", 1, 0 });
	rules.push_back({ 5, "node", 1, 0 });
	rules.push_back({ 1, "symbol", 4, 0 });
	rules.push_back({ 2, "symbol", 4, 0 });
	rules.push_back({ 3, "symbol", 4, 0 });
	rules.push_back({ 4, "symbol", 4, 0 });
	rules.push_back({ 5, "symbol", 4, 0 });
	rules.push_back({ 0, "define", 5, 0 });
	rules.push_back({ 1, "define", 5, 0 });
	rules.push_back({ 2, "define", 5, 0 });
	rules.push_back({ 3, "define", 5, 0 });
	rules.push_back({ 4, "define", 5, 0 });
	rules.push_back({ 5, "define", 5, 0 });
	rules.push_back({ 5, "attr", 6, -1 });
	rules.push_back({ 6, "attr", 6, 5 });
	rules.push_back({ 6, "node", 1, 0 });
	rules.push_back({ 6, "symbol", 4, 0 });
	rules.push_back({ 6, "define", 5, 0 });
	
	common::cSimpleData2 sdata(rules);
	sdata.Read(fileName);
	RETV(!sdata.m_root, false);

	for (auto &p : sdata.m_root->children)
	{
		if (p->name == "node")
		{
			sNode node;
			node.type = eNodeType::FromString(sdata.Get<string>(p, "type", "Event"));
			node.id = sdata.Get<int>(p, "id", 0);
			node.name = sdata.Get<string>(p, "name", "name");
			for (auto &c : p->children)
			{
				if ((c->name == "output") || (c->name == "input"))
				{
					sPin pin;

					const string typeStr = sdata.Get<string>(c, "type", " ");
					pin.typeStr = typeStr;
					pin.type = ePinType::FromString(typeStr);
					if (pin.type == ePinType::COUNT) // not found type, check enum type
					{
						if (m_variables.FindSymbol(typeStr))
						{
							pin.type = ePinType::Enums;
						}
						else
						{
							assert(!"cVProgFile::Read() Error!, not defined type name");
							pin.type = ePinType::Int;
						}
					}

					pin.id = sdata.Get<int>(c, "id", 0);
					pin.name = sdata.Get<string>(c, "name", "name");
					auto &ar = sdata.GetArray(c, "links");
					for (uint i = 1; i < ar.size(); ++i)
					{
						//todo: maybe link id duplicated
						const int toPinId = atoi(ar[i].c_str());
						pin.links.push_back(toPinId);
					}

					if (c->name == "output")
					{
						pin.kind = ePinKind::Output;
						node.outputs.push_back(pin);
					}
					else // input
					{
						pin.kind = ePinKind::Input;
						node.inputs.push_back(pin);
					}
				}
			}
			m_nodes.push_back(node);
		}
		else if (p->name == "symbol")
		{
			sPin pin;
			pin.name = "@symbol@";
			pin.id = sdata.Get<int>(p, "id", 0);

			// add variable table
			sNode *n = nullptr;
			sPin *pp = nullptr;
			std::tie(n, pp) = FindContainPin(pin.id);
			if (n && pp)
			{
				string scopeName = MakeScopeName(*n);
				string varName = pp->name.c_str();
				variant_t val;
				switch (pp->type)
				{
				case ePinType::Bool: val = sdata.Get<bool>(p, "value", false); break;
				case ePinType::Enums:
				case ePinType::Int: val = sdata.Get<int>(p, "value", 0); break;
				case ePinType::Float: val = sdata.Get<float>(p, "value", 0.f); break;
				case ePinType::String: 
					val = common::str2variant(VT_BSTR
						, sdata.Get<string>(p, "value", ""));
					break;
				default:
					common::dbg::Logc(1
						, "Error!! cVProgFile::Read() symbol parse error!!\n");
					break;
				}

				if (!m_variables.Set(scopeName, varName, val))
				{
					common::dbg::Logc(1
						, "Error!! cVProgFile::Read() symbol parse error!!\n");
				}
				common::clearvariant(val);
			}
			else
			{
				common::dbg::Logc(1
					, "Error!! cVProgFile::Read() symbol parse error!!\n");
			}
		}
		else if (p->name == "define")
		{
			namespace script = common::script;
			script::cSymbolTable::sSymbol type;
			const string typeStr = sdata.Get<string>(p, "type", "Enum");
			type.type = (typeStr == "Enum") ?
				script::cSymbolTable::eType::Enum : script::cSymbolTable::eType::None;
			type.name = sdata.Get<string>(p, "name", " ");

			if (type.type == script::cSymbolTable::eType::Enum)
			{
				for (auto &c : p->children)
				{
					if (c->name == "attr")
					{
						script::cSymbolTable::sEnum e;
						e.name = sdata.Get<string>(c, "name", " ");
						e.value = sdata.Get<int>(c, "value", 0);
						type.enums.push_back(e);
					}
				}
			}
			else
			{
				assert(!"cVProgFile::Read() Error, not defined type parse");
			}

			m_variables.AddSymbol(type);
		}
		else
		{
			assert(!"cVProgFile::Read() Error, not defined node type");
			break;
		}
	} //~for nodes, type, symbol

	return true;
}


bool cVProgFile::Write(const StrPath &fileName)
{
	// nothing~
	return true;
}


// generate script intermediate code
bool cVProgFile::GenerateIntermediateCode(OUT common::script::cIntermediateCode &out)
{
	RETV(m_nodes.empty(), false);

	m_visit.clear(); // avoid duplicate execution

	// make main branch
	{
		out.m_codes.push_back({ script::eCommand::nop });
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = "main";
		out.m_codes.push_back(code);
		out.m_codes.push_back({ script::eCommand::nop });
	}

	// make intermediate code
	for (auto &node : m_nodes)
		if (eNodeType::Event == node.type)
			GenerateCode_Event(node, out);

	// make blank branch
	{
		out.m_codes.push_back({ script::eCommand::nop });
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = "blank";
		out.m_codes.push_back(code);
		out.m_codes.push_back({ script::eCommand::nop });
	}

	// make intermediate code, insert initial symbol
	for (auto &kv1 : m_variables.m_vars)
	{
		for (auto &kv2 : kv1.second)
		{
			script::sInstruction code;
			switch (kv2.second.var.vt)
			{
			case VT_BOOL: code.cmd = script::eCommand::symbolb; break;
			case VT_INT: code.cmd = script::eCommand::symboli; break;
			case VT_R4: code.cmd = script::eCommand::symbolf; break;
			case VT_BSTR: code.cmd = script::eCommand::symbols; break;				
			default:
				common::dbg::Logc(3, "Error!! cVProgFile::GenerateIntermediateCode(), invalid symbol type\n");
				break;
			}
			code.str1 = kv1.first;
			code.str2 = kv2.first;
			code.var1 = kv2.second.var;
			out.m_codes.push_back(code);
		}
	}

	out.m_variables = m_variables;
	out.m_fileName = m_fileName.GetFileNameExceptExt2();
	out.m_fileName += ".icode";

	return true;
}


// generate intermediate code, event node
bool cVProgFile::GenerateCode_Event(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Event, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	out.m_codes.push_back({ script::eCommand::nop });
	const string labelName = node.name.c_str();
	out.m_codes.push_back({ script::eCommand::label, labelName });

	for (auto &pin : node.outputs)
	{
		if ((ePinType::Flow == pin.type) && (!pin.links.empty()))
		{
			if (pin.links.size() >= 2)
				common::dbg::Logc(3, "Error!! cVProgFile::Generate intermediate code, flow link too many setting \n");

			sNode *next = nullptr;
			sPin *np = nullptr;
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (!next)
				continue; // error occurred!!
			GenerateCode_Node(node, *next, out);
		}
	}

	out.m_codes.push_back({ script::eCommand::nop });
	return true;
}


// generate intermediate code, node
bool cVProgFile::GenerateCode_Node(const sNode &prevNode, const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	switch (node.type)
	{
	case eNodeType::Function:
	case eNodeType::Macro: GenerateCode_Function(prevNode, node, out); break;
	case eNodeType::Control: GenerateCode_Control(prevNode, node, out); break;
	case eNodeType::Operator: GenerateCode_Operator(node, out); break;
	case eNodeType::Variable: GenerateCode_Variable(node, out); break;
		break;
	default:
		common::dbg::Logc(1
			, "Error!! cVProgFile::GenerateCode_Node(), node type is invalid\n");
		return false; // nothing generate this type
	}

	return true;
}


// generate intermediate code, function node type
bool cVProgFile::GenerateCode_Function(const sNode &prevNode, const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	// function, macro type
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	GenerateCode_DebugInfo(prevNode, node, out);

	// get input variable
	const uint reg = 0;
	for (auto &pin : node.inputs)
	{
		switch (pin.type)
		{
		case ePinType::Flow:
		case ePinType::Function:
		case ePinType::Delegate:
			continue;
		}

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(nullNode, *prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from prev output pin
			GenerateCode_Pin(node, pin, reg, out); // set data to input pin
			GenerateCode_DebugInfo(*pp, pin, out); // insert debuginfo
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			GenerateCode_Pin(node, pin, reg, out); // set data to input pin
		}
	}

	// call function
	script::sInstruction code;
	code.cmd = script::eCommand::call;
	code.str1 = MakeScopeName(node);
	out.m_codes.push_back(code);

	// next flow node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next)
			{
				GenerateCode_Node(node, *next, out);
			}
		}
	}

	return true;
}


// generate intermediate code, control node
bool cVProgFile::GenerateCode_Control(const sNode &prevNode, const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	if (node.name == "Branch")
		GenerateCode_Branch(prevNode, node, out);
	else if (node.name == "Switch")
		GenerateCode_Switch(prevNode, node, out);

	return true;
}


// generate intermediate code, control node
bool cVProgFile::GenerateCode_Branch(const sNode &prevNode, const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	GenerateCode_DebugInfo(prevNode, node, out);

	// get input variable
	uint reg = 0;
	uint condReg = 0; // condition register
	for (auto &pin : node.inputs)
	{
		switch (pin.type)
		{
		case ePinType::Flow:
		case ePinType::Function:
		case ePinType::Delegate:
			continue;
		}

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(nullNode, *prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from prev output pin
			GenerateCode_DebugInfo(*pp, pin, out); // insert debuginfo
			++reg;
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			++reg;
		}

		if (pin.name == "Condition")
			condReg = reg - 1;
	}

	// insert branch code

	// compare condition is zero?  (false condition)
	{
		script::sInstruction code;
		code.cmd = script::eCommand::eqic;
		code.reg1 = condReg;
		code.var1 = variant_t((int)0);
		out.m_codes.push_back(code);
	}
	{
		// jump if true, jump false flow
		// find False branch pin
		string jumpLabel;
		for (auto &pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type)
				&& (pin.name == "False"))
			{
				sNode *next = nullptr; // next node
				sPin *np = nullptr; // next pin
				const int linkId = pin.links.empty() ? -1 : pin.links.front();
				std::tie(next, np) = FindContainPin(linkId);
				if (next)
					jumpLabel = MakeScopeName(*next);
			}
		}

		if (jumpLabel.empty())
		{
			// no branch node, jump blank code
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = "blank";
			out.m_codes.push_back(code);
			common::dbg::Logc(1, "cVProgFile::GenerateCode_Branch, no branch label\n");
		}
		else
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = jumpLabel;
			out.m_codes.push_back(code);
		}
	}

	// next flow node (generate True branch node)
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "True"))
			{
				GenerateCode_Node(node, *next, out);
			}
		}
	}

	// generate False branch node
	// seperate from this node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "False"))
			{
				// insert nop
				{
					script::sInstruction code;
					code.cmd = script::eCommand::nop;
					out.m_codes.push_back(code);
				}
				// insert jump label
				{
					script::sInstruction code;
					code.cmd = script::eCommand::label;
					code.str1 = MakeScopeName(*next);
					out.m_codes.push_back(code);
				}

				GenerateCode_Node(node, *next, out);
				break;
			}
		}
	}

	return true;
}


// generate intermediate code, switch case node
bool cVProgFile::GenerateCode_Switch(const sNode &prevNode, const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	GenerateCode_DebugInfo(prevNode, node, out);

	// get input variable
	const sPin *selPin = nullptr;
	for (auto &pin : node.inputs)
	{
		if (pin.name != "Selection")
			continue;

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(nullNode, *prev, out);

			selPin = &pin;
			GenerateCode_Pin(*prev, *pp, 0, out); // get data from prev output pin
			GenerateCode_DebugInfo(*pp, pin, out); // insert debuginfo
		}
		else
		{
			// load temporal variable
			selPin = &pin;
			GenerateCode_TemporalPin(node, pin, 0, out);
		}
		break;
	}

	if (!selPin)
		return false; // error occurred!!

	common::script::cSymbolTable::sSymbol *symbol = nullptr;
	if (ePinType::Enums == selPin->type)
		symbol = m_variables.FindSymbol(selPin->typeStr.c_str());

	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow != pin.type)
			continue;
		if (pin.name == "Default")
			continue; // not yet

		int value = pin.value;
		if (symbol)
		{
			auto it = std::find_if(symbol->enums.begin(), symbol->enums.end()
				, [&](const auto &a) {return a.name == pin.name; });
			if (symbol->enums.end() == it)
			{
				assert(!"cVProgFile::GenerateCode_Switch() error");
				continue; // error occurred!!
			}
			value = it->value;
		}
		else
		{
			// int selection type, name is value
			value = atoi(pin.name.c_str());
		}

		string jumpLabel = "blank";
		sNode *next = nullptr; // next node
		sPin *np = nullptr; // next pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (next)
			jumpLabel = MakeScopeName(*next);

		// compare reg0, enum value (int type)
		// if result is true, jump correspond flow code
		{
			script::sInstruction code;
			code.cmd = script::eCommand::eqic;
			code.reg1 = 0;
			code.var1 = value;
			out.m_codes.push_back(code);
		}
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = jumpLabel;
			out.m_codes.push_back(code);
		}
	}//~for

	// jump default case
	auto it = std::find_if(node.outputs.begin(), node.outputs.end()
		, [&](const auto &a) {return a.name == "Default"; });
	if (node.outputs.end() != it)
	{
		auto &pin = *it;
		if (!pin.links.empty())
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next)
			{
				string jumpLabel = MakeScopeName(*next);

				script::sInstruction code;
				code.cmd = script::eCommand::jmp;
				code.str1 = jumpLabel;
				out.m_codes.push_back(code);
			}
		}
	}

	// generate output flow node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow != pin.type)
			continue;

		sNode *next = nullptr; // next node
		sPin *np = nullptr; // next pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (!next)
			continue; // not connect flow

		// insert nop
		{
			script::sInstruction code;
			code.cmd = script::eCommand::nop;
			out.m_codes.push_back(code);
		}
		// insert jump label
		{
			script::sInstruction code;
			code.cmd = script::eCommand::label;
			code.str1 = MakeScopeName(*next);
			out.m_codes.push_back(code);
		}

		GenerateCode_Node(node, *next, out);
	}

	return true;
}


// generate intermediate code, operator node
bool cVProgFile::GenerateCode_Operator(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Operator, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// get input variable
	uint reg = 8;
	for (auto &pin : node.inputs)
	{
		switch (pin.type)
		{
		case ePinType::Flow:
		case ePinType::Function:
		case ePinType::Delegate:
			continue;
		}

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(nullNode, *prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from prev output pin
			GenerateCode_DebugInfo(*pp, pin, out); // insert debuginfo
			++reg;
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			++reg;
		}
	}

	// get operator type (by input pin type)
	VARTYPE vt = VT_EMPTY;
	for (auto &pin : node.inputs)
	{
		vt = vprog::GetPin2VarType(pin.type);
		if (VT_EMPTY != vt)
			break;
	}

	// insert compare code
	if (node.name == "<")
	{
		script::sInstruction code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::lesi; break;
		case VT_R4: code.cmd = script::eCommand::lesf; break;
		default: 
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == "<=")
	{
		script::sInstruction code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::leqi; break;
		case VT_R4: code.cmd = script::eCommand::leqf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == ">")
	{
		script::sInstruction code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::gri; break;
		case VT_R4: code.cmd = script::eCommand::grf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == ">=")
	{
		script::sInstruction code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::greqi; break;
		case VT_R4: code.cmd = script::eCommand::greqf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == "==")
	{
		script::sInstruction code;
		switch (vt)
		{
		case VT_BOOL:
		case VT_INT: code.cmd = script::eCommand::eqi; break;
		case VT_R4: code.cmd = script::eCommand::eqf; break;
		case VT_BSTR: code.cmd = script::eCommand::eqs; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}

	// load compare flag
	{
		script::sInstruction code;
		code.cmd = script::eCommand::ldcmp;
		code.reg1 = 9;
		out.m_codes.push_back(code);
	}

	// insert compare data to symboltable
	{
		script::sInstruction code;
		code.cmd = script::eCommand::setb;
		code.str1 = MakeScopeName(node);
		code.str2 = "O";
		code.reg1 = 9;
		out.m_codes.push_back(code);
	}

	return true;
}


// generate intermediate code, variable node
bool cVProgFile::GenerateCode_Variable(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Operator, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// nothing to operation

	return true;
}


// generate intermediate code, pin
bool cVProgFile::GenerateCode_Pin(const sNode &node, const sPin &pin, const uint reg
	, OUT common::script::cIntermediateCode &out)
{

	if (ePinKind::Input == pin.kind)
	{
		script::sInstruction code;
		switch (pin.type)
		{
		case ePinType::Bool: code.cmd = script::eCommand::setb; break;
		case ePinType::Enums:
		case ePinType::Int: code.cmd = script::eCommand::seti; break;
		case ePinType::Float: code.cmd = script::eCommand::setf; break;
		case ePinType::String: code.cmd = script::eCommand::sets; break;
		default:
			return false;
		}

		code.str1 = MakeScopeName(node);
		code.str2 = pin.name.c_str();
		code.reg1 = reg;
		out.m_codes.push_back(code);
	}
	else // output
	{
		script::sInstruction code;
		switch (pin.type)
		{
		case ePinType::Bool: code.cmd = script::eCommand::getb; break;
		case ePinType::Enums:
		case ePinType::Int: code.cmd = script::eCommand::geti; break;
		case ePinType::Float: code.cmd = script::eCommand::getf; break;
		case ePinType::String: code.cmd = script::eCommand::gets; break;
		default:
			return false;
		}

		code.str1 = MakeScopeName(node);
		code.str2 = pin.name.c_str();
		code.reg1 = reg;
		out.m_codes.push_back(code);
	}

	return true;
}


// generate intermedate code, load temporal value to register
bool cVProgFile::GenerateCode_TemporalPin(const sNode &node, const sPin &pin
	, const uint reg, OUT common::script::cIntermediateCode &out)
{
	const _bstr_t emptyStr(""); // avoid crash local variable
	if (ePinKind::Input == pin.kind)
	{
		script::sInstruction code;
		switch (pin.type)
		{
		case ePinType::Bool: 
			code.cmd = script::eCommand::ldbc;
			code.var1.vt = VT_BOOL;
			code.var1.boolVal = false;
			code.reg1 = reg;
			break;
		case ePinType::Enums:
		case ePinType::Int: 
			code.cmd = script::eCommand::ldic; 
			code.var1.vt = VT_INT;
			code.var1.intVal = 0;
			code.reg1 = reg;
			break;
		case ePinType::Float: 
			code.cmd = script::eCommand::ldfc; 
			code.var1.vt = VT_R4;
			code.var1.fltVal = 0.f;
			code.reg1 = reg;
			break;
		case ePinType::String: 
			code.cmd = script::eCommand::ldsc; 
			code.var1.vt = VT_BSTR;
			code.var1.bstrVal = emptyStr;
			code.reg1 = reg;
			break;
		default:
			return false;
		}
		out.m_codes.push_back(code);
	}
	else // output
	{
		// nothing~
	}

	return true;
}


// insert flow debug information
bool cVProgFile::GenerateCode_DebugInfo(const sPin &from, const sPin &to
	, OUT common::script::cIntermediateCode &out)
{
	script::sInstruction inst;
	inst.cmd = script::eCommand::cmt;
	inst.str1 = "flow";
	inst.reg1 = from.id;
	inst.reg2 = to.id;
	out.m_codes.push_back(inst);
	return true;
}


// insert flow debug information, if from-to node was linked
bool cVProgFile::GenerateCode_DebugInfo(const sNode &from, const sNode &to
	, OUT common::script::cIntermediateCode &out)
{
	RETV(from.id == 0, false);

	int fromId = -1;
	int toId = -1;
	for (auto &p1 : from.outputs)
	{
		if (p1.type != ePinType::Flow)
			continue;
		if ((fromId != -1) && (toId != -1))
			break;
		for (auto &p2 : to.inputs)
		{
			if (p2.type != ePinType::Flow)
				continue;
			if ((fromId != -1) && (toId != -1))
				break;
			for (auto &id : p2.links)
			{
				if (p1.id == id)
				{
					// two node link with flow pin
					fromId = p1.id;
					toId = p2.id;
					break;
				}
			}
		}
	}

	if ((fromId != -1) && (toId != -1))
	{
		script::sInstruction inst;
		inst.cmd = script::eCommand::cmt;
		inst.str1 = "flow";
		inst.reg1 = fromId;
		inst.reg2 = toId;
		out.m_codes.push_back(inst);
	}

	return true;
}


bool cVProgFile::AddPin(const int parseState, sNode &node, const sPin &pin)
{
	sPin p = pin;
	p.kind = (parseState == 2) ? ePinKind::Input : ePinKind::Output;

	if (parseState == 2) // input
		node.inputs.push_back(p);
	else if (parseState == 3) // output
		node.outputs.push_back(p);
	return true;
}


// return pin contain node
std::pair<cVProgFile::sNode*, cVProgFile::sPin*> 
	cVProgFile::FindContainPin(const int pinId)
{
	if (pinId < 0)
		return std::make_pair(nullptr, nullptr);

	for (auto &node : m_nodes)
	{
		for (auto &pin : node.inputs)
			if (pin.id == pinId)
				return std::make_pair(&node, &pin);
		for (auto &pin : node.outputs)
			if (pin.id == pinId)
				return std::make_pair(&node, &pin);
	}
	return std::make_pair(nullptr, nullptr);
}


string cVProgFile::MakeScopeName(const sNode &node)
{
	if (eNodeType::Event == node.type)
		return node.name.c_str();
	else
		return script::cSymbolTable::MakeScopeName(node.name.c_str(), node.id).c_str();
}


void cVProgFile::Clear()
{
	m_nodes.clear();
	m_variables.Clear();
}
