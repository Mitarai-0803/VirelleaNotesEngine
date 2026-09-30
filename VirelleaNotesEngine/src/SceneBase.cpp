//----------------------------------------------------------------------------
//! @file   SceneBase.cpp
//! @brief  シーン基底クラスの実装
//! @detail シーン内のオブジェクト管理およびライフサイクル管理の機能を提供します。
//----------------------------------------------------------------------------
#include "SceneBase.h"
#include <algorithm>

namespace Scene
{
	//-----------------------------------------------------------
	//! @brief シーン初期化
	//-----------------------------------------------------------
	void SceneBase::Init()
	{
		ObjectInit();
		SceneInit();
	}

	//-----------------------------------------------------------
	//! @brief シーン更新
	//-----------------------------------------------------------
	void SceneBase::Update()
	{
		ObjectUpdate();
		SceneUpdate();
	}

	//-----------------------------------------------------------
	//! @brief シーン描画
	//-----------------------------------------------------------
	void SceneBase::Draw()
	{
		ObjectDraw();
		SceneDraw();
	}

	//-----------------------------------------------------------
	//! @brief シーン終了処理
	//-----------------------------------------------------------
	void SceneBase::End()
	{
		ObjectEnd();
		SceneEnd();
	}

	//-----------------------------------------------------------
	//! @brief オブジェクト追加
	//-----------------------------------------------------------
	void SceneBase::AddObject(std::shared_ptr<GameObject> obj)
	{
		obj->Init();
		scene_objects_.push_back(obj);
	}

	//-----------------------------------------------------------
	//! @brief 全オブジェクト初期化
	//-----------------------------------------------------------
	void SceneBase::ObjectInit()
	{
		for (auto& obj : scene_objects_)
		{
			obj->Init();
		}
	}

	//-----------------------------------------------------------
	//! @brief 全オブジェクト更新
	//-----------------------------------------------------------
	void SceneBase::ObjectUpdate()
	{
		frame_count_++;

		for (auto& obj : scene_objects_)
		{
			obj->Update();
		}
	}

	//-----------------------------------------------------------
	//! @brief 全オブジェクト描画
	//-----------------------------------------------------------
	void SceneBase::ObjectDraw()
	{
		// 描画順でソートして描画
		std::sort(scene_objects_.begin(), scene_objects_.end(), [](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b){
			return a->GetDrawOrder() < b->GetDrawOrder();
		});

		for (auto& obj : scene_objects_)
		{
			obj->Draw();
		}
	}

	//-----------------------------------------------------------
	//! @brief 全オブジェクト終了処理
	//-----------------------------------------------------------
	void SceneBase::ObjectEnd()
	{
		for (auto& obj : scene_objects_)
		{
			obj->End();
		}

		scene_objects_.clear();
	}
}