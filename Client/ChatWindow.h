#pragma once
#include "..\bootstrap\imgui_glfw3.h"
#include <vector>
#include <string>
#include <sstream>
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>	
#include "ChatServer.h"
#include "imgui_tabs.h"
#include <sstream>


#define BUFFER_SIZE 1000

class ChatWindow 
{
public:
	ChatWindow(const char* windowName, int clientID, std::string clientName, RakNet::RakPeerInterface* pPeerInterface);
	~ChatWindow();

	void Draw();

	void SetClientID(int clientID);

	void SetChatID(int chatID);

	int GetChatID();
	
	void AddMessage(std::string message);

	const char* GetChatName();

	void AddChatRecipient(RakNet::SystemAddress address);

	std::string m_chatNameStr;

protected:
	std::vector<std::string> SplitByHyperlink(const char* line, const char* splitBy);

	void SendText();

	void SendTextToAddress(RakNet::SystemAddress address);

	void SendChatInvite();

private:
	std::vector<std::string> m_messages;
	std::vector<RakNet::SystemAddress> m_chatRecipients; //proof of concept for now

	//potential issues
	//could become slow if there are many keys being held, because a boundary check must be performed for all of them
	std::map<std::pair<ImVec2, ImVec2>, std::string> m_hyperLinkPositions;

	char m_textInField[BUFFER_SIZE] = "Enter text";
	char m_textInInviteField[BUFFER_SIZE] = "Enter name";
	const char* m_windowName; //so you can attach it to the main window
	const char* m_chatName; //individual chat name, like Chat 1 or Friends
	int m_clientID;
	int m_chatID;

	static int m_windowCount;

	std::stringstream ss;
	std::string m_clientName;

	RakNet::RakPeerInterface *m_peerInterface;
};

