//
// 2019-01-08, jjuiddong
// server, client launcher
//
#pragma once


namespace network2
{

	bool LaunchTCPClient(const std::string &ip, const int port
		, OUT SOCKET &out, const bool isLog = true, const int clientSidePort = -1);

	bool LaunchUDPClient(const std::string &ip, const int port
		, OUT SOCKADDR_IN &sockAddr, OUT SOCKET &out, const bool isLog = true);

	bool LaunchTCPServer(const int port, OUT SOCKET &out, const bool isLog = true);

	bool LaunchUDPServer(const int port, OUT SOCKET &out, const bool isLog = true);

}