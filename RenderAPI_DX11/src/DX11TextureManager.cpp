#include "DX11TextureManager.h"
#include "../../RamJamEngine/include/System.h"

DX11TextureManager* DX11TextureManager::sInstance = nullptr;

//////////////////////////////////////////////////////////////////////////
void DX11TextureManager::Initialize(ID3D11Device* device)
{
	mDevice       = device;
	mTextureCount = 0;
}

//////////////////////////////////////////////////////////////////////////
void DX11TextureManager::ReleaseTextures()
{
	for ( auto it = mTextures.begin(); it != mTextures.end(); ++it )
	{
		RJE_SAFE_RELEASE(it->second);
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL DX11TextureManager::IsTextureLoaded( std::string shaderName )
{
	std::unordered_map<std::string,ID3D11ShaderResourceView*>::const_iterator texture = mTextures.find(shaderName);

	return (texture != mTextures.end());
}

//////////////////////////////////////////////////////////////////////////
void DX11TextureManager::LoadTexture(string texturePath, string textureName)
{	
	wstring texturePathW = StringToWString(texturePath);
	wstring textureExtension = texturePathW.substr(texturePath.find('.'));
	// lower the case so we can compare more easily
	std::transform(textureExtension.begin(), textureExtension.end(), textureExtension.begin(), ::tolower);

	TexMetadata  metadata;
	ScratchImage image;

	if (textureExtension.compare(L".png") == 0 || textureExtension.compare(L".bmp") == 0 || textureExtension.compare(L".gif") == 0 ||
		textureExtension.compare(L".jpg") == 0 || textureExtension.compare(L".jpeg") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromWICFile( texturePathW.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image ));
	}
	else if (textureExtension.compare(L".tga") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromTGAFile( texturePathW.c_str(), &metadata, image ));
	}
	else if  (textureExtension.compare(L".dds") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromDDSFile( texturePathW.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, &metadata, image ));
	}
	ID3D11ShaderResourceView* textureSRV = nullptr;
	RJE_CHECK_FOR_SUCCESS(CreateShaderResourceView( mDevice, image.GetImages(), image.GetImageCount(), metadata, &textureSRV ));
	mTextures[textureName] = textureSRV;
	++mTextureCount;
}

//////////////////////////////////////////////////////////////////////////
void DX11TextureManager::LoadTexture(string keyName, ID3D11ShaderResourceView** shaderResourceView)
{	
	wstring texturePath = StringToWString(System::Instance()->mDataPath) + CIniFile::GetValueW(keyName, "textures", System::Instance()->mResourcesPath);
	wstring textureExtension = texturePath.substr(texturePath.find('.'));
	// lower the case so we can compare more easily
	std::transform(textureExtension.begin(), textureExtension.end(), textureExtension.begin(), ::tolower);

	TexMetadata  metadata;
	ScratchImage image;

	if (textureExtension.compare(L".png") == 0 || textureExtension.compare(L".bmp") == 0 || textureExtension.compare(L".gif") == 0 ||
		textureExtension.compare(L".jpg") == 0 || textureExtension.compare(L".jpeg") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromWICFile( texturePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image ));
	}
	else if (textureExtension.compare(L".tga") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromTGAFile( texturePath.c_str(), &metadata, image ));
	}
	else if  (textureExtension.compare(L".dds") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromDDSFile( texturePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, &metadata, image ));
	}
	RJE_CHECK_FOR_SUCCESS(CreateShaderResourceView( mDevice, image.GetImages(), image.GetImageCount(), metadata, shaderResourceView ));
}

//////////////////////////////////////////////////////////////////////////
void DX11TextureManager::LoadTextureFromPath(string texturePath, ID3D11ShaderResourceView** shaderResourceView)
{	
	string textureExtension = texturePath.substr(texturePath.find('.'));
	// lower the case so we can compare more easily
	std::transform(textureExtension.begin(), textureExtension.end(), textureExtension.begin(), ::tolower);

	TexMetadata  metadata;
	ScratchImage image;

	if (textureExtension.compare(".png") == 0 || textureExtension.compare(".bmp") == 0 || textureExtension.compare(".gif") == 0 ||
		textureExtension.compare(".jpg") == 0 || textureExtension.compare(".jpeg") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromWICFile( StringToWString(texturePath).c_str(), WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image ));
	}
	else if (textureExtension.compare(".tga") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromTGAFile( StringToWString(texturePath).c_str(), &metadata, image ));
	}
	else if  (textureExtension.compare(".dds") == 0)
	{
		RJE_CHECK_FOR_SUCCESS(LoadFromDDSFile( StringToWString(texturePath).c_str(), DDS_FLAGS::DDS_FLAGS_NONE, &metadata, image ));
	}
	RJE_CHECK_FOR_SUCCESS(CreateShaderResourceView( mDevice, image.GetImages(), image.GetImageCount(), metadata, shaderResourceView ));
}

//////////////////////////////////////////////////////////////////////////
void DX11TextureManager::Create2DTextureFixedColor(i32 size, RJE_COLOR::Color color, std::string textureName)
{
	RJE_ASSERT(size >=1);
	RJE_ASSERT(!IsTextureLoaded(textureName));

	u32 textureSize = size * size;
	u8* texArray = rje_new u8[textureSize*4];

	for (u32 i=0; i<textureSize*4; i+=4)
	{	// Unsigned Normalized 8-bits values
		texArray[i+0] = color.GetRed();
		texArray[i+1] = color.GetGreen();
		texArray[i+2] = color.GetBlue();
		texArray[i+3] = color.GetAlpha();
	}

	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(D3D11_TEXTURE2D_DESC));
	tex2dDesc.Height    = size;
	tex2dDesc.Width     = size;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.Usage     = D3D11_USAGE_DEFAULT;
	tex2dDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2dDesc.SampleDesc.Count   = 1;
	tex2dDesc.SampleDesc.Quality = 0;
	tex2dDesc.CPUAccessFlags = 0;
	tex2dDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA tex2dInitData;
	ZeroMemory(&tex2dInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	tex2dInitData.pSysMem     = (void *)texArray;
	tex2dInitData.SysMemPitch = size * 4 * sizeof(u8);
	//tex2dInitData.SysMemSlicePitch = size * size * 4 * sizeof(u8);	// Not used since this is a 2d texture

	ID3D11Texture2D *tex2D               = nullptr;
	ID3D11ShaderResourceView* textureSRV = nullptr;
	RJE_CHECK_FOR_SUCCESS(mDevice->CreateTexture2D(&tex2dDesc, &tex2dInitData, &tex2D));
	RJE_CHECK_FOR_SUCCESS(mDevice->CreateShaderResourceView(tex2D, NULL, &textureSRV));
	RJE_SAFE_RELEASE(tex2D);

	delete[] texArray;

	mTextures[textureName] = textureSRV;
	++mTextureCount;
}
