//
// Created by Tobiathan on 1/21/22.
//

#ifndef SENIORRESEARCH_TILEDTEXTUREATLAS_H
#define SENIORRESEARCH_TILEDTEXTUREATLAS_H

#include <vector>
#include "Texture.h"
#include "../util/Util.h"

struct TexCoords {
    Vec2 v1, v2;
};

class TiledTextureAtlas : public Texture {
public:
    TiledTextureAtlas();
    void Generate(int tileSize, const std::vector<std::vector<unsigned char>*>& textureArr);
    TexCoords GetCoords(int index) const;
private:
    int count, tileSize;
};


#endif //SENIORRESEARCH_TILEDTEXTUREATLAS_H
