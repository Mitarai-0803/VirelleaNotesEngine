//----------------------------------------------------------------------------
//! @file   Font.cpp
//! @brief  フォント管理の実装
//! @detail 各サイズのフォントデータをロードし、管理する機能を提供します。
//----------------------------------------------------------------------------
#include "Font.h"
#include <DxLib.h>

namespace Font
{
	//-----------------------------------------------------------
	//! @brief フォント初期化
	//-----------------------------------------------------------
	void FontInit()
	{
		font_handle[FONT_SIZE_70_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black70.dft");
		font_handle[FONT_SIZE_60_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black60.dft");
		font_handle[FONT_SIZE_50_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black50.dft");
		font_handle[FONT_SIZE_40_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black40.dft");
		font_handle[FONT_SIZE_30_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black30.dft");
		font_handle[FONT_SIZE_20_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black20.dft");
		font_handle[FONT_SIZE_10_INDEX] = LoadFontDataToHandle("EngineData/Font/NotoSansJP-Black10.dft");
	}

	//-----------------------------------------------------------
	//! @brief フォント解放
	//-----------------------------------------------------------
	void FontExit()
	{
		for (int i = 0; i < FONT_MAX; i++)
		{
			if (font_handle[i] != -1)
			{
				DeleteFontToHandle(font_handle[i]);
				font_handle[i] = -1;
			}
		}
	}

	//-----------------------------------------------------------
	//! @brief フォントハンドル取得
	//-----------------------------------------------------------
	int GetFont(int font_index)
	{
		if (font_index < 0 || font_index >= FONT_MAX)
			font_index = FONT_SIZE_70_INDEX;

		return font_handle[font_index];
	}
}