#pragma once
#include "global.h"
#include "SDL.h"
#include "CPlayerInterface.h"
//#include "boost/tuple/tuple.hpp"
class CGTownInstance;
class CTownHandler;
class CHallInterface;
struct Structure;
template <typename T> class AdventureMapButton;
class CBuildingRect : public Hoverable, public MotionInterested, public ClickableL, public ClickableR//, public TimeInterested
{
public:
	Structure* str;
	CDefHandler* def;
	SDL_Surface* border;
	SDL_Surface* area;
	CBuildingRect(Structure *Str);
	~CBuildingRect();
	void activate();
	void deactivate();
	bool operator<(const CBuildingRect & p2) const;
	void hover(bool on);
	void clickLeft (tribool down);
	void clickRight (tribool down);
	void mouseMoved (SDL_MouseMotionEvent & sEvent);
};

class CCastleInterface : public IShowable, public IActivable
{
public:
	bool showing;
	CBuildingRect * hBuild; //highlighted building
	SDL_Surface * townInt;
	SDL_Surface * cityBg;
	const CGTownInstance * town;
	CStatusBar * statusbar;
	CHallInterface * hallInt;
	unsigned char animval, count;

	CDefHandler *hall,*fort, *flag;
	CDefEssential* bicons; //150x70 buildings imgs
	CTownList<CCastleInterface> * townlist;

	CGarrisonInt * garr;
	AdventureMapButton<CCastleInterface> * exit, *split;

	std::vector<CBuildingRect*> buildings; //building id, building def, structure struct, border, filling

	CCastleInterface(const CGTownInstance * Town, bool Activate=true);
	~CCastleInterface();
	void townChange();
	void show(SDL_Surface * to=NULL);
	void showAll(SDL_Surface * to=NULL);
	void buildingClicked(int building);
	void enterHall();
	void close();
	void splitF();
	void activate();
	void deactivate();
};

class CHallInterface : public IShowable, public IActivable
{
public:
	class CResDataBar : public IShowable, public CIntObject
	{
	public:
		SDL_Surface *bg;
		void show(SDL_Surface * to=NULL);
		CResDataBar();
		~CResDataBar();
	} resdatabar;

	class CBuildingBox : public Hoverable, public ClickableL, public ClickableR
	{
	public:
		int BID;
		int state;// 0 - no more than one capitol, 1 - lack of water, 2 - forbidden, 3 - Add another level to Mage Guild, 4 - already built, 5 - cannot build, 6 - cannot afford, 7 - build
		//(-1) - forbidden in this town, 0 - possible, 1 - lack of res, 2 - requirements/buildings per turn limit, (3) - already exists
		void hover(bool on);
		void clickLeft (tribool down);
		void clickRight (tribool down);
		void show(SDL_Surface * to=NULL);
		void activate();
		void deactivate();
		CBuildingBox(int id);
		CBuildingBox(int id, int x, int y);
		~CBuildingBox();
	};

	class CBuildWindow: public IShowable, public ClickableR
	{
	public: 
		int tid, bid, state; //town id, building id, state
		bool mode; // 0 - normal (with buttons), 1 - r-click popup
		SDL_Surface * bitmap; //main window bitmap, with blitted res/text, without buttons/subtitle in "statusbar"
		AdventureMapButton<CBuildWindow> *buy, *cancel;

		void activate();
		void deactivate();
		void clickRight (tribool down);
		void show(SDL_Surface * to=NULL);
		void Buy();
		void close();
		CBuildWindow(int Tid, int Bid, int State, bool Mode);
		~CBuildWindow();
	};

	CDefEssential *bars, //0 - yellow, 1 - green, 2 - red, 3 - gray
		*status; //0 - already, 1 - can't, 2 - lack of resources
	std::vector< std::vector<CBuildingBox*> >boxes;

	AdventureMapButton<CHallInterface> * exit;

	SDL_Surface * bg;


	CHallInterface(CCastleInterface * owner);
	~CHallInterface();
	void close();
	void show(SDL_Surface * to=NULL);
	void activate();
	void deactivate();
};