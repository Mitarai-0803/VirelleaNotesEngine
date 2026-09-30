//----------------------------------------------------------------------------
//! @file   MyLibraryFunctions.cpp
//! @brief  ユーティリティ関数群の実装
//! @detail 時間変換、当たり判定、角度変換、カメラ制御、ZIP作成などの実装を提供します。
//----------------------------------------------------------------------------
#include "MyLibraryFunctions.h"
#include "GameConstants.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdint>

namespace MyLibrary
{
	//----------------------------------------------------------------------------
	//! ミリ秒を秒に変換します。
	//----------------------------------------------------------------------------
	float ConvertMilliToSec(int milli)
	{
		// 注意: 先にfloatにキャストすることで精度を損なわない
		return static_cast<float>(milli) / static_cast<float>(MS_PER_SEC);
	}

	//----------------------------------------------------------------------------
	//! ミリ秒を分に変換します。
	//----------------------------------------------------------------------------
	float ConvertMilliToMinute(int milli)
	{
		return static_cast<float>(milli) / static_cast<float>(MS_PER_MIN);
	}

	//---------------------------------------------------------------
	//! 秒数をフレーム数に変換する関数
	//---------------------------------------------------------------
	int SecondsToFrames(float seconds)
	{
		return static_cast<int>(seconds * FRAME_RATE); // 秒数をフレーム数に変換
	}

	//----------------------------------------------------------------------------
	//! ファイルパスの拡張子を取得します。
	//----------------------------------------------------------------------------
	std::string GetFileExtension(const char* filepath)
	{
		std::string path(filepath);
		size_t pos = path.rfind('.');
		if (pos == std::string::npos || pos == path.length() - 1) return "";
		return path.substr(pos + 1);
	}

	//----------------------------------------------------------------------------
	//! 目標座標に向けた回転角を計算します。
	//----------------------------------------------------------------------------
	float ObjectPointToDirection(FloatPos now_pos, FloatPos goal_pos, float dir, float rot_speed)
	{
		float now_dir = TODEGREE(dir);
		float dx = goal_pos.x - now_pos.x;
		float dy = goal_pos.y - now_pos.y;
		float goal_dir = TODEGREE(atan2f(dy, dx)) + 90.0f;

		float diff = goal_dir - now_dir;
		while (diff >= 180.0f) diff -= 360.0f;
		while (diff < -180.0f) diff += 360.0f;

		if (diff > rot_speed) now_dir += rot_speed;
		else if (diff < -rot_speed) now_dir -= rot_speed;
		else now_dir = goal_dir;

		return TORADIAN(now_dir);
	}

	//----------------------------------------------------------------------------
	//! カメラの停止座標を計算します。
	//----------------------------------------------------------------------------
	void CameraStop(float pos_x, float pos_y, float w, float h, float& cam_x, float& cam_y)
	{
		if (pos_x > HALF_SCREEN_W && pos_x < w - HALF_SCREEN_W)
			cam_x = HALF_SCREEN_W - pos_x;

		if (pos_y > HALF_SCREEN_H && pos_y < h - HALF_SCREEN_H)
			cam_y = HALF_SCREEN_H - pos_y;
	}

	//----------------------------------------------------------------------------
	//! 位置をエリア内に収めます。
	//----------------------------------------------------------------------------
	void ClampPositionToArea(FloatPos& target_pos, int target_w, int target_h, float area_w, float area_h)
	{
		target_pos.x = std::max(static_cast<float>(target_w) * HALF, std::min(target_pos.x, area_w - target_w * HALF));
		target_pos.y = std::max(static_cast<float>(target_h) * HALF, std::min(target_pos.y, area_h - target_h * HALF));
	}

	//----------------------------------------------------------------------------
	//! 点と矩形の当たり判定(浮動小数点)
	//----------------------------------------------------------------------------
	bool CheckPointBoxHitF(const FloatPos& point, const FloatPos& rect_pos, const FloatSize& rect_size)
	{
		return (point.x >= rect_pos.x && point.x <= rect_pos.x + rect_size.w) &&
			(point.y >= rect_pos.y && point.y <= rect_pos.y + rect_size.h);
	}

	//----------------------------------------------------------------------------
	//! 点と矩形の当たり判定
	//----------------------------------------------------------------------------
	bool CheckPointBoxHit(float point_x, float point_y, float box_x, float box_y, float box_w, float box_h)
	{
		return (point_x >= box_x && point_x <= box_x + box_w) &&
			(point_y >= box_y && point_y <= box_y + box_h);
	}

	//----------------------------------------------------------------------------
	//! 矩形同士の当たり判定
	//----------------------------------------------------------------------------
	bool CheckBoxHit(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
	{
		return (x1 + w1 >= x2 && x1 <= x2 + w2) &&
			(y1 + h1 >= y2 && y1 <= y2 + h2);
	}

	//----------------------------------------------------------------------------
	//! 円同士の当たり判定
	//----------------------------------------------------------------------------
	bool CheckCircleHit(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		float dist = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		return dist <= (r1 + r2);
	}

	//----------------------------------------------------------------------------
	//! 点と円の当たり判定
	//----------------------------------------------------------------------------
	bool CheckPointCircleHit(float point_x, float point_y, float circle_x, float circle_y, float circle_r)
	{
		float dist = sqrt((point_x - circle_x) * (point_x - circle_x) + (point_y - circle_y) * (point_y - circle_y));
		return dist <= circle_r;
	}

	//----------------------------------------------------------------------------
	//! 度→ラジアン変換
	//----------------------------------------------------------------------------
	float TORADIAN(float degree) { return degree * PI / 180.0f; }

	//----------------------------------------------------------------------------
	//! ラジアン→度変換
	//----------------------------------------------------------------------------
	float TODEGREE(float radian) { return radian * 180.0f / PI; }

	//----------------------------------------------------------------------------
	//! 回転方向のX成分
	//----------------------------------------------------------------------------
	float GetLengthAlongRotation(float rot) { return cosf(rot); }

	//----------------------------------------------------------------------------
	//! 回転方向のY成分
	//----------------------------------------------------------------------------
	float GetLengthOppositeRotation(float rot) { return sinf(rot); }

	//----------------------------------------------------------------------------
	//! 簡易ZIP作成(Storeのみ、CRCは0固定)
	//----------------------------------------------------------------------------
#pragma pack(push, 1)
	struct LocalFileHeader
	{
		uint32_t signature;
		uint16_t versionNeeded;
		uint16_t flags;
		uint16_t compression;
		uint16_t modTime;
		uint16_t modDate;
		uint32_t crc32;
		uint32_t compSize;
		uint32_t uncompSize;
		uint16_t nameLen;
		uint16_t extraLen;
	};
	struct CentralDirHeader
	{
		uint32_t signature;
		uint16_t versionMade;
		uint16_t versionNeeded;
		uint16_t flags;
		uint16_t compression;
		uint16_t modTime;
		uint16_t modDate;
		uint32_t crc32;
		uint32_t compSize;
		uint32_t uncompSize;
		uint16_t nameLen;
		uint16_t extraLen;
		uint16_t commentLen;
		uint16_t diskStart;
		uint16_t intAttr;
		uint32_t extAttr;
		uint32_t localHeaderOfs;
	};
	struct EndOfCentralDir
	{
		uint32_t signature;
		uint16_t diskNum;
		uint16_t startDisk;
		uint16_t entryOnDisk;
		uint16_t totalEntries;
		uint32_t centralSize;
		uint32_t centralOfs;
		uint16_t commentLen;
	};
#pragma pack(pop)

	static bool ReadAll(const std::string& path, std::vector<uint8_t>& out)
	{
		FILE* fp = nullptr;
#if defined(_MSC_VER)
		if (fopen_s(&fp, path.c_str(), "rb")) return false;
#else
		fp = fopen(path.c_str(), "rb");
		if (!fp) return false;
#endif
		fseek(fp, 0, SEEK_END);
		long sz = ftell(fp);
		if (sz < 0)
		{
			fclose(fp);
			return false;
		}
		fseek(fp, 0, SEEK_SET);
		out.resize(static_cast<size_t>(sz));
		if (sz > 0)
		{
			if (fread(out.data(), 1, static_cast<size_t>(sz), fp) != static_cast<size_t>(sz))
			{
				fclose(fp);
				return false;
			}
		}
		fclose(fp);
		return true;
	}

	static bool WriteAll(FILE* fp, const void* data, size_t size)
	{
		if (size == 0) return true;
		return fwrite(data, 1, size, fp) == size;
	}

	bool CreateZipStore(const std::string& zip_path,
		const std::vector<std::pair<std::string, std::string>>& src_and_names)
	{
		FILE* fp = nullptr;
#if defined(_MSC_VER)
		if (fopen_s(&fp, zip_path.c_str(), "wb")) return false;
#else
		fp = fopen(zip_path.c_str(), "wb");
		if (!fp) return false;
#endif

		struct Entry
		{
			std::string name;
			uint32_t crc32;      // 未使用(0)
			uint32_t compSize;
			uint32_t uncompSize;
			uint32_t localOfs;
		};
		std::vector<Entry> entries;
		entries.reserve(src_and_names.size());

		for (const auto& p : src_and_names)
		{
			const std::string& src = p.first;
			const std::string& name = p.second;

			std::vector<uint8_t> data;
			if (!ReadAll(src, data))
			{
				fclose(fp);
				return false;
			}

			long localOfs = ftell(fp);
			if (localOfs < 0)
			{
				fclose(fp);
				return false;
			}

			LocalFileHeader lh{};
			lh.signature = 0x04034b50;
			lh.versionNeeded = 20;
			lh.flags = 0;
			lh.compression = 0;
			lh.modTime = 0;
			lh.modDate = 0;
			lh.crc32 = 0;
			lh.compSize = static_cast<uint32_t>(data.size());
			lh.uncompSize = static_cast<uint32_t>(data.size());
			lh.nameLen = static_cast<uint16_t>(name.size());
			lh.extraLen = 0;

			if (!WriteAll(fp, &lh, sizeof(lh))) { fclose(fp); return false; }
			if (!WriteAll(fp, name.data(), name.size())) { fclose(fp); return false; }
			if (!WriteAll(fp, data.data(), data.size())) { fclose(fp); return false; }

			entries.push_back(Entry{ name, 0, lh.compSize, lh.uncompSize, static_cast<uint32_t>(localOfs) });
		}

		long centralOfs = ftell(fp);
		if (centralOfs < 0) { fclose(fp); return false; }

		uint32_t centralSize = 0;
		for (const auto& e : entries)
		{
			CentralDirHeader ch{};
			ch.signature = 0x02014b50;
			ch.versionMade = 20;
			ch.versionNeeded = 20;
			ch.flags = 0;
			ch.compression = 0;
			ch.modTime = 0;
			ch.modDate = 0;
			ch.crc32 = 0;
			ch.compSize = e.compSize;
			ch.uncompSize = e.uncompSize;
			ch.nameLen = static_cast<uint16_t>(e.name.size());
			ch.extraLen = 0;
			ch.commentLen = 0;
			ch.diskStart = 0;
			ch.intAttr = 0;
			ch.extAttr = 0;
			ch.localHeaderOfs = e.localOfs;

			if (!WriteAll(fp, &ch, sizeof(ch))) { fclose(fp); return false; }
			if (!WriteAll(fp, e.name.data(), e.name.size())) { fclose(fp); return false; }

			centralSize += static_cast<uint32_t>(sizeof(ch) + e.name.size());
		}

		EndOfCentralDir eocd{};
		eocd.signature = 0x06054b50;
		eocd.diskNum = 0;
		eocd.startDisk = 0;
		eocd.entryOnDisk = static_cast<uint16_t>(entries.size());
		eocd.totalEntries = static_cast<uint16_t>(entries.size());
		eocd.centralSize = centralSize;
		eocd.centralOfs = static_cast<uint32_t>(centralOfs);
		eocd.commentLen = 0;

		if (!WriteAll(fp, &eocd, sizeof(eocd))) { fclose(fp); return false; }

		fclose(fp);
		return true;
	}
}
