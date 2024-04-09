#pragma once
// ƒƒ‚ƒŠƒŠ[ƒN‚ÌêŠ•\¦
#if _DEBUG
#define new_ new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define new_ new
#endif