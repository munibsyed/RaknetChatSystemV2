#include "Client.h"
#include "Gizmos.h"
#include "Input.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include "..\bootstrap\imgui_glfw3.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "..\dependencies\stb\stb_image.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

std::vector<std::string> SplitByHyperlink(const char* line)
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

Client::Client() {

}

Client::~Client() {
}

bool Client::startup() {

	m_fileTransfer = new RakNet::FileListTransfer;
	m_colourEditor = new ColorEditor;
	m_openFileDialog = new OpenFileDialog;
	m_callbackInterface = new CallbackInterfaceClient;
	m_isSendingFile = false;
	m_sendPacketCounter = 0;
	m_sendPacketInterval = 1;
	setBackgroundColour(0.25f, 0.25f, 0.25f);
	m_sendFileID = -1;

	std::ifstream freqDatabase;
	freqDatabase.open("emojiFrequencyDatabase.txt");
	std::vector<std::string> allLines;
	std::map<std::string, int> emojiNamesWithFrequencies;
	if (freqDatabase.is_open())
	{
		while (freqDatabase.eof() == false)
		{
			std::string line;
			std::getline(freqDatabase, line);
			allLines.push_back(line);

			if (line != "" && line != "\n")
			{
				if (emojiNamesWithFrequencies.count(line) == 0)
				{
					emojiNamesWithFrequencies[line] = 1;
				}

				else
				{
					emojiNamesWithFrequencies[line] += 1;
				}
			}
		}
	}

	std::vector<std::pair<int, std::string>> emojiFrequenciesWithNamesVector;
	for (std::map <std::string, int> ::iterator it = emojiNamesWithFrequencies.begin(); it != emojiNamesWithFrequencies.end(); it++)
	{
		emojiFrequenciesWithNamesVector.push_back(std::make_pair((*it).second, (*it).first));
	}

	std::sort(emojiFrequenciesWithNamesVector.begin(), emojiFrequenciesWithNamesVector.end());
	std::reverse(emojiFrequenciesWithNamesVector.begin(), emojiFrequenciesWithNamesVector.end());
	for (int i = 0; i < emojiFrequenciesWithNamesVector.size(); i++)
	{
		std::stringstream ss;
		ss << "160x160\\" << emojiFrequenciesWithNamesVector[i].second << ".png";
		aie::Texture* texture = new aie::Texture(emojiFrequenciesWithNamesVector[i].second.c_str());

		m_textures.push_back(texture);
		m_textureIDs.push_back(std::make_pair((void*)texture->getHandle(), emojiFrequenciesWithNamesVector[i].second));
	}

	for (int i = 1; i <= 846; i++)
	{
		std::stringstream ss;
		ss << "160x160\\" << i << ".png";
		bool hasNotBeenAddedAlready = true;
		for (int ii = 0; ii < emojiFrequenciesWithNamesVector.size(); ii++)
		{	
			if (ss.str() == emojiFrequenciesWithNamesVector[ii].second)
			{
				hasNotBeenAddedAlready = false;
			}
		}

		if (hasNotBeenAddedAlready == true)
		{
			aie::Texture* texture = new aie::Texture(ss.str().c_str());
		
			m_textures.push_back(texture);
			m_textureIDs.push_back(std::make_pair((void*)texture->getHandle(), ss.str()));
		}
	}

	//figure out how to write a sorting key function
	
	//(5,3,2)
	//(1,2,3,4,5)
	//5,3,2,1,4
	

	//ImGui::ModifyStyle(1, ImVec4(1, 1, 1, 1));

	m_ImGuiButtonColour = ImVec4(0.67f, 0.4f, 0.4f, 0.6f);
	m_ImGuiButtonHoveredColour = ImVec4(0.67f, 0.4f, 0.4f, 1.0f);
	m_ImGuiButtonActiveColour = ImVec4(0.8f, 0.5f, 0.5f, 1.0f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	HandleNetworkConnections();

	//m_chatWindows.push_back(new ChatWindow("Chat", "Chat 1", m_clientID, m_pPeerInterface));
	//m_chatWindows.push_back(new ChatWindow("Chat", "Chat 2", m_clientID, m_pPeerInterface));
	//m_chatWindows.push_back(new ChatWindow("Chat", "Chat 3", m_clientID, m_pPeerInterface));
	return true;
}

void Client::shutdown() {

	delete m_fileTransfer;
	delete m_colourEditor;
	delete m_openFileDialog;
	//delete m_chatWindow;
	for (int i = 0; i < m_chatWindows.size(); i++)
	{
		delete m_chatWindows[i];
	}

	for (int i = 0; i < m_popupWindows.size(); i++)
	{
		delete m_popupWindows[i];
	}

	for (int i = 0; i < m_textures.size(); i++)
	{
		delete m_textures[i];
	}

	Gizmos::destroy();
	delete m_callbackInterface;
}


void Client::update(float deltaTime) {
	//Listen for incoming message
	HandleNetworkMessages(false);
	// query time since application started
	float time = getTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	/*tabs*/
	ImGui::Begin("Chat");
	
	if (m_chatWindows.size() > 0)
	{
		ImGui::BeginTabBar("Chat List");
		
		for (int i = 0; i < m_chatWindows.size(); i++)
		{
			std::string name = m_chatWindows[i]->m_chatNameStr;
			if (ImGui::AddTab("Chat")) //if active
			{
				m_chatWindows[i]->Draw();
			}
		}
		ImGui::EndTabBar();
	}

	if (ImGui::Button("Add Chat"))
	{
		//this will actually come later
		CreateNewChatRequest();
	}

	ImGui::Checkbox("Show Colour Picker", m_colourEditor->GetVisiblePointer());
	
	m_colourEditor->Draw();

	if (m_isSendingFile == true)
	{
		m_ImGuiButtonColour = ImVec4(0.5f, 0.5f, 0.5f, 1);
		m_ImGuiButtonHoveredColour = ImVec4(0.5f, 0.5f, 0.5f, 1);
		m_ImGuiButtonActiveColour = ImVec4(0.5f, 0.5f, 0.5f, 1);
	}
	else
	{
		m_ImGuiButtonColour = ImVec4(0.67f, 0.4f, 0.4f, 0.6f);
		m_ImGuiButtonHoveredColour = ImVec4(0.67f, 0.4f, 0.4f, 1.0f);
		m_ImGuiButtonActiveColour = ImVec4(0.8f, 0.5f, 0.5f, 1.0f);
	}

	//SEND FILE
	if (ImGui::ButtonColoured("Send File", 
		ImGui::ColorConvertFloat4ToU32(m_ImGuiButtonColour), 
		ImGui::ColorConvertFloat4ToU32(m_ImGuiButtonHoveredColour), 
		ImGui::ColorConvertFloat4ToU32(m_ImGuiButtonActiveColour))
		&& m_isSendingFile == false)
	{
		m_isSendingFile = true;
		//

		if (m_fileSendThreads.size() > 0) //if threads have already been created
		{
			m_fileSendThreads[m_fileSendThreads.size() - 1].join();
		}

		m_fileSendThreads.push_back(std::thread(&Client::SendFileTest, this));
	}

	ImGui::End();

	

	//draw any pop-up windows
	
	for (std::vector<PopupWindow*>::iterator it = m_popupWindows.begin(); it != m_popupWindows.end(); )
	{
		if ((*it)->IsOpen() == false)
		{
			delete *it;
			it = m_popupWindows.erase(it);
		}
		else
		{
			(*it)->Draw();
			it++;
		}
	}
	//
	//for (int i = 0; i < m_popupWindows.size(); i++)
	//{
	//	m_popupWindows[i]->Draw();
	//}

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
	{
		//	m_pPeerInterface->CloseConnection(serverAddress, false);
		quit();
	}
}

void Client::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);

}

//when server sends back a packet containing client ID
void Client::OnSetClientIDPacket(RakNet::Packet* packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(unsigned char));
	bsIn.Read(m_clientID);
	
	for (int i = 0; i < m_chatWindows.size(); i++)
		m_chatWindows[i]->SetClientID(m_clientID);

	std::stringstream ss;
	ss << "Client " << m_clientID;

	m_clientName = ss.str();

	std::cout << "Set client ID to: " << m_clientID << std::endl;
	m_receiveFileID = m_fileTransfer->SetupReceive(m_callbackInterface, true, serverAddress);
	SendFileReceiveID(m_receiveFileID);
}

void Client::OnDisconnectionSendPacket()
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)ID_DISCONNECTION_NOTIFICATION);
	bsOut.Write(m_clientID);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}


void Client::SendFileTest()
{
	//once we are connected to the server
	/*CallbackInterfaceClient* cbHandler = new CallbackInterfaceClient;
	int fileSendID = m_fileTransfer->SetupReceive(cbHandler, true, serverAddress);*/
	
	//files that work so far: .txt, .cpp, .h, .bat, 
	RakNet::FileList *fileList = new RakNet::FileList;
	FileListNodeContext context;

	//ADDITIONAL SETTINGS, PLAY AROUND WITH THIS
	m_openFileDialog->Flags |= OFN_SHOWHELP;
	//m_openFileDialog->InitialDir = _T("C:\\Windows\\");

	m_openFileDialog->FilterIndex = 1;
	m_openFileDialog->Title = _T("Choose File to Send");

	if (m_openFileDialog->ShowDialog())
	{
		
	}	
	//HWND hwnd = new HWND;
	//GetWindowText(hwnd, m_openFileDialog->FileName, 255);
	std::wstring s = m_openFileDialog->FileName;

	//convert from wchar to char
	const wchar_t* wstr = s.c_str();
	size_t wlen = wcslen(wstr) + 1;
	char filepath[255];
	size_t convertedChars = 0;
	wcstombs_s(&convertedChars, filepath, wlen, wstr, _TRUNCATE);
	
	if (strlen(filepath) == 0)
	{
		m_isSendingFile = false;
		return;
	}

	std::ifstream fileStream(filepath, std::ios::binary);

	std::string dataBufferStr((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	fileStream.close();

	std::string filename = "";
	for (int i = strlen(filepath) - 1; i >= 0; i--)
	{
		if (filepath[i] == '\\')
		{
			break;
		}

		filename += filepath[i];
	}

	std::reverse(filename.begin(), filename.end());

    fileList->AddFile(filepath, filename.c_str(), context);	
	//two variations of AddFile exist, one for adding a file from memory and one for adding a file from disk
	std::cout << "Sending to: " << m_sendFileID << std::endl;
	//NOTE -------- NOT SURE IF FILE_ID GETS SET PROPERLY FOR ALL CLIENTS -------- NOTE
	if (m_sendFileID != -1)
		m_fileTransfer->Send(fileList, 0, serverAddress, m_sendFileID, HIGH_PRIORITY, 0);
	delete fileList;
	
	std::cout << "Sent." << std::endl;


	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)ID_REINITIALIZE_FILE_HANDLERS);
	bsOut.Write(std::to_string(m_clientID).c_str());
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);

	m_isSendingFile = false;
}


void Client::SendFileReceiveID(int id)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)ID_CLIENT_SET_FILE_SEND_ID);
	std::stringstream ss;
	ss << "Client " << m_clientID << "|" << m_receiveFileID;
	RakNet::RakString rakStr = ss.str().c_str();
	bsOut.Write(rakStr);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);

}

void Client::CreateNewChatRequest()
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)ID_CREATE_NEW_CHAT_REQUEST);
	
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::DisplayHighScoresRequest()
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)ID_DISPLAY_HIGH_SCORES_REQUEST);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::HandleNetworkConnections()
{
	//Initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	InitializeClientConnection();
}

void Client::InitializeClientConnection()
{
	//startup the RakPeerInterface, but allow only one connection to the server, then attempt to connect to server

	//Create socket descriptor, no data needed
	RakNet::SocketDescriptor sd;
		
	//call startup on m_peerInterface with max connections = 1
	m_pPeerInterface->Startup(1, &sd, 1);
	m_pPeerInterface->AttachPlugin(m_fileTransfer);

	std::cout << "Connecting to server at " << IP << std::endl;

	//Now call connect to attempt to connect with given server
	RakNet::ConnectionAttemptResult result = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);

	//check if connection
	if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection. Error number " << result << std::endl;
	}
	
}
	
void Client::HandleNetworkMessages(bool loop)
{
	RakNet::Packet *packet = nullptr;
	do
	{
		for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
		{
			//inspect first byte of the packet to receive its ID
			switch (packet->data[0])
			{

			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				std::cout << "Another client has disconnected." << std::endl;
				break;
			case ID_REMOTE_CONNECTION_LOST:
				std::cout << "Another client has lost the connection." << std::endl;
				break;

			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				std::cout << "Another client has connected." << std::endl;
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				std::cout << "Our connection request has been accepted." << std::endl;
				serverAddress = packet->systemAddress;
				break;
			}

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "Server is full." << std::endl;
				break;

			case ID_DISCONNECTION_NOTIFICATION:
			{
				std::cout << "We have been disconnected." << std::endl;
				OnDisconnectionSendPacket();
				break;
			}

			case ID_CONNECTION_LOST:
				std::cout << "Connection lost." << std::endl;
				break;

			case ID_CHAT_MESSAGE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				RakNet::RakString str;
				bsIn.Read(str);
				std::string message = str.C_String();

				int senderID = ParseIDNumber(message.c_str());
				if (senderID == m_clientID)
				{
					std::string modifiedStr = "(You) ";
					bool foundEnd = false;
					for (int i = 0; i < message.length(); i++)
					{
						if (foundEnd == true)
						{
							modifiedStr += message[i];
						}

						if (message[i] == ')')
							foundEnd = true;
					}
					message = modifiedStr;
				}

				//parse chat ID number
				std::string toParse = message;
				std::string idNumAsStr = "";
				for (int i = toParse.length() - 1; i >= 0; i--)
				{
					if (toParse[i] == '[')
					{
						toParse = toParse.substr(0, i);
						break;
					}

					else
					{
						idNumAsStr += toParse[i];
					}
				}

				//we have the chat ID, we aren't doing anything with it yet
				std::reverse(idNumAsStr.begin(), idNumAsStr.end());
				int idNum = std::stoi(idNumAsStr);

				message = toParse;

				for (int i = 0; i < m_chatWindows.size(); i++)
				{
					//message will only be received by clients in the same chatID as sender
					//however, clientID will still have to be sent across so this client knows which chatWindow to update

					if (m_chatWindows[i]->GetChatID() == idNum)
						m_chatWindows[i]->AddMessage(message);
				}
				break;

			}

			case ID_SERVER_SET_CLIENT_ID:
			{
				//when server sends back a packet containing client ID
				OnSetClientIDPacket(packet);
				break;
			}

			case ID_CLIENT_CLIENT_DATA:
			{
				break;
			}

			case ID_DISCONNECTED_CLIENT_ID:
			{
				std::cout << "Someone disconnected, deleting their entry." << std::endl;
			
				break;
			}

			case ID_SERVER_SET_FILE_SEND_ID:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(unsigned char));
				serverAddress = packet->systemAddress;
				int fileSendID;
				bsIn.Read(fileSendID);
				std::cout << "Received file send ID: " << fileSendID << std::endl;
				m_sendFileID = fileSendID;
				break;
			}

			case ID_CREATE_NEW_CHAT_REQUEST:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(unsigned char));
				RakNet::RakString str;
				bsIn.Read(str);

				std::string idAsStr = str.C_String();
				int chatID = std::stoi(idAsStr);

				ChatWindow* window = new ChatWindow("Chat", m_clientID, m_clientName, m_pPeerInterface);
				window->SetChatID(chatID);
				window->SetTextureIDs(&m_textureIDs);

				//send client ID back to be put in map?

				m_chatWindows.push_back(window);
				break;
			}

			case ID_SEND_INVITED_CLIENT_ADDRESS:
			{
				//receiving the address of an invited client and the corresponding chat ID
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(unsigned char));
				RakNet::RakString str;
				bsIn.Read(str);

				std::string stdStr = str.C_String();
				int braceIndex = 0;
				for (int i = stdStr.length() - 1; i >= 0; i--)
				{
					if (stdStr[i] == '[')
					{
						braceIndex = i;
						break;
					}
						
				}

				std::string strTrimmed = stdStr.substr(0, braceIndex);
				std::string chatIDStr = stdStr.substr(braceIndex + 1, stdStr.length());
				int chatID = std::stoi(chatIDStr);
				RakNet::SystemAddress address;

				if (address.FromString(strTrimmed.c_str(), '|'))
				{
					for (int i = 0; i < m_chatWindows.size(); i++)
					{
						if (chatID == m_chatWindows[i]->GetChatID())
						{
							m_chatWindows[i]->AddChatRecipient(address);
						}
					}
				}


				break;
			}

			case ID_SEND_INVITED_CLIENT_ADDRESS_FAIL:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(ID_SEND_INVITED_CLIENT_ADDRESS_FAIL));

				RakNet::RakString str;
				bsIn.Read(str);

				std::string stdStr = str.C_String();
//				int clientID = std::stoi(stdStr);

				//not sure if clientID needed

				m_popupWindows.push_back(new PopupWindow("Error message", "Invalid client name"));
				//create error window/pop up window class?

				break;
			}

			case ID_REINITIALIZE_FILE_HANDLERS:
			{
				//delete m_callbackInterface;
				m_callbackInterface = new CallbackInterfaceClient;
				int newID = m_fileTransfer->SetupReceive(m_callbackInterface, true, serverAddress);
				RakNet::BitStream bsOut;
				bsOut.Write((unsigned char)ID_CLIENT_SET_FILE_SEND_ID);
				std::stringstream ss;
				ss << "Client " << m_clientID << '|' << newID;
				bsOut.Write(ss.str().c_str());

				m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);

				break;
			}

			default:
				std::cout << "Received a message with an unknown ID: " << packet->data[0] << std::endl;
				break;
			}
			
		}
	} while (loop);
}

int Client::ParseIDNumber(const char* str)
{
	std::string result;
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] != '(')
		{
			if (str[i] == ')')
			{
				break;
			}

			else if (str[i] >= 48 && str[i] <= 57)
			{
				result += str[i];
			}
		}
	}
	int idNum = std::stoi(result);
	return idNum;
}