#include "ChatServer.h"



ChatServer::ChatServer(const unsigned short port, unsigned int maxClients) : PORT(port), m_maxClients(maxClients)
{
	//start up the server and start listening to clients
	std::cout << "Starting up server..." << std::endl;
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	m_fileListTransfer = new RakNet::FileListTransfer;

	RakNet::SocketDescriptor sd(PORT, 0);
	m_peerInterface->Startup(m_maxClients, &sd, 1);
	m_peerInterface->SetMaximumIncomingConnections(m_maxClients);
	m_peerInterface->AttachPlugin(m_fileListTransfer);
	m_runUpdateLoop = true;
	m_sendChatMsgBackToSender = true;

	m_callbackInterface = new CallbackInterface();

	//find out if this returns a different m_fileSendID depending on what system address is given. This setup probably won't work
	m_nextClientID = 0;
	m_nextChatID = 0;

	m_fileListTransfer->StartIncrementalReadThreads(1);

}


ChatServer::~ChatServer()
{
	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
	RakNet::FileListTransfer::DestroyInstance(m_fileListTransfer);
}

void ChatServer::Update()
{
	//m_fileListTransfer->StartIncrementalReadThreads(1);

	RakNet::Packet *packet = nullptr;
	while (m_runUpdateLoop == true)
	{
		for (packet = m_peerInterface->Receive(); packet; m_peerInterface->DeallocatePacket(packet), packet = m_peerInterface->Receive())
		{
			switch (packet->data[0])
			{
				//inspect first byte of the packet to receive its ID

			case ID_NEW_INCOMING_CONNECTION:
			{
				std::cout << "A connection is incoming." << std::endl;

				//FILE TRANSFER SETUP YET TO BE ADDED

				//setup for file receive
				//testCB = new TestCB;
				//fileSendID = fileReceiver->SetupReceive(testCB, true, packet->systemAddress);
				m_fileSendID = m_fileListTransfer->SetupReceive(m_callbackInterface, true, packet->systemAddress);
				SendNewClientID(packet->systemAddress);
				SendFileSendID(m_fileSendID, packet->systemAddress);

				//add to map
				std::stringstream ss;
				ss << "Client " << m_nextClientID - 1;
				m_clientNameToAddress[ss.str()] = packet->systemAddress;

				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			{
				//Inform all other clients about which client has been disconnected
				std::cout << "A client has disconnected." << std::endl;
				SendDisconectionNotification(packet);
				break;
			}
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection." << std::endl;
				break;
			case ID_CHAT_MESSAGE:
			{
				std::cout << "Received chat message." << std::endl;
				ReceiveAndSendChatMessage(packet);
				break;
			}

			case ID_CHAT_MESSAGE_WITH_ADDRESS:
			{
				std::cout << "Received chat message." << std::endl;
				ReceiveAndSendChatMessageToAddress(packet);
				break;
			}

			case ID_CLIENT_CLIENT_DATA:
			{
				RakNet::BitStream bsOut(packet->data, packet->length, false);
				m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
				break;
			}

			case ID_CREATE_NEW_CHAT_REQUEST:
			{
				RakNet::BitStream bsOut;
				bsOut.Write((unsigned char)ID_CREATE_NEW_CHAT_REQUEST);
				RakNet::RakString str;
				str = std::to_string(m_nextChatID).c_str();
				bsOut.Write(str);

				m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

				m_nextChatID++;
				break;
			}

			case ID_CHAT_INVITE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(unsigned char));
				RakNet::RakString str;
				bsIn.Read(str);

				//cut off the chatID from the string
				std::string strTrimmed;
				std::string stdStr = str.C_String();
				int bracePosition = 0;
				for (int i = stdStr.length() - 1; i >= 0; i--)
				{
					if (stdStr[i] == '[')
					{
						bracePosition = i;
						break;
					}
				}

				//trim [chatID from string
				strTrimmed = stdStr.substr(0, bracePosition);
				//get chat ID
				std::string chatIDStr = stdStr.substr(bracePosition+1, stdStr.length());
				int chatID = std::stoi(chatIDStr);

				//send back address of invited client
				if (m_clientNameToAddress.count(strTrimmed.c_str()) > 0)
				{
					RakNet::SystemAddress address = m_clientNameToAddress[strTrimmed.c_str()];
					std::cout << "Retrieved: " << address.ToString() << std::endl;
					RakNet::BitStream bsOut;
					std::stringstream ss;
					ss << address.ToString() << "[" << chatID;
					bsOut.Write((unsigned char)ID_SEND_INVITED_CLIENT_ADDRESS);
					bsOut.Write(ss.str().c_str());
					//send address of invited client back to original sender
					m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

					//send sender address to invited client
					std::stringstream ss2;
					ss2 << packet->systemAddress.ToString() << "[" << chatID;
					RakNet::BitStream bsOut2;
					bsOut2.Write((unsigned char)ID_SEND_INVITED_CLIENT_ADDRESS);
					bsOut2.Write(ss2.str().c_str());

					m_peerInterface->Send(&bsOut2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);


				}

				else
				{
					//send special failure packet
					
					RakNet::BitStream bsOut;
					bsOut.Write((unsigned char)ID_SEND_INVITED_CLIENT_ADDRESS_FAIL);
					bsOut.Write(chatIDStr);

					m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}

				break;
			}


			default:
			{
				std::cout << "Received a message with an unknown ID: " << packet->data[0] << std::endl;
				RakNet::FileList *fileList = RakNet::FileList::GetInstance();
				RakNet::FileListTransfer *fileListTransfer = RakNet::FileListTransfer::GetInstance();

				break;

			}
			}
		}
	}
}

void ChatServer::SendFileSendID(int fileSendID, RakNet::SystemAddress &address)
{
	RakNet::BitStream bs;
	bs.Write((unsigned char)ID_SERVER_SET_FILE_SEND_ID);
	bs.Write(fileSendID);

	m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

void ChatServer::SendNewClientID(RakNet::SystemAddress &address)
{
	RakNet::BitStream bs;
	bs.Write((unsigned char)Messages::ID_SERVER_SET_CLIENT_ID);
	bs.Write(m_nextClientID);
	m_nextClientID++;

	m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

void ChatServer::SendDisconectionNotification(RakNet::Packet * packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	RakNet::RakString str;
	bsIn.IgnoreBytes(sizeof(unsigned char));
	int clientID;
	bsIn.Read(str);
	clientID = std::atoi(str);
	std::cout << "Client ID: " << clientID << std::endl;

	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)ID_DISCONNECTED_CLIENT_ID);
	bsOut.Write(clientID);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void ChatServer::ReceiveAndSendChatMessage(RakNet::Packet * packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	RakNet::RakString str;

	bsIn.IgnoreBytes(sizeof(unsigned char));
	bsIn.Read(str);
	std::cout << "Chat message: " << str << std::endl;

	//parse chat ID number
	std::string toParse = str.C_String();
	std::string idNumAsStr = "";
	for (int i = toParse.length() - 1; i >= 0; i--)
	{
		if (toParse[i] == '[')
		{
			//toParse = toParse.substr(0, i);
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
	if (m_chatIDToClientAddresses.count(idNum) == 0)
		m_chatIDToClientAddresses[idNum].push_back(packet->systemAddress);

	//str = toParse.c_str();

	//send message back to all clients
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)Messages::ID_CHAT_MESSAGE);
	bsOut.Write(str);

	if (m_sendChatMsgBackToSender == true)
	{
		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	else
	{
		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true); //send to everyone except packet->systemAddress, i.e. original sender
	}
}

void ChatServer::ReceiveAndSendChatMessageToAddress(RakNet::Packet * packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	RakNet::RakString str;

	bsIn.IgnoreBytes(sizeof(unsigned char));
	bsIn.Read(str);
	std::cout << "Chat message: " << str << std::endl;

	//parse address
	std::string toParse = str.C_String();
	std::string addressAsStr = "";
	int index = 0;
	for (int i = toParse.length() - 1; i >= 0; i--)
	{
		if (toParse[i] == ']')
		{
			index = i;
			break;
		}

		else
		{
			addressAsStr += toParse[i];
		}
		
	}

	std::reverse(addressAsStr.begin(), addressAsStr.end());
	RakNet::SystemAddress recipientAddress;
	recipientAddress.FromString(addressAsStr.c_str());

	toParse = toParse.substr(0, index);

	//parse chat ID number
	std::string idNumAsStr = "";
	for (int i = toParse.length() - 1; i >= 0; i--)
	{
		if (toParse[i] == '[')
		{
			//toParse = toParse.substr(0, i);
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
	if (m_chatIDToClientAddresses.count(idNum) == 0)
		m_chatIDToClientAddresses[idNum].push_back(packet->systemAddress);

	//str = toParse.c_str();

	std::string temp = str.C_String();
	temp = temp.substr(0, index);
	str = temp.c_str();

	//send message back to all clients
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)Messages::ID_CHAT_MESSAGE);
	bsOut.Write(str);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipientAddress, false);
	
}
