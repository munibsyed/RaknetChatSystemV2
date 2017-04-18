#pragma once
#define BUFFER_SIZE 1000 

#include "Application.h"
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>	
#include <FileList.h>
#include <FileListTransfer.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <unordered_map>
#include "ChatServer.h"
#include "ChatWindow.h"
#include "PopupWindow.h"
#include <thread>
#include <map>
#include <sstream>
#include "ColorEditor.h"
#include "OpenFileDialog.h"
#include "CallbackInterfaceClient.h"

class Client : public aie::Application {
public:

	Client();
	virtual ~Client();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();
	
	void OnSetClientIDPacket(RakNet::Packet * packet);

	void OnDisconnectionSendPacket();

	void SendFileTest();

	void SendScore();

	//void SendChatID();

	void CreateNewChatRequest();

	void DisplayHighScoresRequest();

	//Initialize the connections
	void HandleNetworkConnections();
	void InitializeClientConnection();

	void HandleNetworkMessages(bool loop);

	int ParseIDNumber(const char * str);

protected:
	RakNet::RakPeerInterface *m_pPeerInterface;
	RakNet::FileListTransfer *m_fileTransfer;
	const char* IP = "127.0.0.1" /*"122.99.85.44"*/;
	const unsigned short PORT = 5456;
	
	char textInField[BUFFER_SIZE] = "Enter text";
	std::vector<std::string> m_recentMessages;
	
	int m_sendPacketInterval;
	int m_sendPacketCounter;
	int m_receiveFileID;
	int m_sendFileID;
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	DWORD m_processID;
	unsigned int m_clientID;
	std::string m_clientName;
	RakNet::SystemAddress serverAddress;

	//ChatWindow *m_chatWindow;
	
	std::vector<ChatWindow*> m_chatWindows;
	std::vector<PopupWindow*> m_popupWindows;

	OpenFileDialog *m_openFileDialog;
	ColorEditor *m_colourEditor;

	//File receiving stuff

};