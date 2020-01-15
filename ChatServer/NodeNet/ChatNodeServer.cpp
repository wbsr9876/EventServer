#include "ChatNodeServer.h"
#include "JsonConfig.h"
#include "LogHelper.h"
#include "SeFNodeNet.pb.h"
#include "SeFNet.h"


bool ChatNodeServer::InitHelper()
{
	mNetServModule->AddReceiveCallBack(GAME_ROUTE_TO_CHAT, this, &ChatNodeServer::OnGameRouteBack);

	//init server info
	if (!mNetServModule->InitNet(g_JsonConfig->m_ServerConf["NodePort"].asUInt()))
	{
		LOG_ERR("init ChatNodeServer failed");
		return false;
	}
	CLOG_INFO << "init ChatNodeServer ok" << CLOG_END;
	return true;
}


bool ChatNodeServer::SendPackToGame(int nServerID, const int msgid, google::protobuf::Message* xData)
{

	return true;
}

bool ChatNodeServer::SendPackToGame(const int msgid, google::protobuf::Message* xData)
{
	std::string send_msg = xData.SerializeAsString();
	ChatToGamePacket game_packet;
	game_packet.set_msg_id(msgid);
	game_packet.set_msg_body(send_msg);
	game_packet.set_player_id(0);
	SendPbByServType(ServerType::SERVER_TYPE_GAME, CHAT_ROUTE_TO_GAME, &game_packet);
	return true;
}

void ChatNodeServer::OnGameRouteBack(socket_t nFd, const int msgid, const char* msg, const uint32_t nLen)
{
}

