#ifndef _PATH_H
#define _PATH_H

#include <stdio.h>
#include "main.h"

#define PATH_CANNOT_MOVE			0xffff
#define PATH_AVOID_MOVE				0xfffe


class Trail {
friend class Path;

protected:
	struct trailstep {
		int16_t x;
		int16_t y;
		uint8_t dir;
	};
	trailstep *trail;
	uint32_t ind;
	uint32_t len;

public:
	Trail();
	~Trail();

	int bytes() { return 8 + 5*len; }
	void pack(uint8_t **p);
	void unpack(uint8_t **p);

	int getX() const { return ind<len? trail[ind].x : -1; }
	int getY() const { return ind<len? trail[ind].y : -1; }
	int getDir() { return ind<len? trail[ind].dir : -1; }
	int index() { return ind; }
	int setIndex(unsigned long i) { return ind = i>=0 && i<len? i : 0; }
	void first() { ind = 0; }
	void next() { if(ind<len-1) ind++; }
	void previous() { if(ind>0) ind--; }
	void last() { if(len>0) ind = len-1; }
	int length() { return len; }
	int steps() { return len-ind; }
	bool hasMoreSteps() { return ind<len-1; }
};


class Path {
protected:
	struct node {
		unsigned short key;
		short x;
		short y;
		short s;
		short g;
		short h;
		node *parent;
		node *open;
		node *closed;

		node(int x,int y,int g,int h,node *p)
			: x(x),y(y),g(g),h(h),parent(p),open(0),closed(0) { key = (x<<8)|y,s = p? p->s+1 : 0; }
		~node() {}
	};

	Unit *u;
	node *open;
	node **closed;
	node *closest;
	unsigned long cap;
	unsigned long sz;
	unsigned long full;

	bool canmove(int x,int y);
	int movecost(int x,int y);
	int nodes(int x,int y);
	int getHeuristic(int x1,int y1,int x2,int y2,int n);
	int getHexDir1(int x1,int y1,int x2,int y2);
	int getHexDir2(int x1,int y1,int x2,int y2);
	void moveHex(int x1,int y1,int &x2,int &y2,int dir);

	void put(node *n);
	node *get(unsigned short key);
	void rehash();
	void push(node *n);
	node *pop();
	void remove(node *n);
	void clear();

public:
	Path();
	~Path();

	Trail *search(Unit *u,int x1,int y1,int x2,int y2);
};




#endif

