#pragma once

#include "Include.h"

#define GRID_WIDTH 10
#define GRID_HEIGHT 18

class Grid
{
public:	// DEBUG
	struct Cell
	{
		enum TYPE
		{
			LN, SQ,
			L, J,
			S, Z,
			T, EM

		} type;

		D3DXCOLOR color;

		Cell()
		:type(TYPE::EM), color(D3DCOLOR_ARGB(255, 255, 255, 255)) {}

		Cell(TYPE arg)
			:type(arg), color(D3DCOLOR_ARGB(255, 255, 255, 255)) {}

		bool operator!=(const Cell &arg)
		{
			return type != arg.type ? true : false;
		}
	};

	struct Piece
	{
		Cell::TYPE type;

		vec2 coords[4];

		bool surface[4];

		int rotation;

		D3DXCOLOR color;

	} m_activePiece, m_preview, m_pieceBuffer;

	Cell m_aCells[GRID_WIDTH][GRID_HEIGHT];

	float m_fTick,
		m_fTickRate,
		m_fElapsed;

	bool m_bPieceDropped,
		m_bAccelerate,
		m_bLine,
		m_bGameOver;

	std::vector<int> m_vLines;

	int m_nLineCount;

	float color[4];

public:
	Grid();

	bool* GetGameOver();

	void Update(float);

	int PreviewType();

	float* PreviewColor();

	void SpawnPiece();

	bool PieceDropped();

	void MovePiece(int, int);

	bool CheckBoundaries(Piece);

	void ClearPiece();

	Cell GetCell(int, int);

	bool Drawn(int, int);

	bool Contact();

	void RotatePiece();

	void Accelerate(bool);

	bool Accelerated();

	std::vector<int> GetLines();

	void ClearLines();

	void EmptyLine(int);

	void DropLines();

	void IncreaseSpeed();

	void ClearGrid();

	void Reset();

	void Cheat();
};