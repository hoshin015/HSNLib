#pragma once
#include "Library/2D/Sprite.h"

class SpriteUi : public Sprite
{
public:
    DirectX::XMFLOAT2 pos = {};
    DirectX::XMFLOAT2 size = { 100,100 };
    DirectX::XMFLOAT4 color = { 1,1,1,1 };
    float angle = 0.0f;
    DirectX::XMFLOAT2 texPos = {};
    DirectX::XMFLOAT2 texSize = { 100,100 };
};