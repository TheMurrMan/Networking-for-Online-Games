/*
* Main Client CPP
* 
* Ellie Perez:added to document
* Kyle Murray:added to document
* Main source for console client application.
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

	main-client.c/.cpp
	Main source for console client application.
*/

#include "gpro-net/gpro-net.h"
#include "gpro-net/Message.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/GetTime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
//tutorial 3
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID

#include "RakNet/RakPeerInterface.h"

// Project 2 stuff
#include "gpro-net/gpro-net-common/gpro-net-gamestate.h"
#include "gpro-net/gpro-net-common/gpro-net-console.h"
#define SERVER_PORT 4024

using namespace std;
//tutorial 3
enum GameMessages
{
	ID_USERNAME_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_GAME_MESSAGE_2 = ID_USER_PACKET_ENUM + 2,
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 3,
	ID_USERNAME_LIST = ID_USER_PACKET_ENUM + 4,
	ID_REQUEST_USERNAME = ID_USER_PACKET_ENUM + 5,

	//battleship
	ID_SEND_GAME_INSTANCE_LIST = ID_USER_PACKET_ENUM + 6,
	ID_JOIN_GAME_INSTANCE_LIST = ID_USER_PACKET_ENUM + 7,
	ID_ASK_SHIP_SETUP = ID_USER_PACKET_ENUM + 8,
	ID_SEND_SHIP_SETUP = ID_USER_PACKET_ENUM + 9,
	ID_ASK_PLAYER_HIT_COORD = ID_USER_PACKET_ENUM + 10,
	ID_SEND_HIT_MESSAGE = ID_USER_PACKET_ENUM + 11,
	ID_SEND_MISS_MESSAGE = ID_USER_PACKET_ENUM + 12,
	ID_SEND_SANK_MESSAGE = ID_USER_PACKET_ENUM + 13,
	ID_SEND_WON_MESSAGE = ID_USER_PACKET_ENUM + 14,
	
};

void showGui(RakNet::RakPeerInterface* peer, RakNet::Packet* packet, string username);
void chooseLobby(RakNet::RakPeerInterface* peer, RakNet::Packet* packet);
void sendShipCoord(RakNet::RakPeerInterface* peer, RakNet::Packet* packet, int size, string name);
void sendHitCoord(RakNet::RakPeerInterface* peer, RakNet::Packet* packet);

int main(int const argc, char const* const argv[])
{
	//gameLoop();
	//ask the user for this info and then send it to the server
	string username;
	cout << "Please type in your preferred username;" << endl;
	cin >> username;
	
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);

	printf("Starting the client.\n");
	peer->Connect("172.16.2.57", SERVER_PORT, 0, 0);

	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{

				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					//from raknet website cited above
					printf("Our connection request has been accepted.\n");
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_USERNAME_MESSAGE);


					//send user info
					bsOut.Write(username.c_str());
					peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

					break;
				}

				case ID_CONNECTION_LOST:
					printf("Connection lost.\n");
					break;

				case ID_USERNAME_LIST:
				{
					//get user name list from server
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					while (bsIn.Read(rs))
					{
						cout << rs << endl;
					}
					break;
				}

				case ID_GAME_MESSAGE_2:
				{
					//receive any messages from the server
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					cout << rs;
					bsIn.Read(rs);
					cout << rs << endl;
					bsIn.Read(rs);
					cout << "Message: " << rs << endl;
					break;
				}
				case ID_SEND_GAME_INSTANCE_LIST:
				{
					//reading out the lobby menu
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					cout << rs;
					chooseLobby(peer, packet);
					break;
				}
				case ID_ASK_SHIP_SETUP:
				{
					//reading in starting ship
					RakNet::RakString rs;
					int size = 0;
					RakNet::RakString shipName;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					bsIn.Read(size);
					bsIn.Read(shipName);
					cout << rs;
					//get both coords and send packet
					sendShipCoord(peer, packet, size, shipName.C_String());

					break;
				}

				case ID_ASK_PLAYER_HIT_COORD:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					cout << rs;
					sendHitCoord(peer, packet);

					break;
				}

				case ID_SEND_HIT_MESSAGE:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					cout << rs;

					break;
				}

				case ID_SEND_MISS_MESSAGE:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					cout << rs;

					break;
				}

				default:
				{
					break;
				}
			}
		}

		
	}
	//destroy instance
	RakNet::RakPeerInterface::DestroyInstance(peer);
	return 0;
	printf("\n\n");
	system("pause");
}

void showGui(RakNet::RakPeerInterface* peer, RakNet::Packet* packet, string username)
{
	//shows user the menu
	cout << "1) Do you want to the list of users?" << endl
		<< "2) Do you want to send a message?" << endl
		<< "3) Do you want to wait to receive a message?" << endl;
	char answer;
	cin >> answer;
	if (answer == '1')
	{
		//tell server we want the username list
		RakNet::BitStream bsRequestOut;
		bsRequestOut.Write((RakNet::MessageID)ID_REQUEST_USERNAME);
		bsRequestOut.Write("Someone requested to see the username list.");
		peer->Send(&bsRequestOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	}
	else if (answer == '2')
	{
		Message message;
		message.mSName = username;
		//asking the user for the message information
		char publicAns;
		cout << "Who do you want to send it to?" << endl;
		cin >> message.mRName;
		cout << "is this public? (Y/N)" << endl;
		cin >> publicAns;
		if (publicAns == 'Y')
		{
			message.mIsPublic = true;
		}
		cout << "Enter your message:" << endl;
		cin.ignore();
		getline(cin, message.mMessage);

		//sending message to server
		RakNet::BitStream bsRequestOut;
		bsRequestOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsRequestOut.Write((RakNet::Time)RakNet::GetTime());
		bsRequestOut.Write(message.mIsPublic);
		bsRequestOut.Write(message.mMessage.c_str());
		bsRequestOut.Write(message.mRName.c_str());
		bsRequestOut.Write(message.mSName.c_str());
		peer->Send(&bsRequestOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	}
	else if (answer == '3')
	{
		//tells user they will not see the menu again until they get a message
		cout << "We will wait until you get a message." << endl;
	}
}

void chooseLobby(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	int lobbyNum;
	cin >> lobbyNum;
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_JOIN_GAME_INSTANCE_LIST);
	bsOut.Write(lobbyNum);
	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
}

void sendShipCoord(RakNet::RakPeerInterface* peer, RakNet::Packet* packet, int size, string name)
{
	int sx = 0;
	int sy = 0;
	int ex = 0;
	int ey = 0;
	cin >> sx >> sy >> ex >> ey;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_SEND_SHIP_SETUP);
	bsOut.Write(sx);
	bsOut.Write(sy);
	bsOut.Write(ex);
	bsOut.Write(ey);
	bsOut.Write(size);
	bsOut.Write(name.c_str());
	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
}

void sendHitCoord(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	int x = 0;
	int y = 0;
	cin >> x >> y;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_SEND_SHIP_SETUP);
	bsOut.Write(x);
	bsOut.Write(y);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
}