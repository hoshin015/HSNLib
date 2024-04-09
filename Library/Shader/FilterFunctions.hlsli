//--------------------------------------------
//	RGB色空間の数値からHSV色空間の数値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float3 RGB2HSV(float3 rgb)
{
    float3 hsv = 0;
    
    float vMax = max(rgb.r, max(rgb.g, rgb.b)); // RGBの３つの値で最大のもの
    float vMin = min(rgb.r, min(rgb.g, rgb.b)); // RGBの３つの値で最小のもの
    float delta = vMax - vMin;                  // 最大値と最小値の差
    
    // V (明度) 一番強い色をV値にする
    hsv.z = vMax;
    
    // S (彩度) 最大値と最小値の差を正規化して求める
    hsv.y = (delta / vMax) * step(0, vMax);
    
    // H (色相) RGBのうち最大値と最小値の差から求める
    if(hsv.y > 0.0f)
    {
        if(rgb.r == vMax)
        {
            hsv.x = 60 * ((rgb.g - rgb.b) / delta);
        }
        else if(rgb.g == vMax)
        {
            hsv.x = 60 * ((rgb.b - rgb.r) / delta) + 120;
        }
        else
        {
            hsv.x = 60 * ((rgb.r - rgb.g) / delta) + 240;
        }
        
        if(hsv.x < 0)
        {
            hsv.x += 360;
        }
    }
    return hsv;
}

//--------------------------------------------
//	HSV色空間の数値からRGB色空間の数値への変換関数
//--------------------------------------------
//hsv:HSV色空間の数値
float3 HSV2RGB(float3 hsv)
{
    float3 rgb = 0;
    
    if(hsv.y == 0)
    {
        // S (彩度) が０と等しいなら無色もしくは灰色
        rgb.r = rgb.g = rgb.b = hsv.z;
    }
    else
    {
        // 色相の H (色相) の位置と S (彩度) 、 V (明度) からRGB値を算出する
        float vMax = hsv.z;
        float vMin = vMax - (hsv.y * vMax);
        
        hsv.x %= 360;   // 0 ～ 360 に変換
        float Huei = (int) (hsv.x / 60);
        float Huef = hsv.x / 60 - Huei;
        float p = vMax * (1.0f - hsv.y);
        float q = vMax * (1.0f - hsv.y * Huef);
        float t = vMax * (1.0f - hsv.y * (1 - Huef));
        if(Huei == 0)
        {
            rgb.r = vMax;
            rgb.g = t;
            rgb.b = p;
        }
        else if(Huei == 1)
        {
            rgb.r = q;
            rgb.g = vMax;
            rgb.b = p;
        }
        else if(Huei == 2)
        {
            rgb.r = p;
            rgb.g = vMax;
            rgb.b = t;
        }
        else if(Huei == 3)
        {
            rgb.r = p;
            rgb.g = q;
            rgb.b = vMax;
        }
        else if(Huei == 4)
        {
            rgb.r = t;
            rgb.g = p;
            rgb.b = vMax;
        }
        else if(Huei == 5)
        {
            rgb.r = vMax;
            rgb.g = p;
            rgb.b = q;
        }
    }
    return rgb;
}

//--------------------------------------------
//	RGB色空間の数値から輝度値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float RGB2Luminance(float3 rgb)
{
    //static const float3 luminanceValue = float3(0.299f, 0.587f, 0.114f);
    static const float3 luminanceValue = float3(0.2125f, 0.75154f, 0.0721f);
    return dot(luminanceValue, rgb);
}