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
	m_mostRecentMessageSeenBy = 0;
	m_messageWindowSizeY = 50;
	m_seenAllMessages = true;
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

	//best spot?
	if (m_seenAllMessages == false)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Unread message(s).");
	}


	//PRINT ALL MESSAGES

	//Improvement: Automatically format lengthy messages into a readable block, both in the textField and the output

	std::string toPrint = "";
	//clear positions every frame
	m_hyperLinkPositions.clear();
	m_hyperLinkPositionsStr.clear();
	//put all this in a child window
	ImGui::BeginChild(m_windowName, ImVec2(0, m_messageWindowSizeY));

	//probably easier: come up with a way to notify the user of an unread message. If they are scrolled to the top and a message comes along,
	//a notification should come up

	int currentScrollPosY = ImGui::GetScrollY();
	int maxScrollY = ImGui::GetScrollMaxY();

	if (m_messages.size() > 0)
	{
		if (m_seenAllMessages == false) //if we just got a new message
		{
			//std::cout << maxScrollY - currentScrollPosY << std::endl; //when this is positive, that means there is scroll room
			if (maxScrollY - currentScrollPosY <= 0)
			{
				m_seenAllMessages = true;
			}

		}
	}
	//std::cout << m_seenAllMessages << std::endl;

    for (int i = 0; i < m_messages.size(); i++)
	{
		std::vector<std::string> split = SplitByHyperlink(m_messages[i].c_str(), "https://", "http://");

		ImVec2 colPos = ImGui::GetCursorPos();

		/*if (m_messages.size() >= 2)
		{
			int x = 0;
		}*/

		for (int ii = 0; ii < split.size(); ii++)
		{
			if (split[ii].find("https://") != std::string::npos || split[ii].find("http://") != std::string::npos)
			{
				//find some way to track position of this hyperlink
				ImGui::TextColored(ImVec4(0, 0, 0.933f, 1), split[ii].c_str());
				ImVec2 rectMin = ImGui::GetItemRectMin();
				ImVec2 rectMax = ImGui::GetItemRectMax();

				std::pair<ImVec2, ImVec2> rectPair(rectMin, rectMax);

				
				//std::cout << split[ii] << ": " << rectMin.x << ", " << rectMin.y << " : " << rectMax.x << ", " << rectMax.y << std::endl;

				if (m_hyperLinkPositions.count(rectPair) > 0)
				{
					//std::cout << "Overwriting" << std::endl;
				}
				m_hyperLinkPositions[rectPair] = split[ii];
				std::stringstream ss;
				ss << rectMin.x << "," << rectMin.y << ":" << rectMax.x << "," << rectMax.y;
				m_hyperLinkPositionsStr[ss.str()] = split[ii];

				//if (i == split.size())
				/*if (i == m_messages.size() - 1)
					std::cout << rectMax.x << " " << rectMax.y << std::endl;*/

				//ImGui::TextColored(ImVec4(0.2f, 0.4f, 0.733f, 1), split[i].c_str());
			}

			else if (split[ii] == "\n")
			{
				ImGui::Text("\n");
				float xPos = ImGui::GetCursorPosX();

				//SEE WHAT YOU CAN DO WITH THIS
				ImGuiStyle style = ImGui::GetStyle();
				const ImVec2 padding = style.WindowPadding;
				const ImVec2 framePadding = style.FramePadding;

				//split by colon
				std::string subStr = split[0].substr(0, split[0].find(":"));
				xPos += padding.x;
				for (int iii = 0; iii < subStr.length() + 2; iii++)
				{
					//Doesn't line up perfectly. Most likely because of window padding. 
					std::string c = "";
					c += m_messages[i][iii];
					xPos += ImGui::CalcTextSize(c.c_str()).x;
					//ImGui::SameLine(xPos);
				}
				ImGui::SetCursorPosX(xPos);

			}

			else
			{
				ImGui::Text(split[ii].c_str());
			}

			if (split[ii] != "\n")
			{
				colPos.x += ImGui::CalcTextSize(split[ii].c_str()).x;
				ImGui::SameLine(colPos.x);
			}

		}
		//Display read status
		
		ImGui::Text("\n");
		
		//display only if your message is the most recent one
		if (i == m_messages.size() - 1)
		{
			if (m_messages[i].substr(0, 5) == "(You)")
			{ 
				//ImGui::TextDisabled("Seen by all");
			}
		}

		//if there isn't already a new line

	}
	
	//std::cout << "Max Scroll Y: " << ImGui::GetScrollMaxY() << ", Scroll X: " << ImGui::GetScrollY() << std::endl;

	//scroll increases by 17 each time we add a new message

	//calculate how many lines of text can fit in our window without scrolling
	//this would be m_messageWindowSizeY / ImGui::GetTextSize().y

	//if scroll position is 0, that means we can't see anything more than 17*lines ahead of us

	ImGui::EndChild();

	ImGui::InputText("", m_textInInviteField, BUFFER_SIZE);
	if (ImGui::Button("Send Invite"))
	{
		SendChatInvite();
	}

	if (ImGui::IsMouseClicked(0))
	{
		//FOR CLICKING ON HYPERLINKS
		ImVec2 mousePos = ImGui::GetMousePos();
		
		//std::cout << m_hyperLinkPositionsStr.size() << std::endl;

		/*for (std::map<std::pair<ImVec2, ImVec2>, std::string>::iterator it = m_hyperLinkPositions.begin(); it != m_hyperLinkPositions.end(); it++)
		{
			if (mousePos.x >= (*it).first.first.x && mousePos.x <= (*it).first.second.x)
			{
				if (mousePos.y >= (*it).first.first.y && mousePos.y <= (*it).first.second.y)
				{
					ShellExecuteA(NULL, "open", (*it).second.c_str(), NULL, NULL, SW_SHOWDEFAULT);
				}
			}
		}*/

		//SEE IF CLICK WAS IN THE BOUNDS OF ANY HYPERLINK
		for (std::map<std::string, std::string>::iterator it = m_hyperLinkPositionsStr.begin(); it != m_hyperLinkPositionsStr.end(); it++)
		{
			//split up the string in the form of (minX,minY:maxX,maxY) and convert into valid floats
			std::string posData = (*it).first;
			int colonIndex = posData.find(":");
			std::string minData = posData.substr(0, colonIndex);
			std::string maxData = posData.substr(colonIndex + 1, posData.length() - colonIndex + 1);
			int commaIndex = minData.find(",");
			std::string minXStr = minData.substr(0, commaIndex);
			std::string minYStr = minData.substr(commaIndex + 1, minData.length() - commaIndex + 1);
			commaIndex = maxData.find(",");
			std::string maxXStr = maxData.substr(0, commaIndex);
			std::string maxYStr = maxData.substr(commaIndex + 1, maxData.length() - commaIndex + 1);

			int minX = std::stoi(minXStr);
			int minY = std::stoi(minYStr);

			int maxX = std::stoi(maxXStr);
			int maxY = std::stoi(maxYStr);

			if (mousePos.x >= minX && mousePos.x <= maxX)
			{
				if (mousePos.y >= minY && mousePos.y <= maxY)
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
	m_mostRecentMessageSeenBy = 0;

	if (message.substr(0,5) != "(You)")
		m_seenAllMessages = false;
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

std::vector<std::string> ChatWindow::SplitByHyperlink(const char * line, const char* splitBy1, const char* splitBy2)
{
	//if no hyperlinks, return 1 element vector with original line

	//if hyperlinks, split line into (non-hyperlink, hyperlink, non-hyperlink) format and return as vector
	std::vector<std::string> lineSplit;
	std::string lineStr = line;
	size_t found1 = lineStr.find(splitBy1);
	size_t found2 = lineStr.find(splitBy2);
	size_t found = found1;

	if (found == std::string::npos)
	{
		found = found2;
	}

	else
	{
		if (found2 < found1)
		{
			found = found2;
		}
	}

	int begin = 0;
	while (found != std::string::npos)
	{
		//take begin to found string and add it to lineSplit
		std::string subStr = lineStr.substr(begin, found);
		lineStr = lineStr.substr(found, lineStr.length());
		lineSplit.push_back(subStr);
		size_t foundSpace = lineStr.find(" ");
		size_t foundNewLine = lineStr.find('\n');
		//if there was a space
		if (foundSpace != std::string::npos)
		{
			//get hyperlink by going 0 - foundSpace on shorter string
			std::string hyperLink = lineStr.substr(0, foundSpace);
			lineSplit.push_back(hyperLink);
			lineStr = lineStr.substr(foundSpace, lineStr.length());
		}

		else if (foundNewLine != std::string::npos)
		{
			//get hyperlink by going 0 - foundNewline on shorter string
			std::string hyperLink = lineStr.substr(0, foundNewLine);
			lineSplit.push_back(hyperLink);
			lineStr = lineStr.substr(foundNewLine, lineStr.length());
		}

		//if not
		else
		{
			std::string hyperLink = lineStr.substr(0, lineStr.length());
			lineSplit.push_back(hyperLink);
			lineStr = "";
		}
		size_t found1 = lineStr.find(splitBy1);
		size_t found2 = lineStr.find(splitBy2);
		found = found1;

		if (found == std::string::npos)
		{
			found = found2;
		}

		else
		{
			if (found2 < found1)
			{
				found = found2;
			}
		}
	}

	if (lineSplit.size() == 0) //no splits occured
	{
		//split by newline anyway
		size_t newLinePos = lineStr.find('\n');
		while (newLinePos != std::string::npos)
		{
			std::string segment = lineStr.substr(0, newLinePos);
			lineSplit.push_back(segment);
			lineSplit.push_back("\n");

			lineStr = lineStr.substr(newLinePos + 1, lineStr.length() - newLinePos - 1);
			newLinePos = lineStr.find('\n');
		}
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
