
// Map.h

// Implementation of in-game coloured maps





#pragma once





#include "BlockID.h"





class cClientHandle;
class cWorld;
class cPlayer;
class cMap;





/** Encapsulates a map decorator. */
class cMapDecorator
{
public:

	enum eType
	{
		E_TYPE_PLAYER         = 0x00,
		E_TYPE_ITEM_FRAME     = 0x01,

		/** Player outside of the boundaries of the map. */
		E_TYPE_PLAYER_OUTSIDE = 0x06
	};


public:

	/** Constructs a map decorator fixed at the specified pixel coordinates. (DEBUG) */
	cMapDecorator(cMap * a_Map, eType a_Type, int a_X, int a_Z, int a_Rot);

	/** Constructs a map decorator that tracks a player. */
	cMapDecorator(cMap * a_Map, cPlayer * a_Player);

	/** Updates the pixel coordinates of the decorator. */
	void Update(void);

	unsigned int GetPixelX(void) const { return m_PixelX; }
	unsigned int GetPixelZ(void) const { return m_PixelZ; }

	int GetRot(void) const { return m_Rot; }

	eType GetType(void) const { return m_Type; }

	cPlayer * GetPlayer(void) { return m_Player; }


protected:

	cMap * m_Map;

	eType m_Type;

	unsigned int m_PixelX;
	unsigned int m_PixelZ;

	unsigned int m_Rot;

	cPlayer * m_Player;

};

typedef std::list<cMapDecorator> cMapDecoratorList;





// tolua_begin

/** Encapsulates an in-game world map. */
class cMap
{
public:

	typedef Byte ColorID;

	// tolua_end

	typedef std::vector<ColorID> cColorList;

	/** Encapsulates the state of a map client. */
	struct cMapClient
	{
		cClientHandle * m_Handle;

		/** Whether the map scale was modified and needs to be resent. */
		bool m_SendInfo;

		/** Ticks since last decorator update. */
		unsigned int m_NextDecoratorUpdate;

		/** Number of pixel data updates. */
		Int64 m_DataUpdate;

		Int64 m_LastUpdate;
	};

	typedef std::list<cMapClient> cMapClientList;


public:

	/** Construct an empty map. */
	cMap(unsigned int a_ID, cWorld * a_World);

	/** Constructs an empty map at the specified coordinates. */
	cMap(unsigned int a_ID, int a_CenterX, int a_CenterZ, cWorld * a_World, unsigned int a_Scale = 3);

	/** Send this map to the specified client. WARNING: Slow */
	void SendTo(cClientHandle & a_Client);

	/** Update a circular region with the specified radius and center (in pixels). */
	void UpdateRadius(int a_PixelX, int a_PixelZ, unsigned int a_Radius);

	/** Update a circular region around the specified player. */
	void UpdateRadius(cPlayer & a_Player, unsigned int a_Radius);

	/** Send next update packet and remove invalid decorators */
	void UpdateClient(cPlayer * a_Player);

	// tolua_begin

	/** Erase pixel data */
	void EraseData(void);

	void Resize(unsigned int a_Width, unsigned int a_Height);

	void SetPosition(int a_CenterX, int a_CenterZ);

	void SetScale(unsigned int a_Scale);

	unsigned int GetWidth (void) const { return m_Width;  }
	unsigned int GetHeight(void) const { return m_Height; }

	unsigned int GetScale(void) const { return m_Scale; }

	int GetCenterX(void) const { return m_CenterX; }
	int GetCenterZ(void) const { return m_CenterZ; }

	unsigned int GetID(void) const { return m_ID; }

	cWorld * GetWorld(void) { return m_World; }

	AString GetName(void) { return m_Name; }

	eDimension GetDimension(void) const;

	const cColorList & GetData(void) const { return m_Data; }

	unsigned int GetNumPixels(void) const;

	unsigned int GetPixelWidth(void) const;

	// tolua_end


private:

	/** Update the associated decorators. */
	void UpdateDecorators(void);

	/** Update the specified pixel. */
	bool UpdatePixel(unsigned int a_X, unsigned int a_Z);

	unsigned int m_ID;

	unsigned int m_Width;
	unsigned int m_Height;

	/** The zoom level, 2^scale square blocks per pixel */
	unsigned int m_Scale;

	int m_CenterX;
	int m_CenterZ;

	/** Column-major array of colours */
	cColorList m_Data;

	cWorld * m_World;

	cMapDecoratorList m_Decorators;

	cMapClientList m_Clients;

	AString m_Name;

	friend class cMapSerializer;

};
