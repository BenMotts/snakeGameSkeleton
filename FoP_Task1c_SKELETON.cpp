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
const char MOUSE('m');		//mouse
const char SNAKE('o');		//snake body
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game



struct Item {
	int x, y;
	char symbol;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, Item& mouse, vector<Item>& snakeBody);
	void renderGame(const char g[][SIZEX], const string& mess, const Item& spot, const Item& mouse);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], Item& s, const int kc, string& mess, Item& mouse, vector<Item>& snakeBody);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void endProgram();

	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	Item mouse = { 0, 0, MOUSE };
	string message("LET'S START...");	//current message to player
	vector<Item> snakeBody;

	//action...
	seed();								//seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze, spot, mouse, snakeBody);	//initialise grid (incl. walls and spot)
	int key;							//current key selected by player
	do {
		renderGame(grid, message, spot, mouse);			//display game info, modified grid and message
		key = toupper(getKeyPress()); 	//read in  selected key: arrow or letter command
		if (isArrowKey(key))
			updateGame(grid, maze, spot, key, message, mouse, snakeBody);
		else
			message = "INVALID KEY!";  //set 'Invalid key' message
	} while (!wantsToQuit(key));		//while user does not want to quit
	renderGame(grid, message, spot, mouse);			//display game info, modified grid and messages
	endProgram();						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, Item& mouse, vector<Item>& snakeBody)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSpotInitialCoordinates(Item& spot, const char[][SIZEX]);
	void setMouseCoordinates(Item& mouse, const char[][SIZEX]);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], Item& i, Item& mouse, vector<Item>& snakeBody);
	void initialiseSnakeBody(vector<Item>& snakeBody, Item& spot);

	setInitialMazeStructure(maze);			//initialise maze
	setSpotInitialCoordinates(spot, maze);	//Initialise snake head location
	initialiseSnakeBody(snakeBody, spot);	//Initialise body of snake
	setMouseCoordinates(mouse, maze);		//Initialise mouse location

	updateGrid(grid, maze, spot, mouse, snakeBody);		//prepare grid
}
void growSnake(vector<Item>& snakeBody, Item& mouse, Item& spot) {
	Item bodyPart;
	bodyPart.symbol = SNAKE;

	snakeBody.push_back(bodyPart);
}
void moveSnake(vector<Item>& snakeBody, Item& spot, int dx, int dy) {
	for (int i(snakeBody.size()); i != 1; --i) {

		snakeBody.at(i - 1).x = snakeBody.at(i - 2).x;
		snakeBody.at(i - 1).y = snakeBody.at(i - 2).y;
	}
	snakeBody.at(0).x = spot.x;
	snakeBody.at(0).y = spot.y;
	spot.x += dx;
	spot.y += dy;
}
void initialiseSnakeBody(vector<Item>& snakeBody, Item& spot) {
	for (int i(0); i < 3; ++i) {
		Item bodyPart;
		bodyPart.symbol = SNAKE;
		if (i == 0) {
			bodyPart.x = spot.x - 1;
			bodyPart.y = spot.y;
		}
		else {
			bodyPart.x = snakeBody.at(i - 1).x - 1;
			bodyPart.y = snakeBody.at(i - 1).y;
		}
		snakeBody.push_back(bodyPart);
	}
}

void setSpotInitialCoordinates(Item& spot, const char maze[][SIZEX])
{ //set spot coordinates inside the grid at random at beginning of game
	//do {
	//	spot.y = random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	//	spot.x = random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]
	//} while (maze[spot.y][spot.x] == WALL); //Ensuring spot does not land on WALL space, will make new spot if this is the case
	spot.y = 1;
	spot.x = 5;
}

void setMouseCoordinates(Item& mouse, const char maze[][SIZEX]) { //Random mouse coordinate
	do {
		mouse.y = random(SIZEY - 2);
		mouse.x = random(SIZEX - 2);
	} while (maze[mouse.y][mouse.x] == WALL || maze[mouse.y][mouse.x] == SNAKE || maze[mouse.y][mouse.x] == SPOT);
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

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], Item& spot, const int keyCode, string& mess, Item& mouse, vector<Item>& snakeBody)
{ //update game
	void updateGameData(const char g[][SIZEX], Item& s, const int kc, string& m, Item& mouse, vector<Item>& snakeBody);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], Item& s, Item& mouse, vector<Item>& snakeBody);

	updateGameData(grid, spot, keyCode, mess, mouse, snakeBody);		//move spot in required direction
	updateGrid(grid, maze, spot, mouse, snakeBody);					//update grid information
}


void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, Item& mouse, vector<Item>& snakeBody)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void eatMouse(Item& spot, Item& mouse, vector<Item>& snakeBody);
	void setMouseCoordinates(Item& mouse, const char[][SIZEX]);
	void moveSnake(vector<Item>& snakeBody, Item& spot, int dx, int dy);

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
		moveSnake(snakeBody, spot, dx, dy);
		break;
	case WALL:  		//hit a wall and stay there
		mess = "CANNOT GO THERE!";
		break;
	case MOUSE:
		mess = "nomnomnom";
		eatMouse(spot, mouse, snakeBody);
		moveSnake(snakeBody, spot, dx, dy);
		setMouseCoordinates(mouse, g);
		break;
	}
}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], Item& spot, Item& mouse, vector<Item>& snakeBody)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item& spot);

	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	placeItem(grid, spot);	//set spot in grid
	placeItem(grid, mouse); //set mouse position in grid
	for (int i(0); i < snakeBody.size(); ++i) {
		placeItem(grid, snakeBody.at(i));
	}
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item& item)
{ //place item at its new position in grid
	g[item.y][item.x] = item.symbol;
}

void eatMouse(Item& spot, Item& mouse, vector<Item>& snakeBody) {
	void growSnake(vector<Item>& snakeBody, Item& mouse, Item& spot);

	growSnake(snakeBody, mouse, spot);

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
	case DOWN:
		dx = 0;
		dy = +1;
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
void renderGame(const char g[][SIZEX], const string& mess, const Item& spot, const Item& mouse)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void paintGrid(const char g[][SIZEX], const Item&, const Item&);
	const string getDate();
	const string getTime();

	//display game title

	int hour, min, sec; //Variables for date and time

	getSystemTime(hour, min, sec);
	showMessage(clBlue, clWhite, 0, 0, "Ben And Ryan's SNAKE GAME");
	showMessage(clBlack, clBlack, 11, 0, " ");
	showMessage(clWhite, clRed, 40, 2, "FoP Task 1c - February 2019   ");
	showMessage(clBlack, clGreen, 90, 2, getDate() + " " + getTime());
	showMessage(clBlack, clBlack, 40, 3, " ");
	showMessage(clBlue, clWhite, 40, 4, "SE, Ryan Evans b8017748");
	showMessage(clBlue, clWhite, 40, 5, "Ben Mottershead b7003892");
	//display menu options available
	showMessage(clBlack, clBlack, 40, 6, " ");
	showMessage(clRed, clYellow, 40, 7, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clRed, clYellow, 40, 10, "TO QUIT - ENTER 'Q'           ");
	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message

	//display grid contents
	paintGrid(g, spot, mouse);

}

void paintGrid(const char g[][SIZEX], const Item& spot, const Item& mouse)
{ //display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
			if (g[row][col] != g[spot.y][spot.x] && g[row][col] != g[mouse.y][mouse.x]) //If isn't spot
				cout << g[row][col];	//output cell content
			else if (g[row][col] == g[spot.y][spot.x]) { //If it is a spot
				selectTextColour(clMagenta);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
			else {
				selectTextColour(clDarkYellow);
				cout << g[row][col];
				selectTextColour(clWhite);
			}
		cout << endl;
	}
	
}

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	showMessage(clYellow, clDarkRed, 40, 13, "Thanks For Playing!");
	system("pause");	//hold output screen until a keyboard key is hit
}
