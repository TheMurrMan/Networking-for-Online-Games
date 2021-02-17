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
	ID_REQUEST_USERNAME = ID_USER_PACKET_ENUM + 5
};

void showGui(RakNet::RakPeerInterface* peer, RakNet::Packet* packet, string username);void setUp(int lengthOfShip, gpro_battleship board, gpro_battleship_flag ship);
bool checkIfShipCanBePlaced(int howManyMoreTimes, gpro_battleship board, int x, int y);
bool checkIfEndCoordValid(int x, int y, int endX, int endY, gpro_battleship board, int size);
void placeShip(int x, int y, int endX, int endY, gpro_battleship board, gpro_battleship_flag ship);
void gameLoop();
void setUpPlayer(gpro_battleship board);
void formatBoard(gpro_battleship board);
bool checkCoord(int x, int y, int xMod, int yMod, gpro_battleship board, int iteration);

int main(int const argc, char const* const argv[])
{
	gameLoop();
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
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				
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
			default:
			{
				
			}
				break;
			}
			showGui(peer, packet, username);
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


void formatBoard(gpro_battleship board)
{
	cout << "------------------\n";
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			cout << "|";
			printf("%u", board[i][j]);
		}
		cout << "|";
		cout << endl << "------------------";
		cout << endl;
	}
}

void setUpPlayer(gpro_battleship board)
{
	cout << "Place your Destroyer (2 long)" << endl;
	setUp(2, board, gpro_battleship_ship_p2);
	formatBoard(board);
	cout << "Place your Submarine (3 long)" << endl;
	setUp(3, board, gpro_battleship_ship_s3);
	formatBoard(board);
	cout << "Place your Cruiser (3 long)" << endl;
	setUp(3, board, gpro_battleship_ship_d3);
	formatBoard(board);
	cout << "Place your Battleship (4 long)" << endl;
	setUp(4, board, gpro_battleship_ship_b4);
	formatBoard(board);
	cout << "Place your Carrier (5 long)" << endl;
	setUp(5, board, gpro_battleship_ship_c5);
	formatBoard(board);
}

void setUp(int lengthOfShip, gpro_battleship board, gpro_battleship_flag ship)
{
	//ask for start coord
	bool notValid = true;
	int x = 0;
	int y = 0;
	while (notValid)
	{
		cout << "Please enter the x part of the start coordiante of this ship (must be within bounds):" << endl;
		cin >> x;
		cout << "Please enter the y part of the start coordiante of this ship (must be within bounds):" << endl;
		cin >> y;
		//check valid
		notValid = !checkIfShipCanBePlaced(lengthOfShip, board, x, y);
		if (notValid)
		{
			cout << "invalid coords" << endl;
		}
		int x = 1;
	}
	notValid = true;
	//ask for end coord
	int endX = 0;
	int endY = 0;
	while (notValid)
	{
		cout << "Please enter the x part of the end coordiante of this ship (must be within bounds):" << endl;
		cin >> endX;
		cout << "Please enter the y part of the end coordiante of this ship (must be within bounds):" << endl;
		cin >> endY;
		//check valid
		notValid = !checkIfEndCoordValid(x, y, endX, endY, board, lengthOfShip);
		if (notValid)
		{
			cout << "invalid coords" << endl;
		}
	}
	//actually place the ship
	placeShip(x, y, endX, endY, board, ship);
}

bool checkIfShipCanBePlaced(int howManyMoreTimes, gpro_battleship board, int x, int y)
{
	bool retval = false;
	if (x < 10 && x >= 0 && y < 10 && y >= 0) //check if actually on board
	{
		if (!(board[x][y] && gpro_battleship_ship)) //check if a ship is already there
		{
			if (checkCoord(x, y, 1, 0, board, howManyMoreTimes))
			{
				retval = true;
			}
			if (checkCoord(x, y, -1, 0, board, howManyMoreTimes))
			{
				retval = true;
			}
			if (checkCoord(x, y, 0, 1, board, howManyMoreTimes))
			{
				retval = true;
			}
			if (checkCoord(x, y, 0, -1, board, howManyMoreTimes))
			{
				retval = true;
			}
		}
	}
	return retval;
	
}

bool checkCoord(int x, int y, int xMod, int yMod, gpro_battleship board, int iteration)
{
	bool retval = true;
	for (int i = 0; i < iteration; i++)
	{
		x += xMod;
		y += yMod;
		//checks if coord on the board AND if there is already a ship there
		if (!(x < 10 && x >= 0 && y < 10 && y >= 0 && !(board[x][y] && gpro_battleship_ship)))
		{
			retval = false;
		}
	}
	return retval;
}

bool checkIfEndCoordValid(int x, int y, int endX, int endY, gpro_battleship board, int size)
{
	//check which direction we have to iterate
	bool continueLoop = true;
	bool retval = false;
	if (abs(x - endX) == size - 1 || abs(y - endY) == size - 1)
	{
		if (endY == y)
		{
			while (continueLoop)
			{
				if (x >= endX)
				{
					x--;
					retval = !(board[x][y] && gpro_battleship_ship);
					continueLoop = x >= endX && retval; //continue while we still need to check AND while retval is valid
				}
				else
				{
					x++;
					retval = !(board[x][y] && gpro_battleship_ship);
					continueLoop = x <= endX && retval; //continue while we still need to check AND while retval is valid
				}
			}
		}
		else
		{
			while (continueLoop)
			{
				if (y >= endY)
				{
					y--;
					retval = !(board[x][y] && gpro_battleship_ship);
					continueLoop = y >= endY && retval; //continue while we still need to check AND while retval is valid
				}
				else
				{
					y++;
					retval = !(board[x][y] && gpro_battleship_ship);
					continueLoop = y <= endY && retval; //continue while we still need to check AND while retval is valid
				}
			}
		}
	}
	return retval;
}

void placeShip(int x, int y, int endX, int endY, gpro_battleship board, gpro_battleship_flag ship)
{
	bool continueLoop = true;
	bool retval = false;
	if (endY == y)
	{
		while (continueLoop)
		{
			if (x >= endX)
			{
				board[x][y] = ship;
				x--;
				continueLoop = x >= endX;
			}
			else
			{
				board[x][y] = ship;
				x++;
				continueLoop = x <= endX;
			}
			
		}
	}
	else
	{
		while (continueLoop)
		{
			if (y >= endY)
			{
				board[x][y] = ship;
				y--;
				continueLoop = y >= endY;
			}
			else
			{
				board[x][y] = ship;
				y++;
				continueLoop = y <= endY;
			}
		}
	}
}

void gameLoop()
{
	gpro_battleship board1;
	gpro_battleship_reset(board1);
	cout << "Player1 set up!" << endl;
	gpro_battleship board2;
	cout << "Player2 set up!" << endl;
	gpro_battleship_reset(board2);
	//show board
	bool gameContinue = true;
	while (gameContinue)
	{
		//player1
		//ask coord to hit and check if valid
		bool notValid = true;
		int x, y;
		while (notValid)
		{
			cout << "Player 1 choose a coord to hit (type x [enter] then y [enter])" << endl;
			cin >> x >> y;
			notValid = !canHit(board1, x, y);
		}
		notValid = true;
		//notify hit or miss
		if (gpro_battleship_ship && board2[x][y])
		{
			//NOT DONE IMPLEMENTING
			//check if sunk
			//check if won
		}
		else
		{
			cout << "You missed, you dumbass" << endl;
		}
		
	}
	
}



bool canHit(gpro_battleship board, int x, int y)
{
	bool retval = false;
	if (x < 10 && x >= 0 && y < 10 && y >= 0) //on board
	{
		return gpro_battleship_open && board[x][y]; //check if open
	}
	return retval;
}

