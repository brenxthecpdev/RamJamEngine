//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "lightHelper.fx"
#include "PerFrameConstants.hlsli"

cbuffer cbPerObject
{
	float4x4 gWorld;
	//float4x4 gWorldInvTranspose;
	float4x4 gDiffuseMapTrf	: Texture_Diffuse_Trf;
	//-------
	float4 gMatAmbient		: Ambient;
	float4 gMatDiffuse		: Diffuse;
	float4 gMatSpecular		: Specular;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap		: Texture_Diffuse;

SamplerState gTextureSampler;

// SamplerState samAnisotropic
// {
// 	Filter = ANISOTROPIC;
// 	MaxAnisotropy = 4;
// 
// 	AddressU = WRAP;
// 	AddressV = WRAP;
// };

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float3 TanL    : TANGENT;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float3 TanW    : TANGENT;
	float2 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);		// Non-uniform scaling needs inverseTranspose !! i.e. mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.TanW    = mul(vin.TanL, (float3x3)gWorld);
		
	// Transform to homogeneous clip space.
	float4x4 worldViewProj = mul(gWorld, gViewProj);
	vout.PosH = mul(float4(vin.PosL, 1.0f), worldViewProj);

	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gDiffuseMapTrf).xy;
	
	return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye); 

	// Normalize.
	toEye /= distToEye;

	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	if(gUseTexture)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample( gTextureSampler, pin.Tex );

		if(gUseAlphaClip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texColor.a - 0.1f);
		}
	}
	
	//
	// Lighting.
	//

	float4 litColor = texColor;
	
	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.  
	float4 A, D, S;
	Material mat;
	mat.Ambient  = gMatAmbient;
	mat.Diffuse  = gMatDiffuse;
	mat.Specular = gMatSpecular;
	uint totalLights, dummy;

	// Directionnal Lighting
	gDirLights.GetDimensions(totalLights, dummy);
	for (uint dirLightIdx = 0; dirLightIdx < totalLights; ++dirLightIdx)
	{
		DirectionalLight light = gDirLights[dirLightIdx];
		ComputeDirectionalLight(mat, light, pin.NormalW, toEye, A, D, S);

		ambient += A;
		diffuse += D;
		spec    += S;
	}

	// Point Lighting
	gPointLights.GetDimensions(totalLights, dummy);
	for (uint pointLightIdx = 0; pointLightIdx < totalLights; ++pointLightIdx)
	{
		PointLight light = gPointLights[pointLightIdx];
		ComputePointLight(mat, light, pin.PosW, pin.NormalW, toEye, A, D, S);

		ambient += A;
		diffuse += D;
		spec    += S;
	}

	// Spot Lighting
	gSpotLights.GetDimensions(totalLights, dummy);
	for (uint spotLightIdx = 0; spotLightIdx < totalLights; ++spotLightIdx)
	{
		SpotLight light = gSpotLights[spotLightIdx];
		ComputeSpotLight(mat, light, pin.PosW, pin.NormalW, toEye, A, D, S);

		ambient += A;
		diffuse += D;
		spec    += S;
	}

	// Modulate with late add.
	litColor = texColor*(ambient + diffuse) + spec;

	//
	// Fogging
	//

	if( gUseFog )
	{
		float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 

		// Blend the fog color and the lit color.
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	// Common to take alpha from diffuse material.
	litColor.a = gMatDiffuse.a * texColor.a;

	return litColor;
}

//--------------------------------------------------------------------------------------------------

technique11 Basic
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PS() ) );
	}
}
