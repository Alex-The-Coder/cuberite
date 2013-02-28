
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "FireSimulator.h"
#include "../World.h"
#include "../BlockID.h"
#include "../Defines.h"





cFireSimulator::cFireSimulator(cWorld & a_World)
	: cSimulator(a_World)
	, m_Blocks(new BlockList)
	, m_Buffer(new BlockList)
	, m_BurningBlocks(new BlockList)
{
}





cFireSimulator::~cFireSimulator()
{
	delete m_Buffer;
	delete m_Blocks;
	delete m_BurningBlocks;
}





void cFireSimulator::Simulate(float a_Dt)
{
	m_Buffer->clear();
	std::swap(m_Blocks, m_Buffer);

	for (BlockList::iterator itr = m_Buffer->begin(); itr != m_Buffer->end(); ++itr)
	{
		Vector3i Pos = *itr;

		BLOCKTYPE BlockID = m_World.GetBlock(Pos.x, Pos.y, Pos.z);

		if (!IsAllowedBlock(BlockID))	 // Check wheather the block is still burning
		{
			continue;
		}

		if (BurnBlockAround(Pos.x, Pos.y, Pos.z))	//Burn single block and if there was one -> next time again
		{
			m_Blocks->push_back(Pos);
		}
		else
		{
			if (!IsForeverBurnable(m_World.GetBlock(Pos.x, Pos.y - 1, Pos.z)) && !FiresForever(BlockID))
			{
				m_World.SetBlock(Pos.x, Pos.y, Pos.z, E_BLOCK_AIR, 0);
			}
		}
	}  // for itr - m_Buffer[]
}





bool cFireSimulator::IsAllowedBlock(BLOCKTYPE a_BlockType)
{
	return (a_BlockType == E_BLOCK_FIRE) || IsBlockLava(a_BlockType);
}





void cFireSimulator::AddBlock(int a_BlockX, int a_BlockY, int a_BlockZ, cChunk * a_Chunk)
{
	// TODO: This can be optimized
	BLOCKTYPE BlockType = m_World.GetBlock(a_BlockX, a_BlockY, a_BlockZ);
	if (!IsAllowedBlock(BlockType))
	{
		return;
	}
	
	// Check for duplicates:
	for (BlockList::iterator itr = m_Blocks->begin(); itr != m_Blocks->end(); ++itr )
	{
		Vector3i Pos = *itr;
		if ((Pos.x == a_BlockX) && (Pos.y == a_BlockY) && (Pos.z == a_BlockZ))
		{
			return;
		}
	}

	m_Blocks->push_back(Vector3i(a_BlockX, a_BlockY, a_BlockZ));
}





bool cFireSimulator::IsForeverBurnable( BLOCKTYPE a_BlockType )
{
	return a_BlockType == E_BLOCK_NETHERRACK;
}





bool cFireSimulator::IsBurnable( BLOCKTYPE a_BlockType )
{
	return a_BlockType == E_BLOCK_PLANKS
		|| a_BlockType == E_BLOCK_LEAVES
		|| a_BlockType == E_BLOCK_LOG
		|| a_BlockType == E_BLOCK_WOOL
		|| a_BlockType == E_BLOCK_BOOKCASE
		|| a_BlockType == E_BLOCK_FENCE
		|| a_BlockType == E_BLOCK_TNT
		|| a_BlockType == E_BLOCK_VINES;
}





bool cFireSimulator::FiresForever( BLOCKTYPE a_BlockType )
{
	return a_BlockType != E_BLOCK_FIRE;
}





bool cFireSimulator::BurnBlockAround(int a_X, int a_Y, int a_Z)
{
	return BurnBlock(a_X + 1, a_Y, a_Z)
		|| BurnBlock(a_X - 1, a_Y, a_Z)
		|| BurnBlock(a_X, a_Y + 1, a_Z)
		|| BurnBlock(a_X, a_Y - 1, a_Z)
		|| BurnBlock(a_X, a_Y, a_Z + 1)
		|| BurnBlock(a_X, a_Y, a_Z - 1);
}





bool cFireSimulator::BurnBlock(int a_X, int a_Y, int a_Z)
{
	BLOCKTYPE BlockID = m_World.GetBlock(a_X, a_Y, a_Z);
	if (IsBurnable(BlockID))
	{
		m_World.SetBlock(a_X, a_Y, a_Z, E_BLOCK_FIRE, 0);
		return true;
	}
	if (IsForeverBurnable(BlockID))
	{
		BLOCKTYPE BlockAbove = m_World.GetBlock(a_X, a_Y + 1, a_Z);
		if (BlockAbove == E_BLOCK_AIR)
		{
			m_World.SetBlock(a_X, a_Y + 1, a_Z, E_BLOCK_FIRE, 0);	//Doesn�t notify the simulator so it won�t go off
			return true;
		}
		return false;
	}

	return false;
}




