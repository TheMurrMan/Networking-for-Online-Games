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

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>
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

int main(int const argc, char const* const argv[])
{
	//code from http://www.jenkinssoftware.com/raknet/manual/tutorialsample1.html
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	map<RakNet::SystemAddress,string> usersAndIps;


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
				//need to delete client from map
				printf("Another client has disconnected.\n");
			}
				break;
			case ID_REMOTE_CONNECTION_LOST:
			{
				//need to delete client from map
				printf("Another client has lost the connection.\n");
			}
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				printf("Another client has connected.\n");
				
				//we need to add client to map
			}	
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
			{
				//we need to delete client to map
				printf("A client has disconnected.\n");
			}
				break;
			case ID_CONNECTION_LOST:
			{
				//we need to delete client to map
				printf("A client lost the connection.\n");
			}
				break;
			case ID_USERNAME_MESSAGE:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				usersAndIps[packet->systemAddress] = rs.C_String();
				cout << "User with username joined:" << usersAndIps.at(packet->systemAddress) << endl;
			}
			break;
			case ID_REQUEST_USERNAME:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_USERNAME_LIST);
				for (map<RakNet::SystemAddress, string>::iterator it = usersAndIps.begin(); it != usersAndIps.end(); it++)
				{
					bsOut.Write(it->second.c_str());
				}
				
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
			case ID_GAME_MESSAGE_1:
			{
				//if still no worky, try without struct
				RakNet::Time time;
				string pub, message, rName, sName;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(time);
				cout << time << endl;
				bsIn.Read(pub);
				bsIn.Read(message);
				bsIn.Read(rName);
				bsIn.Read(sName);
				cout << pub << message << rName << sName << "  n" << endl;
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}

		
	}
	RakNet::RakPeerInterface::DestroyInstance(peer);
	return 0;
	printf("\n\n");
	system("pause");
}
