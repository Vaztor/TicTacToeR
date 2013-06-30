#include <iostream>
#include <fstream>
#include <iostream>
#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <string>
#include <Windows.h>
#include <time.h>

using namespace std;

#pragma region Enums
enum Position
{
	TOPL,
	TOPM,
	TOPR,
	MIDL,
	MIDM,
	MIDR,
	BOTL,
	BOTM,
	BOTR
};

enum Value 
{
	X,
	O,
	None
};

#pragma endregion Enums

//Class for each box on the game board
class Section
{

public:

	bool locked;
	Position position;
	Value marker;

	Section()
	{
		locked = false;
		marker = None;
	}

	void setPosition(Position location)
	{
		if(locked == false)
		{
			position = location;
		}

	}

	void setValue(Value XO)
	{
		if(locked == false)
		{
		marker = XO;
		}
		
	}

	Position getPosition()
	{
		return position;
	}

	Value getValue()
	{
		return marker;
	}

};

#pragma region Declarations

const int SCREEN_WIDTH = 294;
const int SCREEN_HEIGHT = 276;
const int SCREEN_BPP = 32;

//Surface variable
SDL_Surface* x = NULL;
SDL_Surface* o = NULL;
SDL_Surface* box = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* message = NULL;

int player = 0;
Section board[3][3];

TTF_Font* font = NULL;
SDL_Color textColor = {255, 255, 255};
//Clipping Rectangles
SDL_Rect clipper[2];

//Event
SDL_Event event;

bool popup = false;

#pragma endregion Declarations

#pragma region Functions

#pragma region Init
bool init()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	if(screen == NULL)
	{
		return false;
	}

	if(TTF_Init() == -1)
	{
		return false;
	}

	SDL_WM_SetCaption("Tic-Tac-Toe", NULL);

	return true;
}

#pragma endregion Init

#pragma region Load Images
//Optimize all the images
SDL_Surface* loadImages(string filename)
{
	SDL_Surface* loaded = NULL;
	SDL_Surface* optimized = NULL;

	loaded = IMG_Load(filename.c_str());

	if(loaded != NULL)
	{
		optimized =  SDL_DisplayFormatAlpha(loaded);
		SDL_FreeSurface(loaded);
	}

	return optimized;
}

#pragma endregion Load Images

#pragma region Load Files

bool loadFiles()
{
	box = loadImages("box.png");
	x = loadImages("XO.tga");
	o = loadImages("XO.tga");
	font = TTF_OpenFont("lazy.ttf", 30);

	if(box == NULL||x == NULL || o == NULL || font == NULL)
	{
		return false;
	}

	return true;
}

#pragma endregion Load Files

#pragma region Clip XO

void clipXO()
{
	//Clipper 0 = X
	clipper[0].x = 0;
	clipper[0].y = 92;
	clipper[0].w = 98;
	clipper[0].h = 92;
	//Clipper 1 = 0
	clipper[1].x = 0;
	clipper[1].y = 0;
	clipper[1].w = 98;
	clipper[1].h = 92;

	/*
	apply(0,0, x, screen, &clipper[0]);
	apply(188,184, o, screen, &clipper[1]);
	*/
}

#pragma endregion Clip XO

#pragma region Apply Surface

void apply(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL)
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, clip, destination, &offset);
}

#pragma endregion Apply Surface

#pragma region Create Board

void createBoard()
{
	//Create the 9 boxes and give their positional values
	board[0][0].position = TOPL;
	board[0][1].position = TOPM;
	board[0][2].position = TOPR;
	board[1][0].position = MIDL;
	board[1][1].position = MIDM;
	board[1][2].position = MIDR;
	board[2][0].position = BOTL;
	board[2][1].position = BOTM;
	board[2][2].position = BOTR;

	for(int i = 0; i < 3; i++)
	{
		for(int j =0; j<3; j++)
		{
			board[i][j].locked = false;
			board[i][j].setValue(None);
		}
	}

	player = 0;
	popup = false;

	//Create all the boxes on the screen
	apply(0,0, box, screen);
	apply(98,0, box, screen);
	apply(196,0, box, screen);
	apply(0,92, box, screen);
	apply(98,92, box, screen);
	apply(196,92, box, screen);
	apply(0,184, box, screen);
	apply(98,184, box, screen);
	apply(196,184, box, screen);
}

#pragma endregion Create Board

#pragma region Clean Up
void cleanUp()
{
	SDL_FreeSurface(box);
	SDL_FreeSurface(x);
	SDL_FreeSurface(o);
	SDL_FreeSurface(message);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}

#pragma endregion Clean Up

#pragma endregion Functions

bool checkWin()
{

#pragma region Horizontal Checks 

	if ((board[0][0].getValue() == O) && (board[0][1].getValue() == O) && (board[0][2].getValue() == O))
	{
		return true;
	}

	if ((board[0][0].getValue() == X) && (board[0][1].getValue() == X) && (board[0][2].getValue() == X))
	{
		return true;
	}

	if ((board[1][0].getValue() == O) && (board[1][1].getValue() == O) && (board[1][2].getValue() == O))
	{
		return true;
	}

	if ((board[1][0].getValue() == X) && (board[1][1].getValue() == X) && (board[1][2].getValue() == X))
	{
		return true;
	}

	if ((board[2][0].getValue() == O) && (board[2][1].getValue() == O) && (board[2][2].getValue() == O))
	{
		return true;
	}

	if ((board[2][0].getValue() == X) && (board[2][1].getValue() == X) && (board[2][2].getValue() == X))
	{
		return true;
	}

#pragma endregion Horizontal Checks

#pragma region Vertical Checks


	if ((board[0][0].getValue() == O) && (board[1][0].getValue() == O) && (board[2][0].getValue() == O))
	{
		return true;
	}

	if ((board[0][0].getValue() == X) && (board[1][0].getValue() == X) && (board[2][0].getValue() == X))
	{
		return true;
	}

	if ((board[0][1].getValue() == O) && (board[1][1].getValue() == O) && (board[2][1].getValue() == O))
	{
		return true;
	}

	if ((board[0][1].getValue() == X) && (board[1][1].getValue() == X) && (board[2][1].getValue() == X))
	{
		return true;
	}

	if ((board[0][2].getValue() == O) && (board[1][2].getValue() == O) && (board[2][2].getValue() == O))
	{
		return true;
	}

	if ((board[0][2].getValue() == X) && (board[1][2].getValue() == X) && (board[2][2].getValue() == X))
	{
		return true;
	}

#pragma endregion Vertical Checks

#pragma region Diagonal Checks

	if ((board[0][0].getValue() == O) && (board[1][1].getValue() == O) && (board[2][2].getValue() == O))
	{
		return true;
	}

	if ((board[0][0].getValue() == X) && (board[1][1].getValue() == X) && (board[2][2].getValue() == X))
	{
		return true;
	}

	if ((board[0][2].getValue() == O) && (board[1][1].getValue() == O) && (board[2][0].getValue() == O))
	{
		return true;
	}

	if ((board[0][2].getValue() == X) && (board[1][1].getValue() == X) && (board[2][0].getValue() == X))
	{
		return true;
	}

#pragma endregion Diagonal Checks

	return false;
}

#pragma region AI

class AI
{
public:
	AI()
	{

	}

	bool win()
	{

#pragma region Win Horizontal

		if((board[0][0].getValue() == O) && (board[0][1].getValue() == O) && (board[0][2].getValue() == None))
		{
			apply(192,0,o,screen, &clipper[1]);
			board[0][2].setValue(O);
			board[0][2].locked = true;
			player =0;
			return true;
		}

		if((board[0][0].getValue() == O) && (board[0][1].getValue() == None) && (board[0][2].getValue() == O))
		{
			apply(96,0,o,screen, &clipper[1]);
			board[0][1].setValue(O);
			board[0][1].locked = true;
			player = 0;
			return true;
		}


		if((board[0][0].getValue() == None) && (board[0][1].getValue() == O) && (board[0][2].getValue() == O))
		{
			apply(-3,0,o,screen, &clipper[1]);
			board[0][0].setValue(O);
			board[0][0].locked = true;
			player = 0;
			return true;
		}

		if((board[1][0].getValue() == O) && (board[1][1].getValue() == O) && (board[1][2].getValue() == None))
		{
			apply(192,92,o,screen, &clipper[1]);
			board[1][2].setValue(O);
			board[1][2].locked = true;
			player =0;
			return true;
		}

		if((board[1][0].getValue() == O) && (board[1][1].getValue() == None) && (board[1][2].getValue() == O))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}

		if((board[1][0].getValue() == None) && (board[1][1].getValue() == O) && (board[1][2].getValue() == O))
		{
			apply(-3,92,o,screen, &clipper[1]);
			board[1][0].setValue(O);
			board[1][0].locked = true;
			player =0;
			return true;
		}


		if((board[2][0].getValue() == O) && (board[2][1].getValue() == O) && (board[2][2].getValue() == None))
		{
			apply(192,184,o,screen, &clipper[1]);
			board[2][2].setValue(O);
			board[2][2].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == O) && (board[2][1].getValue() == None) && (board[2][2].getValue() == O))
		{
			apply(96,184,o,screen, &clipper[1]);
			board[2][1].setValue(O);
			board[2][1].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == None) && (board[2][1].getValue() == O) && (board[2][2].getValue() == O))
		{
			apply(-3,184,o,screen, &clipper[1]);
			board[2][0].setValue(O);
			board[2][0].locked = true;
			player =0;
			return true;
		
		}
	#pragma endregion Win Horizontal

#pragma region Win Vertical

		if((board[0][0].getValue() == None) && (board[1][0].getValue() == O) && (board[2][0].getValue() == O))
		{
			apply(-3,0,o,screen, &clipper[1]);
			board[0][0].setValue(O);
			board[0][0].locked = true;
			player = 0;
			return true;
		}

		if((board[0][0].getValue() == O) && (board[1][0].getValue() == None) && (board[2][0].getValue() == O))
		{
			apply(-3,92,o,screen, &clipper[1]);
			board[1][0].setValue(O);
			board[1][0].locked = true;
			player =0;
			return true;
		}

		if((board[0][0].getValue() == O) && (board[1][0].getValue() == O) && (board[2][0].getValue() == None))
		{
			apply(-3,184,o,screen, &clipper[1]);
			board[2][0].setValue(O);
			board[2][0].locked = true;
			player =0;
			return true;
		}

		if((board[0][1].getValue() == None) && (board[1][1].getValue() == O) && (board[2][1].getValue() == O))
		{
			apply(96,0,o,screen, &clipper[1]);
			board[0][1].setValue(O);
			board[0][1].locked = true;
			player = 0;
			return true;
		}

		if((board[0][1].getValue() == O) && (board[1][1].getValue() == None) && (board[2][1].getValue() == O))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}


		if((board[0][1].getValue() == O) && (board[1][1].getValue() == O) && (board[2][1].getValue() == None))
		{
			apply(96,184,o,screen, &clipper[1]);
			board[2][1].setValue(O);
			board[2][1].locked = true;
			player =0;
			return true;
		}

		if((board[0][2].getValue() == None) && (board[1][2].getValue() == O) && (board[2][2].getValue() == O))
		{
			apply(192,0,o,screen, &clipper[1]);
			board[0][2].setValue(O);
			board[0][2].locked = true;
			player =0;
			return true;
		}

		if((board[0][2].getValue() == O) && (board[1][2].getValue() == None) && (board[2][2].getValue() == O))
		{
			apply(192,92,o,screen, &clipper[1]);
			board[1][2].setValue(O);
			board[1][2].locked = true;
			player =0;
			return true;
		}


		if((board[0][2].getValue() == O) && (board[1][2].getValue() == O) && (board[2][2].getValue() == None))
		{
			apply(192,184,o,screen, &clipper[1]);
			board[2][2].setValue(O);
			board[2][2].locked = true;
			player =0;
			return true;
		}

#pragma endregion Win Vertical

#pragma region Win Diag

		if((board[0][0].getValue() == None) && (board[1][1].getValue() == O) && (board[2][2].getValue() == O))
		{
			apply(-3,0,o,screen, &clipper[1]);
			board[0][0].setValue(O);
			board[0][0].locked = true;
			player = 0;
			return true;
		}


		if((board[0][0].getValue() == O) && (board[1][1].getValue() == None) && (board[2][2].getValue() == O))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}

		if((board[0][0].getValue() == O) && (board[1][1].getValue() == O) && (board[2][2].getValue() == None))
		{
			apply(192,184,o,screen, &clipper[1]);
			board[2][2].setValue(O);
			board[2][2].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == None) && (board[1][1].getValue() == O) && (board[0][2].getValue() == O))
		{
			apply(-3,184,o,screen, &clipper[1]);
			board[2][0].setValue(O);
			board[2][0].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == O) && (board[1][1].getValue() == None) && (board[0][2].getValue() == O))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == O) && (board[1][1].getValue() == O) && (board[0][2].getValue() == None))
		{
			apply(192,0,o,screen, &clipper[1]);
			board[0][2].setValue(O);
			board[0][2].locked = true;
			player =0;
			return true;
		}


#pragma endregion Win Diag

		return false;
	}

	bool block()
	{
#pragma region Block Horizontal

		if((board[0][0].getValue() == X) && (board[0][1].getValue() == X) && (board[0][2].getValue() == None))
		{
			apply(192,0,o,screen, &clipper[1]);
			board[0][2].setValue(O);
			board[0][2].locked = true;
			player =0;
			return true;
		}

		if((board[0][0].getValue() == X) && (board[0][1].getValue() == None) && (board[0][2].getValue() == X))
		{
			apply(96,0,o,screen, &clipper[1]);
			board[0][1].setValue(O);
			board[0][1].locked = true;
			player = 0;
			return true;
		}


		if((board[0][0].getValue() == None) && (board[0][1].getValue() == X) && (board[0][2].getValue() == X))
		{
			apply(-3,0,o,screen, &clipper[1]);
			board[0][0].setValue(O);
			board[0][0].locked = true;
			player = 0;
			return true;
		}

		if((board[1][0].getValue() == X) && (board[1][1].getValue() == X) && (board[1][2].getValue() == None))
		{
			apply(192,92,o,screen, &clipper[1]);
			board[1][2].setValue(O);
			board[1][2].locked = true;
			player =0;
			return true;
		}

		if((board[1][0].getValue() == X) && (board[1][1].getValue() == None) && (board[1][2].getValue() == X))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}

		if((board[1][0].getValue() == None) && (board[1][1].getValue() == X) && (board[1][2].getValue() == X))
		{
			apply(-3,92,o,screen, &clipper[1]);
			board[1][0].setValue(O);
			board[1][0].locked = true;
			player =0;
			return true;
		}


		if((board[2][0].getValue() == X) && (board[2][1].getValue() == X) && (board[2][2].getValue() == None))
		{
			apply(192,184,o,screen, &clipper[1]);
			board[2][2].setValue(O);
			board[2][2].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == X) && (board[2][1].getValue() == None) && (board[2][2].getValue() == X))
		{
			apply(96,184,o,screen, &clipper[1]);
			board[2][1].setValue(O);
			board[2][1].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == None) && (board[2][1].getValue() == X) && (board[2][2].getValue() == X))
		{
			apply(-3,184,o,screen, &clipper[1]);
			board[2][0].setValue(O);
			board[2][0].locked = true;
			player =0;
			return true;

		}
#pragma endregion Block Horizontal

#pragma region Block Vertical

		if((board[0][0].getValue() == None) && (board[1][0].getValue() == X) && (board[2][0].getValue() == X))
		{
			apply(-3,0,o,screen, &clipper[1]);
			board[0][0].setValue(O);
			board[0][0].locked = true;
			player = 0;
			return true;
		}

		if((board[0][0].getValue() == X) && (board[1][0].getValue() == None) && (board[2][0].getValue() == X))
		{
			apply(-3,92,o,screen, &clipper[1]);
			board[1][0].setValue(O);
			board[1][0].locked = true;
			player =0;
			return true;
		}

		if((board[0][0].getValue() == X) && (board[1][0].getValue() == X) && (board[2][0].getValue() == None))
		{
			apply(-3,184,o,screen, &clipper[1]);
			board[2][0].setValue(O);
			board[2][0].locked = true;
			player =0;
			return true;
		}

		if((board[0][1].getValue() == None) && (board[1][1].getValue() == X) && (board[2][1].getValue() == X))
		{
			apply(96,0,o,screen, &clipper[1]);
			board[0][1].setValue(O);
			board[0][1].locked = true;
			player = 0;
			return true;
		}

		if((board[0][1].getValue() == X) && (board[1][1].getValue() == None) && (board[2][1].getValue() == X))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}


		if((board[0][1].getValue() == X) && (board[1][1].getValue() == X) && (board[2][1].getValue() == None))
		{
			apply(96,184,o,screen, &clipper[1]);
			board[2][1].setValue(O);
			board[2][1].locked = true;
			player =0;
			return true;
		}

		if((board[0][2].getValue() == None) && (board[1][2].getValue() == X) && (board[2][2].getValue() == X))
		{
			apply(192,0,o,screen, &clipper[1]);
			board[0][2].setValue(O);
			board[0][2].locked = true;
			player =0;
			return true;
		}

		if((board[0][2].getValue() == X) && (board[1][2].getValue() == None) && (board[2][2].getValue() == X))
		{
			apply(192,92,o,screen, &clipper[1]);
			board[1][2].setValue(O);
			board[1][2].locked = true;
			player =0;
			return true;
		}


		if((board[0][2].getValue() == X) && (board[1][2].getValue() == X) && (board[2][2].getValue() == None))
		{
			apply(192,184,o,screen, &clipper[1]);
			board[2][2].setValue(O);
			board[2][2].locked = true;
			player =0;
			return true;
		}

#pragma endregion Block Vertical

#pragma region Block Diag

		if((board[0][0].getValue() == None) && (board[1][1].getValue() == X) && (board[2][2].getValue() == X))
		{
			apply(-3,0,o,screen, &clipper[1]);
			board[0][0].setValue(O);
			board[0][0].locked = true;
			player = 0;
			return true;
		}


		if((board[0][0].getValue() == X) && (board[1][1].getValue() == None) && (board[2][2].getValue() == X))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}

		if((board[0][0].getValue() == X) && (board[1][1].getValue() == X) && (board[2][2].getValue() == None))
		{
			apply(192,184,o,screen, &clipper[1]);
			board[2][2].setValue(O);
			board[2][2].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == None) && (board[1][1].getValue() == X) && (board[0][2].getValue() == X))
		{
			apply(-3,184,o,screen, &clipper[1]);
			board[2][0].setValue(O);
			board[2][0].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == X) && (board[1][1].getValue() == None) && (board[0][2].getValue() == X))
		{
			apply(96,92,o,screen, &clipper[1]);
			board[1][1].setValue(O);
			board[1][1].locked = true;
			player =0;
			return true;
		}

		if((board[2][0].getValue() == X) && (board[1][1].getValue() == X) && (board[0][2].getValue() == None))
		{
			apply(192,0,o,screen, &clipper[1]);
			board[0][2].setValue(O);
			board[0][2].locked = true;
			player =0;
			return true;
		}


#pragma endregion Block Diag

		return false;
	}
#pragma region Random Move

	void random_move()
	{
		srand(SDL_GetTicks());
		int randomMove = rand()%9;

		switch(randomMove)

		{
		case 0:

			if(board[0][0].locked == true)
			{
				player = 1;
				break;
			}

			if(board[0][0].locked == false)
			{
				apply(-3,0,o,screen, &clipper[1]);
				board[0][0].setValue(O);
				board[0][0].locked = true;
				player = 0;
				break;
			}

		case 1:

			if(board[0][1].locked == true)
			{
				player = 1;
				break;
			}

			if(board[0][1].locked == false)
			{
				apply(96,0,o,screen, &clipper[1]);
				board[0][1].setValue(O);
				board[0][1].locked = true;
				player = 0;
				break;
			}

		case 2:

			if(board[0][2].locked == true)
			{
				player = 1;
				break;
			}

			if(board[0][2].locked == false)
			{
				apply(192,0,o,screen, &clipper[1]);
				board[0][2].setValue(O);
				board[0][2].locked = true;
				player =0;
				break;
			}

			

		case 3:

			if(board[1][0].locked == true)
			{
				player = 1;
				break;
			}


			if(board[1][0].locked == false)
			{
				apply(-3,92,o,screen, &clipper[1]);
				board[1][0].setValue(O);
				board[1][0].locked = true;
				player =0;
				break;
			}

			

		case 4:

			if(board[1][1].locked == true)
			{
				player = 1;
				break;
			}

			if(board[1][1].locked == false)
			{
				apply(96,92,o,screen, &clipper[1]);
				board[1][1].setValue(O);
				board[1][1].locked = true;
				player =0;
				break;
			}

			

		case 5:

			if(board[1][2].locked == true)
			{
				player = 1;
				break;
			}

			if(board[1][2].locked == false)
			{
				apply(192,92,o,screen, &clipper[1]);
				board[1][2].setValue(O);
				board[1][2].locked = true;
				player =0;
				break;
			}

			

		case 6:

			if(board[2][0].locked == true)
			{
				player = 1;
				break;
			}

			if(board[2][0].locked == false)
			{
				apply(-3,184,o,screen, &clipper[1]);
				board[2][0].setValue(O);
				board[2][0].locked = true;
				player =0;
				break;
			}

			

		case 7:

			if(board[2][1].locked == true)
			{
				player = 1;
				break;
			}

			if(board[2][1].locked == false)
			{
				apply(96,184,o,screen, &clipper[1]);
				board[2][1].setValue(O);
				board[2][1].locked = true;
				player =0;
				break;
			}

			
		case 8:


			if(board[2][2].locked == true)
			{
				player = 1;
				break;
			}

			if(board[2][2].locked == false)
			{
				apply(192,184,o,screen, &clipper[1]);
				board[2][2].setValue(O);
				board[2][2].locked = true;
				player =0;
				break;
			}
	
		}
	}
	#pragma endregion Random Move
};

#pragma endregion AI

AI myAI;

void move()
{
		switch(event.key.keysym.sym)
		{
		case SDLK_KP7:


			if(board[0][0].locked == false)
			{
				apply(-3,0,x,screen, &clipper[0]);
				board[0][0].setValue(X);
				board[0][0].locked = true;
				player =1;
				break;
			}

			break;

		case SDLK_KP8:

			if(board[0][1].locked == false)
			{
				apply(96,0,x,screen, &clipper[0]);
				board[0][1].setValue(X);
				board[0][1].locked = true;
				player =1;
				break;
			}

			break;

		case SDLK_KP9:


			if(board[0][2].locked == false)
			{
				apply(192,0,x,screen, &clipper[0]);
				board[0][2].setValue(X);
				board[0][2].locked = true;
				player =1;
				break;
			}

			break;
		case SDLK_KP4:


			if(board[1][0].locked == false)
			{
				apply(-3,92,x,screen, &clipper[0]);
				board[1][0].setValue(X);
				board[1][0].locked = true;
				player =1;
				break;
			}
			
			break;
		case SDLK_KP5:


			if(board[1][1].locked == false)
			{
				apply(96,92,x,screen, &clipper[0]);
				board[1][1].setValue(X);
				board[1][1].locked = true;
				player =1;
				break;

			}
			break;
		case SDLK_KP6:


			if(board[1][2].locked == false)
			{
				apply(192,92,x,screen, &clipper[0]);
				board[1][2].setValue(X);
				board[1][2].locked = true;
				player =1;
				break;
			}
			break;
		case SDLK_KP1:

			if(board[2][0].locked == false)
			{
				apply(-3,184,x,screen, &clipper[0]);
				board[2][0].setValue(X);
				board[2][0].locked = true;
				player =1;
				break;
			}
			break;
		case SDLK_KP2:

			if(board[2][1].locked == false)
			{
				apply(96,184,x,screen, &clipper[0]);
				board[2][1].setValue(X);
				board[2][1].locked = true;
				player =1;
				break;
			}
			break;
		case SDLK_KP3:

			if(board[2][2].locked == false)
			{
				apply(192,184,x,screen, &clipper[0]);
				board[2][2].setValue(X);
				board[2][2].locked = true;
				player =1;
				break;
			}
		
	}
}

void moveAI()
{

	if(myAI.win() == false)
	{
		if(myAI.block() == false)
		{
			myAI.random_move();
		}
		
	}

}

int main( int argc, char* args[] )
{

	init();
	loadFiles();
	createBoard();
	clipXO();

	bool quit = false;

	while(quit == false)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_LCTRL)
				{
					SDL_FillRect(screen,NULL,0x000000);
					createBoard();
				}
				
				move();
				
			}	


			if(event.type == SDL_QUIT)
			{
				quit = true;
			}

			if(SDL_Flip(screen) == -1)
			{
				return 1;
			}

			if(checkWin())
			{
				for(int i = 0; i < 3; i++)
				{
					for(int j =0; j<3; j++)
					{
						board[i][j].locked = true;
					}
				}

				if(popup == false)
				{
					int pop = MessageBox(NULL, "Congratulations! Hit Left CTRL to Restart!", "Winner", MB_DEFBUTTON2);
					popup = true;
				}

				
			}
		}
		
		if(player ==1 )
		{
		moveAI();
		}
		
	}

	cleanUp();
	return 0;
}