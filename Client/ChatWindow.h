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
#include "..\bootstrap\Input.h"
#include <sstream>
#include <ctime>

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

	void SetTextureIDs(std::vector<std::pair<ImTextureID, std::string>> *textureIDs);

	std::string m_chatNameStr;
protected:
	std::vector<std::string> SplitBy(const char* line, const char* splitBy);

	std::vector<std::string> SplitBy(const char * line, const char * splitBy1, const char * splitBy2);

	void SendText();

	void SendTextToAddress(RakNet::SystemAddress address);

	void SendChatInvite();


private:
	std::vector<std::string> m_messages;
	std::vector<RakNet::SystemAddress> m_chatRecipients; //proof of concept for now

	//potential issues
	//could become slow if there are many keys being held, because a boundary check must be performed for all of them
	std::map<std::pair<ImVec2, ImVec2>, std::string> m_hyperLinkPositions;
	std::map<std::string, std::string> m_hyperLinkPositionsStr;

	//store boundaries for all messages, so that the timestamp tooltip can appear when hovered
	std::map<int, std::string> m_messageTimestamps;

	char m_textInField[BUFFER_SIZE] = "Enter text";
	char m_textInInviteField[BUFFER_SIZE] = "Enter name";
	char m_searchConversationField[BUFFER_SIZE] = "";
	bool m_showSearchField;
	bool m_performedSearch; //modify this everytime a new message is received or the search term has been changed
	const char* m_windowName; //so you can attach it to the main window
	const char* m_chatName; //individual chat name, like Chat 1 or Friends
	int m_clientID;
	int m_chatID;
	
	int m_messageWindowSizeY; //most messages in a line before you have to scroll
	bool m_seenAllMessages;

	bool m_hasDepressedEnter;
	bool m_hasDepressedCtrlF;
	bool m_setInputFieldFocus;

	bool m_showEmojiKeyboard;
	bool m_sortEmojisByUsageFreq;
	static int m_windowCount;

	std::stringstream ss;
	std::string m_clientName;

	std::vector<std::pair<ImTextureID, std::string>> * m_textureIDs;

	RakNet::RakPeerInterface *m_peerInterface;
	int m_mostRecentMessageSeenBy;

	aie::Input *m_input;
};

