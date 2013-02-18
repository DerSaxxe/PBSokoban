#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <dlfcn.h>
#include "inkview.h"

#define historyLimit 1
#define boardMaxSize 25 //Hard to tell - currently this is 25 = (800 - 50)/30.

extern const ibitmap box30, boxOnTheSpot30, empty30, player30, playerRight30, playerOnTheSpot30, playerOnTheSpotRight30, spot30, wall30, kup, kdown, kleft, kright, kback, kmenu;
extern const ibitmap box50, boxOnTheSpot50, empty50, player50, playerRight50, playerOnTheSpot50, playerOnTheSpotRight50, spot50, wall50;
ifont *font;
ifont *fonttitle;

char *originalLevel =
  ";MINICOSMOS 01\n\n  #####\n###   #\n# $ # ##\n# #  . #\n#    # #\n## #   #\n #@  ###\n #####\n\n;MINICOSMOS 02\n\n  #####\n###   #\n# $ # ##\n# #  . #\n#    # #\n##$#.  #\n #@  ###\n #####\n\n;MINICOSMOS 03\n\n  #####\n###   #\n# $ # ##\n# #  . #\n# .  # #\n##$#.$ #\n #@  ###\n #####\n\n;MINICOSMOS 04\n\n    ####\n#####  #\n#   $  #\n#  .#  #\n## ## ##\n#      #\n# @#   #\n#  #####\n####\n\n;MINICOSMOS 05\n\n    ####\n#####  #\n#   $  #\n# *.#  #\n## ## ##\n#      #\n# @#   #\n#  #####\n####\n\n;MINICOSMOS 06\n\n    ####\n#####  #\n#   *  #\n# *.#  #\n## ## ##\n# $    #\n# @#   #\n#  #####\n####\n\n;MINICOSMOS 07\n\n #####\n #   ##\n## #$ ##\n# $    #\n#. .#  #\n### @ ##\n  # # #\n  #   #\n  #####\n\n;MINICOSMOS 08\n\n #####\n #   ##\n##.#$ ##\n# $    #\n#. .#$ #\n### @ ##\n  # # #\n  #   #\n  #####\n\n;MINICOSMOS 09\n\n #####\n #   #\n##$# ###\n#   $@ #\n# #  # #\n# #. . #\n#   ####\n#####\n\n;MINICOSMOS 10\n\n #####\n #   #\n##$# ###\n#  .$@ #\n# #  # #\n# #..$ #\n#   ####\n#####\n\n;MINICOSMOS 11\n\n #####\n##   ###\n# . .  #\n# # ## ##\n#    $$@#\n### #   #\n  #   ###\n  #####\n\n;MINICOSMOS 12\n\n #####\n##   ###\n# . . .#\n# # ## ##\n#    $$@#\n### # $ #\n  #   ###\n  #####\n\n;MINICOSMOS 13\n\n    ####\n ####  #\n## $   #\n#  # #$#\n#.@.   ##\n## # #  #\n #      #\n #  #####\n ####\n\n;MINICOSMOS 14\n\n    ####\n ####  #\n## $   #\n#  # #$#\n#.@..  ##\n## # #  #\n #   $  #\n #  #####\n ####\n\n;MINICOSMOS 15\n\n   ####\n####  #\n# $   #\n#  .# ##\n## #.  #\n# @  $ #\n#   ####\n#####\n\n;MINICOSMOS 16\n\n   ####\n####  #\n# $ $ #\n#  .# ##\n## #.  #\n# @  $ #\n#.  ####\n#####\n\n;MINICOSMOS 17\n\n    #####\n   ##   ##\n  ## .#  #\n ##   @  #\n##    #  #\n#  $ #####\n# * ##\n#  ##\n####\n\n;MINICOSMOS 18\n\n    #####\n   ##   ##\n  ## .#  #\n ##   @  #\n##  * #  #\n#  $ #####\n# * ##\n#  ##\n####\n\n;MINICOSMOS 19\n\n####\n#  ####\n#     #\n#     #\n### ###\n# $$  ##\n# . .@ #\n####   #\n   #####\n\n;MINICOSMOS 20\n\n####\n#  ####\n#     #\n#     #\n### ###\n# $$$ ##\n# ...@ #\n####   #\n   #####\n\n;MINICOSMOS 21\n\n#####\n#   ###\n#     #\n##    #\n####$##\n#  $ ##\n# @   #\n###. .#\n  #####\n\n;MINICOSMOS 22\n\n#####\n#   ###\n# .   #\n## $  #\n####$##\n#  $ ##\n# @   #\n###. .#\n  #####\n\n;MINICOSMOS 23\n\n      ####\n#######  #\n#        #\n#  $ #.# #\n#  $## # ##\n###   @   #\n  ###  #  #\n    ##.  ##\n     #####\n\n;MINICOSMOS 24\n\n      ####\n#######  #\n#        #\n#  $ #.# #\n# $$## # ##\n### . @   #\n  ###  #  #\n    ##.  ##\n     #####\n\n;MINICOSMOS 25\n\n     ####\n   ###  #\n ### .. #\n # $$#  #\n## # #@##\n#       #\n#   #   #\n######  #\n     ####\n\n;MINICOSMOS 26\n\n     ####\n   ###  #\n ### .. #\n # $$#  #\n## # #@##\n#  *    #\n#   #   #\n######  #\n     ####\n\n;MINICOSMOS 27\n\n ####\n #  ####\n #     #\n # #.  #\n##*##$##\n#      #\n# # @  #\n#    ###\n######\n\n;MINICOSMOS 28\n\n ####\n #  ####\n #     #\n # #.  #\n##*##$##\n#    * #\n# # @  #\n#    ###\n######\n\n;MINICOSMOS 29\n\n########\n#   #  #\n#      #\n## #.  #\n#    ###\n# # . #\n# $$# #\n###  @#\n  #####\n\n;MINICOSMOS 30\n\n########\n#   #  #\n#      #\n## #. .#\n#    ###\n# # * #\n# $$# #\n###  @#\n  #####\n\n;MINICOSMOS 31\n\n#####\n#   ##\n# #  ##\n#. #$ #\n#  @  #\n#.##$##\n#    #\n######\n\n;MINICOSMOS 32\n\n####\n#  ###\n#    ##\n# .#$ #\n## @  #\n #.#$##\n #   #\n #####\n\n;MINICOSMOS 33\n\n #######\n #  #  ##\n## **$. #\n#   #   #\n#   @ ###\n#  ####\n####\n\n;MINICOSMOS 34\n\n #######\n##  #  #\n#  **$.##\n#   #   #\n### @   #\n  ####  #\n     ####\n\n;MINICOSMOS 35\n\n  ####\n###  ###\n#   *$ #\n# #  #@#\n# # *. #\n#   ####\n#####\n\n;MINICOSMOS 36\n\n#####\n#   ##\n# #  ###\n#   *$ #\n###  #@#\n  # *. #\n  #  ###\n  ####\n\n;MINICOSMOS 37\n\n  ####\n ##  #\n##   ##\n#  *$ #\n# # #@#\n#  *. #\n###  ##\n  #  #\n  ####\n\n;MINICOSMOS 38\n\n  ####\n  #  ###\n ## .  #\n##@$$$ #\n# . . ##\n#   ###\n#  ##\n####\n\n;MINICOSMOS 39\n\n  #####\n###   #\n#     #\n#  #.###\n##@$$$ #\n #.#.# #\n #     #\n #  ####\n ####\n\n;MINICOSMOS 40\n\n  ####\n ##  #\n## . ##\n#@$$$ #\n#. .# #\n# #   #\n#   ###\n#####\n";

char *level;

#ifndef EMULATOR
const char *configFileName = STATEPATH "/sokoban.cfg";
const char *levelsFolder = USERDATA "/share/PBSokoban/";
#else
const char *configFileName = "sokoban.cfg";
const char *levelsFolder = "SokobanLevels/"; 
#endif
	int swq=60;
	int sww=100;
	int swh=40;
	int dist=10;
	int fh=17;

int levelNo;
int levelMax;
int levels;
char levelSetFileName[1024];

void PrepareBoard();
void DrawBoard();
void Move(int dx, int dy);

void setLevelNo(int newLevelNo) {
	levelNo = newLevelNo;
	FILE *config = fopen(configFileName, "w");
//	fprintf(config, "%i\n%s", levelNo, levelSetFileName);
	fprintf(config, "%s\t%i\t%i\n", levelSetFileName, levelNo, levelMax);
	fclose(config);
	PrepareBoard();
	DrawBoard();
}

enum BoardElement {
	Box = '$',
	BoxOnTheSpot = '*',
	Empty = ' ',
	Player = '@',
	PlayerOnTheSpot = '+',
	Spot = '.',
	Wall = '#'
};

static imenu mainMenu[] = {
	{ ITEM_HEADER,   0, "Menü", NULL },
//	{ ITEM_ACTIVE, 101, "Zug zurück", NULL },
	{ ITEM_ACTIVE, 102, "Level neu starten", NULL },
//	{ ITEM_ACTIVE, 103, "Nächster Level", NULL },
//	{ ITEM_ACTIVE, 104, "Vorheriger Level", NULL },
	{ ITEM_ACTIVE, 105, "Level-Set öffnen...", NULL },
//	{ ITEM_ACTIVE, 106, "G-Sensor benutzen", NULL },
	{ ITEM_ACTIVE, 107, "Hilfe", NULL },
	{ ITEM_ACTIVE, 108, "Über", NULL },
	{ ITEM_ACTIVE, 109, "Beenden", NULL },
	{ 0, 0, NULL, NULL }
};

imenu levelMenu[100];
char* levelNames[100];

int tileSize;
int playerX, playerY;
int boxCount;
int boxesOnPlace;
int lastMoveLeft = 1;

///****This bunch of items creates something like incapsulated two-dimensional array - that's just because I've forget, and unable to find
//an example, how to manage dynamic two-dimensional array in C
int boardHeight;
int boardWidth;
int transpose;
char board[boardMaxSize * boardMaxSize/*max (currently) supported linear level size is size of screen*/];

int getCoordinateForXY(int x, int y) {
	return transpose ? y + x * boardHeight : x + y * boardWidth;
}

char getBoard(int x, int y) {
	if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) {
		return Wall;
		//if someone is asking about the world around level - there are only bricks. This will help in case of open level
	}
	return board[getCoordinateForXY(x, y)];
}

void setBoard(int x, int y, char value) {
	if (x >= 0 && x < boardWidth && y >= 0 && y < boardHeight) {
		//we're writing data only while inbounds of board - also temporary helps when board is too big
		board[getCoordinateForXY(x, y)] = value;
	}
}

void boardReset() {
	//Set board size to (currently) max available - TODO: think what to do with this
	transpose = 0;
	boardWidth = boardMaxSize;
	boardHeight = boardMaxSize;
	int i, j;
	for (i = 0; i < boardMaxSize; i++) {
		for (j = 0; j < boardMaxSize; j++) {
			setBoard(i, j, Empty);
		}
	}
}

void boardCompress(int newWidth, int newHeight) {
	//old width was boardMaxSize - but in fact items are present only in first newWidth items of each row, and in first newHeight rows. so...
	int i, j;
	for (j = 0; j < newHeight; j++) {
		for (i = 0; i < newWidth; i++) {
			//order of cycles here has meaning - we're moving row by row, emptying space for next ones, not column by column - as they are stored in
			board[i + j * newWidth] = board[i + j * boardMaxSize];
		}
	}
	
	int width50 = ScreenWidth() / 50;
	int height50 = (ScreenHeight() - swh*2 - dist*3 - 60) / 50;
	int width30 = ScreenWidth() / 30;
	int height30 = (ScreenHeight() - swh*2 - dist*3 - 60) / 30;

	if (newWidth > newHeight) {
		transpose = 1;
		int swap = newWidth;
		newWidth = newHeight;
		newHeight = swap;
		swap = playerX;
		playerX = playerY;
		playerY = swap;
	}
	else {
		transpose = 0;
	}
	
	
	if (newWidth <= width50 && newHeight <= height50) {
		boardWidth = newWidth;
		boardHeight = newHeight;
		tileSize = 50;
	}
	else {
		//prevention of too big level
		boardWidth = newWidth > width30 ? width30 : newWidth;
		boardHeight = newHeight > height30 ? height30 : newHeight;
		tileSize = 30;
	}
}
///****end of board

///****another pseudo-class - stack of moves, limited to 1000 moves, then starts to overwrite
int moves[3 * historyLimit];
int movesPointer = -1;
int undoTill = -1;
void resetMovesStack() {
	movesPointer = -1;
	undoTill = -1;
}

void pushMove(int dx, int dy, int pull) {
	movesPointer++;
	if (movesPointer >= historyLimit && movesPointer - historyLimit > undoTill) {
		undoTill = movesPointer - historyLimit;
	}
	moves[movesPointer % historyLimit * 3] = dx;
	moves[movesPointer % historyLimit * 3 + 1] = dy;
	moves[movesPointer % historyLimit * 3 + 2] = pull;
}

//if stack is not empty, returns 1
int popMove(int* dx, int* dy, int* pull) {
	if (movesPointer > undoTill) {
		*dx = moves[movesPointer % historyLimit * 3];
		*dy = moves[movesPointer % historyLimit * 3 + 1];
		*pull = moves[movesPointer % historyLimit * 3 + 2];
		movesPointer--;
		return 1;
	} else if (undoTill != -1) {
		Message(ICON_WARNING, "Hinweis",
		"Mehr Züge lassen sich nicht rückgängig machen!", 10000);
	}
	return 0;
}
///****end of movement stack

///****nextLine This part of code will try to reproduce functionality of strtok(char*, "\r\n")
///again have no idea how to create new char* - so using global variable for response
int levelIndex = 0;
char currentLine[100];
int getNextLine() {
	if (level[levelIndex] == 0) {
		return 0;
	}
	int lineBegin = levelIndex;
	//seek for end
	while (level[levelIndex] != '\r' && level[levelIndex] != '\n'
			&& level[levelIndex] != 0) {
		levelIndex++;
	}
	int lineEnd = levelIndex;
	//this to remove trailing blanks
	while (level[lineEnd - 1] == Empty) {
		lineEnd--;
	}
	//seek for first character of next line
	while (level[levelIndex] == '\r' || level[levelIndex] == '\n') {
		levelIndex++;
	}
	int i;
	for (i = 0; i < lineEnd - lineBegin; i++) {
		currentLine[i] = level[lineBegin + i];
	}
	currentLine[lineEnd - lineBegin] = 0;
	return 1;
}

///****end of nextLine

int baseX;
int baseY;

//moves player to the position (+dx, +dy) if possible
//returns 1 if succeeded, 0 else
int movePlayer(int dx, int dy) {
	if (getBoard(playerX + dx, playerY + dy) == Empty || getBoard(playerX + dx,
			playerY + dy) == Spot) {
		if (getBoard(playerX + dx, playerY + dy) == Empty) {
			setBoard(playerX + dx, playerY + dy, Player);
		} else if (getBoard(playerX + dx, playerY + dy) == Spot) {
			setBoard(playerX + dx, playerY + dy, PlayerOnTheSpot);
		}
		if (getBoard(playerX, playerY) == Player) {
			setBoard(playerX, playerY, Empty);
		} else if (getBoard(playerX, playerY) == PlayerOnTheSpot) {
			setBoard(playerX, playerY, Spot);
		}
		return 1;
	}
	return 0;
}

//actually, if there is box and it is possible, moves box from position (x, y) to (x + dx, y + dy)
int moveBox(int x, int y, int dx, int dy) {
	if ((getBoard(x, y) == Box || getBoard(x, y) == BoxOnTheSpot) && (getBoard(
			x + dx, y + dy) == Empty || getBoard(x + dx, y + dy) == Spot)) {
		if (getBoard(x + dx, y + dy) == Empty) {
			setBoard(x + dx, y + dy, Box);
		} else {// getBoard(x + dx, y + dy) == Spot
			setBoard(x + dx, y + dy, BoxOnTheSpot);
			boxesOnPlace++;
		}
		if (getBoard(x, y) == Box) {
			setBoard(x, y, Empty);
		} else {// getBoard(x, y) == BoxOnTheSpot
			setBoard(x, y, Spot);
			boxesOnPlace--;
		}
		return 1;
	}
	return 0;
}

void PrepareBoard() {
	boardReset();
	resetMovesStack();
	int currentLevel = -1;
	int previousLineHasData = 0;
	int currentLineIndex = 0;
	levelIndex = 0;
	int currentLevelWidth = 0;
	int currentLevelHeight = 0;
	int i;
	boxCount = 0;
	boxesOnPlace = 0;
	while (getNextLine()) {
		int hasData;
		i = 0;
		while (currentLine[i] == Empty) {
			i++;
		}
		hasData = currentLine[i] == Wall;
		if (hasData) {
			if (!previousLineHasData) {
				currentLevel++;
			}
			if (currentLevel == levelNo) {
				currentLevelHeight++;
				if (currentLevelWidth < strlen(currentLine)) {
					currentLevelWidth = strlen(currentLine);
				}
				for (i = 0; i < strlen(currentLine); i++) {
					setBoard(i, currentLineIndex, currentLine[i]);
					if (currentLine[i] == Player || currentLine[i]
							== PlayerOnTheSpot) {
						playerX = i;
						playerY = currentLineIndex;
					}
					if (currentLine[i] == Box) {
						boxCount++;
					}
					if (currentLine[i] == BoxOnTheSpot) {
						boxCount++;
						boxesOnPlace++;
					}
				}
				currentLineIndex++;
			}
			previousLineHasData = 1;
		} else {
			if (currentLevel == levelNo) {
				break;
			}
			previousLineHasData = 0;
		}
	}
	boardCompress(currentLevelWidth, currentLevelHeight);

	baseX = (ScreenWidth() - boardWidth * tileSize) / 2;
	baseY = (ScreenHeight() - 50 - boardHeight * tileSize) / 2;
}

void GetLevelCount() {
	int i;
	levels = -1;
	int previousLineHasData = 0;
	levelIndex = 0;
	while (getNextLine()) {
		i = 0;
		while(currentLine[i] == Empty) {
			i++;
		}
		if (currentLine[i] == Wall) {
			if (!previousLineHasData) {
				levels++;
			}
			previousLineHasData = 1;
		} else {
			previousLineHasData = 0;
		}
	}
}

void DrawCell(int i, int j) {
	//NOTE: failed usage of ibitmap *image = &name; and then DrawBitmap(,, image); - find out why?
	int x = baseX + i * tileSize;
	int y = baseY + j * tileSize;
	switch (tileSize){
	case 30:
		switch (getBoard(i, j)) {
		case Box:
			DrawBitmap(x, y, &box30);
			break;
		case BoxOnTheSpot:
			DrawBitmap(x, y, &boxOnTheSpot30);
			break;
		case Empty:
			DrawBitmap(x, y, &empty30);
			break;
		case Player:
			if (lastMoveLeft == 1) {
				DrawBitmap(x, y, &player30);
			}
			else {
				DrawBitmap(x, y, &playerRight30);
			}
			break;
		case PlayerOnTheSpot:
			if (lastMoveLeft == 1) {
				DrawBitmap(x, y, &playerOnTheSpot30);
			}
			else {
				DrawBitmap(x, y, &playerOnTheSpotRight30);
			}
			break;
		case Spot:
			DrawBitmap(x, y, &spot30);
			break;
		case Wall:
			DrawBitmap(x, y, &wall30);
			break;
		}
		break;
	case 50:
		switch (getBoard(i, j)) {
		case Box:
			DrawBitmap(x, y, &box50);
			break;
		case BoxOnTheSpot:
			DrawBitmap(x, y, &boxOnTheSpot50);
			break;
		case Empty:
			DrawBitmap(x, y, &empty50);
			break;
		case Player:
			if (lastMoveLeft == 1) {
				DrawBitmap(x, y, &player50);
			}
			else {
				DrawBitmap(x, y, &playerRight50);
			}
			break;
		case PlayerOnTheSpot:
			if (lastMoveLeft == 1) {
				DrawBitmap(x, y, &playerOnTheSpot50);
			}
			else {
				DrawBitmap(x, y, &playerOnTheSpotRight50);
			}
			break;
		case Spot:
			DrawBitmap(x, y, &spot50);
			break;
		case Wall:
			DrawBitmap(x, y, &wall50);
			break;
		}
		break;
	}
}

void UpdateRegion(int x, int y, int dx, int dy) {
	int minx = dx > 0 ? x : x + dx;
	int width = 1 + (dx < 0 ? -dx : dx);
	int miny = dy > 0 ? y : y + dy;
	int height = 1 + (dy < 0 ? -dy : dy);

	PartialUpdateBW(baseX + minx * tileSize, baseY + miny * tileSize, width * tileSize, height
			* tileSize);
}

void DrawBoard() {
	ClearScreen();
	int i, j;
	for (i = 0; i < boardWidth; i++) {
		for (j = 0; j < boardHeight; j++) {
			DrawCell(i, j);
		}
	}
	SetFont(fonttitle, WHITE);
	FillArea(0, 0, ScreenWidth(), 60, BLACK);
	char buf[32];
	sprintf(buf, "PBSokoban - Level %i von %i (%i)", levelNo + 1, levels + 1, levelMax +1);
	DrawString((ScreenWidth()-StringWidth(buf))/2, 5, buf);
//	DrawString(5, ScreenHeight() - 20, "Move: Up/Down/Left/Right. Undo: OK. Menu: Hold OK. Level navigation: +/-");
//	int swcolor=0xdfdfdf;

	SetFont(font, BLACK);

	FillArea(0, ScreenHeight() - swh*2 - dist*3, ScreenWidth(), swh*2+dist*3, 0x4f4f4f);
	FillArea(0, ScreenHeight() - swh*2 - dist*3, ScreenWidth(), 1, BLACK);

//	FillArea(swq/2, ScreenHeight() - swh - dist*3/2 - swq/2, swq, swq, swcolor);
//	DrawRect(swq/2, ScreenHeight() - swh - dist*3/2 - swq/2, swq, swq, BLACK);
//	DrawString(swq-8, ScreenHeight() - dist/2 - swh - dist - fh , "<");
	DrawBitmap(swq/2, ScreenHeight() - swh - dist*3/2 - swq/2, &kback);

//	FillArea(ScreenWidth()-swq*3/2, ScreenHeight() - swh - dist*3/2 - swq/2, swq, swq, swcolor);
//	DrawRect(ScreenWidth()-swq*3/2, ScreenHeight() - swh - dist*3/2 - swq/2, swq, swq, BLACK);
//	DrawString(ScreenWidth()-swq-10, ScreenHeight() - dist/2 - swh - dist - fh , "M");
	DrawBitmap(ScreenWidth()-swq*3/2, ScreenHeight() - swh - dist*3/2 - swq/2, &kmenu);

//	FillArea((ScreenWidth()-sww*3-dist*2)/2, ScreenHeight() - (swh+dist)/2 - swh - dist, sww, swh, swcolor);
//	DrawRect((ScreenWidth()-sww*3-dist*2)/2, ScreenHeight() - (swh+dist)/2 - swh - dist, sww, swh, BLACK);
//	DrawString((ScreenWidth()-sww*3-dist*2)/2+28, ScreenHeight() - dist/2 - swh - dist - fh , "links");
	DrawBitmap((ScreenWidth()-sww*3-dist*2)/2, ScreenHeight() - (swh+dist)/2 - swh - dist, &kleft);

//	FillArea((ScreenWidth()-sww)/2, ScreenHeight() - (swh+dist)*2 , sww, swh, swcolor);
//	DrawRect((ScreenWidth()-sww)/2, ScreenHeight() - (swh+dist)*2 , sww, swh, BLACK);
//	DrawString((ScreenWidth()-sww)/2+27, ScreenHeight() - dist*2 - swh*3/2 -fh, "hoch");
	DrawBitmap((ScreenWidth()-sww)/2, ScreenHeight() - (swh+dist)*2, &kup);

//	FillArea((ScreenWidth()-sww)/2, ScreenHeight() - (swh+dist) , sww, swh, swcolor);
//	DrawRect((ScreenWidth()-sww)/2, ScreenHeight() - (swh+dist) , sww, swh, BLACK);
//	DrawString((ScreenWidth()-sww)/2+22, ScreenHeight() - dist - swh/2 - fh, "runter");
	DrawBitmap((ScreenWidth()-sww)/2, ScreenHeight() - (swh+dist), &kdown);

//	FillArea((ScreenWidth()+sww)/2+dist, ScreenHeight() - (swh+dist)/2 - swh - dist, sww, swh, swcolor);
//	DrawRect((ScreenWidth()+sww)/2+dist, ScreenHeight() - (swh+dist)/2 - swh - dist, sww, swh, BLACK);
//	DrawString((ScreenWidth()+sww)/2+dist+20, ScreenHeight() - dist/2 - swh - dist - fh, "rechts");
	DrawBitmap((ScreenWidth()+sww)/2+dist, ScreenHeight() - (swh+dist)/2 - swh - dist, &kright);

	FullUpdate();
}

int Push(int par1,int par2){
//    int i=(par1-baseX)/tileSize;
//    int j=(par2-baseY)/tileSize;
//    if(i==playerX){
//	if(j<playerY)Move(0,-1);
//	if(j>playerY)Move(0,1);
//    }else if(j==playerY){
//	if(i<playerX)Move(-1,0);
//	if(i>playerX)Move(1,0);
//    }
    if( par1 > (ScreenWidth()-sww*3-dist*2)/2 && par1 < (ScreenWidth()-sww*3-dist*2)/2+sww && par2 > ScreenHeight() - (swh+dist)/2 - swh - dist && par2 < ScreenHeight() - (swh+dist)/2 - dist){
	Move(-1,0);
    }
    if( par1 > (ScreenWidth()+sww)/2+dist && par1 < (ScreenWidth()+sww)/2+dist+sww && par2 > ScreenHeight() - (swh+dist)/2 - swh - dist && par2 < ScreenHeight() - (swh+dist)/2 - dist ){
	Move(1,0);
    }
    if( par1 > (ScreenWidth()-sww)/2 && par1 < (ScreenWidth()+sww)/2 && par2 > ScreenHeight() - (swh+dist)*2 && par2 < ScreenHeight() - swh - dist*2 ){
	Move(0,-1);
    }
    if( par1 > (ScreenWidth()-sww)/2 && par1 < (ScreenWidth()+sww)/2 && par2 > ScreenHeight() - (swh+dist) && par2 < ScreenHeight() - dist ){
	Move(0,1);
    }
    if( par1 > swq/2 && par1 < swq*3/2 && par2 > ScreenHeight() - swh - dist*3/2 - swq/2 && par2 < ScreenHeight() - swh - dist*3/2 + swq/2 ){
	return 1;
    }
    if( par1 > ScreenWidth()-swq*3/2 && par1 < ScreenWidth()-swq/2 && par2 > ScreenHeight() - swh - dist*3/2 - swq/2 && par2 < ScreenHeight() - swh - dist*3/2 + swq/2 ){
	return 2;
    }
    return 0;
}

void Move(int dx, int dy) {
	//if there is box in front of player - it will be pushed one square further if possible.
	//Then, if there free space in front of player - it definitely will, if there was box and it was moved - he'll move himself
	//sum of this actions will inform us how much squares to update
	int refresh = 0;
	if (dx > 0) {
		lastMoveLeft = 1;
		refresh = 1;
	}
	else if (dx < 0) {
		lastMoveLeft = 0;
		refresh = 1;
	}
	int shift = moveBox(playerX + dx, playerY + dy, dx, dy) + movePlayer(dx, dy);
	if (shift != 0) {
		DrawCell(playerX, playerY);
		DrawCell(playerX + dx, playerY + dy);
		if (shift == 2) {
			DrawCell(playerX + 2 * dx, playerY + 2 * dy);
		}
		//FullUpdate();
		UpdateRegion(playerX, playerY, shift * dx, shift * dy);
		playerX = playerX + dx;
		playerY = playerY + dy;
		//last value allows to distinguish, was box pushed on this stage.
		pushMove(dx, dy, shift - 1);
	} 
	else if (refresh != 0) {
		DrawCell(playerX, playerY);
		UpdateRegion(playerX, playerY, 0, 0);
	}
}

void loadLevel() {
	FILE *levelFile;
	if (levelSetFileName != "MiniCosmos" && (levelFile = fopen(levelSetFileName, "rb"))){
		fseek(levelFile, 0, SEEK_END);
  		int fileSize = ftell (levelFile);
  		rewind(levelFile);
		free(level);
  		level = (char*) malloc(sizeof(char) * fileSize);
  		fread (level, 1, fileSize,levelFile);
  		fclose (levelFile);
  	}
	else {
		level = strdup(originalLevel);
	}
}

void loadLevelsHandler(int index) {
	if (levelNames[index] != NULL) {
		strcpy(levelSetFileName, levelsFolder);
		strcat(levelSetFileName, levelNames[index]);
	}
	else {
		strcpy(levelSetFileName, "MiniCosmos");
	}
	loadLevel();
	GetLevelCount();
	setLevelNo(0);
}

void PickLevelSet() {
	DIR* dir = opendir(levelsFolder);
	if (dir == NULL) {
		Message(ICON_WARNING, "Keine Level-Sets vorhanden!", "Momentan hast Du keine zusätzlichen Level-Sets auf Deinem PocketBook.\n\nUm weitere Level-Sets hinzuzufügen, erstelle einen Ordner \"/system/share/PBSokoban\" auf Deinem PocketBook und speicher darin die Sets im xsb-Format.", 20000);
		return;
	}
	struct dirent *file;
	int i = 2;
	levelMenu[0].type = ITEM_HEADER;
	levelMenu[0].index = 0;
	levelMenu[0].text = "Level-Set auswählen";
	levelMenu[1].type = ITEM_ACTIVE;
	levelMenu[1].index = 1;
	levelMenu[1].text = "Mini Cosmos";

	while ((file = readdir(dir)) && i < 99) {
		if (file->d_name[0] != '.') {
			levelMenu[i].type = ITEM_ACTIVE;
 			levelMenu[i].index = i;
			free(levelMenu[i].text);
			free(levelNames[i]);
			levelMenu[i].text = strdup(file->d_name);
			levelNames[i] = strdup(file->d_name);
			i++;
		}
	}
	closedir(dir);
	levelMenu[i].type = 0;
	levelMenu[i].index = 0;
	levelMenu[i].text = NULL;
	OpenMenu(levelMenu, 0, 10, 2, loadLevelsHandler);
	levelMax = 0;
}

void Undo() {
	int dx, dy, pull;
//	while (popMove(&dx, &dy, &pull)) {
	if (popMove(&dx, &dy, &pull)) {
		//This is reversed movement of player
		movePlayer(-dx, -dy);
		if (pull) {
			//and if there was push on previous movement - we're reverting it
			moveBox(playerX + dx, playerY + dy, -dx, -dy);
		}
		playerX = playerX - dx;
		playerY = playerY - dy;

		DrawCell(playerX, playerY);
		DrawCell(playerX + dx, playerY + dy);
		if (pull == 1) {
			DrawCell(playerX + 2 * dx, playerY + 2 * dy);
		}
		UpdateRegion(playerX, playerY, (pull + 1) * dx, (pull + 1) * dy);
		if (pull == 1) {
			return;
		}
	}
}

int checkWin(){
	if (boxesOnPlace == boxCount) {
		if (levelNo < levels) {
			if (levelMax <= levelNo) {
			    levelMax = levelNo+1;
			    Message(ICON_INFORMATION, "Herzlichen Glückwunsch!",
					"Du hast diesen Level geschafft!\nMach Dich fit und auf zum nächsten Level...", 5000);
			}
			else {
			    Message(ICON_INFORMATION, "Herzlichen Glückwunsch!",
					"Du hast diesen Level wiederholt geschafft!\nÜbung macht den Meister...", 5000);
			}
			setLevelNo(levelNo + 1);
		}
		else {
			Message(ICON_INFORMATION, "Herzlichen Glückwunsch!",
					"Du hast alle Level geschafft!\nDamit hast Du die Ausbildung zum Lageristen geschafft :-)", 10000);
			PickLevelSet();
		}
		return 1;
	}
	return 0;
}

int selectedIndex = 0;

static int use_timer=0;
void sensor_thread(){
  int x,y,z;
  //static int x_old=-1000,y_old,z_old;
//  char str[64];
  if(ReadGSensor(&x,&y,&z)){
    if(!use_timer)return;
    if(abs(x)>abs(y)){
	if( x<-25 ) Move(1,0);
	else if( x>25 ) Move(-1,0);
    }else{
	if( y<-25 ) Move(0,1);
	else if( y>25 ) Move(0,-1);
    }
    DrawCell(playerX, playerY);
    UpdateRegion(playerX, playerY, 1, 1);
    checkWin();
  }
  if(use_timer)SetWeakTimer("stmr",sensor_thread,150);
}

void mainMenuHandler(int index) {
	selectedIndex = index;
	switch (index) {
	case 101:
		Undo();
		break;
	case 102:
		setLevelNo(levelNo);
		break;
	case 103:
		if (levelNo < levels) {
		    if (levelNo < levelMax) {
			setLevelNo(levelNo + 1);
		    }
		}
		break;
	case 104:
		if (levelNo > 0) {
			setLevelNo(levelNo - 1);
		}
		break;
	case 105:
		PickLevelSet();
		break;
	case 106:
	  if(ITEM_BULLET==mainMenu[6].type){
	    use_timer=0;
	    mainMenu[6].type=ITEM_ACTIVE;
	    ClearTimer(sensor_thread);
	  } else{
	    use_timer=1;
	    mainMenu[6].type=ITEM_BULLET;
	    SetWeakTimer("stmr",sensor_thread,250);
	  }
	  break;
	case 107:
		Message(0, "Hilfe", "Sokoban ist ein Computerspiel, welches Anfang der 80er Jahre von dem Japaner Hiroyuki Imabayashi entwickelt wurde.\n\n"
"Du musst Dich als Lagerist betätigen und alle Kisten an den dafür vorgesehenen Plätzen bewegen. Die Kisten können nur geschoben werden.\n\n"
"Die Steuerung erfolgt über das unten in der Mitte befindliche Steuerkreuz. Du hast die Möglichkeit den letzten Zug mit dem Zurück-Button rückgängig zu machen. Das Menü erreichst Du entweder über die Menü-Taste oder über den Menü-Button.\n"
"Erst, wenn Du einen Level geschafft hast, wird der nächste freigeschaltet. Mit den Pfeil-Tasten des PocketBook kannst Du alle freigeschalteten Level auswählen. Wieviel Level bereits freigeschaltet sind, steht in der Titelzeile in Klammern.",60000);
		break;
	case 108:
		Message(0, "Über PBSokoban V1.2", 
		"PBSokoban für PocketBook Touch 622\n\n"
		"Angepasst und übersetzt von Ronny Steiner, 2013\n"
		"Angepasst von Yury P. Fedorchenko, 2011\n"
		"Entwickelt von Andriy Kvasnytsya, 2009\n\n"
		"Das integrierte Level-Set \"Minicosmos\" stammt von Aymeric du Peloux\n\n"
		"Dieses Spiel ist freie Software nach GNU GPL v2, nachzulesen unter:\n\n http://www.gnu.de/documents/gpl-2.0.de.html\n", 15000);
		break;
	case 109:
		CloseApp();
		break;
	}
}

void ShowMenu() {
  use_timer=0;
  OpenMenu(mainMenu, selectedIndex, 10, 70, mainMenuHandler);
  if(ITEM_BULLET==mainMenu[6].type){
    use_timer=1;
    SetWeakTimer("stmr",sensor_thread,250);
  }
}

void KeyPressed(int key) {
	switch (key) {
	/*case KEY_BACK: //removed for use with other models
		CloseApp();
		return;*/
	case KEY_PLUS:
	case KEY_NEXT:
		if (levelNo < levels) {
		    if (levelNo < levelMax) {
			setLevelNo(levelNo + 1);
		    }
		}
		return;
	case KEY_MINUS:
	case KEY_PREV:
		if (levelNo > 0) {
			setLevelNo(levelNo - 1);
		}
		return;
	case KEY_UP:
		Move(0, -1);
		break;
	case KEY_DOWN:
		Move(0, 1);
		break;
	case KEY_LEFT:
		Move(-1, 0);
		break;
	case KEY_RIGHT:
		Move(1, 0);
		break;
	case KEY_OK:
	//case KEY_PREV:
		Undo();
		break;
	case KEY_MENU:
	case KEY_BACK:
		ShowMenu();
		break;
	}
	checkWin();
}


int main_handler(int type, int par1, int par2) {
	int pushret;
	if (type == EVT_INIT) {
		font = OpenFont("LiberationSans", 22, 0);
		fonttitle = OpenFont("LiberationSans", 30, 0);
		loadLevel();
		GetLevelCount();
		PrepareBoard();
//		if(QueryGSensor()){
//		   use_timer=1;
//		   mainMenu[6].type = ITEM_BULLET;
//		   SetWeakTimer("stmr",sensor_thread,250);
//		}else{
//		   use_timer=0;
//		   mainMenu[6].type = ITEM_INACTIVE;
//		}
	} else if (type == EVT_SHOW) {
		DrawBoard();
	} else if (type == EVT_KEYPRESS && par1 != KEY_OK) {
		KeyPressed(par1);
	} else if (type == EVT_KEYREPEAT && par1 == KEY_OK) {
		KeyPressed(KEY_MENU);
	} else if (type == EVT_KEYRELEASE && par1 == KEY_OK && par2 == 0) {
		KeyPressed(KEY_OK);
	} else if (EVT_POINTERDOWN ==type ){
		pushret=Push(par1,par2);
		if (pushret == 1) {
		    Undo();
		} else if (pushret == 2) {
		    ShowMenu();
		} else {
		    checkWin();
		}
	} else if (EVT_POINTERLONG ==type ){
//	    ShowMenu();
	}
	return 0;
}

int main(int argc, char **argv) {
	FILE *config = fopen(configFileName, "r");
	if (config != NULL) {
		fscanf(config, "%s\t%i\t%i\n", levelSetFileName, &levelNo, &levelMax);
		fclose(config);
	} else {
		levelNo = 0;
		strcpy(levelSetFileName ,"MiniCosmos");
	}
	InkViewMain(main_handler);
	return 0;
}
