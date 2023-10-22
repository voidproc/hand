//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

//
//	Textures
//
Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

namespace s3d
{
	//
	//	VS Input
	//
	struct VSInput
	{
		float2 position	: POSITION;
		float2 uv		: TEXCOORD0;
		float4 color	: COLOR0;
	};

	//
	//	VS Output / PS Input
	//
	struct PSInput
	{
		float4 position	: SV_POSITION;
		float4 color	: COLOR0;
		float2 uv		: TEXCOORD0;
	};

	//
	//	Siv3D Functions
	//
	float4 Transform2D(float2 pos, float2x4 t)
	{
		return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
	}
}

//
//	Constant Buffer
//
cbuffer VSConstants2D : register(b0)
{
	row_major float2x4 g_transform;
	float4 g_colorMul;
}

cbuffer PSConstants2D : register(b0)
{
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColor;
	float4 g_sdfShadowColor;
	float4 g_internal;
}

//
//	Functions
//
s3d::PSInput VS(s3d::VSInput input)
{
	s3d::PSInput result;
	result.position	= s3d::Transform2D(input.position, g_transform);
	result.color	= input.color * g_colorMul;
	result.uv		= input.uv;
	return result;
}

float4 PS_Shape(s3d::PSInput input) : SV_TARGET
{
	return (input.color + g_colorAdd);
}

float4 PS_Texture(s3d::PSInput input) : SV_TARGET
{
    const float PW = 1.0 / 160.0;
    const float PH = 1.0 / 144.0;

	// Scanline
    const float4 ColorWhite = float4(1, 1, 1, 1);
    const float4 ColorLine = float4(1, 1, 1, 0.9);
    const float4 scanline = fmod(input.uv.y, PH) < PH * 0.4 ? ColorWhite : ColorLine;
	
	// RGB shift
	
    const float2 ra = g_texture0.Sample(g_sampler0, input.uv + float2(-PW/1.5, 0.0)).ra;
    const float2 ga = g_texture0.Sample(g_sampler0, input.uv).ga;
    const float2 ba = g_texture0.Sample(g_sampler0, input.uv + float2(PH/1.5, 0.0)).ba;
    const float a = (ra.y + ga.y + ba.y) / 3;
	
    const float4 texColor = g_texture0.Sample(g_sampler0, input.uv) * 0.5 + float4(ra.x, ga.x, ba.x, a) * 0.5;
	
    return ((texColor * scanline * input.color) + g_colorAdd);
}
