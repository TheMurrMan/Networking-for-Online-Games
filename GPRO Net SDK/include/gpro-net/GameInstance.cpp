#include "GameInstance.h"

GameInstance::GameInstance()
{
	playerCount = 0;
	gpro_battleship_reset(attackBoard1);
	gpro_battleship_reset(attackBoard2);
	gpro_battleship_reset(defendBoard1);
	gpro_battleship_reset(defendBoard2);
	playersThatAreDone = 0;
}

void GameInstance::formatBoard(gpro_battleship board)
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


bool GameInstance::setUp(int lengthOfShip, string playerName, string ship, int sx, int sy, int ex, int ey)
{
	//get flag we need
	gpro_battleship_flag flag = takeStringIntoShipType(ship);
	bool startGood = false;
	bool endGood = false;
	bool retval = false;
	//check coords if player 1
	if (playerName == player1)
	{
		startGood = checkIfShipCanBePlaced(lengthOfShip, defendBoard1, sx, sy);
		endGood = checkIfEndCoordValid(sx, sy, ex, ey, defendBoard1, lengthOfShip);
		retval = startGood && endGood;
		if (retval)
		{
			//actually place the ship
			placeShip(sx, sy, ex, ey, defendBoard1, takeStringIntoShipType(ship));
		}
	}
	//check for player 2
	else
	{
		startGood = checkIfShipCanBePlaced(lengthOfShip, defendBoard2, sx, sy);
		endGood = checkIfEndCoordValid(sx, sy, ex, ey, defendBoard2, lengthOfShip);
		retval = startGood && endGood;
		if (retval)
		{
			//actually place the ship
			placeShip(sx, sy, ex, ey, defendBoard2, takeStringIntoShipType(ship));
		}
	}
	return retval;
}

gpro_battleship_flag GameInstance::takeStringIntoShipType(string ship)
{
	if (ship == "Patrol Boat")
	{
		return gpro_battleship_ship_p2;
	}
	else if (ship == "Destroyer")
	{
		return gpro_battleship_ship_d3;
	}
	else if (ship == "Submarine")
	{
		return gpro_battleship_ship_s3;
	}
	else if (ship == "Battleship")
	{
		return gpro_battleship_ship_b4;
	}
	else
	{
		return gpro_battleship_ship_c5;
	}
}

bool GameInstance::checkIfShipCanBePlaced(int howManyMoreTimes, gpro_battleship board, int x, int y)
{
	bool retval = false;
	if (x < 10 && x >= 0 && y < 10 && y >= 0) //check if actually on board
	{
		if (!(board[x][y] & gpro_battleship_ship)) //check if a ship is already there
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

bool GameInstance::checkCoord(int x, int y, int xMod, int yMod, gpro_battleship board, int iteration)
{
	bool retval = true;
	for (int i = 0; i < iteration; i++)
	{
		x += xMod;
		y += yMod;
		//checks if coord on the board AND if there is already a ship there
		if (!(x < 10 && x >= 0 && y < 10 && y >= 0 && !(board[x][y] & gpro_battleship_ship)))
		{
			retval = false;
		}
	}
	return retval;
}

bool GameInstance::checkIfEndCoordValid(int x, int y, int endX, int endY, gpro_battleship board, int size)
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
					retval = !(board[x][y] & gpro_battleship_ship);
					continueLoop = x >= endX && retval; //continue while we still need to check AND while retval is valid
				}
				else
				{
					x++;
					retval = !(board[x][y] & gpro_battleship_ship);
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
					retval = !(board[x][y] & gpro_battleship_ship);
					continueLoop = y >= endY && retval; //continue while we still need to check AND while retval is valid
				}
				else
				{
					y++;
					retval = !(board[x][y] & gpro_battleship_ship);
					continueLoop = y <= endY && retval; //continue while we still need to check AND while retval is valid
				}
			}
		}
	}
	return retval;
}

void GameInstance::placeShip(int x, int y, int endX, int endY, gpro_battleship board, gpro_battleship_flag ship)
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


void GameInstance::gameLoop()
{
	gpro_battleship board1;
	gpro_battleship_reset(board1);

	gpro_battleship board1_attack;
	gpro_battleship_reset(board1_attack);
	//setUpPlayer(board1);

	TestBoard1(board1);
	cout << "Player1 set up!" << endl;
	formatBoard(board1);

	//
	gpro_battleship board2;
	gpro_battleship_reset(board2);

	gpro_battleship board2_attack;
	gpro_battleship_reset(board2_attack);
	//setUpPlayer(board2);
	TestBoard2(board2);
	cout << "Player2 set up!" << endl;
	formatBoard(board2);

	//show board
	bool gameContinue = true;
	while (gameContinue)
	{
		gameContinue = play(board1_attack, board2, "Player1");
		if (gameContinue)
		{
			gameContinue = play(board2_attack, board1, "Player2");
		}
	}
}

bool GameInstance::play(gpro_battleship attackBoard, gpro_battleship defendBoard, string playerName)
{
	bool retVal = true;
	//ask coord to hit and check if valid
	bool notValid = true;
	int x, y;
	while (notValid)
	{
		cout << playerName << ", choose a coord to hit (type x [enter] then y [enter])" << endl;
		cin >> x >> y;
		notValid = !canHit(attackBoard, x, y);
		if (notValid)
		{
			cout << "You've Already tapped this ass/ Coord doesn't exist\n";
		}
	}

	notValid = true;
	//notify hit or miss
	if (gpro_battleship_ship & defendBoard[x][y])
	{
		//NOT DONE IMPLEMENTING
		cout << "You Hit, smartass\n";
		attackBoard[x][y] += gpro_battleship_hit;
		//check if sunk
		bool sunk = checkIfSunk(defendBoard, x, y);
		defendBoard[x][y] += gpro_battleship_damage;
		if (sunk)
		{
			//check if won
			if (checkIfWon(defendBoard))
			{
				retVal = false;
				cout << playerName + " Won :)\n";
			}
		}
	}
	else
	{
		cout << "You missed, you dumbass" << endl;
		attackBoard[x][y] += gpro_battleship_miss;
	}
	formatBoard(attackBoard);
	formatBoard(defendBoard);

	return retVal;
}

bool GameInstance::checkIfWon(gpro_battleship board)
{
	bool retVal = true;
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			if (board[i][j] & gpro_battleship_ship && !(board[i][j] & gpro_battleship_damage))
			{
				retVal = false;
			}
		}
	}

	return retVal;
}
bool GameInstance::checkIfSunk(gpro_battleship board, int x, int y)
{
	gpro_battleship_flag type;
	int size = 0;
	//assigning the size and ship type
	if (board[x][y] & gpro_battleship_ship_p2)
	{
		type = gpro_battleship_ship_p2;
		size = 2;
	}
	else if (board[x][y] & gpro_battleship_ship_s3)
	{
		type = gpro_battleship_ship_s3;
		size = 3;
	}
	else if (board[x][y] & gpro_battleship_ship_d3)
	{
		type = gpro_battleship_ship_d3;
		size = 3;
	}
	else if (board[x][y] & gpro_battleship_ship_b4)
	{
		type = gpro_battleship_ship_b4;
		size = 4;
	}
	else if (board[x][y] & gpro_battleship_ship_c5)
	{
		type = gpro_battleship_ship_c5;
		size = 5;
	}

	//checks if there are any connecting spaces left of the same ship
	bool left = true;
	bool up = true;
	bool down = true;
	bool right = true;
	for (int i = 1; i < size; i++)
	{
		if (down && x + i < 10 && x + i >= 0)
		{
			down = !(board[x + i][y] & type) || (board[x + 1][y] & gpro_battleship_damage); //is true when there are no ship spaces downward
		}
		if (up && x - i < 10 && x - i >= 0)
		{
			up = !(board[x - i][y] & type) || (board[x - 1][y] & gpro_battleship_damage); //is true when there are no ship spaces to the upward
		}
		if (left && y - i < 10 && y - i >= 0)
		{
			left = !(board[x][y - i] & type) || (board[x][y - i] & gpro_battleship_damage); //is true when there are no ship spaces to the left
		}
		if (right && y + i < 10 && y + i >= 0)
		{
			right = !(board[x][y + i] & type) || (board[x][y + i] & gpro_battleship_damage); //is true when there are no ship spaces to the right
		}
	}
	if (left && right && up && down)
	{
		cout << "You sunk my " << printType(type) << "!" << endl;
	}
	return left && right && up && down;
}

string GameInstance::printType(gpro_battleship_flag type)
{
	string ship;
	switch (type)
	{
	case 0x08:
		ship = "Patrol Boat";
		break;
	case 0x10:
		ship = "Submarine";
		break;
	case 0x20:
		ship = "Destroyer";
		break;
	case 0x40:
		ship = "Battleship";
		break;
	case 0x80:
		ship = "Carrier";
		break;
	default:
		ship = "There is no ship";
		break;
	}
	return ship;
}
bool GameInstance::canHit(gpro_battleship board, int x, int y)
{
	bool retval = false;
	if (x < 10 && x >= 0 && y < 10 && y >= 0) //on board
	{
		retval = !(gpro_battleship_hit & board[x][y]) && !(gpro_battleship_miss & board[x][y]); //check if open

	}
	return retval;
}

void GameInstance::TestBoard1(gpro_battleship board)
{
	gpro_battleship_reset(board);

	// setup destroyer
	board[0][0] += gpro_battleship_ship_p2;
	board[0][1] += gpro_battleship_ship_p2;

	// setup 	
	board[5][1] += gpro_battleship_ship_s3;
	board[5][2] += gpro_battleship_ship_s3;
	board[5][3] += gpro_battleship_ship_s3;

	//			
	board[5][4] += gpro_battleship_ship_d3;
	board[6][4] += gpro_battleship_ship_d3;
	board[7][4] += gpro_battleship_ship_d3;

	//			
	board[4][7] += gpro_battleship_ship_b4;
	board[5][7] += gpro_battleship_ship_b4;
	board[6][7] += gpro_battleship_ship_b4;
	board[7][7] += gpro_battleship_ship_b4;

	//
	board[4][0] += gpro_battleship_ship_c5;
	board[4][1] += gpro_battleship_ship_c5;
	board[4][2] += gpro_battleship_ship_c5;
	board[4][3] += gpro_battleship_ship_c5;
	board[4][4] += gpro_battleship_ship_c5;

}

void GameInstance::TestBoard2(gpro_battleship board)
{
	// setup destroyer
	board[0][8] += gpro_battleship_ship_p2;
	board[0][9] += gpro_battleship_ship_p2;

	//// setup 	
	//board[2][2] += gpro_battleship_ship_s3;
	//board[2][3] += gpro_battleship_ship_s3;
	//board[2][4] += gpro_battleship_ship_s3;
	//			
	////			
	//board[3][3] += gpro_battleship_ship_d3;
	//board[4][3] += gpro_battleship_ship_d3;
	//board[5][3]=+ gpro_battleship_ship_d3;
	//			
	////			
	//board[7][2] += gpro_battleship_ship_b4;
	//board[7][3] += gpro_battleship_ship_b4;
	//board[7][4] += gpro_battleship_ship_b4;
	//board[7][5] += gpro_battleship_ship_b4;
	//			
	////			
	//board[0][0] += gpro_battleship_ship_c5;
	//board[0][1] += gpro_battleship_ship_c5;
	//board[0][2] += gpro_battleship_ship_c5;
	//board[0][3] += gpro_battleship_ship_c5;
	//board[0][4] += gpro_battleship_ship_c5;
}
