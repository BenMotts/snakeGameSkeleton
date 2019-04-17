//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 23 February 2018
//---------------------------------------------------------------------------

//Go to 'View > Task List' menu to open the 'Task List' pane listing the initial amendments needed to this program

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <windows.h> //For beep sound effect
#include <fstream> //For saving player info
#include <sstream> //For reading player info strings and converting to integers

using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for seed, random
#include "ConsoleUtils.h"	//for clrscr, gotoxy, etc.
#include "TimeUtils.h"		//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(13);    	//horizontal dimension
const int  SIZEY(10);		//vertical dimension
//defining symbols used for display of the grid and content
const char SPOT('0');   	//spot
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char MOUSE('@');		//mouse
const char SNAKE('o');		//snake body
const char POWERUP('+');	//Power up pill
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
//defining game attributes
const int MAXSCORE(10);
const int MOVESFORPILL(10);


struct Item {		//Item, for power up pill and mice
	int x, y;
	char symbol;
};

struct Player {		//Player, to contain all variables relating to player

	string name;			//Player's name. 
	int playerScore;		//Current score
	int miceNumber;			//Number of mice eaten
	int movesForPill;		//Moves while pill is active (10 MAXIMUM(defined as global))
	int miceUntilPill;		//2 Mice allowed between each pill
	bool gameOver;			//Is the game over for this player
	bool mouseEaten;		//For checking if snake has eaten a mouse during current movement
	bool isCheat;			//Is this current player cheating
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, Item& mouse, vector<Item>& snakeBody, Item& pill);
	void renderGame(const char g[][SIZEX], const string& mess, const Item& spot, const Item& mouse, const Item& pill, const Player& player);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], Item& s, const int kc, string& mess, Item& pill, Item& mouse, vector<Item>& snakeBody, Player& player);
	bool wantsToQuit(const int key);	//Check if player presses 'Q'
	bool isArrowKey(const int k);		//Is they keypress an arrowkey?
	int  getKeyPress();			
	void endProgram(Player& player);	//End the game, display save option
	void saveData(const Player& player);	//Save the player's data
	void shrinkSnake(vector<Item>& snakeBody);	//Decrease size of snake back to original
	void getPlayer(Player& player);		//Get a new player, or load an old one

	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	Item mouse = { 0, 0, MOUSE };
	Item pill = { 0,0, TUNNEL };
	Player player = {"",0, 0, 0, 2, false, false, false };
	string message("LET'S START...");	//current message to player
	vector<Item> snakeBody;		//Vector containing all snakebody items

	//action...
	seed();								//seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze, spot, mouse, snakeBody, pill);	//initialise grid (incl. walls and spot)
	int key(NULL), oldKey(NULL);							//current key selected by player
	getPlayer(player);
	renderGame(grid, message, spot, mouse, pill, player);			//display game info, modified grid and message
	key = toupper(getKeyPress());
	do {
		renderGame(grid, message, spot, mouse, pill, player);			//display game info, modified grid and message
		oldKey = key;
		if (_kbhit())
			key = toupper(getKeyPress()); 	//read in  selected key: arrow or letter command
		else key = oldKey;
		if (key != NULL) {
			if (isArrowKey(key)) {
				updateGame(grid, maze, spot, key, message, pill, mouse, snakeBody, player);
				Sleep(300);	//COntrol speed at which snake moves. Lower number = faster
			}
			else if (toupper(key) == 'C') {	//Player enables cheat mode
				if (!player.isCheat) {
					shrinkSnake(snakeBody);
					player.isCheat = true;
					Beep(500, 200);
					Beep(500, 200);
					Beep(500, 200);
				}
				else {
					player.isCheat = false;
					Beep(500, 200);
					Beep(500, 200);
					Beep(500, 200);
				}
			}
			else if (toupper(key) == 'S') {	//Player saves game
				saveData(player);
				key = oldKey;	//Ensures game continues without stopping
			}
		}
	} while (!wantsToQuit(key) && !player.gameOver);		//while user does not want to quit
	renderGame(grid, message, spot, mouse, pill, player);			//display game info, modified grid and messages
	endProgram(player);						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, Item& mouse, vector<Item>& snakeBody, Item& pill)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSpotInitialCoordinates(Item& spot, const char[][SIZEX]);
	void setItemCoordinates(Item& mouse, const char[][SIZEX]);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], Item& i, Item& mouse, vector<Item>& snakeBody, Item& pill);
	void initialiseSnakeBody(vector<Item>& snakeBody, Item& spot);

	setInitialMazeStructure(maze);			//initialise maze
	setSpotInitialCoordinates(spot, maze);	//Initialise snake head location
	initialiseSnakeBody(snakeBody, spot);	//Initialise body of snake
	setItemCoordinates(mouse, maze);		//Initialise mouse location

	updateGrid(grid, maze, spot, mouse, snakeBody, pill);		//prepare grid
}

void growSnake(vector<Item>& snakeBody, Item& mouse, Item& spot) {			//Increase the size of the snake
	Item bodyPart;														//Create new Item bodypart
	bodyPart.symbol = SNAKE;											//Set bodypart symbol to body (declared as global variable)

	snakeBody.push_back(bodyPart);									//Add to end of vector

}

void shrinkSnake(vector<Item>& snakeBody) {				//Decrease snake size down to 3, on keypress 'c'
	for (int i(snakeBody.size()); i > 3; --i) {			//For every bodypart past size 3
		snakeBody.pop_back();							//Remove from vector
	}
}

void moveSnake(vector<Item>& snakeBody, Item& spot, int dx, int dy) {			//Move the body of the snake, to follow the spot
	for (int i(snakeBody.size()); i != 1; --i) {

		snakeBody.at(i - 1).x = snakeBody.at(i - 2).x;
		snakeBody.at(i - 1).y = snakeBody.at(i - 2).y;
	}
	snakeBody.at(0).x = spot.x;
	snakeBody.at(0).y = spot.y;
	spot.x += dx;
	spot.y += dy;
}

void initialiseSnakeBody(vector<Item>& snakeBody, Item& spot) {					//Create initial snakebody. Adds Items to vector of items
	for (int i(0); i < 3; ++i) {
		Item bodyPart;
		bodyPart.symbol = SNAKE;
		//		if (i == 0) {
		bodyPart.x = spot.x;
		bodyPart.y = spot.y;
		//		}
				//else {
				//	bodyPart.x = snakeBody.at(i - 1).x - 1;
				//	bodyPart.y = snakeBody.at(i - 1).y;
				//}
		snakeBody.push_back(bodyPart);
	}
}

void setSpotInitialCoordinates(Item& spot, const char maze[][SIZEX])				//Set initaial coordinates of head of snake.
{
	//Can make this random, specification currently asks us to spawn the snake in a central position
	spot.y = 5;
	spot.x = 5;
}

void setItemCoordinates(Item& item, const char maze[][SIZEX]) {					//Random mouse coordinate, Avoids snakeBody and spot
	do {
		item.y = random(SIZEY - 2);
		item.x = random(SIZEX - 2);
	} while (maze[item.y][item.x] == WALL || maze[item.y][item.x] == SNAKE || maze[item.y][item.x] == SPOT || maze[item.y][item.x] == POWERUP);
}
void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', '#', '#', '#', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' } };
	//with '#' for wall, ' ' for tunnel, etc. 
	//copy into maze structure with appropriate symbols
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				//not a direct copy, in case the symbols used are changed
			case '#': maze[row][col] = WALL; break;
			case ' ': maze[row][col] = TUNNEL; break;
			}
}

//---------------------------------------------------------------------------
//----- Update Game
//---------------------------------------------------------------------------

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], Item& spot, const int keyCode, string& mess, Item& pill, Item& mouse, vector<Item>& snakeBody, Player& player)
{ //update game
	void updateGameData(const char g[][SIZEX], Item& s, const int kc, string& m, Item& pill, Item& mouse, vector<Item>& snakeBody, Player& player);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], Item& s, Item& mouse, vector<Item>& snakeBody, Item& pill);

	updateGameData(grid, spot, keyCode, mess, pill, mouse, snakeBody, player);		//move spot in required direction
	updateGrid(grid, maze, spot, mouse, snakeBody, pill);					//update grid information
}


void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, Item& pill, Item& mouse, vector<Item>& snakeBody, Player& player)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void setItemCoordinates(Item& Item, const char[][SIZEX]);
	void moveSnake(vector<Item>& snakeBody, Item& spot, int dx, int dy);
	void powerUpPill(Item& pill, Player& player, const char g[][SIZEX]);
	void growSnake(vector<Item>& snakeBody, Item& mouse, Item& spot);
	assert(isArrowKey(key));

	//reset message to blank
	mess = "";

	//calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);
	//check new target position in grid and update game data (incl. spot coordinates) if move is possible
	switch (g[spot.y + dy][spot.x + dx])
	{			//...depending on what's on the target position in grid...
	case TUNNEL:		//can move
		if (!player.isCheat) {
			player.playerScore++;		//Increase player score as long as they aren't currently cheating
		}
		if (player.mouseEaten) {
			growSnake(snakeBody, mouse, spot); //Will make snake length increase by 2
			player.mouseEaten = false;
		}
		moveSnake(snakeBody, spot, dx, dy);
		powerUpPill(pill, player, g);
		break;
	case WALL:  		//hit a wall and stay there
		mess = "CANNOT GO THERE!";
		player.gameOver = true;
		break;
	case MOUSE:
		mess = "nomnomnom";
		player.miceNumber++;
		if (!player.isCheat) {
			growSnake(snakeBody, mouse, spot);
			player.mouseEaten = true;
		}
		moveSnake(snakeBody, spot, dx, dy);
		setItemCoordinates(mouse, g);
		player.miceUntilPill--;
		powerUpPill(pill, player, g);
		break;
	case POWERUP:
		mess = "POWER UP!";
		shrinkSnake(snakeBody);
		moveSnake(snakeBody, spot, dx, dy);
		pill.symbol = TUNNEL;
		setItemCoordinates(pill, g);
		player.movesForPill = 0;
		break;
	case SNAKE:
		mess = "CANNOT GO THERE!";
		player.gameOver = true;
	}
}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], Item& spot, Item& mouse, vector<Item>& snakeBody, Item& pill)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item& spot);

	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	placeItem(grid, mouse); //set mouse position in grid
	if (pill.symbol == POWERUP)
		placeItem(grid, pill);
	for (int i(0); i < snakeBody.size(); ++i) {
		placeItem(grid, snakeBody.at(i));
	}
	placeItem(grid, spot);	//set spot in grid
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void powerUpPill(Item& pill, Player& player, const char g[][SIZEX]) {	//For when a player hits a powerup
	void setItemCoordinates(Item& pill, const char g[][SIZEX]);

	if (pill.symbol == POWERUP) {	//If powerup currently active
		player.movesForPill++;
		if (player.movesForPill == MOVESFORPILL) {
			pill.symbol = TUNNEL;
			setItemCoordinates(pill, g);
		}
	}
	else if (pill.symbol == TUNNEL && player.miceUntilPill == 0) {
		pill.symbol = POWERUP;
		setItemCoordinates(pill, g);
		player.movesForPill = 0;
		player.miceUntilPill = 2;
	}
}
void placeItem(char g[][SIZEX], const Item& item)
{ //place item at its new position in grid
	g[item.y][item.x] = item.symbol;
}

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
	bool isArrowKey(const int k);
	assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case DOWN:			dx = 0;
		dy = +1;
		dx = 0;
		break;
	case UP:
		dx = 0;
		dy = -1;
		break;
	}
}

int getKeyPress()
{ //get key or command selected by user
  //KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
	return keyPressed;
}

bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return toupper(key) == QUIT;
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(int x)
{	//convert an integer to a string
	std::ostringstream os;
	os << x;
	return os.str();
}

string tostring(char x)
{	//convert a char to a string
	std::ostringstream os;
	os << x;
	return os.str();
}

void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message)
{	//display a string using specified colour at a given position 
	gotoxy(x, y);
	selectBackColour(backColour);
	selectTextColour(textColour);
	cout << message + string(40 - message.length(), ' ');
}

void renderGame(const char g[][SIZEX], const string& mess, const Item& spot, const Item& mouse, const Item& pill, const Player& player)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void paintGrid(const char g[][SIZEX], const Item& spot, const Item& mouse, const Item& pill);
	const string getDate();
	const string getTime();

	//display game title

	int hour, min, sec; //Variables for date and time

	getSystemTime(hour, min, sec);
	showMessage(clBlue, clWhite, 0, 0, "Ben And Ryan's SNAKE GAME");
	showMessage(clBlack, clBlack, 11, 0, " ");
	showMessage(clWhite, clRed, 40, 2, "FoP Task 1c - February 2019   ");
	showMessage(clBlack, clGreen, 90, 2, getDate() + " " + getTime());
	showMessage(clWhite, clRed, 40, 3, "SE, Ryan Evans b8017748");
	showMessage(clWhite, clRed, 40, 4, "Ben Mottershead b7003892");
	showMessage(clYellow, clBlack, 40, 6, "Player name: " + player.name);
	showMessage(clYellow, clBlack, 40, 7, "Score: " + tostring(player.playerScore));
	showMessage(clYellow, clBlack, 40, 8, "Mice caught: " + tostring(player.miceNumber));
	//display menu options available
	showMessage(clBlack, clBlack, 40, 9, " ");
	showMessage(clRed, clYellow, 40, 10, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clRed, clYellow, 40, 11, "TO CHEAT - USE 'C' ");
	showMessage(clRed, clYellow, 40, 12, "TO SAVE - USE 'S'");
	showMessage(clRed, clYellow, 40, 13, "TO QUIT -USE 'Q'");
	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 14, mess);	//display current message
	if (player.isCheat) {
		showMessage(clDarkRed, clWhite, 40, 16, "CHEAT MODE ACTIVATED");
		showMessage(clDarkRed, clWhite, 40, 17, "SCORE NOT RECORDED");
	}
	else {
		showMessage(clDarkGreen, clWhite, 40, 16, "CHEAT MODE DEACTIVATED");
		showMessage(clDarkGreen, clWhite, 40, 17, "GOOD LUCK!");
	}

	//display grid contents
	paintGrid(g, spot, mouse, pill);

}

void paintGrid(const char g[][SIZEX], const Item& spot, const Item& mouse, const Item& pill)
{ //display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)	//If snake head
			if (g[row][col] == g[spot.y][spot.x]) {
				selectTextColour(clGreen);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
			else if (g[row][col] == WALL) {		//If wall
				selectTextColour(clRed);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
			else if (g[row][col] == SNAKE) {	//If snake body
				selectTextColour(clMagenta);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
			else if (g[row][col] == g[pill.y][pill.x]) {	//If powerup pill
				selectTextColour(clCyan);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
			else if (g[row][col] == g[mouse.y][mouse.x]) {	//If mouse
				selectTextColour(clDarkYellow);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
			else {
				cout << g[row][col];	//Tunnel pieces
			}
		cout << endl;
	}

}

//---------------------------------------------------------------------------
//----- Saving and loading data
//---------------------------------------------------------------------------

void getPlayer(Player& player) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void loadGame(Player& player);
	string readyText;

	clrscr();
	showMessage(clBlack, clDarkRed, 20, 14, "Enter your name: ");
	cin >> player.name;
	loadGame(player);	//Load or start new game
	showMessage(clBlack, clDarkRed, 20, 16, "Get ready");
	showMessage(clBlack, clDarkYellow, 20, 23, "@ = MOUSE: Increase snake size");
	Sleep(250);
	showMessage(clBlack, clDarkRed, 20, 17, "Get ready.");
	showMessage(clBlack, clCyan, 20, 24, "+ = POWERUP: Decrease snake size");
	Sleep(250);
	showMessage(clBlack, clDarkRed, 20, 18, "Get ready..");
	Sleep(250);
	showMessage(clBlack, clDarkRed, 20, 19, "Get ready...");
	Sleep(500);
	showMessage(clBlack, clCyan, 20, 21, "Press enter to start: ");
	getchar();
	getchar();
	clrscr();
}

void saveData(const 
	Player& player) {
	ofstream fout(player.name + ".save");		//Get write file
	char dem = '\n';	//Data end marker, to put data onto new line
	if (fout.fail())	//Error while creating/accessing file
		cout << "An error has occured while saving the file.";
	else {	//Save information to file
		fout << player.playerScore << dem;	//Save player score
		fout << player.miceNumber;	//Save number of mice eaten
		fout.close();	//Close file
	}
}

void loadGame(Player& player) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	char loadChoice;

	ifstream fin(player.name + ".save");		//Get read file

	if (!fin.fail()) {
		clrscr();
		showMessage(clBlack, clBlue, 20, 14, "Load game? (Y / N): ");
		do {
			cin >> loadChoice;
			loadChoice = toupper(loadChoice);
			if (loadChoice != 'Y' && loadChoice != 'N')
				showMessage(clBlack, clRed, 10, 11, "Incorrect choice. (Y / N): ");
		} while (loadChoice != 'Y' && loadChoice != 'N');

		if (loadChoice == 'Y') {
			string dataLine;		//String variable for each line of data read in
			getline(fin, dataLine);		//Get new line of data
			istringstream ips(dataLine);		//Converting to integer
			ips >> player.playerScore;			//Set that integer to player score
			getline(fin, dataLine);
			istringstream ime(dataLine);
			ime >> player.miceNumber;
			clrscr();
			fin.close();			//Close file
		}
	}
	clrscr();
}

//---------------------------------------------------------------------------
//----- End of game
//---------------------------------------------------------------------------

void endProgram(Player& player)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void saveData(const Player& player);
	char saveChoice = ' ';

	clrscr();
	showMessage(clBlack, clDarkRed, 20, 12, "GAME OVER");
	showMessage(clBlack, clDarkYellow, 20, 13, "Final score: " + to_string(player.playerScore));
	showMessage(clBlack, clDarkYellow, 20, 14, "Mice eaten: " + to_string(player.miceNumber));
	showMessage(clBlack, clDarkRed, 20, 15, "Thanks For Playing!");
	if (!player.isCheat) {	//Player can only save game if they die while not cheating
		do {
			showMessage(clBlack, clBlue, 20, 16, "Save game (Y / N):");		//Give user choice to save progress
			cin >> saveChoice;
			saveChoice = toupper(saveChoice);
		} while (saveChoice != 'Y' && saveChoice != 'N');	//Ensure user's option is Y or N
		if (saveChoice == 'Y')
			saveData(player);		//Save game
	}
	selectTextColour(clCyan);
	cout << "                    ";
	system("pause");	//Hold end screen
}