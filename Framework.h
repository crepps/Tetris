#pragma once

#include "Include.h"
#include "Grid.h"

// Macro to release COM objects
#define SAFE_RELEASE(x) if(x){x->Release(); x = 0;}

class Framework
{
	// DirectX
	HWND				m_hWnd;					//Handle

	HINSTANCE			m_hInst;				//Instance

	D3DPRESENT_PARAMETERS m_D3Dpp;				//Presentation parameters

	bool				m_bVsync;				//Vertical sync

	IDirect3D9*			m_pD3DObject;			//Direct3D object

	IDirect3DDevice9*	m_pD3DDevice;			//Direct3D device

	D3DCAPS9			m_D3DCaps;				//Direct3D capabilities

	ID3DXFont*			m_pD3DFont[6];			//Fonts

	ID3DXSprite*		m_pD3DSprite;			//Sprite

	IDirect3DTexture9	*m_pTextures[3],		//Textures
						*m_pBackgroundTex[1],
						*m_pForegroundTex[2],
						*m_pPieceTex[7];

	D3DXVECTOR3			m_backgroundCenters[1];	//Texture centers

	D3DXIMAGE_INFO		m_imageInfo;			//Texture info

	D3DXMATRIX			m_scaleMat,				//Transformation matrices
						m_rotMat,
						m_transMat,
						m_worldMat;

	// FMOD
	FMOD_RESULT result;

	FMOD::System *system;

	FMOD::Sound *sound[6],
			*stream[3];

	FMOD::Channel* channel[2];

	enum SOUND
	{
		MOVE,
		ROTATE,
		DROP_1,
		DROP_2,
		LINE,
		GAME_OVER
	};

	// Animation
	struct Trans
	{
		float *currentValue,
			endValue,
			increment;

		Trans()
			:currentValue(NULL), endValue(0.0f), increment(0.0f) {}

		Trans(float* a_current, float a_end, float a_increment)
			:currentValue(a_current), endValue(a_end), increment(a_increment) {}
	};

	std::vector<Trans> m_vTransitions;

	// Game
	wchar_t m_acTextBuffer[128];

	RECT m_textRect;

	Input* m_pInput;

	Grid m_grid;

	vec2 m_cellPos;

	std::vector<int> m_vLines;

	D3DXVECTOR3 m_bgCenter;

	int m_nLevel,
		m_nLines,
		m_fScoreIncrement,
		m_nRawTime,
		m_nSeconds,
		m_nMenuSelect;

	bool* m_pbGameOver,
		m_bFlashing,
		m_bPause,
		m_bEndSound,
		m_abEndLines[18],
		m_abGridFill[3];

	float m_fScore,
		m_fAnimTimer,
		m_fBackRot,
		m_fBackRotRate,
		m_fBackRotBase,
		m_fScoreAlpha,
		m_fPauseAlpha,
		m_fPauseMenuAlpha,
		m_fLineTimer,
		m_fFlashTime,
		m_fEndTimer,
		*m_pfPreviewColor;

	std::wstring m_sTime;

public:
	Framework();

	void Init(HWND&, HINSTANCE&, Input*, bool);

	void Update(float);

	void Render();

	void Transform(float, float, float, float, float);

	void Transition(float*, float, float, float);

	void UpdateTime();

	bool KeyPress(int);

	void Restart();

	~Framework();

	void Shutdown();
};

/*

From level 0 to 8, a player has to clear 10 lines to reach the next level. From level 9 to 20, it's 20 lines to the next level.

*/