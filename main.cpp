#include "card.h"
#include "Text.h"
#include "GameBase.h"
#include "menu.h"


using namespace std;

Mix_Music* gMusic = nullptr;
Mix_Music* gMenuMusic = nullptr;
Mix_Music* gWin = nullptr;
Mix_Chunk* gFlip = nullptr;
Mix_Chunk* gLose = nullptr;
Mix_Chunk* gTing = nullptr;
TTF_Font* gFontText = nullptr;

// Khởi tạo SDL
SDL_Window* window = NULL;

bool Quit_Menu = false;
bool Play_Game = true;

void CloseProgramme()
{
    Mix_FreeMusic(gMusic);
    Mix_FreeMusic(gMenuMusic);
    Mix_FreeChunk(gLose);
    Mix_FreeChunk(gFlip);
    Mix_FreeChunk(gTing);
    gMusic = nullptr;
    gMenuMusic = nullptr;
    gLose = nullptr;
    gFlip = nullptr;
    gTing = nullptr;

    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}


vector<Card> cards;

bool LoadMediaAndTtf()
{
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) return  false;
    bool success = true;
    gMusic = Mix_LoadMUS("memory_game/play.mp3");
    gMenuMusic = Mix_LoadMUS("memory_game/menu.mp3");
    gWin = Mix_LoadMUS("memory_game/win.mp3");
    gFlip = Mix_LoadWAV("memory_game/flip.wav");
    gLose = Mix_LoadWAV("memory_game/lose.wav");
    gTing = Mix_LoadWAV("memory_game/ting.wav");

    if (TTF_Init() == -1) return false;
    gFontText = TTF_OpenFont("memory_game/font.ttf", 24);

    return success;
}

/*void ShuffleDeck(std::vector<Card>& cards) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}*/

void ShuffleDeck(std::vector<Card>& cards, std::default_random_engine& rng) {
    int n = cards.size();
    for (int i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(rng);
        std::swap(cards[i], cards[j]);
    }

    // Cập nhật lại vị trí của các thẻ bài
    for (int i = 0; i < n; ++i) {
        int row = i / 6; // Tính toán hàng của thẻ bài
        int col = i % 6; // Tính toán cột của thẻ bài

        // Tính toán vị trí x và y của thẻ bài
        int x = col * (100 + 20) + 20;
        int y = row * (100 + 20) + 20;

        // Cập nhật vị trí của thẻ bài trong cards[i]
        cards[i].x = x;
        cards[i].y = y;
    }
}


// Hàm kiểm tra điểm click chuột có nằm trong card không
bool isWithinCard(int mouseX, int mouseY, int textureX, int textureY) {
  if (mouseX >= textureX && mouseX <= textureX + 100 &&
      mouseY >= textureY && mouseY <= textureY + 100) {
    return true;
  }
  return false;
}

int compare[2] = {-1,-1};


int clickedCardIndex1;
int clickedCardIndex2;

int currentLevel = 1;

// Biến để theo dõi trạng thái của các thẻ bài
bool allCardsFlipped(const vector<Card>& cards) {
    for (const Card& card : cards) {
        if (!card.isFlipped) {
            return false;
        }
    }
    return true;
}

// Hàm kiểm tra xem một vị trí đã được sử dụng hay chưa
bool isPositionUsed(const std::vector<std::pair<int, int>>& usedPositions, int x, int y) {
    return std::find(usedPositions.begin(), usedPositions.end(), std::make_pair(x, y)) != usedPositions.end();
}

// Hàm cập nhật vị trí của các thẻ bài
void UpdateCardPositions(std::vector<Card>& cards, std::vector<std::pair<int, int>>& usedPositions, std::default_random_engine& rng) {
    usedPositions.clear(); // Xóa các vị trí đã sử dụng trước đó
    int numRows = 4; // Số hàng
    int numCols = 6; // Số cột
    int cardWidth = 100; // Chiều rộng của mỗi thẻ bài
    int cardHeight = 100; // Chiều cao của mỗi thẻ bài
    int spacing = 20; // Khoảng cách giữa các thẻ bài

    // Tạo danh sách tất cả các vị trí có thể của các thẻ bài
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            int x = col * (cardWidth + spacing) + spacing;
            int y = row * (cardHeight + spacing) + spacing;
            usedPositions.push_back(std::make_pair(x, y));
        }
    }
    // Random lại vị trí cho mỗi thẻ bài
    for (int i = 0; i < cards.size(); ++i) {
        std::uniform_int_distribution<int> dist(0, usedPositions.size() - 1);
        int index = dist(rng);
        cards[i].x = usedPositions[index].first;
        cards[i].y = usedPositions[index].second;
        usedPositions.erase(usedPositions.begin() + index);
    }
}

void LoadCard(vector<Card> cards, SDL_Renderer* renderer) {

       // Vòng lặp để load ảnh cho mỗi thẻ bài
       for (int i = 0; i < 24; ++i) {
            // Truy cập thẻ theo thứ tự đã xáo trộn
            string cardImagePath = "memory_game/card_" + to_string(cards[i].type) + ".jpg"; // Đặt tên ảnh theo loại thẻ bài
            const char* imagePath = cardImagePath.c_str();

            int row = i / 6; // Tính toán hàng của thẻ bài
            int col = i % 6; // Tính toán cột của thẻ bài

            // Tính toán vị trí x và y của thẻ bài
            int x = col * (100 + 20) + 20;
            int y = row * (100 + 20) + 20;

            // Cập nhật SDL_Rect
            SDL_Rect rect;
            rect.x = x;
            rect.y = y;
            rect.w = 100;
            rect.h = 100;

            if (!cards[i].isFlipped) {
            SDL_Texture* backCardTexture = loadCardTexture("memory_game/back_of_card.png", renderer);
            SDL_RenderCopy(renderer, backCardTexture, NULL, &rect);
            }

            else {
            SDL_Texture* cardTexture = loadCardTexture(imagePath, renderer);
            SDL_RenderCopy(renderer, cardTexture, NULL, &rect);
            }
            SDL_RenderPresent(renderer);

       }
}


//Lật và so sánh
void HandleCardClick (int mouseX, int mouseY, vector<Card> &cards, SDL_Renderer* renderer) {
    bool process1 = true;
    bool process2 = true;
    if (compare[0] == -1) {
      for (int i = 0; i < cards.size(); ++i) {
         // Tính toán vị trí x và y của thẻ bài
         int cardX = (i % 6) * (100 + 20) + 20;
         int cardY = (i / 6) * (100 + 20) + 20;
         // Kiểm tra xem người dùng có nhấp chuột vào thẻ bài hay không
         if (isWithinCard(mouseX, mouseY, cardX, cardY)) {
            Mix_PlayChannel(-1, gFlip, 0);
            // Nếu có, trả về giá trị của thẻ bài
            if (!cards[i].isFlipped) { // Kiểm tra xem thẻ bài đã lật hay chưa
                 cards[i].isFlipped = true;
                 clickedCardIndex1 = i;
                 string cardImagePath = "memory_game/card_" + to_string(cards[i].type) + ".jpg"; // Đặt tên ảnh theo loại thẻ bài
                 const char* imagePath = cardImagePath.c_str();

                 // Cập nhật SDL_Rect
                 SDL_Rect rect;
                 rect.x = cardX;
                 rect.y = cardY;
                 rect.w = 100;
                 rect.h = 100;

                 SDL_Texture* cardTexture = loadCardTexture(imagePath, renderer);
                 SDL_RenderCopy(renderer, cardTexture, NULL, &rect);
                 SDL_RenderPresent(renderer);

                 compare[0] = cards[clickedCardIndex1].type;
                 process1 = false;
             }
           }
      }
    }
    else if (compare[0] != -1 && compare[1] == -1 && process1) {
        for (int i = 0; i < cards.size(); ++i) {
    // Tính toán vị trí x và y của thẻ bài
    int cardX = (i % 6) * (100 + 20) + 20;
    int cardY = (i / 6) * (100 + 20) + 20;

    // Kiểm tra xem người dùng có nhấp chuột vào thẻ bài hay không
    if (isWithinCard(mouseX, mouseY, cardX, cardY)) {
      // Nếu có, trả về giá trị của thẻ bài
       if (!cards[i].isFlipped) {// Kiểm tra xem thẻ bài đã lật hay chưa
          cards[i].isFlipped = true;
          clickedCardIndex2 = i;
            string cardImagePath = "memory_game/card_" + to_string(cards[i].type) + ".jpg"; // Đặt tên ảnh theo loại thẻ bài
            const char* imagePath = cardImagePath.c_str();

            // Cập nhật SDL_Rect
            SDL_Rect rect;
            rect.x = cardX;
            rect.y = cardY;
            rect.w = 100;
            rect.h = 100;

            SDL_Texture* cardTexture = loadCardTexture(imagePath, renderer);
            SDL_RenderCopy(renderer, cardTexture, NULL, &rect);
            SDL_RenderPresent(renderer);


                // Lưu giá trị thẻ bài vào mảng compare
                compare[1] = cards[clickedCardIndex2].type;

          // So sánh 2 thẻ bài trong mảng compare
          if (compare[0] != compare[1]) {
                Mix_PlayChannel(-1, gLose, 0);
              // Lật úp 2 thẻ bài
              cards[clickedCardIndex1].isFlipped = false;
              int row1 = clickedCardIndex1 / 6; // Tính toán hàng của thẻ bài
              int col1 = clickedCardIndex1 % 6; // Tính toán cột của thẻ bài

              // Tính toán vị trí x và y của thẻ bài
              int x1 = col1 * (100 + 20) + 20;
              int y1 = row1 * (100 + 20) + 20;

              // Cập nhật SDL_Rect
              SDL_Rect rect;
              rect.x = x1;
              rect.y = y1;
              rect.w = 100;
              rect.h = 100;

              // Delay for 1 seconds
              SDL_Delay(1000); // 1000 milliseconds = 1 seconds

            SDL_Texture* backCardTexture = loadCardTexture("memory_game/back_of_card.png", renderer);
            SDL_RenderCopy(renderer, backCardTexture, NULL, &rect);
            SDL_RenderPresent(renderer);

              cards[clickedCardIndex2].isFlipped = false;
              int row2 = clickedCardIndex2 / 6; // Tính toán hàng của thẻ bài
              int col2 = clickedCardIndex2 % 6; // Tính toán cột của thẻ bài

              // Tính toán vị trí x và y của thẻ bài
              int x2 = col2 * (100 + 20) + 20;
              int y2 = row2 * (100 + 20) + 20;

              // Cập nhật SDL_Rect
              rect.x = x2;
              rect.y = y2;
              rect.w = 100;
              rect.h = 100;

            SDL_RenderCopy(renderer, backCardTexture, NULL, &rect);
            SDL_RenderPresent(renderer);

             // Reset mảng Compare
             compare[0] = -1;
             compare[1] = -1;


             process2 = false;
                        }
         else if (compare[0] == compare[1] && process2) {

             // Xử lý khi 2 thẻ bài giống nhau (tùy theo thiết kế game)
             Mix_PlayChannel(-1, gTing, 0);
             if (allCardsFlipped(cards)) {
                    SDL_Delay(1000);
                Mix_PlayMusic(gWin, -1);
                /*SDL_Texture* win = loadCardTexture("memory_game/win.png", renderer);
                SDL_RenderCopy(renderer, win, NULL, NULL);
                SDL_RenderPresent(renderer);*/
                int checkExit = ShowWin(cards, renderer, gFontText);
                    if (checkExit == 2)
                        {
                            CloseProgramme();
                        }
                        else
                        {
                                                        ReplayGame(cards);
                                                        SDL_Texture* background = loadCardTexture("memory_game/background.jpg", renderer);
                             SDL_RenderCopy(renderer, background, NULL, NULL);
                             SDL_RenderPresent(renderer);

                            // Reset trạng thái game cho một ván mới
            std::default_random_engine rng(std::random_device{}());
            ShuffleDeck(cards, rng);
            LoadCard(cards, renderer);
            // Đặt các biến cờ
            Play_Game = false;
            replay = true;


                        }


             }
             else {
                    // Reset mảng Compare
            compare[0] = -1;
            compare[1] = -1;
             }


            }

        }
    }
    }

}
        }



int main (int argc, char* argv[]) {

        SDL_Renderer* renderer;


          vector<Card> cards;
       for (int i = 0; i < 24; ++i) {
            Card newCard = {i % 12, false, i / 12};
            newCard.isFlipped = false;
            cards.push_back(newCard); // 12 cặp thẻ bài với 2 hình ảnh mỗi cặp
       }

       if (LoadMediaAndTtf()) {
            SDL_Event event;
    SDL_WaitEvent(&event);

    SDL_Window* window;
        initSDL(window, renderer);

    SDL_Texture* background = loadCardTexture("memory_game/background.jpg", renderer);
    int checkExit = -1;

        while(!Quit_Menu) {
            Mix_PlayMusic(gMenuMusic, -1);

            checkExit = ShowMenu(renderer, gFontText);
            while (checkExit != 0 && Play_Game == true)
            {
                if (checkExit == 2)
                {
                    CloseProgramme();
                    return 0;
                }
                else
                {
                    if (checkExit == 1)
                    {

                        checkExit = ShowInformation(renderer, gFontText);
                        if (checkExit == 2)
                        {
                            CloseProgramme();
                        }
                        else
                        {
                            checkExit = ShowMenu(renderer, gFontText);

                        }
                    }
                }
            }
            Quit_Menu = true;
        }

        if (Mix_PausedMusic()) Mix_ResumeMusic();
            else Mix_PlayMusic(gMusic, -1);
            SDL_RenderCopy(renderer, background, NULL, NULL);
            // Khởi tạo random engine
    std::default_random_engine rng(std::random_device{}());

    // Random lại thứ tự của thẻ bài
    ShuffleDeck(cards, rng);

    // Cập nhật vị trí của các thẻ bài
    std::vector<std::pair<int, int>> usedPositions;
    UpdateCardPositions(cards, usedPositions, rng);
        LoadCard(cards, renderer);



       while (Play_Game) {

       // Vòng lặp chính
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                Play_Game = false;
            }

            else if (event.type == SDL_MOUSEBUTTONDOWN) {

                int mouseX = event.button.x;
                int mouseY = event.button.y;
                HandleCardClick(mouseX, mouseY, cards, renderer);

            }
        }

        }
        while (replay) {
            while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                Play_Game = false;
            }

            else if (event.type == SDL_MOUSEBUTTONDOWN) {

                int mouseX = event.button.x;
                int mouseY = event.button.y;
                HandleCardClick(mouseX, mouseY, cards, renderer);
            }
        }
        }


        }



    SDL_RenderPresent(renderer);
    waitUntilKeyPressed();
    CloseProgramme();
    return 0;
}
