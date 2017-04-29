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
	m_showSearchField = false;
	m_performedSearch = false;
	m_hasDepressedEnter = true;
	m_hasDepressedCtrlF = true;
	m_sortEmojisByUsageFreq = true;
	m_setInputFieldFocus = false;
	m_showEmojiKeyboard = false;
	m_input = aie::Input::getInstance();
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
	if (m_setInputFieldFocus == true)
	{
		ImGui::SetKeyboardFocusHere(); //set focus to next widget
		m_setInputFieldFocus = false;
	}
	ImGui::InputTextMultiline(m_clientName.c_str(), m_textInField, BUFFER_SIZE);
	if (ImGui::Button("Send") && m_textInField[0] != 0)
	{
		SendText();
		memset(m_textInField, 0, sizeof(m_textInField));
		m_setInputFieldFocus = true;

	}

	ImGui::SameLine();
	if (ImGui::Button("Emoji Keyboard"))
	{
		m_showEmojiKeyboard = !m_showEmojiKeyboard;
	}


	if (m_showEmojiKeyboard == true)
	{
		ImGui::BeginChild("Emoji Keyboard", ImVec2(160, 56));

		for (int i = 0; i < m_textureIDs->size(); i++)
		{
			if (ImGui::ImageButton((*m_textureIDs)[i].first, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
			{
				//add this to a file containing frequencies
				std::ofstream freqFile;
				freqFile.open("emojiFrequencyDatabase.txt", std::ios::app);

				freqFile << '\n' << (*m_textureIDs)[i].second;
				freqFile.close();

				std::stringstream ss;
				ss << "E#" << i << "#";

				std::string emojiCode = ss.str();
				int lenOfTextInField = strlen(m_textInField);
				for (int ii = lenOfTextInField; ii < lenOfTextInField + emojiCode.length(); ii++)
				{
					//memset(m_textInField, 0, sizeof(m_textInField));
					m_textInField[ii] = emojiCode[ii - lenOfTextInField];
				}
			}

			if (i % 6 != 0 || i == 0)
				ImGui::SameLine();
		}

		ImGui::EndChild();
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
	ImVec2 cursorPosAbsolute = ImGui::GetCursorPos(); //get cursor pos
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
		//add to map
		std::vector<std::string> split = SplitBy(m_messages[i].c_str(), "https://", "http://");

		ImVec2 colPos = ImGui::GetCursorPos();
		float windowX = ImGui::GetWindowPos().x;
		
		//count number of lines
		int noLines = 1;
		int lengthOfCurrentLine = 0;
		int lengthOfLongestLine = 0;
		for (int c = 0; c < m_messages[i].length(); c++)
		{
			lengthOfCurrentLine++;
			if (m_messages[i][c] == '\n')
			{
				if (lengthOfCurrentLine > lengthOfLongestLine)
				{
					lengthOfLongestLine = lengthOfCurrentLine;
				}
				lengthOfCurrentLine = 0;
				noLines++;
			}
		}
		int lastInvisibleIndex = floor(ImGui::GetScrollY() / 17.0f) - 1; //could also use this for other optimizations (don't draw text that is out of view)
		ImVec2 currentMessageSize = ImGui::CalcTextSize(m_messages[i].c_str());
		ImRect currentMessageBB;

		/*currentMessageBB.Min = ImVec2(windowX, cursorPosAbsolute.y + (13 * i) + (ImClamp(lastInvisibleIndex, 0, INT_MAX) * 13));
		currentMessageBB.Max = ImVec2(windowX + currentMessageSize.x, cursorPosAbsolute.y + currentMessageSize.y + (13*i) + (ImClamp(lastInvisibleIndex, 0, INT_MAX) + 3));

		ImVec2 currMousePos = ImGui::GetMousePos();

		if (ImGui::IsMouseHoveringRect(currentMessageBB.Min, currentMessageBB.Max) && i > lastInvisibleIndex)
		{
			std::cout << "Hovered over " << i << std::endl;
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::OpenPopup("Tooltip");
			ImGui::BeginPopup("Tooltip");

			ImGui::Text(" Popup!");

			ImGui::EndPopup();
		}*/


		//height of this message will equal how many lines it is multiplied by how high a line is
		//

		for (int ii = 0; ii < split.size(); ii++)
		{
			if (split[ii].find("https://") != std::string::npos || split[ii].find("http://") != std::string::npos)
			{
				//REWRITE ALL OF THIS WITH IMRECT AND IMGUI::ISHOVERED

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

				//E#1#

				int emojiIndex = split[ii].find("E#");
				if (emojiIndex == std::string::npos) //MESSAGE DOES NOT CONTAIN EMOJIS
				{
					ImGui::Text(split[ii].c_str());
					ImVec2 rectMin = ImGui::GetItemRectMin();
					ImVec2 rectMax = ImGui::GetItemRectMax();

					//currentMessageBB.Min = ImVec2(windowX, cursorPosAbsolute.y + ImGui::GetCursorPosY()+5);
					//currentMessageBB.Max = ImVec2(windowX + currentMessageSize.x, cursorPosAbsolute.y + ImGui::GetCursorPosY() + currentMessageSize.y + 5);

					currentMessageBB.Min = rectMin;
					currentMessageBB.Max = rectMax;

					//std::cout << currentMessageBB.Min.x << ", " << currentMessageBB.Max.x << std::endl;
					//std::cout << currentMessageBB.Min.y << ", " << currentMessageBB.Max.y << std::endl;

					ImVec2 currMousePos = ImGui::GetMousePos();
					if (currMousePos.x > currentMessageBB.Min.x && currMousePos.x < currentMessageBB.Max.x 
						&& currMousePos.y > currentMessageBB.Min.y && currMousePos.y < currentMessageBB.Max.y)
					{		
						//std::cout << "Hovered over " << i << std::endl;
						ImGui::SetNextWindowPos(ImGui::GetMousePos());
						ImGui::OpenPopup("Tooltip");
						ImGui::BeginPopup("Tooltip");
						ImGui::Text(m_messageTimestamps[i].c_str());

						ImGui::EndPopup();
					}
				}
				else  //MESSAGE CONTAINS EMOJIS
				{
					ImVec2 rectMin;
					ImVec2 rectMax;

					std::string currentTimestamp = m_messageTimestamps[i]; //get time stamp, could be needed later
					int iterations = 0;
					std::string current = split[ii];
					while (emojiIndex != std::string::npos)
					{
						ImGui::Text(current.substr(0, emojiIndex).c_str());

						if (iterations == 0)
						{
 							rectMin = ImGui::GetItemRectMin();
						}

						rectMax.x += ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x;
						rectMax.y = ImGui::GetItemRectMax().y;

						ImGui::SameLine();

						//cut off E#1# to 1#
						std::string substr = current.substr(emojiIndex + 2, current.length());
						int endHashIndex = substr.find("#");
						std::string emojiIDStr = substr.substr(0, endHashIndex);
						int emojiID = std::stoi(emojiIDStr);
						if (emojiID < m_textureIDs->size())
						{
							ImGui::Image((*m_textureIDs)[emojiID].first, ImVec2(16,16));
							rectMax.x += 16;
							ImGui::SameLine();
						}
						int subStrStartIndex = endHashIndex /*+ emojiIDStr.length() + emojiIndex*/+1;
						if (subStrStartIndex >= current.length())
						{
							break;
						}
						current = substr.substr(subStrStartIndex, split[ii].length());
						emojiIndex = current.find("E#");
						iterations++;
					}
					ImGui::Text(current.c_str());
					rectMax.x += ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x;

					//std::cout << currentMessageBB.Min.x << ", " << currentMessageBB.Max.x << std::endl;
					//std::cout << currentMessageBB.Min.y << ", " << currentMessageBB.Max.y << std::endl;

					ImVec2 currMousePos = ImGui::GetMousePos();
					if (currMousePos.x > rectMin.x && currMousePos.x < rectMax.x
						&& currMousePos.y > rectMin.y && currMousePos.y < rectMax.y)
					{
						//std::cout << "Hovered over " << i << std::endl;
						ImGui::SetNextWindowPos(ImGui::GetMousePos());
						ImGui::OpenPopup("Tooltip");
						ImGui::BeginPopup("Tooltip");
						ImGui::Text(currentTimestamp.c_str());

						ImGui::EndPopup();
					}
				}

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
	
	//check for hover over a message, and draw tooltip if it happens


	//scroll increases by 17 each time we add a new message

	//calculate how many lines of text can fit in our window without scrolling
	//this would be m_messageWindowSizeY / ImGui::GetTextSize().y

	//if scroll position is 0, that means we can't see anything more than 17*lines ahead of us

	if (m_input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL))
	{
		if (m_input->isKeyDown(aie::INPUT_KEY_F) && m_hasDepressedCtrlF == true)
		{
			//m_showSearchField = true;
			m_hasDepressedCtrlF = false;
			m_showSearchField = !m_showSearchField;
		}
	}

	if (m_input->isKeyDown(aie::INPUT_KEY_F) == false)
	{
		m_hasDepressedCtrlF = true;
	}

	if (m_showSearchField == true && m_performedSearch == false)
	{
		//probably going to be a live search, no need for a submit button
		//get text and run search. 
		//find a way to memoize the search, we don't want to be re-searching for terms that we've already looked for
		int scrollVal = -1;
		//std::cout << ImGui::GetScrollY() << std::endl;
		if (strlen(m_searchConversationField) != 0)
		{
			//create new messages list, split by newline
			std::vector<std::string> messagesSplit;
			for (int i = 0; i < m_messages.size(); i++)
			{
				std::string currentMsg = m_messages[i];
				currentMsg = currentMsg.substr(m_messages[i].find(':') + 2);
				size_t pos = currentMsg.find('\n');
				while (pos != std::string::npos)
				{
					std::string subStr = currentMsg.substr(0, pos);
					messagesSplit.push_back(subStr);
					currentMsg = currentMsg.substr(pos + 1, currentMsg.length());
					pos = currentMsg.find('\n');
				}
				messagesSplit.push_back(currentMsg);
			}

			//start from current scroll position, not used for now
			int currentScrollPosY = ImGui::GetScrollY();
			int startIndex = currentScrollPosY / 17;
			//for (int i = 0; i < m_messages.size(); i++)
			//{
			//	std::string trimmed = m_messages[i].substr(m_messages[i].find(':') + 2, m_messages[i].length());
			//	if (trimmed.find(m_searchConversationField) != std::string::npos)
			//	{
			//		scrollVal = i * 17;
			//		//prevent this loop from executing unneccessarilyk
			//		break;
			//	}
			//}
			for (int i = 0; i < messagesSplit.size(); i++)
			{
				if (messagesSplit[i].find(m_searchConversationField) != std::string::npos)
				{
					scrollVal = i * 17;
					break;
				}
			}

			m_performedSearch = true;
		}

		if (scrollVal != -1)
		{
			ImGui::SetScrollY(scrollVal);
		}
	}

	int scrollVal = -1;
	if (m_input->isKeyDown(aie::INPUT_KEY_ENTER) == false)
	{
		m_hasDepressedEnter = true;
	}

	if (m_input->isKeyDown(aie::INPUT_KEY_ENTER) && m_showSearchField == true && strlen(m_searchConversationField) != 0 && m_hasDepressedEnter == true)
	{
		m_hasDepressedEnter = false;
		int currentScrollPosY = ImGui::GetScrollY();
		int startLine = currentScrollPosY / 17;
		startLine++;
		std::vector<std::string> messagesSplit;
		for (int i = 0; i < m_messages.size(); i++)
		{
			std::string currentMsg = m_messages[i];
			currentMsg = currentMsg.substr(m_messages[i].find(':') + 2);
			size_t pos = currentMsg.find('\n');
			while (pos != std::string::npos)
			{
				std::string subStr = currentMsg.substr(0, pos);
				messagesSplit.push_back(subStr);
				currentMsg = currentMsg.substr(pos + 1, currentMsg.length());
				pos = currentMsg.find('\n');
			}
			messagesSplit.push_back(currentMsg);
		}
		for (int i = startLine; i < messagesSplit.size(); i++)
		{
			if (messagesSplit[i].find(m_searchConversationField) != std::string::npos)
			{
				scrollVal = i * 17;
				break;	
			}
		}
		if (scrollVal != -1)
		{
			ImGui::SetScrollY(scrollVal);
		}
	}

	ImGui::EndChild();

	//ImGui::Checkbox("Search ", &m_showSearchField);
	if (m_showSearchField == true)
	{
		if (ImGui::InputText("Search", m_searchConversationField, BUFFER_SIZE)) //if modified
		{
			m_performedSearch = false;
		}
		
	}


	//COMMENTED OUT INVITE STUFF FOR NOW (NOT REALLY USED)
	//ImGui::InputText("", m_textInInviteField, BUFFER_SIZE);
	//if (ImGui::Button("Send Invite"))
	//{
	//	SendChatInvite();
	//}

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
	m_performedSearch = false;
	ImGui::SetKeyboardFocusHere();
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	std::string timeString = " ";
	int hour = now->tm_hour;
	std::string hourStr;
	std::string meridiem;
	std::string minStr = std::to_string(now->tm_min);

	if (minStr.size() == 1)
	{
		minStr = "0" + minStr;
	}

	if (hour >= 12)
	{
		hour -= 12;
		hourStr = std::to_string(hour);
		meridiem = "PM";
	}

	else
	{
		hourStr = std::to_string(hour);
		meridiem = "AM";
	}

	timeString += hourStr + ":" + minStr + meridiem;
	//message += timeString;

	m_messageTimestamps[m_messages.size()] = timeString;

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

void ChatWindow::SetTextureIDs(std::vector<std::pair<ImTextureID, std::string>> *textureIDs)
{
	m_textureIDs = textureIDs;
}

std::vector<std::string> ChatWindow::SplitBy(const char * line, const char* splitBy)
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

std::vector<std::string> ChatWindow::SplitBy(const char * line, const char* splitBy1, const char* splitBy2)
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
