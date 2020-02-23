#pragma once

#include <memory>
#include "MemPool.hpp"
#include "SocketDefine.h"

class Session;

class ClientPlayer : public MemElem
{
public:
	ClientPlayer(){}
	~ClientPlayer(){}
	virtual bool Init();
	virtual bool Clear();
	int GetId();
	socket_t GetSockFd();
	uint64_t GetPlayerId() { return m_playerid; }   // Ĭ��playeridΪmemid
	void SetSceneId(int sceneId) { m_sceneid = sceneId; }
	void SetLastSceneId(int sceneId) { m_lastsceneid = sceneId; }
	void SetSession(Session* pSession) { m_session = pSession; }
	void SetConnTime(int64_t ti) { m_conntime = ti; }
	void SetPlayerId(uint64_t playerId) { m_playerid = playerId; }
public:
	bool OnModuleGateMessage(const int msg_id, const char* msg, size_t msg_len, socket_t sock_fd);
	bool OnModuleLoginMessage(const int msg_id, const char* msg, size_t msg_len);
	bool OnModuleGameMessage(const int msg_id, const char* msg, size_t msg_len);
	bool OnModuleChatMessage(const int msg_id, const char* msg, size_t msg_len);
	bool OnModuleWorldMessage(const int msg_id, const char* msg, size_t msg_len);
	void SendToClient(const int msg_id, const std::string& msg);
private:
	uint64_t m_playerid;
	int m_lineid;
	int m_sceneid;
	int m_lastsceneid;
	int64_t m_conntime;
	Session* m_session;
};

