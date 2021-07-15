
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/net/aSocket.h>
#include "main.h"
#include "path.h"


Trail::Trail() : trail(0),ind(0),len(0) {}

Trail::~Trail() {
	if(trail) { free(trail);trail = 0; }
	ind = 0;
	len = 0;
}

void Trail::pack(uint8_t **p) {
	uint32_t i;
	trailstep *s;
	pack_uint32(p,ind);
	pack_uint32(p,len);
	for(i=0; i<len; i++) {
		s = &trail[i];
		pack_int16(p,s->x);
		pack_int16(p,s->y);
		pack_uint8(p,s->dir);
	}
}

void Trail::unpack(uint8_t **p) {
	uint32_t i;
	trailstep *s;
	unpack_uint32(p,ind);
	unpack_uint32(p,len);
	if(trail) free(trail);
	trail = (trailstep *)malloc(sizeof(trailstep)*len);
	for(i=0; i<len; i++) {
		s = &trail[i];
		unpack_int16(p,s->x);
		unpack_int16(p,s->y);
		unpack_uint8(p,s->dir);
	}
}



Path::Path() {
	open = 0;
	closed = 0;
	cap = 11;
	sz = 0;
	full = 0;
}

Path::~Path() {
}


#define coutput(c) //{putc(c,stdout);fflush(stdout);}
#define soutput(s,d) //{fprintf(stdout,s,d);fflush(stdout);}


Trail *Path::search(Unit *u,int x1,int y1,int x2,int y2) {
debug_output("Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
	Trail *t = 0;
	this->u = u;
	//while((x1!=x2 || y1!=y2) && !canmove(x2,y2)) moveHex(x2,y2,x2,y2,getHexDir(x2,y2,x1,y1));
	if(x1==x2 && y1==y2) return t;

debug_output("Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
	node *p1 = 0,*p2 = 0;
	cap = getHeuristic(x1,y1,x2,y2,0)*8+1;
	p1 = new node(x1,y1,0,getHeuristic(x1,y1,x2,y2,3),0);
	closest = p1;
	put(p1);
	push(p1);

debug_output("Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);

	int i,n,x,y,c;
	while(open) {
		p1 = pop();
debug_output("key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\ts=%d\tg=%d\th=%d ",p1->key,p1->x,p1->y,
	p1->parent? p1->parent->x : -1,p1->parent? p1->parent->y : -1,p1->s,p1->g,p1->h);
		for(i=0,n=nodes(p1->x,p1->y); i<n; i++) {
			moveHex(p1->x,p1->y,x,y,i);
			c = movecost(x,y);																			soutput("(%d",x)soutput(",%d)",y)
			if(x==x2 && y==y2) {																			coutput('!');
				if(c==PATH_CANNOT_MOVE) {
					tile &h = app.map.tiles[y*app.map.width+x];
					if(h.unit && h.unit->player==u->player && (h.unit->status&U_TRANSPORT)) c = 1;
debug_output("\nPath::search(Cannot move, test if transport there... (h.unit=%p,h.player=%p,u.player=%p,h.unit.status=%d,c=%d))\n",
h.unit,h.unit? h.unit->player : 0,u->player,h.unit? h.unit->status : 0,c);
				}
				if(c!=PATH_CANNOT_MOVE) {
					p1 = new node(x,y,p1->g+1,0,p1);
					closest = p1;
					put(p1);
				}
				open = 0;
				break;
			} else if(c!=PATH_CANNOT_MOVE && c!=PATH_AVOID_MOVE) {
				if((p2=get((x<<8)|y))) {																coutput('+');
					if(p1->g+c<p2->g) {																	coutput('*');
						remove(p2);
						p2->parent = p1,p2->g = p1->g+c,p2->s = p1->s+1;
						push(p2);
					}
				} else {																						coutput('-');
					p2 = new node(x,y,p1->g+c,getHeuristic(x,y,x2,y2,3),p1);
					if(p2->h<closest->h || (p2->h==closest->h && p2->g<closest->g)) closest = p2;
					put(p2);																					coutput('=');
					push(p2);
				}
			}
		}																										coutput('\n');
	}

char mem[app.map.height][app.map.width];
memset(mem,' ',app.map.width*app.map.height);
for(int y=0; y<app.map.height; y++) for(int x=0; x<app.map.width; x++) if(get((x<<8)|y)) mem[y][x] = '+';

	p1 = closest;
	if(p1 && p1->g) {
		t = new Trail();
debug_output("Path::search(p.x=%d,p.y=%d,p.g=%d)\n",p1->x,p1->y,p1->g);
		for(t->len=1,p1->open=0; p1->parent && p1->parent!=p1; t->len++,p1=p1->parent) p1->parent->open = p1;
debug_output("Path::search(trail.lenght=%d)\n",t->len);
		if(t->len>1) {
			t->trail = (Trail::trailstep *)malloc(sizeof(Trail::trailstep)*t->len);
			for(i=0,c='a'; p1; i++,p1=p2) {
				p2 = p1->open;
debug_output("key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\tdir=%d\n",
p1->key,p1->x,p1->y,p2? p2->x : -1,p2? p2->y : -1,p2? getHexDir1(p1->x,p1->y,p2->x,p2->y) : 5);
				t->trail[i] = (Trail::trailstep){ p1->x,p1->y,p2? getHexDir1(p1->x,p1->y,p2->x,p2->y) : 5 };
				mem[p1->y][p1->x] = (char)c++;
				if(c=='z'+1) c = 'A';
				else if(c=='Z'+1) c = 'a';
			}
debug_output("Path::search()\n");
		}
	}
for(int y=0; y<app.map.height; y++) {
for(int x=0; x<app.map.width; x++) putc(mem[y][x],stdout);
fputc('\n',stdout);
fflush(stdout);
}
	clear();
	return t;
}

int Path::nodes(int x,int y) { return 6; }
bool Path::canmove(int x,int y) { return unit_types[u->type].terrain[app.map.tiles[y*app.map.width+x].terrain].moves!=0; }

int Path::movecost(int x,int y) {
	tile &h = app.map.tiles[y*app.map.width+x];
	int c = unit_types[u->type].terrain[h.terrain].moves;
	if(c==0 || (u->player && app.showFogOfWar() && app.isFogOfWar(*u->player,h.x,h.y))) return PATH_CANNOT_MOVE;
	if((h.city && h.city->player!=u->player) || (h.unit/* && h.unit->player!=u->player*/)) return PATH_AVOID_MOVE;
	return c;
}



int Path::getHeuristic(int x1,int y1,int x2,int y2,int n) {
	if(app.isHWrap()) { if(x1+app.map.width-x2<x2-x1) x1 += app.map.width;else if(x2+app.map.width-x1<x1-x2) x2 += app.map.width; }
	if(app.isVWrap()) { if(y1+app.map.height-y2<y2-y1) y1 += app.map.height;else if(y2+app.map.height-y1<y1-y2) y2 += app.map.height; }
	x1 = abs(x1-x2),y1 = abs(y1-y2);
	if(n==1) return x1>y1? x1+y1/2 : y1+x1/2;
	else if(n==2) return x1+y1;
	else if(n==3) return (x1+y1)*2;
	else return (x1>y1? x1 : y1);
}

int Path::getHexDir1(int x1,int y1,int x2,int y2) {
	if(app.isHWrap()) { if(x1+app.map.width-x2<x2-x1) x1 += app.map.width;else if(x2+app.map.width-x1<x1-x2) x2 += app.map.width; }
	if(app.isVWrap()) { if(y1+app.map.height-y2<y2-y1) y1 += app.map.height;else if(y2+app.map.height-y1<y1-y2) y2 += app.map.height; }
	x2 -= x1+(y1&1);
	if(abs(y2-y1)==2) return y2<y1? 5 : 2;
	return y2<y1? (x2<0? 4 : 0) : (x2<0? 3 : 1);
}

int Path::getHexDir2(int x1,int y1,int x2,int y2) {
	if(app.isHWrap()) { if(x1+app.map.width-x2<x2-x1) x1 += app.map.width;else if(x2+app.map.width-x1<x1-x2) x2 += app.map.width; }
	if(app.isVWrap()) { if(y1+app.map.height-y2<y2-y1) y1 += app.map.height;else if(y2+app.map.height-y1<y1-y2) y2 += app.map.height; }
	x2 -= x1+(y1&1);
	return y2<y1? (abs(x2)>abs(y2-y1)? 5 : (x2<0? 4 : 0)) : (abs(x2)>abs(y2-y1)? 2 : (x2<0? 3 : 1));
}

void Path::moveHex(int x1,int y1,int &x2,int &y2,int dir) {
	static const int xcoords[2][6] = {{ 0,0,0,-1,-1,0 },{ 1,1,0,0,0,0 }};
	static const int ycoords[6] = { -1,1,2,1,-1,-2 };
//debug_output("Path::moveIso(x=%d,y=%d,dir=%d)\n",x1,y1,dir);
	x1 += xcoords[y1&1][dir],y1 += ycoords[dir];

	if(app.isHWrap()) { if(x1<0) x1 += app.map.width;else if(x1>=app.map.width) x1 -= app.map.width; }
	else { if(x1<0) x1 = 0;else if(x1>=app.map.width) x1 = app.map.width-1; }
	if(app.isVWrap()) { if(y1<0) y1 += app.map.height;else if(y1>=app.map.height) y1 -= app.map.height; }
	else { if(y1<0) y1 = 0;else if(y1>=app.map.height) y1 = app.map.height-1; }
	x2 = x1,y2 = y1;
}



void Path::put(node *n) {
	if(sz==full) rehash();
	int h = n->key%cap;
	n->closed = closed[h],closed[h] = n;
	sz++;
}

Path::node *Path::get(unsigned short key) {
	if(!sz) return 0;
	node *n = closed[key%cap];
	while(n && n->key!=key) n = n->closed;
	return n;
}

void Path::rehash() {
	if(!closed) {
		closed = (node **)malloc(cap*sizeof(node *));
		memset(closed,0,cap*sizeof(node *));
	} else {
		int c = cap<<1;
		if(!(c&1)) c++;
		unsigned long i,h;
		node **t = (node **)malloc(c*sizeof(node *));
		memset(t,0,c*sizeof(node *));
		node *n1,*n2;
		for(i=0; i<cap; i++) if((n1=closed[i])) while(n1)
			n2 = n1->closed,h = n1->key%c,n1->closed = t[h],t[h] = n1,n1 = n2;
		free(closed);
		closed = t,cap = c;
	}
	full = cap>>1;
}


void Path::push(node *p) {														coutput('p');
	node *p0 = 0,*p1 = open;
	int i=0;
	while(p1 && (p->g>p1->g || (p->g==p1->g && p->h>p1->h)))
		p0 = p1,p1 = p1->open,i++;												soutput("%dp",i);
	if(!p0) p->open = open,open = p;
	else p0->open = p,p->open = p1;
}

Path::node *Path::pop() {
	node *p = open;
	if(p) open = p->open;
	return p;
}

void Path::remove(node *p) {
	if(!open) return;
	if(open!=p) {
		node *p1 = open;
		while(p1->open && p1->open!=p) p1 = p1->open;
		if(p1->open) p1->open = p1->open->open;
	} else open = open->open;
}

void Path::clear() {
	if(closed) {
		node *n1,*n2;
		for(unsigned long i=0; i<cap; i++) if((n1=closed[i])) do {
			n2 = n1->closed;
			delete n1;
			n1 = n2;
		} while(n1);
		free(closed);
		closed = 0;
	}
	open = 0;
}




