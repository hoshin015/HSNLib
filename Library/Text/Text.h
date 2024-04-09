#pragma once

#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "FontEnum.h"


class Text
{
public:
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont[16][16];

private:
    Text();
    ~Text();

public:
    static Text& Instance()
    {
        static Text instance;
        return instance;
    }

    void drawText(FONTNO fontNo, FONTSIZE fontSize, std::string str, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale = { 1,1 }, DirectX::XMFLOAT4 color = { 1,1,1,1 }, TEXT_ALIGN align = TEXT_ALIGN::UPPER_LEFT);
    
    float getBufferW(TEXT_ALIGN align, float width);
    float getBufferH(TEXT_ALIGN align, float height);
};