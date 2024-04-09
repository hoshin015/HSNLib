#pragma once

// ラベル定義
enum class FONTNO
{
    meirio,
    tsunagi,
    x12y16pxMaruMonica,
    x8y12pxTheStrongGamer,
    MYFONT4,
    MYFONT5,
    MYFONT6,
};

//Font Size
enum class FONTSIZE
{
    SIZE12,
    SIZE15,
    SIZE22,
    SIZE36,
};

// 文字描画のアライン
enum class TEXT_ALIGN
{
    UPPER_LEFT = 0, // 左上
    UPPER_MIDDLE,   // 上中央
    UPPER_RIGHT,    // 右上
    MIDDLE_LEFT,    // 左中央
    MIDDLE,         // 中央
    MIDDLE_RIGHT,   // 右中央
    LOWER_LEFT,     // 左下
    LOWER_MIDDLE,   // 下中央
    LOWER_RIGHT,    // 右下
};