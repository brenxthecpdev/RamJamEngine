#pragma once

#include "Types.h"
#include "SceneLoader.h"
#include "Transform.h"
#include "Color.h"
#include "GameObject.h"

using namespace RJE_COLOR;

//////////////////////////////////////////////////////////////////////////
struct Scene
{
	SceneLoader				mSceneLoader;
	std::vector<unique_ptr<GameObject>> mGameObjects;
	GameObject*							mEditorGameobject;

	string mSkyboxName;

	//-----------------------------------------------
	// Gameobject Editor Settings
	u32				mCurrentEditorGOIdx, mCurrentEditorGOIdxUI;
	Transform*		mGameObjectEditorTransform;
	std::string		mGameObjectEditorName;
	Vector3			mGameObjectEditorPos;
	Vector3			mGameObjectEditorScale;
	TwQuaternion	mGameObjectEditorRot;
	Color			mGameObjectEditorColor;
	BOOL			mbEnableGizmo;
	//-----------------------------------------------
	// Scene parameters
	BOOL	mbDeferredRendering;
	Vector4	mAmbientLightColor;
	Vector4	mFogColor;
	float	mFogStart;
	float	mFogRange;
	BOOL	mbUseTexture;
	BOOL	mbUseBlending;
	BOOL	mbUseFog;
	BOOL	mbWireframe;
	//---------
	BOOL	mbDrawLightSphere;
	BOOL	mbAnimateLights;
	BOOL	mbDrawSun;
	BOOL	mbAnimateSun;
	float	mSunHeight;
	//---------
	BOOL	mbUseFaceNormals;
	BOOL	mbUseNormalMaps;
	BOOL	mbOnlyPosition;
	BOOL	mbOnlyAlbedo;
	BOOL	mbOnlyNormals;
	BOOL	mbOnlyDepth;
	BOOL	mbOnlySpecular;
	BOOL	mbViewPerSampleShading;
	BOOL	mbViewLightCount;
	//---------
	BOOL	mbDisplayShadows;
	BOOL	mbAlignLightToFrustum;
	BOOL	mbViewLightSpace;
	BOOL	mbViewPartitions;
	BOOL	mbEdgeSoftening;
	float	mEdgeSofteningAmount;
	float	mMaxEdgeSofteningFilter;
	//---------
	float	mShadowStrength;
	BOOL	mbUsePositiveExponent;
	BOOL	mbUseNegativeExponent;
	float	mPositiveExponent;
	float	mNegativeExponent;
	//---------
	// Scene Bounding Volume Info
	Vector3 mSceneCenter;
	Vector3 mSceneExtents;
	float   mSceneRadius;
	//---------
	// Point Light Editor values
	float mPointLightRadius;
	float mPointLightHeight;
	//-----------------------------------------------
	Scene();
	~Scene();
	//---------
	void Init();
	void LoadFromFile(const char* pFile);
	//---------
	void ChangeCurrentEditorGO(u32& idx);
	void ComputeSceneExtents();
	//---------
	void Update();
};
