/*
* Main SERVER CPP
*
* Ellie Perez:added to document
* Kyle Murray:added to document
* Main source for console SERVER application.
*/

//template of this code is from http://www.jenkinssoftware.com/raknet/manual/tutorialsample1.html
/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-server.c/.cpp
	Main source for console server application.
*/

#include "gpro-net/gpro-net.h"
#include "gpro-net/Message.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>
#include <vector>

//tutorial 3
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID

#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakPeerInterface.h"


using namespace std;

#define MAX_CLIENTS 10
#define SERVER_PORT 4024

//tutorial 3
enum GameMessages
{
	ID_USERNAME_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_GAME_MESSAGE_2 = ID_USER_PACKET_ENUM + 2,
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 3,
	ID_USERNAME_LIST = ID_USER_PACKET_ENUM + 4,
	ID_REQUEST_USERNAME = ID_USER_PACKET_ENUM + 5
};

void SendTimeStamps(vector<int> timestamps, vector<string> messages);
int main(int const argc, char const* const argv[])
{
	//raknet set up
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);

	//storage containers
	map<RakNet::SystemAddress,string> usersAndIps;
	vector<int> listOfTimestamps;
	vector<string> listOfMessages;

	printf("Starting the server.\n");
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				//client went bye bye
				printf("Another client has disconnected.\n");
				usersAndIps.erase(packet->systemAddress);
				break;
			}
			
			case ID_REMOTE_CONNECTION_LOST:
			{
				//client went bye bye
				printf("Another client has lost the connection.\n");
				usersAndIps.erase(packet->systemAddress);
				break;
			}
				
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				printf("Another client has connected.\n");
				break;
			}	
				
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
			{
				//client went bye bye
				printf("A client has disconnected.\n");
				usersAndIps.erase(packet->systemAddress);
				break;
			}
				
			case ID_CONNECTION_LOST:
			{
				//client went bye bye
				usersAndIps.erase(packet->systemAddress);
				break;
			}
				
			case ID_USERNAME_MESSAGE:
			{
				//user connected and sent us their username
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				usersAndIps[packet->systemAddress] = rs.C_String();
				cout << "User with username joined:" << usersAndIps.at(packet->systemAddress) << endl;
				break;
			}
			
			case ID_REQUEST_USERNAME:
			{
				//user requested the list so we have to send it
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_USERNAME_LIST);
				cout << "size: " << usersAndIps.size() <<endl;
				for (map<RakNet::SystemAddress, string>::iterator it = usersAndIps.begin(); it != usersAndIps.end(); it++)
				{
					bsOut.Write(it->second.c_str());
					cout << it->second << " : ";
				}
				
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
				break;
			}
			case ID_GAME_MESSAGE_1:
			{
				//get message from the client
				RakNet::Time time;
				RakNet::RakString message, rName, sName;
				bool pub;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(time);
				bsIn.Read(pub);
				bsIn.Read(message);
				bsIn.Read(rName);
				bsIn.Read(sName);
				listOfTimestamps.push_back((int)time);
				listOfMessages.push_back(message.C_String());
				cout << pub << message << rName << sName << "  \n" << endl;

				//send to correct people
				if (pub)
				{
					//send to everyone
					RakNet::BitStream bsRequestOut;
					bsRequestOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_2);
					bsRequestOut.Write("Sender:");
					bsRequestOut.Write(sName);
					bsRequestOut.Write(message);
					for (map<RakNet::SystemAddress, string>::iterator it = usersAndIps.begin(); it != usersAndIps.end(); it++)
					{
						peer->Send(&bsRequestOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->first, true);
					}
					
				}
				else
				{
					//send to only the receiver
					map<RakNet::SystemAddress, string>::iterator it;
					it = usersAndIps.find(rName.C_String());
					if (it != usersAndIps.end())
					{
						RakNet::BitStream bsRequestOut;
						bsRequestOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_2);
						bsRequestOut.Write("Sender:");
						bsRequestOut.Write(sName);
						bsRequestOut.Write(message);
						peer->Send(&bsRequestOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->first, false);
					}
				}
				break;
			}
			
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
		SendTimeStamps(listOfTimestamps, listOfMessages);
	}

	
	RakNet::RakPeerInterface::DestroyInstance(peer);
	return 0;
	printf("\n\n");
	system("pause");
}

//send the timestamps to a text file
void SendTimeStamps(vector<int> timestamps, vector<string> messages)
{
	ofstream file;
	file.open("Timestamp.txt");
	for (int i = 0; i < timestamps.size(); ++i)
	{
		file << timestamps[i] << ":" << messages[i] <<endl;
	}

	file.close();
}
