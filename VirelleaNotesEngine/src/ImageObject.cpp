//----------------------------------------------------------------------------
//! @file   ImageObject.cpp
//! @brief  画像オブジェクトの実装
//! @detail 画像を読み込み座標を設定して描画する機能を提供します。
//----------------------------------------------------------------------------
#include "ImageObject.h"
#include "UIConstants.h"
#include "DxLib.h"
#include <unordered_map>
#include <string>

namespace UI
{
namespace
{
	// ファイルパス -> ハンドルの共有キャッシュ
	static std::unordered_map<std::string, int> g_imageHandles;
	// ファイルパス -> 参照カウント
	static std::unordered_map<std::string, int> g_refCounts;
}

	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void ImageObject::OnInit()
	{
		image_handle_ = 0;
		pos_          = { 0.0f, 0.0f };
		has_image_ = false;
		draw_x_int_ = 0;
		draw_y_int_ = 0;
		image_path_.clear();
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void ImageObject::OnDraw()
	{
		if (!has_image_) return;

		// 整数座標キャッシュを使用して描画
		DrawGraph(draw_x_int_, draw_y_int_, image_handle_, true);
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void ImageObject::OnEnd()
	{
		if (!image_path_.empty())
		{
			auto itRef = g_refCounts.find(image_path_);
			if (itRef != g_refCounts.end())
			{
				itRef->second -= 1;
				if (itRef->second <= 0)
				{
					auto itH = g_imageHandles.find(image_path_);
					if (itH != g_imageHandles.end())
					{
						DeleteGraph(itH->second);
						g_imageHandles.erase(itH);
					}
					g_refCounts.erase(itRef);
				}
			}

			image_path_.clear();
			image_handle_ = 0;
		}

		has_image_ = false;
		draw_x_int_ = 0;
		draw_y_int_ = 0;
	}

	//-----------------------------------------------------------
	//! @brief 読み込む画像パス設定
	//-----------------------------------------------------------
	void ImageObject::SetPath(const char* path)
	{
		if (path == nullptr) return;

		std::string newPath(path);

		// 同じパスなら何もしない
		if (newPath == image_path_)
		{
			return;
		}

		// 既存パスの参照を減らす
		if (!image_path_.empty())
		{
			auto itRef = g_refCounts.find(image_path_);
			if (itRef != g_refCounts.end())
			{
				itRef->second -= 1;
				if (itRef->second <= 0)
				{
					auto itH = g_imageHandles.find(image_path_);
					if (itH != g_imageHandles.end())
					{
						DeleteGraph(itH->second);
						g_imageHandles.erase(itH);
					}
					g_refCounts.erase(itRef);
				}
			}

			image_path_.clear();
			image_handle_ = 0;
		}

		// 既にロード済みならハンドルを共有
		auto it = g_imageHandles.find(newPath);
		if (it != g_imageHandles.end())
		{
			image_handle_ = it->second;
			g_refCounts[newPath] += 1;
			image_path_ = newPath;
			has_image_ = (image_handle_ != 0);
			return;
		}

		// 新規読み込み
		int handle = LoadGraph(newPath.c_str());
		if (handle == 0)
		{
			// 読み込み失敗
			image_handle_ = 0;
			has_image_ = false;
			return;
		}

		// 登録
		g_imageHandles[newPath] = handle;
		g_refCounts[newPath] = 1;
		image_handle_ = handle;
		image_path_ = newPath;
		has_image_ = true;
	}

	//-----------------------------------------------------------
	//! @brief 座標設定
	//-----------------------------------------------------------
	void ImageObject::SetPos(FloatPos pos)
	{
		pos_ = pos;

		// 整数座標キャッシュ更新
		draw_x_int_ = static_cast<int>(pos_.x);
		draw_y_int_ = static_cast<int>(pos_.y);
	}
}
