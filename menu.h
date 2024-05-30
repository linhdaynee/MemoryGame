#ifndef MENU_H_
#define MENU_H_

#include "GameBase.h"
#include "Text.h"
#include "card.h"

SDL_Texture* g_img_Menu = nullptr;
SDL_Texture* g_img_Information = nullptr;
SDL_Texture* g_img_Win = nullptr;

int ShowMenu(SDL_Renderer* renderer, TTF_Font* font)
{
    g_img_Menu = loadCardTexture("memory_game/menu.jpg", renderer);
    if (g_img_Menu == nullptr) return 2;

    const int kMenuItemNum = 3;
    SDL_Rect pos_arr[kMenuItemNum];
    pos_arr[0].x = 50;
    pos_arr[0].y = 320;

    pos_arr[1].x = 50;
    pos_arr[1].y = 370;

    pos_arr[2].x = 50;
    pos_arr[2].y = 420;

    TextObject text_Menu[kMenuItemNum];
    text_Menu[0].SetText("PLAY");
    text_Menu[0].setColor(TextObject::BLACK_TEXT);

    text_Menu[1].SetText("HELP");
    text_Menu[1].setColor(TextObject::BLACK_TEXT);

    text_Menu[2].SetText("EXIT");
    text_Menu[2].setColor(TextObject::BLACK_TEXT);

    int xm = 0, ym = 0;
    bool selected[kMenuItemNum] = { 0, 0, 0 };
    SDL_Event m;
    while (true)
    {

        while (SDL_PollEvent(&m))
        {
            SDL_RenderCopy(renderer, g_img_Menu, NULL, NULL);
            for (int i = 0; i < kMenuItemNum; i++)
            {
                text_Menu[i].loadFromRenderedText(font, renderer);
                text_Menu[i].RenderText(renderer, pos_arr[i].x, pos_arr[i].y);
                pos_arr[i].w = text_Menu[i].getWidth();
                pos_arr[i].h = text_Menu[i].getHeight();
            }

            switch (m.type)
            {
            case SDL_QUIT:
                return 2;
            case SDL_MOUSEMOTION:
            {
                xm = m.motion.x;
                ym = m.motion.y;
                for (int i = 0; i < kMenuItemNum; i++)
                {
                    if (CheckFocusWithRect(xm, ym, pos_arr[i]))
                    {

                        if (selected[i] == false)
                        {
                            selected[i] = 1;
                            text_Menu[i].setColor(TextObject::RED_TEXT);
                        }
                    }
                    else
                    {
                        if (selected[i] == true)
                        {
                            selected[i] = 0;
                            text_Menu[i].setColor(TextObject::BLACK_TEXT);
                        }
                    }

                }
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                xm = m.button.x;
                ym = m.button.y;
                for (int i = 0; i < kMenuItemNum; i++)
                {
                    if (CheckFocusWithRect(xm, ym, pos_arr[i]))
                    {
                        return i;
                    }
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                if (m.key.keysym.sym == SDLK_ESCAPE)
                {
                    return 2;
                }
            }
            default:
                break;
            }
        }
        SDL_FlushEvent(SDL_MOUSEBUTTONDOWN); // Xóa sự kiện nhấn chuột trước

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < kMenuItemNum; i++)
    {
        text_Menu[i].Free();
    }
    SDL_DestroyTexture(g_img_Menu);
    return 2;
}

int ShowInformation( SDL_Renderer* renderer, TTF_Font* font)
{
    g_img_Information = loadCardTexture("memory_game/information.jpg", renderer);
    SDL_Rect pos_arr;
    pos_arr.x = 30;
    pos_arr.y = SCREEN_HEIGHT - 70;

    TextObject text_back;
    text_back.SetText("BACK");
    text_back.setColor(TextObject::BLACK_TEXT);
    int xm = 0, ym = 0;
    bool selected = 0;
    SDL_Event m;
    while (true)
    {

        while (SDL_PollEvent(&m))
        {
            SDL_RenderCopy(renderer, g_img_Information, NULL, NULL);
            text_back.loadFromRenderedText(font, renderer);
            text_back.RenderText(renderer, pos_arr.x, pos_arr.y);
            pos_arr.w = text_back.getWidth();
            pos_arr.h = text_back.getHeight();

            switch (m.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_MOUSEMOTION:
            {
                xm = m.motion.x;
                ym = m.motion.y;
                if (CheckFocusWithRect(xm, ym, pos_arr))
                {

                    if (selected == false)
                    {
                        selected = 1;
                        text_back.setColor(TextObject::RED_TEXT);
                    }
                }
                else
                {
                    if (selected == true)
                    {
                        selected = 0;
                        text_back.setColor(TextObject::BLACK_TEXT);
                    }
                }
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                xm = m.button.x;
                ym = m.button.y;
                if (CheckFocusWithRect(xm, ym, pos_arr))
                {
                    return 1;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                if (m.key.keysym.sym == SDLK_ESCAPE)
                {
                    return 0;
                }
            }
            default:
                break;
            }
        }
        SDL_FlushEvent(SDL_MOUSEBUTTONDOWN); // Xóa sự kiện nhấn chuột trước

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(g_img_Information);
    text_back.Free();

}

bool replay = false; // Biến cờ cho biết liệu trò chơi đang trong quá trình replay hay không

void ResetGame(vector<Card>& cards) {
    // Đặt lại trạng thái của trò chơi ở đây, bao gồm việc reset vị trí các thẻ bài và các biến khác.
    for (Card& card : cards) {
        card.isFlipped = false;
        // Cập nhật các trạng thái khác của thẻ bài nếu cần
    }
}

void ReplayGame(vector<Card>& cards) {
    ResetGame(cards); // Đặt lại trò chơi trước khi replay
    replay = true; // Đánh dấu là đang trong quá trình replay
}


int ShowWin(vector<Card> cards, SDL_Renderer* renderer, TTF_Font* font)
{
    g_img_Win = loadCardTexture("memory_game/win.png", renderer);
    SDL_Rect pos_arr;
    pos_arr.x = 30;
    pos_arr.y = SCREEN_HEIGHT - 70;

    TextObject text_back;
    text_back.SetText("REPLAY");
    text_back.setColor(TextObject::BLACK_TEXT);
    int xm = 0, ym = 0;
    SDL_Event m;
    while (true)
    {

        while (SDL_PollEvent(&m))
        {
            SDL_RenderCopy(renderer, g_img_Win, NULL, NULL);
            text_back.loadFromRenderedText(font, renderer);
            text_back.RenderText(renderer, pos_arr.x, pos_arr.y);
            pos_arr.w = text_back.getWidth();
            pos_arr.h = text_back.getHeight();

            switch (m.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_MOUSEMOTION:
            {
                xm = m.motion.x;
                ym = m.motion.y;
                if (CheckFocusWithRect(xm, ym, pos_arr))
                {
                    text_back.setColor(TextObject::RED_TEXT);
                }
                else
                {
                    text_back.setColor(TextObject::BLACK_TEXT);
                }
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                xm = m.button.x;
                ym = m.button.y;
                if (CheckFocusWithRect(xm, ym, pos_arr))
                {
                    ReplayGame(cards);
                    return 1;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                if (m.key.keysym.sym == SDLK_ESCAPE)
                {
                    return 0;
                }
            }
            default:
                break;
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(g_img_Information);
    text_back.Free();

}



#endif
