
#ifndef GAME_BASE_H_
#define GAME_BASE_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include "card.h"

#define SDL_ERROR 1
#define IMG_ERROR 2
#define MIX_ERROR 3
#define TTF_ERROR 4


using namespace std;



// Kích thước màn hình
const int SCREEN_WIDTH = 740;
const int SCREEN_HEIGHT = 500;
const string WINDOW_TITLE = "MEMORY GAME: ZODIAC SIGN";

// Kích thước của card
const int cardWidth = 100;
const int cardHeight = 100;

// Số lượng thẻ bài
const int NUM_CARDS = 24;


void logSDLError(std::ostream& os, const std::string& msg, bool fatal);
void LogError( string msg, int error_code);

void initSDL(SDL_Window*& window, SDL_Renderer*& renderer);

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void waitUntilKeyPressed();

SDL_Texture *loadCardTexture(const char *imagePath, SDL_Renderer* renderer);

bool CheckFocusWithRect(const int& x, const int& y, SDL_Rect& rect);

#endif

