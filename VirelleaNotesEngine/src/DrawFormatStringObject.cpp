//----------------------------------------------------------------------------
//! @file   DrawFormatStringObject.cpp
//! @brief  フォーマット文字列描画オブジェクトの実装
//! @detail 名前付きプレースホルダーによるフォーマット文字列描画の機能を提供します。
//----------------------------------------------------------------------------
#include "DrawFormatStringObject.h"
#include <regex>
#include <DxLib.h>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void DrawFormatStringObject::OnInit()
	{
		format_      = "";
		formatted_   = "";
		pos_         = { 0.0f, 0.0f };
		color_       = 0xFFFFFF;
		font_handle_ = 0;

		params_.clear();
		format_dirty_ = true;
	}

	//-----------------------------------------------------------
	//! @brief フォーマット文字列設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetFormat(const std::string& fmt)
	{
		format_ = fmt;
		format_dirty_ = true;
	}

	//-----------------------------------------------------------
	//! @brief 整数パラメータ設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetParam(const std::string& name, int v)
	{
		params_[name] = v;
		format_dirty_ = true;
	}

	//-----------------------------------------------------------
	//! @brief 浮動小数点パラメータ設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetParam(const std::string& name, float v)
	{
		params_[name] = v;
		format_dirty_ = true;
	}

	//-----------------------------------------------------------
	//! @brief 文字列パラメータ設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetParam(const std::string& name, const std::string& v)
	{
		params_[name] = v;
		format_dirty_ = true;
	}

	//-----------------------------------------------------------
	//! @brief パラメータをフォーマット文字列に展開
	//-----------------------------------------------------------
	void DrawFormatStringObject::ApplyFormat()
	{
		std::string result = format_;

		for (const auto& [key, value] : params_)
		{
			std::string search_str = "{" + key;
			size_t pos             = 0;

			while ((pos = result.find(search_str, pos)) != std::string::npos)
			{
				size_t end_brace = result.find('}', pos);

				if (end_brace == std::string::npos) break;

				std::string inside = result.substr(pos + 1, end_brace - pos - 1);
				std::string fmt    = "%s";

				size_t colon = inside.find(':');

				if (colon != std::string::npos)
				{
					fmt = inside.substr(colon + 1);
				}

				char buf[128] = {};

				// 型ごとに整形
				if (std::holds_alternative<int>(value))
					snprintf(buf, sizeof(buf), fmt.c_str(), std::get<int>(value));
				else if (std::holds_alternative<float>(value))
					snprintf(buf, sizeof(buf), fmt.c_str(), std::get<float>(value));
				else
					snprintf(buf, sizeof(buf), fmt.c_str(), std::get<std::string>(value).c_str());

				result.replace(pos, end_brace - pos + 1, buf);
				pos += strlen(buf);
			}
		}

		formatted_ = result;
		format_dirty_ = false;
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void DrawFormatStringObject::OnDraw()
	{
		if (format_dirty_) ApplyFormat();
		if (formatted_.empty()) return;

		if (font_handle_ == 0)
			DrawStringF(pos_.x, pos_.y, formatted_.c_str(), color_);
		else
			DrawStringFToHandle(pos_.x, pos_.y, formatted_.c_str(), color_, font_handle_);
	}

	//-----------------------------------------------------------
	//! @brief 座標設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetPos(float x, float y)
	{
		pos_.x = x;
		pos_.y = y;
	}

	//-----------------------------------------------------------
	//! @brief 色設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetColor(unsigned int color)
	{
		color_ = color;
	}

	//-----------------------------------------------------------
	//! @brief フォントハンドル設定
	//-----------------------------------------------------------
	void DrawFormatStringObject::SetFontHandle(int h)
	{
		font_handle_ = h;
	}
}