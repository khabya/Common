
#include "stdafx.h"
#include "log.h"
#include <direct.h>
#include "packetlog.h"


namespace network2
{
	StrPath g_packetLogPath;

	// Packet Log Thread
	common::cWQSemaphore g_logThread;
}

using namespace network2;


//------------------------------------------------------------------------
// Packet Log Thread Task
class cPacketLogTask : public cTask
					, public common::cMemoryPool4<cPacketLogTask>
{
public:
	sPacketLogData m_logData;
	cPacketLogTask() : cTask(0, "cPacketLogTask") {}
	cPacketLogTask(const sPacketLogData &logData)
		: cTask(0, "cPacketLogTask"), m_logData(logData) {
	}
	virtual ~cPacketLogTask() {
	}

	virtual eRunResult::Enum Run(const double deltaSeconds) override
	{
		switch (m_logData.type)
		{
		case 0: // session log
		{
			const StrPath path = GetPacketLogPath() + "sessions.ses";
			std::ofstream ofs(path.c_str(), std::ios::app);
			if (!ofs.is_open())
			{
				assert(0);
				break;
			}

			ofs << (char*)m_logData.data << std::endl;
		}
		break;

		case 1: // packet log
		{
			StrPath path;
			path.Format("%s%d.plog", GetPacketLogPath().c_str(), m_logData.rcvId);
			cPacketLog::Write(path.c_str(), m_logData);
		}
		break;

		default: assert(0); break;
		}
		return eRunResult::END;
	}
};
//-----------------------------------------------------------------------


// set packet log directory path
// ex) SetPacket("log_packet", "20190309102033112")
//	-> set directory "log_packet/20190309102033112/"
void network2::SetPacketLogPath(const string &logFolderName, const string &subFolderName)
{
	_mkdir(logFolderName.c_str());
	g_packetLogPath = logFolderName + "/" + subFolderName + "/";
	_mkdir(g_packetLogPath.c_str());
}


const StrPath& network2::GetPacketLogPath()
{
	// you must set packet log directory path
	// ex) ./log_packet/yyyymmddhhmmssmmm/
	assert(!g_packetLogPath.empty());

	return g_packetLogPath;
}


// write file with session data
bool network2::LogSession(const cSession &session)
{
	cPacketLogTask *task = new cPacketLogTask();
	task->m_logData.type = 0;
	
	if (session.m_name.empty())
	{
		sprintf_s((char*)task->m_logData.data, sizeof(task->m_logData.data)
			, "%s:%d %d", session.m_ip.c_str(), session.m_port, session.m_id);
	}
	else
	{
		sprintf_s((char*)task->m_logData.data, sizeof(task->m_logData.data)
			, "%s %d", session.m_name.c_str(), session.m_id);
	}

	g_logThread.PushTask(task);

	return true;
}


// write packet data to log file
// read by network2::cPacketLog
bool network2::LogPacket(const netid rcvId, const cPacket &packet)
{
	cPacketLogTask *task = new cPacketLogTask();
	task->m_logData.type = 1;
	task->m_logData.sndId = packet.GetSenderId();
	task->m_logData.rcvId = rcvId;
	memcpy(task->m_logData.data, packet.m_data, packet.GetPacketSize());
	task->m_logData.size = packet.GetPacketSize();
	
	g_logThread.PushTask(task);	
	return true;
}
