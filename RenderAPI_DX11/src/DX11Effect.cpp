#include "DX11Effect.h"
#include "../../RamJamEngine/include/System.h"
#include "../../RamJamEngine/include/MaterialFactory.h"

Effect::Effect(ID3D11Device* device, const std::string& filename)
{
	mFX = nullptr;
	MaterialFactory::Instance()->RegisterShader(filename);
	RJE_CHECK_FOR_SUCCESS(D3DX11CreateEffectFromFile(filename.c_str(), 0, device, &mFX));
}

Effect::~Effect()
{
	RJE_SAFE_RELEASE(mFX);
}

//////////////////////////////////////////////////////////////////////////

BasicEffect::BasicEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	BasicTech         = mFX->GetTechniqueByName("Basic");
	DeferredTech      = mFX->GetTechniqueByName("Deferred");
	View              = mFX->GetVariableByName("gView")->AsMatrix();
	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	Proj              = mFX->GetVariableByName("gProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gDiffuseMapTrf")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FaceNormals       = mFX->GetVariableByName("gUseFaceNormals")->AsScalar();
	NormalMaps        = mFX->GetVariableByName("gUseNormalMaps")->AsScalar();
	FogEnabled        = mFX->GetVariableByName("gUseFog")->AsScalar();
	AlphaClipEnabled  = mFX->GetVariableByName("gUseAlphaClip")->AsScalar();
	TextureEnabled    = mFX->GetVariableByName("gUseTexture")->AsScalar();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights")->AsShaderResource();
	AmbientLight      = mFX->GetVariableByName("gAmbientLightColor")->AsVector();
	PointLights       = mFX->GetVariableByName("gPointLights")->AsShaderResource();
	SpotLights        = mFX->GetVariableByName("gSpotLights")->AsShaderResource();
	TextureSampler    = (ID3DX11EffectSamplerVariable*) mFX->GetVariableByName("gTextureSampler");
}
//-----------------------
BasicEffect::~BasicEffect(){}
//-----------------------
HRESULT BasicEffect::SetMaterial(Material* mat)
{
	HRESULT res = S_OK;
	for (u32 i = 0; i < mat->mPropertiesCount; ++i)
	{
		MaterialProperty& property = *(mat->mProperties[i]);
		switch (property.mType)
		{
		case MaterialPropertyType::Type_Int:
			{
				res = mFX->GetVariableBySemantic(property.mName.c_str())->AsScalar()->SetInt(*(reinterpret_cast<int*>(property.mData)));
				if (res != S_OK) return res;
				break;
			}
		case MaterialPropertyType::Type_Bool:
			{
				res = mFX->GetVariableBySemantic(property.mName.c_str())->AsScalar()->SetBool(*(reinterpret_cast<bool*>(property.mData)));
				if (res != S_OK) return res;
				break;
			}
		case MaterialPropertyType::Type_Float:
			{
				res = mFX->GetVariableBySemantic(property.mName.c_str())->AsScalar()->SetFloat(*(reinterpret_cast<float*>(property.mData)));
				if (res != S_OK) return res;
				break;
			}
		case MaterialPropertyType::Type_Vector:
			{
				res = mFX->GetVariableBySemantic(property.mName.c_str())->AsVector()->SetFloatVector(reinterpret_cast<const float*>(property.mData));
				if (res != S_OK) return res;
				break;
			}
		case MaterialPropertyType::Type_Matrix:
			{
				res = mFX->GetVariableBySemantic(property.mName.c_str())->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(property.mData));
				if (res != S_OK) return res;
				break;
			}
		case MaterialPropertyType::Type_Texture:
			{
				res = mFX->GetVariableBySemantic(property.mName.c_str())->AsShaderResource()->SetResource(property.mShaderResource.mTexture);
				if (res != S_OK) return res;
				std::string textureTrfName = property.mName + "_Trf";
				res = mFX->GetVariableBySemantic(textureTrfName.c_str())->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&property.mShaderResource.mTextureMatrix));
				if (res != S_OK) return res;
				break;
			}
		case MaterialPropertyType::Type_Cubemap:
			{
				// TODO: RJE could use a cubemap :)
				break;
			}
		default:
			break;
		}
	}
	return res;
}

//////////////////////////////////////////////////////////////////////////

PostProcessEffect::PostProcessEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	PostProcessTech       = mFX->GetTechniqueByName("PostProcess");
	PostProcessMSTech     = mFX->GetTechniqueByName("PostProcessMS");
	TextureMap            = mFX->GetVariableByName("gTexture")->AsShaderResource();
	TextureMapMS          = mFX->GetVariableByName("gTextureMS")->AsShaderResource();
	FrameBufferSizeX      = mFX->GetVariableByName("gTextureSizeX")->AsScalar();
	FrameBufferSizeY      = mFX->GetVariableByName("gTextureSizeY")->AsScalar();
}

PostProcessEffect::~PostProcessEffect(){}

//////////////////////////////////////////////////////////////////////////

TiledDeferredEffect::TiledDeferredEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	TiledDeferredTech     = mFX->GetTechniqueByName("TiledDeferred");
	GBuffer               = mFX->GetVariableByName("gGbuffer")->AsShaderResource();
	EyePosW               = mFX->GetVariableByName("gEyePosW")->AsVector();
	NearFar               = mFX->GetVariableByName("gNearFar")->AsVector();
	Proj                  = mFX->GetVariableByName("gProj")->AsMatrix();
	View                  = mFX->GetVariableByName("gView")->AsMatrix();
	ViewPerSamplerShading = mFX->GetVariableByName("gVisualizePerSampleShading")->AsScalar();
	ViewLightCount        = mFX->GetVariableByName("gVisualizeLightCount")->AsScalar();
	FogColor              = mFX->GetVariableByName("gFogColor")->AsVector();
	FogEnabled            = mFX->GetVariableByName("gUseFog")->AsScalar();
	TextureEnabled        = mFX->GetVariableByName("gUseTexture")->AsScalar();
	FogStart              = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange              = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights             = mFX->GetVariableByName("gDirLights")->AsShaderResource();
	AmbientLight          = mFX->GetVariableByName("gAmbientLightColor")->AsVector();
	PointLights           = mFX->GetVariableByName("gPointLights")->AsShaderResource();
	SpotLights            = mFX->GetVariableByName("gSpotLights")->AsShaderResource();
	FrameBufferSizeX      = mFX->GetVariableByName("gFramebufferSizeX")->AsScalar();
	FrameBufferSizeY      = mFX->GetVariableByName("gFramebufferSizeY")->AsScalar();
}

TiledDeferredEffect::~TiledDeferredEffect(){}

//////////////////////////////////////////////////////////////////////////

SpriteEffect::SpriteEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	SpriteTech = mFX->GetTechniqueByName("SpriteTech");
	SpriteMap  = mFX->GetVariableByName("gSpriteTex")->AsShaderResource();
}

SpriteEffect::~SpriteEffect(){}

//////////////////////////////////////////////////////////////////////////

ColorEffect::ColorEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	ColorTech		= mFX->GetTechniqueByName("ColorTech");
	Color			= mFX->GetVariableByName("gColor")->AsVector();
	World			= mFX->GetVariableByName("gWorld")->AsMatrix();
	ViewProj		= mFX->GetVariableByName("gViewProj")->AsMatrix();
}

ColorEffect::~ColorEffect(){}

//////////////////////////////////////////////////////////////////////////

SkyboxEffect::SkyboxEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	SkyboxForwardTech  = mFX->GetTechniqueByName("SkyboxForward");
	SkyboxDeferredTech = mFX->GetTechniqueByName("SkyboxDeferred");
	WorldViewProj      = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap            = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	Litbuffer          = mFX->GetVariableByName("gLitTexture")->AsShaderResource();
	GBuffer            = mFX->GetVariableByName("gGbuffer")->AsShaderResource();
	ViewPosition       = mFX->GetVariableByName("gVisualizePosition")->AsScalar();
	ViewAlbedo         = mFX->GetVariableByName("gVisualizeAlbedo")->AsScalar();
	ViewNormals        = mFX->GetVariableByName("gVisualizeNormals")->AsScalar();
	ViewDepth          = mFX->GetVariableByName("gVisualizeDepth")->AsScalar();
	ViewSpecular       = mFX->GetVariableByName("gVisualizeSpecular")->AsScalar();
	ViewLightCount     = mFX->GetVariableByName("gVisualizeLightCount")->AsScalar();
	FrameBufferSizeX   = mFX->GetVariableByName("gFramebufferSizeX")->AsScalar();
	FrameBufferSizeY   = mFX->GetVariableByName("gFramebufferSizeY")->AsScalar();
}

SkyboxEffect::~SkyboxEffect(){}

//////////////////////////////////////////////////////////////////////////

ShadowMapEffect::ShadowMapEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	ShadowMapTech        = mFX->GetTechniqueByName("ShadowMap");
	AccumShadowTech      = mFX->GetTechniqueByName("AccumulateShadow");
	WorldViewProj        = mFX->GetVariableByName("gWVP")->AsMatrix();
	EyePosW              = mFX->GetVariableByName("gEyePosW")->AsVector();
	Partitions           = mFX->GetVariableByName("gPartitions")->AsShaderResource();
	CurrentPartition     = mFX->GetVariableByName("gCurrentPartition")->AsScalar();
	GBuffer              = mFX->GetVariableByName("gGbuffer")->AsShaderResource();
	DirLights            = mFX->GetVariableByName("gDirLights")->AsShaderResource();
	AmbientLight         = mFX->GetVariableByName("gAmbientLightColor")->AsVector();
	ShadowArray          = mFX->GetVariableByName("gShadowTexture")->AsShaderResource();
	View                 = mFX->GetVariableByName("gView")->AsMatrix();
	ViewToLightProj      = mFX->GetVariableByName("gCameraViewToLightProj")->AsMatrix();
	ShadowStrength       = mFX->GetVariableByName("gShadowStrength")->AsScalar();
	PositiveExponents    = mFX->GetVariableByName("gPositiveExponent")->AsScalar();
	NegativeExponents    = mFX->GetVariableByName("gNegativeExponent")->AsScalar();
	UsePositiveExponents = mFX->GetVariableByName("gUsePositiveExponent")->AsScalar();
	UseNegativeExponents = mFX->GetVariableByName("gUseNegativeExponent")->AsScalar();
	ViewPartitions       = mFX->GetVariableByName("gVisualizePartitions")->AsScalar();
}

ShadowMapEffect::~ShadowMapEffect(){}

//////////////////////////////////////////////////////////////////////////

SDSMEffect::SDSMEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	ClearZBoundsTech            = mFX->GetTechniqueByName("ClearZBounds");
	ClearPartitionBoundsTech    = mFX->GetTechniqueByName("ClearPartitionBounds");
	ReduceBoundsTech            = mFX->GetTechniqueByName("ReduceBoundsFromGBuffer");
	ReduceZBoundsTech           = mFX->GetTechniqueByName("ReduceZBoundsFromGBuffer");
	ComputePartitionsTech       = mFX->GetTechniqueByName("ComputePartitionsFromZBounds");
	ComputeCustomPartitionsTech = mFX->GetTechniqueByName("ComputeCustomPartitions");
	//----
	LightSpaceBorder = mFX->GetVariableByName("mLightSpaceBorder")->AsVector();
	MaxScale         = mFX->GetVariableByName("mMaxScale")->AsVector();
	DilationFactor   = mFX->GetVariableByName("mDilationFactor")->AsScalar();
	ReduceTimeDim    = mFX->GetVariableByName("mReduceTileDim")->AsScalar();
	NearFar          = mFX->GetVariableByName("gNearFar")->AsVector();
	View             = mFX->GetVariableByName("gView")->AsMatrix();
	ViewToLightProj  = mFX->GetVariableByName("gCameraViewToLightProj")->AsMatrix();
	Partitions       = mFX->GetVariableByName("gPartitionsReadOnly")->AsShaderResource();
	PartitionsBounds = mFX->GetVariableByName("gPartitionBoundsReadOnly")->AsShaderResource();
}

SDSMEffect::~SDSMEffect(){}

//////////////////////////////////////////////////////////////////////////

EVSMBlurEffect::EVSMBlurEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	EVSMBlurTech     = mFX->GetTechniqueByName("EVSMBlur");
	FilterSize       = mFX->GetVariableByName("mFilterSize")->AsVector();
	CurrentPartition = mFX->GetVariableByName("mPartition")->AsScalar();
	Dimension        = mFX->GetVariableByName("mDimension")->AsScalar();
	InputTexture     = mFX->GetVariableByName("gInputTexture")->AsShaderResource();
	Partitions       = mFX->GetVariableByName("gPartitions")->AsShaderResource();
}

EVSMBlurEffect::~EVSMBlurEffect(){}

//////////////////////////////////////////////////////////////////////////

EVSMConvertEffect::EVSMConvertEffect(ID3D11Device* device, const std::string& filename)
	: Effect(device, filename)
{
	EVSMConvertTech   = mFX->GetTechniqueByName("EVSMConvert");
	ShadowMap         = mFX->GetVariableByName("gShadowDepthTextureMS")->AsShaderResource();
	Partitions        = mFX->GetVariableByName("gPartitions")->AsShaderResource();
	CurrentPartition  = mFX->GetVariableByName("gCurrentPartition")->AsScalar();
	PositiveExponents = mFX->GetVariableByName("gPositiveExponent")->AsScalar();
	NegativeExponents = mFX->GetVariableByName("gNegativeExponent")->AsScalar();

}

EVSMConvertEffect::~EVSMConvertEffect(){}

//////////////////////////////////////////////////////////////////////////

BasicEffect*           DX11Effects::BasicFX         = nullptr;
ColorEffect*           DX11Effects::ColorFX         = nullptr;
SpriteEffect*          DX11Effects::SpriteFX        = nullptr;
PostProcessEffect*     DX11Effects::PostProcessFX   = nullptr;
SkyboxEffect*          DX11Effects::SkyboxFX        = nullptr;
TiledDeferredEffect*   DX11Effects::TiledDeferredFX = nullptr;
ShadowMapEffect*       DX11Effects::ShadowMapFX     = nullptr;
SDSMEffect*            DX11Effects::SDSMFX          = nullptr;
EVSMBlurEffect*        DX11Effects::EVSMBlurFX      = nullptr;
EVSMConvertEffect*     DX11Effects::EVSMConvertFX   = nullptr;

//////////////////////////////////////////////////////////////////////////

void DX11Effects::InitAll(ID3D11Device* device)
{
	string shaderPath;

	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("basic",  "shaders", System::Instance()->mResourcesPath);
	BasicFX  = rje_new BasicEffect( device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("postprocess", "shaders", System::Instance()->mResourcesPath);
	PostProcessFX = rje_new PostProcessEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("sprite", "shaders", System::Instance()->mResourcesPath);
	SpriteFX = rje_new SpriteEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("color", "shaders", System::Instance()->mResourcesPath);
	ColorFX = rje_new ColorEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("skybox", "shaders", System::Instance()->mResourcesPath);
	SkyboxFX = rje_new SkyboxEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("tiled", "shaders", System::Instance()->mResourcesPath);
	TiledDeferredFX = rje_new TiledDeferredEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("shadowmap", "shaders", System::Instance()->mResourcesPath);
	ShadowMapFX = rje_new ShadowMapEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("sdsm", "shaders", System::Instance()->mResourcesPath);
	SDSMFX = rje_new SDSMEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("evsmblur", "shaders", System::Instance()->mResourcesPath);
	EVSMBlurFX = rje_new EVSMBlurEffect(device, shaderPath);
	//-------------
	shaderPath = System::Instance()->mDataPath + CIniFile::GetValue("evsmconvert", "shaders", System::Instance()->mResourcesPath);
	EVSMConvertFX = rje_new EVSMConvertEffect(device, shaderPath);
}

void DX11Effects::DestroyAll()
{
	RJE_SAFE_DELETE(BasicFX);
	RJE_SAFE_DELETE(SpriteFX);
	RJE_SAFE_DELETE(ColorFX);
	RJE_SAFE_DELETE(PostProcessFX);
	RJE_SAFE_DELETE(SkyboxFX);
	RJE_SAFE_DELETE(TiledDeferredFX);
	RJE_SAFE_DELETE(ShadowMapFX);
	RJE_SAFE_DELETE(SDSMFX);
	RJE_SAFE_DELETE(EVSMBlurFX);
	RJE_SAFE_DELETE(EVSMConvertFX);
}
