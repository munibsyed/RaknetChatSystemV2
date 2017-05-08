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
#include "..\bootstrap\Texture.h"


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

	void SendFileReceiveID(int id);

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
	
	//for stress testing
	int m_sendPacketInterval;
	int m_sendPacketCounter;

	//for file transfer
	int m_receiveFileID;
	int m_sendFileID;

	//for rendering of 3D scene
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_clientID;
	std::string m_clientName;
	RakNet::SystemAddress serverAddress;

	
	std::vector<ChatWindow*> m_chatWindows;
	std::vector<PopupWindow*> m_popupWindows;

	std::vector<aie::Texture*> m_textures;
	std::vector<std::pair<ImTextureID, std::string>> m_textureIDs;

	OpenFileDialog *m_openFileDialog;
	ColorEditor *m_colourEditor;
	CallbackInterfaceClient *m_callbackInterface;
	//File receiving stuff

	//Threads
	std::vector<std::thread> m_fileSendThreads;

	//For the file send button
	ImVec4 m_ImGuiButtonColour;
	ImVec4 m_ImGuiButtonHoveredColour;
	ImVec4 m_ImGuiButtonActiveColour;

	bool m_isSendingFile; //is the Send File function still in progress?
};