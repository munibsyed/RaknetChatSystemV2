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

bool operator < (const ImVec2& left, const ImVec2& right)
{
	return (left.x < right.x) || (!(left.x > right.x) && (left.x < right.x));
}
template <typename T1, typename T2>
bool operator == (const std::pair<T1, T2> &left, const std::pair<T1, T2> &right)
{
	if (left.first == right.first && left.second == right.second)
		return true;
	else
		return false;
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
	//clear positions every frame
	m_hyperLinkPositions.clear();
    for (int i = 0; i < m_messages.size(); i++)
	{
		std::vector<std::string> split = SplitByHyperlink(m_messages[i].c_str(), "https://");

		ImVec2 colPos = ImGui::GetCursorPos();

		/*if (m_messages.size() >= 2)
		{
			int x = 0;
		}*/

		for (int ii = 0; ii < split.size(); ii++)
		{
			if (split[ii].find("https://") != std::string::npos /*|| split[ii].find("http://") != std::string::npos*/)
			{
				//find some way to track position of this hyperlink
				ImGui::TextColored(ImVec4(0, 0, 0.933f, 1), split[ii].c_str());
				ImVec2 rectMin = ImGui::GetItemRectMin();
				ImVec2 rectMax = ImGui::GetItemRectMax();

				std::pair<ImVec2, ImVec2> rectPair(rectMin, rectMax);

				std::cout << split[ii] << ": " << rectMin.x << ", " << rectMin.y << " : " << rectMax.x << " : " << rectMax.y << std::endl;

				if (m_hyperLinkPositions.count(rectPair) > 0)
				{
					//std::cout << "Overwriting" << std::endl;
				}
				m_hyperLinkPositions[rectPair] = split[ii];
				

				//if (i == split.size())
				/*if (i == m_messages.size() - 1)
					std::cout << rectMax.x << " " << rectMax.y << std::endl;*/

				//ImGui::TextColored(ImVec4(0.2f, 0.4f, 0.733f, 1), split[i].c_str());
			}
			else
			{
				ImGui::Text(split[ii].c_str());
			}
			colPos.x += ImGui::CalcTextSize(split[ii].c_str()).x;
			ImGui::SameLine(colPos.x);
		}
		ImGui::Text("\n");

	}

	ImGui::InputText("", m_textInInviteField, BUFFER_SIZE);
	if (ImGui::Button("Send Invite"))
	{
		SendChatInvite();
	}

	if (ImGui::IsMouseClicked(0))
	{
		//FOR CLICKING ON HYPERLINKS
		ImVec2 mousePos = ImGui::GetMousePos();
		
		std::cout << m_hyperLinkPositions.size() << std::endl;

		for (std::map<std::pair<ImVec2, ImVec2>, std::string>::iterator it = m_hyperLinkPositions.begin(); it != m_hyperLinkPositions.end(); it++)
		{
			if (mousePos.x >= (*it).first.first.x && mousePos.x <= (*it).first.second.x)
			{
				if (mousePos.y >= (*it).first.first.y && mousePos.y <= (*it).first.second.y)
				{
					ShellExecuteA(NULL, "open", (*it).second.c_str(), NULL, NULL, SW_SHOWDEFAULT);
				}
			}
		}
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

std::vector<std::string> ChatWindow::SplitByHyperlink(const char * line, const char* splitBy)
{
	//if no hyperlinks, return 1 element vector with original line

	//if hyperlinks, split line into (non-hyperlink, hyperlink, non-hyperlink) format and return as vector
	std::vector<std::string> lineSplit;
	std::string lineStr = line;
	size_t found = lineStr.find(splitBy);
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
		found = lineStr.find(splitBy);
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

void ChatWindow::SendTextToAddress(RakNet::SystemAddress address)
{
	std::stringstream ss;
	ss << "(Client " << m_clientID << "): " << m_textInField << '[' << m_chatID << ']' << address.ToString();
	RakNet::BitStream bs;
	bs.Write((unsigned char)ID_CHAT_MESSAGE_WITH_ADDRESS);
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
