#pragma once

namespace hand
{
	inline constexpr int32 SceneWidth = 160;
	inline constexpr int32 SceneHeight = 144;
	inline constexpr Size SceneSize{ SceneWidth, SceneHeight };
	inline constexpr Point SceneCenter{ SceneSize.x / 2, SceneSize.y / 2 };
	inline constexpr Rect SceneRect{ SceneSize };

	// シーンサイズに対するレンダーテクスチャのサイズ（倍率、整数倍）
	inline constexpr int RenderTextureScale = 2;
}
