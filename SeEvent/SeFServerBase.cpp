#include "SeFServerBase.h"
#include "SeFNodeNet.pb.h"
#include "LogHelper.h"
#include "SeFNet.h"
#include "SeNet.h"

bool SeFServerBase::Init()
{
	mNetServModule = new SeFNet();
	mNetServModule->AddEventCallBack(this, &SeFServerBase::OnClientSocketEvent);
	mNetServModule->AddReceiveCallBack(REPORT_CLIENT_INFO_TO_SERVER, this, &SeFServerBase::OnReportToServer);
	return true;
}

void SeFServerBase::OnClientSocketEvent(const socket_t nFd, const SE_NET_EVENT nEvent, SeNet* pNet)
{
	switch (nEvent)
	{
	case SE_NET_EVENT_CONNECTED:
		OnClientConnected(nFd);
		break;
	case SE_NET_EVENT_EOF:
	case SE_NET_EVENT_ERROR:
	case SE_NET_EVENT_TIMEOUT:
	default:
		OnClientDisconnect(nFd);
		break;
	}
}
void SeFServerBase::OnClientConnected(const socket_t nFd)
{
	LOG_INFO("client %d connected ok", nFd);
}
void SeFServerBase::OnClientDisconnect(const socket_t nFd)
{
	LOG_INFO("client %d disconnected", nFd);
	for (auto it = mmServNodes.begin(); it != mmServNodes.end(); it++)
	{
		if (it->second->fd == nFd)
		{
			auto nServerId = it->second->ServerInfo->server_id();
			mmServNodes.erase(nServerId);
			break;
		}
	}
}

ServerDataPtr SeFServerBase::GetClientNodeData(int nServerId)
{
	auto it = mmServNodes.find(nServerId);
	if (it == mmServNodes.end())
	{
		return nullptr;
	}
	return it->second;
}

void SeFServerBase::OnReportToServer(const socket_t nFd, const int nMsgID, const char* msg, const uint32_t nLen)
{
	ServerDataPtr pServerData = std::make_shared<ServerData>();
	ServerReport report;
	if (!SeNet::ReceivePB(nMsgID, msg, nLen, &report)) return;

	pServerData->fd = nFd;
	pServerData->ServerInfo = std::make_shared<ServerReport>(report);
	mmServNodes.emplace(report.server_id(), pServerData);
	for (auto& it : mmServNodes)
	{
		LOG_INFO("clients(%d : %s)", it.first, it.second->ServerInfo->server_name().c_str());
	}
	AfterReportToServer(pServerData);  // 只有master server调用分发服务器信息给各个服务器的客户端进行互联
}
void SeFServerBase::AfterReportToServer(ServerDataPtr& pReportServerData)
{}

void SeFServerBase::SendByServType(ServerType type, const int nMsgID, const char* msg, int len)
{
	std::vector<socket_t> vec;
	for (auto& it : mmServNodes)
	{
		if (it.second->ServerInfo->server_type() == type)
		{
			vec.emplace_back(it.second->fd);
		}
	}
	if(vec.empty()) return;
	for (auto& it : vec)
	{
		mNetServModule->SendMsg(it, nMsgID, msg, len);
	}
}

void SeFServerBase::SendByServId(int nServerId, const int nMsgID, const char* msg, int len)
{
	auto it = mmServNodes.find(nServerId);
	if (it == mmServNodes.end()) return;
	socket_t fd = it->second->fd;
	mNetServModule->SendMsg(fd, nMsgID, msg, len);
}

void SeFServerBase::SendPbByServType(ServerType type, const int nMsgID, ::google::protobuf::Message* pMsg)
{
	std::vector<socket_t> vec;
	for (auto& it : mmServNodes)
	{
		if (it.second->ServerInfo->server_type() == type)
		{
			vec.emplace_back(it.second->fd);
		}
	}
	if (vec.empty()) return;
	for (auto& it : vec)
	{
		mNetServModule->SendPbMsg(it, nMsgID, pMsg);
	}
}

void SeFServerBase::SendPbByServId(int nServerId, const int nMsgID, ::google::protobuf::Message* pMsg)
{
	auto it = mmServNodes.find(nServerId);
	if (it == mmServNodes.end()) return;
	socket_t fd = it->second->fd;
	mNetServModule->SendPbMsg(fd, nMsgID, pMsg);
}

void SeFServerBase::Loop()
{
	mNetServModule->Execute(LOOP_RUN_TYPE::LOOP_RUN_NONBLOCK);
}
