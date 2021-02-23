#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>
#include <vector>
#include "gpro-net/gpro-net-common/gpro-net-gamestate.h"
#include "gpro-net/gpro-net-common/gpro-net-console.h"

using namespace std;

class GameInstance
{
public:
	//variables
	int playerCount;
	string player1;
	string player2;
	vector<string> spectators;
	gpro_battleship attackBoard1;
	gpro_battleship attackBoard2;
	gpro_battleship defendBoard1;
	gpro_battleship defendBoard2;
	//functions
	GameInstance();
	bool setUp(int lengthOfShip, string playerName, string ship, int sx, int sy, int ex, int ey);

private:
	//currently using
	gpro_battleship_flag takeStringIntoShipType(string ship);
	bool checkIfShipCanBePlaced(int howManyMoreTimes, gpro_battleship board, int x, int y);
	bool checkIfEndCoordValid(int x, int y, int endX, int endY, gpro_battleship board, int size);
	void placeShip(int x, int y, int endX, int endY, gpro_battleship board, gpro_battleship_flag ship);
	bool checkCoord(int x, int y, int xMod, int yMod, gpro_battleship board, int iteration);
	
	//not currently using
	void gameLoop();
	void formatBoard(gpro_battleship board);
	bool canHit(gpro_battleship board, int x, int y);
	void TestBoard1(gpro_battleship board);
	void TestBoard2(gpro_battleship board);
	string printType(gpro_battleship_flag type);
	bool checkIfSunk(gpro_battleship board, int x, int y);
	bool checkIfWon(gpro_battleship board);
	bool play(gpro_battleship attackBoard, gpro_battleship defendBoard, string playerName);
};