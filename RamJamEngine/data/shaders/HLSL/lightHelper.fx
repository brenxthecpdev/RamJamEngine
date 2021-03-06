//***************************************************************************************
// LightHelper.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//***************************************************************************************

#include "Rendering.hlsl"

struct DirectionalLight
{
	float4 Color;
	float4 Direction;
	//float2 pad;
};

struct PointLight
{
	float3 Position;
	float3 Color;
	float  Range;
	float  Intensity;
};

struct SpotLight
{
	float3 Color;
	float3 Position;
	float3 Direction;
	float  Spot;
	float  Range;
	float  Intensity;
};


StructuredBuffer<DirectionalLight>	gDirLights;
StructuredBuffer<PointLight>		gPointLights;
StructuredBuffer<SpotLight>			gSpotLights;

struct Material
{
	float4 Albedo;
	float  SpecularAmount;
	float  SpecularPower;
};

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L, 
							 float3 normal, float3 toEye,
							 out float4 diffuse,
							 out float4 spec)
{
	// Initialize outputs.
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction.xyz;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), (mat.SpecularPower<1.0 ? 1.0 : mat.SpecularPower));

		float3 diff = diffuseFactor * mat.Albedo.rgb * L.Color.xyz;
		diffuse = float4(diff,1.0);
		spec    = float4((specFactor * mat.SpecularAmount).xxx, 1.0);
	}
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePointLight(	Material mat, PointLight L,
						float3 pos, float3 normal, float3 toEye,
						out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.Range )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), (mat.SpecularPower<1.0 ? 1.0 : mat.SpecularPower));

		float3 diff = diffuseFactor * mat.Albedo.rgb * L.Color;
		diffuse = float4(diff,1.0);
		spec    = float4((specFactor * mat.SpecularAmount).xxx, 1.0);
	}

	// calculate basic attenuation
	//float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));
	float att = saturate(1-d/L.Range);
	att *= L.Intensity * att;

	diffuse *= att;
	spec    *= att;
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeSpotLight(	Material mat, SpotLight L,
						float3 pos, float3 normal, float3 toEye,
						out float4 diffuse, out float4 spec)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.Range )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), (mat.SpecularPower<1.0 ? 1.0 : mat.SpecularPower));

		float3 diff = diffuseFactor * mat.Albedo.rgb * L.Color;
		diffuse = float4(diff,1.0);
		spec    = float4((specFactor * mat.SpecularAmount).xxx, 1.0);
	}
	
	// Scale by spotlight factor and attenuate.
	float3 lightDir = normalize(L.Direction);
	float spot      = pow(max(dot(-lightVec, lightDir), 0.0f), L.Spot);
	//L.Att = float3(0.05f, 0.025f, 0.005f);
	//float att       = spot / dot(L.Att, float3(1.0f, d, d*d));
	float att = saturate(1-d/L.Range);
	att *= L.Intensity * spot * att;

	diffuse *= att;
	spec    *= att;
}

// As below, we separate this for diffuse/specular parts for convenience in deferred lighting
void AccumulatePhongBRDF(float3 normal, float3 lightDir, float3 viewDir, float3 lightContrib, float specularPower, inout float3 litDiffuse, inout float3 litSpecular)
{
	// Simple Phong
	float NdotL = dot(normal, -lightDir);
	[flatten] if (NdotL > 0.0f)
	{
		float3 r = reflect(lightDir, normal);
		float RdotV = max(0.0f, dot(r, viewDir));
		float specular = pow(RdotV, specularPower<1.0 ? 1.0 : specularPower );

		litDiffuse  += lightContrib * NdotL;
		litSpecular += lightContrib * specular;
	}
}

void AccumulateDirBRDF(SurfaceData surface, DirectionalLight light, float3 toEye, inout float3 lit)
{
	float3 litDiffuse  = float3(0.0f, 0.0f, 0.0f);
	float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
	AccumulatePhongBRDF(	surface.normal,
							normalize(light.Direction.xyz),
							toEye,
							light.Color.xyz,
							surface.specularPower,
							litDiffuse, litSpecular);

	lit += surface.albedo.rgb * (gAmbientLightColor.rgb+litDiffuse) + surface.specularAmount * litSpecular;
}

void AccumulateSpotBRDF(SurfaceData surface, SpotLight light, float3 toEye, inout float3 lit)
{
	float3 lightDir = surface.position - light.Position;
	float distanceToLight   = length(lightDir);

	[branch] if (distanceToLight < light.Range)
	{
		float3 lightVec   = normalize(light.Direction);
		lightDir *= rcp(distanceToLight);       // A full normalize/RSQRT might be as fast here anyways...
		float spot        = pow(max(dot(lightDir, lightVec), 0.0f), light.Spot);
		float attenuation = saturate((light.Range-distanceToLight) / light.Range);
		attenuation *= light.Intensity * spot * attenuation;

		float3 litDiffuse  = float3(0.0f, 0.0f, 0.0f);
		float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
		AccumulatePhongBRDF(	surface.normal,
								lightDir,
								toEye,
								attenuation * light.Color,
								surface.specularPower,
								litDiffuse, litSpecular);

		lit += surface.albedo.rgb * litDiffuse + surface.specularAmount * litSpecular;
	}
}

void AccumulatePointBRDF(SurfaceData surface, PointLight light, float3 toEye, inout float3 lit)
{
	float3 lightDir       = surface.position - light.Position;
	float distanceToLight = length(lightDir);

	[branch] if (distanceToLight < light.Range)
	{
		float attenuation = saturate((light.Range-distanceToLight) / light.Range);
		attenuation *= light.Intensity * attenuation;
		lightDir    *= rcp(distanceToLight);       // A full normalize/RSQRT might be as fast here anyways...

		float3 litDiffuse = float3(0.0f, 0.0f, 0.0f);
		float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
		AccumulatePhongBRDF(surface.normal, lightDir, toEye, attenuation * light.Color, surface.specularPower, litDiffuse, litSpecular);

		lit += surface.albedo.rgb * litDiffuse + surface.specularAmount * litSpecular;
	}
}