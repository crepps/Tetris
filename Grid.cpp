#include "Grid.h"

Grid::Grid()
{
	// Initialize grid to empty
	for (int j = 0; j < GRID_HEIGHT; ++j)
	{
		for (int i = 0; i < GRID_WIDTH; ++i)
		{
			m_aCells[i][j] = Cell(Cell::TYPE::EM);
		}
	}

	m_fTick = m_fTickRate = 850.0f;

	m_fElapsed = 0.0f;

	m_bPieceDropped = m_bAccelerate = m_bLine = m_bGameOver = false;

	srand(time(0));

	m_nLineCount = 0;

	m_preview.type = Cell::TYPE::EM;

	// Initialize preview piece
	SpawnPiece();
}

bool* Grid::GetGameOver()
{
	return &m_bGameOver;
}

void Grid::Update(float a_dt)
{
	m_fElapsed += a_dt;

	// Update grid every tick
	if (m_fElapsed > m_fTick && !m_bGameOver)
	{
		// If no contact
		if (!Contact())
		{
			// Update piece's coordinates
			MovePiece(0, 1);
		}

		// If contact
		else
		{
			// Check for lines
			for (int j = 0; j < 18; ++j)
			{
				for (int i = 0; i < 10; ++i)
				{
					if (m_aCells[i][j].type == Cell::TYPE::EM)
						break;

					else if (i == 9 && std::find(m_vLines.begin(), m_vLines.end(), j) == m_vLines.end())
					{
						m_vLines.push_back(j);

						m_bLine = true;
					}
				}
			}

			// Spawn new piece
			if(!m_vLines.size())
				SpawnPiece();
		}

		m_fElapsed = 0.0f;
	}

	if(!Contact())
		m_bLine = false;

	// Update grid with active piece
	if (!m_bLine)
	{
		for (int i = 0; i < 4; ++i)
		{
			m_aCells[(int)m_activePiece.coords[i].x][(int)m_activePiece.coords[i].y].type = m_activePiece.type;

			m_aCells[(int)m_activePiece.coords[i].x][(int)m_activePiece.coords[i].y].color = m_activePiece.color;
		}
	}

	// Soft drop
	m_fTick = m_bAccelerate ? 50.0f : m_fTickRate;
}

int Grid::PreviewType()
{
	return (int)m_preview.type;
}

float* Grid::PreviewColor()
{
	color[0] = m_preview.color.a;
	color[1] = m_preview.color.r;
	color[2] = m_preview.color.g;
	color[3] = m_preview.color.b;

	return color;
}

void Grid::SpawnPiece()
{
	if(m_preview.type != Cell::TYPE::EM)
		m_activePiece = m_preview;				

	m_preview.type = Cell::TYPE(rand() % 7);

	m_preview.rotation = 0;

	m_preview.coords[0] = vec2(4, 1);

	switch (m_preview.type)
	{
	case Cell::TYPE::LN:
		m_preview.coords[1] = vec2(3, 1);
		m_preview.coords[2] = vec2(5, 1);
		m_preview.coords[3] = vec2(6, 1);

		m_preview.surface[0] = true;
		m_preview.surface[1] = true;
		m_preview.surface[2] = true;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 0, 240, 240);

		break;

	case Cell::TYPE::SQ:
		m_preview.coords[1] = vec2(5, 1);
		m_preview.coords[2] = vec2(4, 2);
		m_preview.coords[3] = vec2(5, 2);

		m_preview.surface[0] = false;
		m_preview.surface[1] = false;
		m_preview.surface[2] = true;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 240, 240, 0);

		break;

	case Cell::TYPE::L:
		m_preview.coords[1] = vec2(5, 1);
		m_preview.coords[2] = vec2(3, 1);
		m_preview.coords[3] = vec2(3, 2);

		m_preview.surface[0] = true;
		m_preview.surface[1] = true;
		m_preview.surface[2] = false;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 255, 191, 0);

		break;

	case Cell::TYPE::J:
		m_preview.coords[1] = vec2(3, 1);
		m_preview.coords[2] = vec2(5, 1);
		m_preview.coords[3] = vec2(5, 2);

		m_preview.surface[0] = true;
		m_preview.surface[1] = true;
		m_preview.surface[2] = false;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 0, 0, 255);

		break;

	case Cell::TYPE::S:
		m_preview.coords[1] = vec2(5, 1);
		m_preview.coords[2] = vec2(4, 2);
		m_preview.coords[3] = vec2(3, 2);

		m_preview.surface[0] = false;
		m_preview.surface[1] = true;
		m_preview.surface[2] = true;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 0, 240, 0);

		break;

	case Cell::TYPE::Z:
		m_preview.coords[0] = vec2(5, 1);
		m_preview.coords[1] = vec2(4, 1);
		m_preview.coords[2] = vec2(5, 2);
		m_preview.coords[3] = vec2(6, 2);

		m_preview.surface[0] = false;
		m_preview.surface[1] = true;
		m_preview.surface[2] = true;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 240, 0, 0);

		break;

	case Cell::TYPE::T:
		m_preview.coords[1] = vec2(3, 1);
		m_preview.coords[2] = vec2(5, 1);
		m_preview.coords[3] = vec2(4, 2);

		m_preview.surface[0] = false;
		m_preview.surface[1] = true;
		m_preview.surface[2] = true;
		m_preview.surface[3] = true;

		m_preview.color = D3DCOLOR_ARGB(255, 240, 0, 240);

		break;
	}

	m_bPieceDropped = true;

	for (int i = 0; i < 4; ++i)
	{
		if (m_aCells[(int)m_preview.coords[i].x][(int)m_preview.coords[i].y].type != Cell::TYPE::EM)
		{
			m_bGameOver = true;

			//m_bPieceDropped = false;

			return;
		}
	}
}

bool Grid::PieceDropped()
{
	if (m_bPieceDropped)
	{
		m_bPieceDropped = false;

		return true;
	}

	return false;
}

void Grid::MovePiece(int x, int y)
{
	m_pieceBuffer = m_activePiece;

	for (int i = 0; i < 4; ++i)
		m_pieceBuffer.coords[i] += vec2(x, y);

	if (CheckBoundaries(m_pieceBuffer))
	{
		ClearPiece();

		m_activePiece = m_pieceBuffer;
	}
}

bool Grid::CheckBoundaries(Piece arg)
{
	ClearPiece();

	for (int i = 0; i < 4; ++i)
	{
		if (arg.coords[i].x < 0 || arg.coords[i].x > 9
			|| arg.coords[i].y > 17)
			return false;

		if (m_aCells[(int)arg.coords[i].x][(int)arg.coords[i].y] != Cell::TYPE::EM)
			return false;
	}

	return true;
}

void Grid::ClearPiece()
{
	for (int i = 0; i < 4; ++i)
		m_aCells[(int)m_activePiece.coords[i].x][(int)m_activePiece.coords[i].y] = Cell::TYPE::EM;
}

bool Grid::Contact()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_activePiece.surface[i]
			&& (m_activePiece.coords[i].y >= 17 || m_aCells[(int)m_activePiece.coords[i].x][(int)m_activePiece.coords[i].y + 1].type != Cell::TYPE::EM))
			return true;
	}

	return false;
}

Grid::Cell Grid::GetCell(int i, int j)
{
	return m_aCells[i][j];
}

bool Grid::Drawn(int i, int j)
{
	return m_aCells[i][j].type != Cell::TYPE::EM ? true : false;
}

void Grid::RotatePiece()
{
	m_pieceBuffer = m_activePiece;

	switch (m_activePiece.type)
	{
	case Cell::TYPE::LN:
		m_pieceBuffer.coords[1] += !(m_pieceBuffer.rotation % 2) ? vec2(1, -1) : vec2(-1, 1);
		m_pieceBuffer.coords[2] += !(m_pieceBuffer.rotation % 2) ? vec2(-1, 1) : vec2(1, -1);
		m_pieceBuffer.coords[3] += !(m_pieceBuffer.rotation % 2) ? vec2(-2, 2) : vec2(2, -2);

		m_pieceBuffer.surface[0] = false;
		m_pieceBuffer.surface[1] = false;
		m_pieceBuffer.surface[2] = false;
		m_pieceBuffer.surface[3] = true;

		break;

	case Cell::TYPE::SQ:
		break;

	case Cell::TYPE::L:
		if (m_activePiece.rotation == 0)
		{
			m_pieceBuffer.coords[1] += vec2(-1, 1);
			m_pieceBuffer.coords[2] += vec2(1, -1);
			m_pieceBuffer.coords[3] += vec2(0, -2);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = false;
			m_pieceBuffer.surface[3] = true;
		}

		else if (m_activePiece.rotation == 1)
		{
			m_pieceBuffer.coords[1] += vec2(-1, -1);
			m_pieceBuffer.coords[2] += vec2(1, 1);
			m_pieceBuffer.coords[3] += vec2(2, 0);

			m_pieceBuffer.surface[0] = true;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = false;
		}

		else if (m_activePiece.rotation == 2)
		{
			m_pieceBuffer.coords[1] += vec2(1, -1);
			m_pieceBuffer.coords[2] += vec2(-1, 1);
			m_pieceBuffer.coords[3] += vec2(0, 2);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = false;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = true;
		}

		else if (m_activePiece.rotation == 3)
		{
			m_pieceBuffer.coords[1] += vec2(1, 1);
			m_pieceBuffer.coords[2] += vec2(-1, -1);
			m_pieceBuffer.coords[3] += vec2(-2, 0);

			m_pieceBuffer.surface[0] = true;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = false;
			m_pieceBuffer.surface[3] = true;
		}

		break;

	case Cell::TYPE::J:
		if (m_activePiece.rotation == 0)
		{
			m_pieceBuffer.coords[1] += vec2(1, -1);
			m_pieceBuffer.coords[2] += vec2(-1, 1);
			m_pieceBuffer.coords[3] += vec2(-2, 0);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = false;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = true;
		}

		else if (m_activePiece.rotation == 1)
		{
			m_pieceBuffer.coords[1] += vec2(1, 1);
			m_pieceBuffer.coords[2] += vec2(-1, -1);
			m_pieceBuffer.coords[3] += vec2(0, -2);

			m_pieceBuffer.surface[0] = true;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = false;
		}

		else if (m_activePiece.rotation == 2)
		{
			m_pieceBuffer.coords[1] += vec2(-1, 1);
			m_pieceBuffer.coords[2] += vec2(1, -1);
			m_pieceBuffer.coords[3] += vec2(2, 0);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = false;
			m_pieceBuffer.surface[3] = true;
		}

		else if (m_activePiece.rotation == 3)
		{
			m_pieceBuffer.coords[1] += vec2(-1, -1);
			m_pieceBuffer.coords[2] += vec2(1, 1);
			m_pieceBuffer.coords[3] += vec2(0, 2);

			m_pieceBuffer.surface[0] = true;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = false;
			m_pieceBuffer.surface[3] = true;
		}

		break;

	case Cell::TYPE::S:
		if (m_activePiece.rotation == 0
			|| m_activePiece.rotation == 2)
		{
			m_pieceBuffer.coords[1] += vec2(-1, 1);
			m_pieceBuffer.coords[2] += vec2(-1, -1);
			m_pieceBuffer.coords[3] += vec2(0, -2);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = false;
		}

		else if (m_activePiece.rotation == 1
			|| m_activePiece.rotation == 3)
		{
			m_pieceBuffer.coords[1] += vec2(1, -1);
			m_pieceBuffer.coords[2] += vec2(1, 1);
			m_pieceBuffer.coords[3] += vec2(0, 2);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = true;
		}

		break;

	case Cell::TYPE::Z:
		if (m_activePiece.rotation == 0
			|| m_activePiece.rotation == 2)
		{
			m_pieceBuffer.coords[0] += vec2(-1, 0);
			m_pieceBuffer.coords[1] += vec2(0, 1);
			m_pieceBuffer.coords[2] += vec2(0, -1);
			m_pieceBuffer.coords[3] += vec2(-1, -2);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = false;
		}

		else if (m_activePiece.rotation == 1
			|| m_activePiece.rotation == 3)
		{
			m_pieceBuffer.coords[0] += vec2(1, 0);
			m_pieceBuffer.coords[1] += vec2(0, -1);
			m_pieceBuffer.coords[2] += vec2(0, 1);
			m_pieceBuffer.coords[3] += vec2(1, 2);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = true;
		}

		break;

	case Cell::TYPE::T:
		if (m_activePiece.rotation == 0)
		{
			m_pieceBuffer.coords[1] += vec2(1, -1);
			m_pieceBuffer.coords[2] += vec2(-1, 1);
			m_pieceBuffer.coords[3] += vec2(-1, -1);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = false;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = true;
		}

		else if (m_activePiece.rotation == 1)
		{
			m_pieceBuffer.coords[1] += vec2(1, 1);
			m_pieceBuffer.coords[2] += vec2(-1, -1);
			m_pieceBuffer.coords[3] += vec2(1, -1);

			m_pieceBuffer.surface[0] = true;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = false;
		}

		else if (m_activePiece.rotation == 2)
		{
			m_pieceBuffer.coords[1] += vec2(-1, 1);
			m_pieceBuffer.coords[2] += vec2(1, -1);
			m_pieceBuffer.coords[3] += vec2(1, 1);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = false;
			m_pieceBuffer.surface[3] = true;
		}

		else if (m_activePiece.rotation == 3)
		{
			m_pieceBuffer.coords[1] += vec2(-1, -1);
			m_pieceBuffer.coords[2] += vec2(1, 1);
			m_pieceBuffer.coords[3] += vec2(-1, 1);

			m_pieceBuffer.surface[0] = false;
			m_pieceBuffer.surface[1] = true;
			m_pieceBuffer.surface[2] = true;
			m_pieceBuffer.surface[3] = true;
		}

	}

	if (!CheckBoundaries(m_pieceBuffer))
		return;

	ClearPiece();

	m_activePiece = m_pieceBuffer;

	m_activePiece.rotation = m_activePiece.rotation >= 3 ? 0 : m_activePiece.rotation += 1;
}

void Grid::Accelerate(bool arg)
{
	m_bAccelerate = arg;
}

bool Grid::Accelerated()
{
	return m_bAccelerate;
}

std::vector<int> Grid::GetLines()
{
	return m_vLines;
}

void Grid::ClearLines()
{
	m_vLines.clear();
}

void Grid::EmptyLine(int arg)
{
	for (int i = 0; i < 10; ++i)
	{
		m_aCells[i][arg].type = Cell::TYPE::EM;

		m_aCells[i][arg].color = D3DCOLOR_ARGB(0, 255, 255, 255);
	}
}

void Grid::DropLines()
{
	if (!m_vLines.size())
		return;

	m_nLineCount = m_vLines.size();

	for (int k = 0; k < m_vLines.size(); ++k)
	{
		for (int j = m_vLines[k]; j >= 0; --j)
		{
			for (int i = 0; i < 10; ++i)
			{
				m_aCells[i][j].type = j ? m_aCells[i][j - 1].type : Cell::TYPE::EM;

				m_aCells[i][j].color = j ? m_aCells[i][j - 1].color : m_aCells[i][j].color;
			}
		}
	}

	m_vLines.clear();
}

void Grid::IncreaseSpeed()
{
	m_fTickRate -= m_fTickRate > 100 ? 75 : 0;

	if (m_fTickRate <= 100 && m_fTickRate >= 30)
		m_fTickRate -= 10;
}

void Grid::ClearGrid()
{
	for (int j = 0; j < 18; ++j)
	{
		for (int i = 0; i < 10; ++i)
			m_aCells[i][j].type = Cell::TYPE::EM;
	}
}

void Grid::Cheat()
{
	m_preview.type = Cell::TYPE::LN;

	m_preview.rotation = 0;

	m_preview.coords[0] = vec2(4, 1);
	m_preview.coords[1] = vec2(3, 1);
	m_preview.coords[2] = vec2(5, 1);
	m_preview.coords[3] = vec2(6, 1);

	m_preview.surface[0] = true;
	m_preview.surface[1] = true;
	m_preview.surface[2] = true;
	m_preview.surface[3] = true;

	m_preview.color = D3DCOLOR_ARGB(240, 0, 220, 220);
}