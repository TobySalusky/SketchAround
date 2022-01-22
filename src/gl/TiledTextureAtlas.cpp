//
// Created by Tobiathan on 1/21/22.
//

#include "TiledTextureAtlas.h"
#include <algorithm>

TiledTextureAtlas::TiledTextureAtlas() : Texture(1, 1, {0, 0, 0}) {}

void TiledTextureAtlas::Generate(int tileSize, const std::vector<std::vector<unsigned char> *> &textureArr) {

    count = (int) textureArr.size();
    this->tileSize = tileSize;

    const int rowByteCount = tileSize * 3;
    const int imgByteCount = tileSize * rowByteCount;

    const int perAxis = 2048 / tileSize; // 32

    const int rows = std::min(count, perAxis), cols = (count / perAxis) + 1;

    const int maxCount = perAxis * perAxis;
    if (count > maxCount) {
        printf("[WARNING]: too many textures for texture atlas: Max=%i -- Count=%i", maxCount, count);
    }

    unsigned char arr[rows * cols * imgByteCount];

    for (int i = 0; i < count; i++) {
        const int row = i % perAxis, col = i / perAxis;

        std::vector<unsigned char>& texture = *textureArr[i];

        for (int j = 0; j < tileSize; j++) {
            unsigned char* fromPtr = &texture[0] + (rowByteCount * j);
            unsigned char* toPtr = arr + (imgByteCount * cols * row + rowByteCount * cols * j + rowByteCount * col);

            memcpy(toPtr, fromPtr, rowByteCount);
        }
    }

    Set(tileSize * cols, tileSize * rows, arr);
}

TexCoords TiledTextureAtlas::GetCoords(int index) const {
    const int perAxis = 2048 / tileSize; // 32
    const int rows = std::min(count, perAxis), cols = (count / perAxis) + 1;
    const int row = index % perAxis, col = index / perAxis;

    const float perX = 1.0f / (float) cols, perY = 1.0f / (float) rows;
    return {{(float) col * perX, (float) (row + 1) * perY}, {(float) (col + 1) * perX, (float) row * perY}};
}
