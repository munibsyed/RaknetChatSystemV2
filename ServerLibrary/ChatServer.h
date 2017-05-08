#pragma once
#include <iostream>
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>	
#include <FileListTransfer.h>
#include <FileListTransferCBInterface.h>
#include "CallbackInterface.h"
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>
#include <exception>
#include <fstream>

enum Messages
{
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_CHAT_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_CHAT_MESSAGE_WITH_ADDRESS,
	ID_SERVER_SET_CLIENT_ID,
	ID_SERVER_SET_FILE_SEND_ID,
	ID_CLIENT_SET_FILE_SEND_ID,
	ID_CLIENT_CLIENT_DATA,
	ID_DISCONNECTED_CLIENT_ID,
	ID_DISPLAY_HIGH_SCORES_REQUEST,
	ID_SUBMIT_NEW_SCORE,
	ID_CREATE_NEW_CHAT_REQUEST,
	ID_ADD_ADDRESS_TO_CHAT_ID,
	ID_CHAT_INVITE,
	ID_SEND_INVITED_CLIENT_ADDRESS,
	ID_SEND_INVITED_CLIENT_ADDRESS_FAIL,
	ID_FILE_DATA,
	ID_REINITIALIZE_FILE_HANDLERS,
	ID_SEND_CONVERSATION_LOG

};

//if multiple conversations are to be held at once, some design changes will have to be made

/*
Clients will members of numbered chats. E.g. Client 1 could be part of chats 1 and 2, but not 3.
Clients will store what chats they are in.
When a client sends a message to the server, the packet will contain what chat this message came from
The server will send this message back to only the clients who are part of that chat. The server will probably have some kind of map structure
within, something along the lines of std::map<int, std::vector<int>> m_clientChats = {{0,{1,2,3}, {1,{1,2}};

There will probably far less broadcasting and more sending packets from the server directly to particular system addresses.

*/

class ChatServer
{
public:
	ChatServer(const unsigned short port, unsigned int maxClients, bool persistentData);
	~ChatServer();

	void Update();

	void ReinitializeFileHandlers();

protected:

	void SendFile(RakNet::FileList *fList, std::string id);

	void SendFileSendID(int fileSendID);

	void SendFileSendID(int fileSendID, RakNet::SystemAddress &address);

	void SendNewClientID(RakNet::SystemAddress &address);

	void SendDisconectionNotification(RakNet::Packet *packet);

	void ReceiveAndSendChatMessage(RakNet::Packet *packet);

	void ReceiveAndSendChatMessageToAddress(RakNet::Packet *packet);

private:
	const unsigned short PORT;
	unsigned int m_maxClients;
	RakNet::RakPeerInterface *m_peerInterface;
	RakNet::FileListTransfer *m_fileListTransfer;
	std::vector<RakNet::FileListTransfer*> m_fileListTransferList;
	CallbackInterface *m_callbackInterface;
	CallbackInterface *m_callbackInterface2;

	int m_nextChatID;
	int m_nextClientID;
	int m_fileSendID;
	bool m_runUpdateLoop;
	bool m_sendChatMsgBackToSender;

	bool m_persistentData;

	std::map<int, std::vector<RakNet::SystemAddress>> m_chatIDToClientAddresses;
	std::map<std::string, RakNet::SystemAddress> m_clientNameToAddress;
	std::map<std::string, int> m_clientNameToSendID;
};

