#pragma once

#include "Framework.h"


Framework::Framework()
{
	m_bVsync = false;

	m_pD3DObject = 0;

	m_pD3DDevice = 0;

	m_bgCenter = D3DXVECTOR3(1024.0f, 1024.0f, 0.0f);

	m_nLevel = m_nLines = m_nSeconds = 0;

	m_bFlashing = m_bPause = m_abGridFill[0] = m_abGridFill[1] = m_abGridFill[2] = false;

	m_fAnimTimer = m_fLineTimer = m_fBackRot = m_fPauseAlpha = m_fEndTimer = 0.0f;

	m_fBackRotBase = m_fBackRotRate = 0.02f;

	m_fScoreAlpha = 150.0f;

	m_fPauseTxtAlpha = 240.0f;

	m_fFlashTime = 1.0f;

	m_nScore = 0.0f;

	m_sTime = L"0:00";

	for (int i = 0; i < 18; ++i)
		m_abEndLines[i] = false;
}

void Framework::Init(HWND& hWnd, HINSTANCE& hInst, Input* input, bool bWindowed)
{
	m_hWnd = hWnd;

	m_hInst = hInst;

	m_pInput = input;

	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	ZeroMemory(&m_D3Dpp, sizeof(m_D3Dpp));

	m_D3Dpp.hDeviceWindow = hWnd;
	m_D3Dpp.Windowed = bWindowed;
	m_D3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_D3Dpp.EnableAutoDepthStencil = TRUE;
	m_D3Dpp.BackBufferCount = 1;
	m_D3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_D3Dpp.BackBufferHeight = height;
	m_D3Dpp.BackBufferWidth = width;
	m_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3Dpp.PresentationInterval = m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	m_D3Dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_D3Dpp.FullScreen_RefreshRateInHz = bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;
	m_D3Dpp.MultiSampleQuality = 0;
	m_D3Dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if (m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (GPU)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// If hardware vertex processing is on, check pure device support
	if (m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;
	}

	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// Which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// Device type to use, set to hardware rasterization
		hWnd,					// Handle to the focused window
		deviceBehaviorFlags,	// Behavior flags
		&m_D3Dpp,				// Presentation parameters
		&m_pD3DDevice);			// Returned device pointer

	//////////////////////////////////////////////////////////////////////////
	// Font Objects
	//////////////////////////////////////////////////////////////////////////

	AddFontResourceEx(L"Delicious-Roman.otf", FR_PRIVATE, 0);

	// Debug
	D3DXCreateFont(m_pD3DDevice, 15, 0, FW_BOLD, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_pD3DFont[0]);

	// Score
	D3DXCreateFont(m_pD3DDevice, 48, 0, FW_BOLD, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Fixedsys"), &m_pD3DFont[1]);

	// Game stat headers
	D3DXCreateFont(m_pD3DDevice, 18, 0, FW_BOLD, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"), &m_pD3DFont[2]);

	// Game stats
	D3DXCreateFont(m_pD3DDevice, 45, 0, FW_BOLD, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"), &m_pD3DFont[3]);

	// Pause menu
	D3DXCreateFont(m_pD3DDevice, 80, 0, FW_BOLD, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Intel Clear"), &m_pD3DFont[4]);

	//////////////////////////////////////////////////////////////////////////
	// Sprite Object and Textures
	//////////////////////////////////////////////////////////////////////////

	D3DXCreateSprite(m_pD3DDevice, &m_pD3DSprite);

	// Backgrounds
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Background_Stars.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pBackgroundTex[0]);
	
	// Foregrounds
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Foreground.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pForegroundTex[0]);

	// Black
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Black.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pForegroundTex[1]);

	// Pieces (preview)
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\LN.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[0]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\SQ.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[1]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\L.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[2]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\J.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[3]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\S.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[4]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Z.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[5]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\T.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pPieceTex[6]);

	// Other textures
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Block.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pTextures[0]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Block_2.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pTextures[1]);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Textures\\Tetorisu.png", 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255), &m_imageInfo, 0, &m_pTextures[2]);


	//////////////////////////////////////////////////////////////////////////
	// Initialize FMOD
	//////////////////////////////////////////////////////////////////////////

	/*result = FMOD::System_Create(&system);		// Create the main system object.

	if (result != FMOD_OK)
	{
		//printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	result = system->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.

	if (result != FMOD_OK)
	{
		//printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	system->createSound("Rotate.wav", FMOD_DEFAULT, 0, &sound[0]);

	channel[0]->setVolume(0.6f);

	system->createStream("example music.wav", FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE, 0, &stream);

	channel[1]->setVolume(0.2f);

	system->playSound(FMOD_CHANNEL_FREE, stream, false, &channel[1]);*/

	m_nRawTime = time(0);

	// Get pointer to 'game over' bool
	m_pbGameOver = m_grid.GetGameOver();

	// Spawn first piece
	m_grid.SpawnPiece();

	m_pfPreviewColor = m_grid.PreviewColor();
}

void Framework::Update(float dt)
{
	// Video device updates
	HRESULT hr;

	hr = m_pD3DDevice->TestCooperativeLevel();

	if (hr == D3DERR_DEVICENOTRESET)
	{
		m_pD3DDevice->Reset(&m_D3Dpp);
	}

	// Update input object
	m_pInput->Update();

	if (KeyPress(VK_ESCAPE) && !*m_pbGameOver)
	{
		m_bPause = m_bPause ? false : true;

		m_vTransitions.clear();

		m_fScoreAlpha = 150.0f;

		if (m_bPause)
		{
			Transition(&m_fPauseAlpha, 0.0f, 130.0f, 1000.0f);

			Transition(&m_fPauseTxtAlpha, 240.0, 50.0f, 1000.0f);
		}

		else
		{
			Transition(&m_fPauseAlpha, 130.0f, 0.0f, 1000.0f);

			Transition(&m_fPauseTxtAlpha, 50.0f, 240.0f, 1000.0f);

			*m_pfPreviewColor = 255.0f;
		}
	}

	// Game updates
	if (!*m_pbGameOver && !m_bPause)
	{
		// DEBUG
		if (KeyPress(VK_RETURN))
		{
			++m_nLevel;

			m_fBackRotRate = (float)((m_nLevel + 1.0f) * m_fBackRotBase);
		}

		// Update time elapsed
		UpdateTime();

		// Check for lines
		if (m_grid.GetLines().size())
			m_vLines = m_grid.GetLines();

		// Update lines
		if (m_vLines.size())
		{
			// Increment line timer with delta time
			m_fLineTimer += dt * 0.0075f;

			// Update background rotation speed based on level
			m_fBackRotRate = (float)m_nLevel * m_fBackRotBase;

			if ((int)m_fLineTimer % (int)m_fFlashTime == 0)
			{
				if ((int)m_fLineTimer % 2 == 0)
					m_bFlashing = true;

				else
				{
					m_bFlashing = false;

					if (m_fLineTimer >= 7.0f)
					{
						for (int i = 0; i < m_vLines.size(); ++i)
							m_grid.EmptyLine(m_vLines[i]);
					}
				}
			}

			if (m_fLineTimer > m_fFlashTime * 8.0f)
			{
				// Accumulate number of lines
				m_nLines += m_vLines.size();

				// Increment score based on number of lines and level
				switch (m_vLines.size())
				{
				case 1:
					m_nScoreIncrement = 40 * (m_nLevel + 1);

					break;

				case 2:
					m_nScoreIncrement = 100 * (m_nLevel + 1);

					break;

				case 3:
					m_nScoreIncrement = 300 * (m_nLevel + 1);

					break;

				case 4:
					m_nScoreIncrement = 1200 * (m_nLevel + 1);
				}

				Transition(&m_nScore, m_nScore, m_nScore + m_nScoreIncrement, 8000.0f);

				// Update level
				if (m_nLevel < m_nLines / 10)
					m_grid.IncreaseSpeed();

				m_nLevel = m_nLines / 10;

				m_fLineTimer = 0.0f;

				m_bFlashing = false;

				m_grid.DropLines();

				m_vLines.clear();
			}
		}

		// If no lines, handle input
		else
		{
			if (KeyPress(VK_LEFT) || KeyPress('A'))
				m_grid.MovePiece(-1, 0);

			else if (KeyPress(VK_RIGHT) || KeyPress('D'))
				m_grid.MovePiece(1, 0);

			if (KeyPress(VK_UP))
				m_grid.RotatePiece();

			if (KeyPress(VK_DOWN))
				m_grid.Accelerate(true);

			else if (!m_pInput->keyboard[VK_DOWN] && m_grid.Accelerated())
				m_grid.Accelerate(false);

			if (KeyPress(VK_SPACE))
				m_grid.Cheat();
		}

		// Interrupt soft drop on piece dropped?
		if (m_grid.PieceDropped())
			m_grid.Accelerate(false);

		// Update grid
		m_grid.Update(dt);
	}

	// Animation updates
	m_fAnimTimer += dt;

	if (m_grid.PieceDropped())
	{
		m_pfPreviewColor = m_grid.PreviewColor();
		
		Transition(&m_pfPreviewColor[0], 0.0f, 255.0f, 5000.0f);
	}

	if (m_fAnimTimer > 1.0f)	// Time-based updates
	{
		// Rotate background
		if(!*m_pbGameOver && !m_bPause)
			m_fBackRot = m_fBackRot >= 360.0f ? 0.0f : m_fBackRot + m_fBackRotRate;

		// Pulse score
		if (m_fScoreAlpha == 150.0f)
			Transition(&m_fScoreAlpha, 151.0f, 220.0f, 5000.0f);

		if (m_fScoreAlpha == 220.0f)
			Transition(&m_fScoreAlpha, 219.0f, 150.0f, 5000.0f);

		// Update transitions
		if (m_vTransitions.size())
		{
			for (int i = 0; i < m_vTransitions.size(); ++i)
			{
				if ((m_vTransitions[i].increment > 0 && *m_vTransitions[i].currentValue >= m_vTransitions[i].endValue)
					|| (m_vTransitions[i].increment < 0 && *m_vTransitions[i].currentValue <= m_vTransitions[i].endValue))
				{
					*m_vTransitions[i].currentValue = m_vTransitions[i].endValue;

					m_vTransitions.erase(m_vTransitions.begin() + i);
				}

				else
				{
					*m_vTransitions[i].currentValue += m_vTransitions[i].increment;
				}
			}
		}

		// Reset animation timer
		m_fAnimTimer = 0.0f;
	}

	// Game over animation
	if (*m_pbGameOver)
	{
		m_fEndTimer += dt;

		if (!m_abGridFill[0] || m_abGridFill[1])
		{
			if (m_fEndTimer >= 25.0f)
			{
				for (int i = 17; i >= 0; --i)
				{
					if (m_abEndLines[i] == !m_abGridFill[0] ? false : true)
					{
						m_abEndLines[i] = !m_abGridFill[0] ? true : false;

						if (!i)
							m_abGridFill[0] = true;

						break;
					}
				}

				m_fEndTimer = 0;
			}
		}

		else
		{
			if (m_fEndTimer > 800.0f)
			{
				m_grid.ClearGrid();

				m_abGridFill[1] = true;

				m_fEndTimer = 0;
			}
		}
	}
}

void Framework::Render()
{
	if (!m_pD3DDevice)
		return;


	if (SUCCEEDED(m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0)))
	{
		if (SUCCEEDED(m_pD3DDevice->BeginScene()))
		{
			if (SUCCEEDED(m_pD3DSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK)))
			{
				//////////////////////////////////////////////////////////////////////////
				// Sprites
				//////////////////////////////////////////////////////////////////////////

				// Layer 0
				Transform(1.1f, 1.1f, m_fBackRot, 955, 550);

				m_pD3DSprite->Draw(m_pBackgroundTex[0], 0, &D3DXVECTOR3(1024.0f, 1024.0f, 0.0f),
					0, D3DCOLOR_ARGB(250, 255, 255, 255));
				
				// Layer 1
				Transform(0.938f, 0.528f, 0.0f, 0.0f, 0.0f);

				m_pD3DSprite->Draw(m_pForegroundTex[0], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					0, D3DCOLOR_ARGB(220, 255, 255, 255));

				// Preview piece
				Transform(0.58f, 0.58f, 0.0f, 1316.0f, 100.0f);

				m_pD3DSprite->Draw(m_pPieceTex[m_grid.PreviewType()], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					0, D3DCOLOR_ARGB((int)(m_pfPreviewColor[0]), (int)(m_pfPreviewColor[1] * 250), (int)(m_pfPreviewColor[2] * 250), (int)(m_pfPreviewColor[3] * 250)));

				// TETORISU
				Transform(0.3f, 0.6f, 0.0f, 1367.0f, 700.0f);

				m_pD3DSprite->Draw(m_pTextures[2], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					0, D3DCOLOR_ARGB(220, 250, 250, 250));


				// <--


				// Single cell
				m_cellPos = vec2(710.0f, 90.0f);

				for (int j = 0; j < 18; ++j)
				{
					for (int i = 0; i < 10; ++i)
					{
						if (m_grid.Drawn(i, j) || *m_pbGameOver)
						{
							Transform(0.78f, 0.78f, 0.0f, m_cellPos.x, m_cellPos.y);

							if (m_bFlashing && std::find(m_vLines.begin(), m_vLines.end(), j) != m_vLines.end())
							{
								m_pD3DSprite->Draw(m_pTextures[0], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
									0, D3DCOLOR_ARGB(255, 255, 255, 255));
							}

							else
							{
								if (m_abEndLines[j])
								{
									m_pD3DSprite->Draw(m_pTextures[1], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
										0, D3DCOLOR_ARGB(240, 150, 150, 150));
								}

								else if(m_grid.Drawn(i, j))
								{
									m_pD3DSprite->Draw(m_pTextures[0], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
										0, m_grid.GetCell(i, j).color);
								}
							}
						}

						m_cellPos.x += 50.0f;
					}

					m_cellPos.x = 710.0f;
					m_cellPos.y += 50.0f;
				}

				// Layer 2
				if (m_bPause)
				{
					Transform(0.938f, 0.528f, 0.0f, 0.0f, 0.0f);

					m_pD3DSprite->Draw(m_pForegroundTex[1], 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
						0, D3DCOLOR_ARGB((int)m_fPauseAlpha, 255, 255, 255));
				}

				m_pD3DSprite->End();
			}

			else
				PostQuitMessage(0);


			//////////////////////////////////////////////////////////////////////////
			// Text
			//////////////////////////////////////////////////////////////////////////

			GetWindowRect(m_hWnd, &m_textRect);

			m_textRect.right = m_textRect.right - m_textRect.left;
			m_textRect.bottom = m_textRect.bottom - m_textRect.top;

			// Score
			m_textRect.top = 138;
			m_textRect.left = 288;

			swprintf_s(m_acTextBuffer, 64, L"%0*i", 10, (int)m_nScore);

			m_pD3DFont[1]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(!m_bPause ? (int)m_fScoreAlpha : (int)m_fPauseTxtAlpha, 200, 200, 200));

			// Game stat headers
			m_textRect.left = -1084;
			m_textRect.top = 415;

			swprintf_s(m_acTextBuffer, 64, L"%s", L"LEVEL");

			m_pD3DFont[2]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB((int)m_fPauseTxtAlpha, 100, 100, 100));

			m_textRect.top += 97;

			swprintf_s(m_acTextBuffer, 64, L"%s", L"LINES");

			m_pD3DFont[2]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB((int)m_fPauseTxtAlpha, 100, 100, 100));

			m_textRect.top += 95;

			swprintf_s(m_acTextBuffer, 64, L"%s", L"ELAPSED");

			m_pD3DFont[2]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB((int)m_fPauseTxtAlpha, 100, 100, 100));

			// Game stats
			m_textRect.top -= 175;

			swprintf_s(m_acTextBuffer, 64, L"%i", m_nLevel);

			m_pD3DFont[3]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB((int)m_fPauseTxtAlpha, 140, 140, 140));

			m_textRect.top += 97;

			swprintf_s(m_acTextBuffer, 64, L"%i", m_nLines);

			m_pD3DFont[3]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB((int)m_fPauseTxtAlpha, 140, 140, 140));

			m_textRect.top += 95;

			swprintf_s(m_acTextBuffer, 64, L"%s", m_sTime.c_str());

			m_pD3DFont[3]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB((int)m_fPauseTxtAlpha, 140, 140, 140));

			// Pause menu
			m_textRect.left = 0;
			m_textRect.top = 500;

			swprintf_s(m_acTextBuffer, 64, L"%s", L"PAUSED");

			m_pD3DFont[4]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB(240 - (int)m_fPauseTxtAlpha, 250, 250, 250));

			// Debug
			m_textRect.top = 50;
			m_textRect.left = 50;

			swprintf_s(m_acTextBuffer, 64, L"Transition vector size: %i", m_vTransitions.size());

			m_pD3DFont[0]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(240, 200, 200, 200));

			m_textRect.top += 25;

			swprintf_s(m_acTextBuffer, 64, L"Score Alpha: %i", (int)m_fScoreAlpha);

			m_pD3DFont[0]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(240, 200, 200, 200));

			m_textRect.top += 25;

			swprintf_s(m_acTextBuffer, 64, L"Background Rotation Rate: %.4f", m_fBackRotRate);

			m_pD3DFont[0]->DrawText(0, m_acTextBuffer, -1, &m_textRect, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(240, 200, 200, 200));

			m_pD3DDevice->EndScene();
		}

		m_pD3DDevice->Present(0, 0, 0, 0);
	}
}

void Framework::Transform(float a_scaleX, float a_scaleY, float a_rot, float a_transX, float a_transY)
{
	D3DXMatrixIdentity(&m_worldMat);

	D3DXMatrixScaling(&m_scaleMat, a_scaleX, a_scaleY, 1.0f);
	D3DXMatrixRotationZ(&m_rotMat, D3DXToRadian(a_rot));
	D3DXMatrixTranslation(&m_transMat, a_transX, a_transY, 0.0f);

	D3DXMatrixMultiply(&m_worldMat, &m_worldMat, &m_scaleMat);
	D3DXMatrixMultiply(&m_worldMat, &m_worldMat, &m_rotMat);
	D3DXMatrixMultiply(&m_worldMat, &m_worldMat, &m_transMat);

	m_pD3DSprite->SetTransform(&m_worldMat);
}

void Framework::Transition(float* a_value, float a_begin, float a_end, float a_duration)
{
	float delta = a_end - a_begin,
		
		increment = delta / (a_duration / 16);

	m_vTransitions.push_back(Trans(a_value, a_end, increment));
}

void Framework::UpdateTime()
{
	if (m_nRawTime != time(0))
	{
		std::wstringstream ss;

		m_nRawTime = time(0);

		++m_nSeconds;

		if (m_nSeconds > 60)
			ss << m_nSeconds / 60 << ":";

		else
			ss << "0:";

		ss << std::setw(2) << std::setfill(wchar_t('0')) << (m_nSeconds > 60 ? m_nSeconds % 60 : m_nSeconds);

		m_sTime = ss.str();
	}
}

bool Framework::KeyPress(int arg)
{
	if(m_pInput->keyboard[arg] && !m_pInput->keyDown[arg])
	{
		m_pInput->keyDown[arg] = true;

		return true;
	}

	return false;
}

Framework::~Framework()
{
	Shutdown();
}

void Framework::Shutdown()
{
	SAFE_RELEASE(m_pD3DSprite);

	for (int i = 0; i < 1; ++i)
		SAFE_RELEASE(m_pD3DFont[i]);

	SAFE_RELEASE(m_pD3DDevice);

	SAFE_RELEASE(m_pD3DObject);
}