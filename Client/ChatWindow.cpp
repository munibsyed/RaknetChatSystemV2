#include "ChatWindow.h"



ChatWindow::ChatWindow(const char* windowName, int clientID, std::string clientName, RakNet::RakPeerInterface* pPeerInterface)
{
	m_windowName = windowName;
	m_clientID = clientID;
	m_clientName = clientName;
	m_peerInterface = pPeerInterface;
	m_windowCount++;
	ss << "Chat " << m_windowCount;

	m_chatNameStr = ss.str();
}


ChatWindow::~ChatWindow()
{
}

void ChatWindow::Draw()
{
	ImGui::Begin(m_windowName);

	//just use client name
	std::stringstream ss;
	ss << "Client " << m_clientID;

	ImGui::InputTextMultiline(m_clientName.c_str(), m_textInField, BUFFER_SIZE);
	if (ImGui::Button("Send") && m_textInField[0] != 0)
	{
		SendText();
		memset(m_textInField, 0, sizeof(m_textInField));
	}

	std::string toPrint = "";

	for (int i = 0; i < m_messages.size(); i++)
	{
		std::vector<std::string> split = SplitByHyperlink(m_messages[i].c_str());
		ImVec2 colPos = ImGui::GetCursorPos();

		for (int i = 0; i < split.size(); i++)
		{
			if (split[i].find("https://") != std::string::npos)
			{
				ImGui::TextColored(ImVec4(0, 0, 0.933f, 1), split[i].c_str());
				//ImGui::TextColored(ImVec4(0.2f, 0.4f, 0.733f, 1), split[i].c_str());
			}
			else
			{
				ImGui::Text(split[i].c_str());
			}
			colPos.x += ImGui::CalcTextSize(split[i].c_str()).x;
			ImGui::SameLine(colPos.x);
		}
		ImGui::Text("\n");

	}

	ImGui::InputText("", m_textInInviteField, BUFFER_SIZE);
	if (ImGui::Button("Send Invite"))
	{
		SendChatInvite();
	}
	//std::stringstream stream;
	//stream << "Chat ID: " << m_chatID;
	//std::string s = stream.str();
	//const char* result = s.c_str();
	//ImGui::Text(result);


	ImGui::End();
}

void ChatWindow::SetClientID(int clientID)
{
	m_clientID = clientID;
}

void ChatWindow::SetChatID(int chatID)
{
	m_chatID = chatID;
}

int ChatWindow::GetChatID()
{
	return m_chatID;
}

void ChatWindow::AddMessage(std::string message)
{
	m_messages.push_back(message);
}

const char * ChatWindow::GetChatName()
{
	return ss.str().c_str();
}

void ChatWindow::AddChatRecipient(RakNet::SystemAddress address)
{
	std::cout << "Recieved address: " << address.ToString() << std::endl;
	m_chatRecipients.push_back(address);
}

std::vector<std::string> ChatWindow::SplitByHyperlink(const char * line)
{
	//if no hyperlinks, return 1 element vector with original line

	//if hyperlinks, split line into (non-hyperlink, hyperlink, non-hyperlink) format and return as vector
	std::vector<std::string> lineSplit;
	std::string lineStr = line;
	size_t found = lineStr.find("https://");
	int begin = 0;
	while (found != std::string::npos)
	{
		//take begin to found string and add it to lineSplit
		std::string subStr = lineStr.substr(begin, found);
		lineStr = lineStr.substr(found, lineStr.length());
		lineSplit.push_back(subStr);
		size_t foundSpace = lineStr.find(" ");

		//if there was a space
		if (foundSpace != std::string::npos)
		{
			//get hyperlink by going 0 - foundSpace on shorter string
			std::string hyperLink = lineStr.substr(0, foundSpace);
			lineSplit.push_back(hyperLink);
			lineStr = lineStr.substr(foundSpace, lineStr.length());
		}

		//if not
		else
		{
			std::string hyperLink = lineStr.substr(0, lineStr.length());
			lineSplit.push_back(hyperLink);
			lineStr = "";
		}
		found = lineStr.find("https://");
	}

	lineSplit.push_back(lineStr);

	return lineSplit;
}

void ChatWindow::SendText()
{
	std::stringstream ss;
	ss << "(Client " << m_clientID << "): " << m_textInField << '[' << m_chatID;
	RakNet::BitStream bs;
	bs.Write((unsigned char)ID_CHAT_MESSAGE);
	bs.Write(ss.str().c_str());
	m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void ChatWindow::SendChatInvite()
{
	std::stringstream ss;
	ss << m_textInInviteField << "[" << m_chatID;
	RakNet::BitStream bs;
	bs.Write((unsigned char)ID_CHAT_INVITE);
	bs.Write(ss.str().c_str());

	m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

int ChatWindow::m_windowCount = 0;
