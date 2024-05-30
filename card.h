#ifndef CARD_H_
#define CARD_H_

#include "GameBase.h"


// Mảng lưu trữ thông tin thẻ bài
struct Card {
    private:
        SDL_Texture* texture;

    public:
        int x,y;
        int type; // Biến lưu trữ loại thẻ
        int pairID; // ID cặp

        bool isFlipped = false; // Biến trạng thái lật (khởi tạo mặc định là false)

        Card(int type, bool isFlipped, int pairID): // Định nghĩa hàm khởi tạo
            type(type), isFlipped(isFlipped), pairID(pairID) {}  // Khởi tạo các thành viên
};


#endif
