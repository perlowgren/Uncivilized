
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amanita/aMath.h>
#include <amanita/aRandom.h>

#include "main.h"
#include "path.h"


#include "../data/images/att.xpm"
#include "../data/images/border.xpm"
#include "../data/images/building.xpm"
#include "../data/images/building_icon.xpm"
#include "../data/images/flag.xpm"
#include "../data/images/font.xpm"
#include "../data/images/fow_edge.xpm"
#include "../data/images/frame_edge.xpm"
#include "../data/images/frame_h.xpm"
#include "../data/images/frame_v.xpm"
#include "../data/images/work.xpm"
#include "../data/images/marker.xpm"
#include "../data/images/resource.xpm"
#include "../data/images/resource_icon.xpm"
#include "../data/images/roman.xpm"
#include "../data/images/stat.xpm"
#include "../data/images/step.xpm"
#include "../data/images/terrain.xpm"
#include "../data/images/unit_icon.xpm"
#include "../data/images/unit_sprite.xpm"


static const tile_rectangle terrain[] = {
	{0,10,46,39,23,23},{46,10,46,39,23,23},{92,9,46,40,23,24},{138,10,46,39,23,23},
};
static const tile_rectangle city_dimension[] = {
	{0,11,46,39,23,23},{46,11,46,39,23,23},{92,8,46,42,23,26},{138,8,46,42,23,26},
};
static const GdkRectangle border[] = {
	{},{35,0,10,20},{35,19,10,19},{10,36,26,2},{0,19,11,19},{0,0,11,20},{10,0,26,2}
};
static const GdkRectangle fow_edge[] = {
	{},{34,10,12,20},{34,29,12,20},{10,45,26,4},{0,29,12,20},{0,10,12,20},{9,0,27,13}
};
static const tile_rectangle unit_dimension[] = {
	{0,3,9,24,5,22},{9,1,15,26,9,24},{24,3,14,24,8,22},{38,1,16,26,9,24},{54,0,22,27,13,25},{76,0,22,27,13,25},
	{98,0,27,27,14,21},{125,1,26,26,14,20},
};
static const tile_rectangle step[] = {
	{1,1,9,8,26,6},{11,1,8,9,29,15},{20,1,6,10,23,23},{27,1,9,8,14,23},{37,1,8,9,11,15},{46,1,6,10,17,7},
	{1,12,8,9,25,8},{11,12,9,8,28,17},{20,12,6,10,23,22},{27,12,8,9,12,22},{37,12,9,8,10,13},{46,12,6,10,17,6},
};
static const GdkRectangle flag[] = {
	{1,24,7,24},{10,1,7,7},{18,1,9,7},{28,1,11,7},{40,1,13,7},{54,1,15,7},
};

static GdkPixmap *maparea_buffer;
static GdkGC *maparea_gc;
static GdkPixmap *paint_pixmap;
static GdkGC *paint_gc;
static PangoFontDescription *font;
static GdkColor color;

static struct {
	GdkPixbuf *att;
	GdkPixbuf *building;
	GdkPixbuf *building_icon;
	GdkPixbuf *border;
	GdkPixbuf *flag;
	GdkPixbuf *font;
	GdkPixbuf *fow_edge;
	GdkPixbuf *frame_edge;
	GdkPixbuf *frame_h;
	GdkPixbuf *frame_v;
	GdkPixbuf *work;
	GdkPixbuf *marker;
	GdkPixbuf *resource;
	GdkPixbuf *resource_icon;
	GdkPixbuf *roman;
	GdkPixbuf *stat;
	GdkPixbuf *step;
	GdkPixbuf *terrain;
	GdkPixbuf *unit_icon;
	GdkPixbuf *unit_sprite;
} images;


void Game::loadGraphics() {
	int i;
	const char **img_names[] = {
		att_xpm,border_xpm,building_xpm,building_icon_xpm,flag_xpm,
		font_xpm,fow_edge_xpm,frame_edge_xpm,frame_h_xpm,frame_v_xpm,
		work_xpm,marker_xpm,resource_xpm,resource_icon_xpm,roman_xpm,
		stat_xpm,step_xpm,terrain_xpm,unit_icon_xpm,unit_sprite_xpm,
	0};
	GdkPixbuf **img[] = {
		&images.att,&images.border,&images.building,&images.building_icon,
		&images.flag,&images.font,&images.fow_edge,&images.frame_edge,
		&images.frame_h,&images.frame_v,&images.work,&images.marker,
		&images.resource,&images.resource_icon,&images.roman,&images.stat,
		&images.step,&images.terrain,&images.unit_icon,&images.unit_sprite,
	};
	for(i=0; img_names[i]; i++) *img[i] = gdk_pixbuf_new_from_xpm_data(img_names[i]);
	font = pango_font_description_from_string("ProFont 8");
}

void Game::openPaint(GtkWidget *widget) {
	if(maparea_buffer) g_object_unref(maparea_buffer);
	maparea_buffer = gdk_pixmap_new(widget->window,width,height,-1);
}

void Game::updatePaint(GtkWidget *widget,GdkEventExpose *event) {
	gdk_draw_pixmap(widget->window,widget->style->fg_gc[GTK_WIDGET_STATE(widget)],maparea_buffer,
		event->area.x,event->area.y,event->area.x,event->area.y,event->area.width,event->area.height);
}

void Game::closePaint() {
	g_object_unref(images.att);
	g_object_unref(images.border);
	g_object_unref(images.building);
	g_object_unref(images.building_icon);
	g_object_unref(images.flag);
	g_object_unref(images.font);
	g_object_unref(images.fow_edge);
	g_object_unref(images.frame_edge);
	g_object_unref(images.frame_h);
	g_object_unref(images.frame_v);
	g_object_unref(images.work);
	g_object_unref(images.marker);
	g_object_unref(images.resource);
	g_object_unref(images.resource_icon);
	g_object_unref(images.roman);
	g_object_unref(images.stat);
	g_object_unref(images.step);
	g_object_unref(images.terrain);
	g_object_unref(images.unit_icon);
	g_object_unref(images.unit_sprite);

	g_object_unref(maparea_buffer);

	pango_font_description_free(font);
}

static inline void set_canvas(GdkPixmap *pm,GdkGC *gc) { paint_pixmap = pm,paint_gc = gc; }

static inline void set_color(int c) {
	color.pixel = 0xff000000|c;
	gdk_gc_set_foreground(paint_gc,&color);
}

static inline void draw_line(int x1,int y1,int x2,int y2) {
	gdk_draw_line(paint_pixmap,paint_gc,x1,y1,x2,y2);
}

static inline void draw_rectangle(int x,int y,int w,int h,bool fill) {
	gdk_draw_rectangle(paint_pixmap,paint_gc,fill,x,y,w,h);
}

static int draw_string(int x,int y,int align,const char *str, ...) {
	static char metrics_w[96] = {
		5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,5,5,5,6,5,5,
		5,5,5,5,5,5,5,6,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,5,5,5,6,5,5,
		5,5,5,5,5,5,5,6,5,5,5,5,5,5,5,5,
	};
	static char buf[129];
	va_list args;
	va_start(args,str);
	vsnprintf(buf,128,str,args);
	va_end(args);
	int i,w = 0;
	char *p;
	for(p=buf; *p; p++) i = *p-32,w += i<96? metrics_w[i] : 0;
	if(align==1) x -= w+1;
	else if(align==2) x -= (w+1)/2;
	for(p=buf; *p; p++) {
		i = *p-32;
		if(i<96) {
			gdk_draw_pixbuf(paint_pixmap,paint_gc,images.font,(i%16)*(FONT_WIDTH+2),(i/16)*FONT_HEIGHT,x,y,metrics_w[i]+1,FONT_HEIGHT,GDK_RGB_DITHER_NONE,0,0);
			x += metrics_w[i];
		}
	}
	return x+1;
}

static int draw_number(int x,int y,int align,int n,int m=0,bool plus=false) {
	char buf[33],*p = &buf[32];
	int i,j;
	*p = '\0';
	if(m>0) {
		for(i=m,j=0; i; i/=10,j++) {
			if(j>0 && (j%3)==0) *--p = '\'';
			*--p = '0'+(i%10);
		}
		*--p = '/';
	}
	if(n==0) *--p = m<=0? '-' : '0';
	else {
		for(i=n<0? -n : n,j=0; i; i/=10,j++) {
			if(j>0 && (j%3)==0) *--p = '\'';
			*--p = '0'+(i%10);
		}
		if(n<0) *--p = '-';
		else if(plus) *--p = '+';
	}
	if(align==1) x -= strlen(p)*5+1;
	else if(align==2) x -= (strlen(p)*5+1)/2;
	for(; *p; p++,x+=FONT_WIDTH) {
		i = *p-32;
		gdk_draw_pixbuf(paint_pixmap,paint_gc,images.font,(i%16)*(FONT_WIDTH+2),(i/16)*FONT_HEIGHT,x,y,FONT_WIDTH+1,FONT_HEIGHT,GDK_RGB_DITHER_NONE,0,0);
	}
	return x+1;
}

static inline void draw_image(GdkDrawable *image,int x,int y,int w,int h,int sx,int sy) {
	gdk_draw_drawable(paint_pixmap,paint_gc,image,sx,sy,x,y,w,h);
}

static inline void draw_image(GdkPixbuf *image,int x,int y,int w,int h,int sx,int sy) {
	gdk_draw_pixbuf(paint_pixmap,paint_gc,image,sx,sy,x,y,w,h,GDK_RGB_DITHER_NONE,0,0);
}

static inline void draw_image(GdkPixbuf *image,int x,int y,const GdkRectangle &r) {
	gdk_draw_pixbuf(paint_pixmap,paint_gc,image,r.x,r.y,x,y,r.width,r.height,GDK_RGB_DITHER_NONE,0,0);
}

static inline void draw_image(GdkPixbuf *image,int x,int y,const GdkRectangle &r,int sx,int sy) {
	gdk_draw_pixbuf(paint_pixmap,paint_gc,image,r.x+sx,r.y+sy,x+r.x,y+r.y,r.width,r.height,GDK_RGB_DITHER_NONE,0,0);
}

static inline void draw_image(GdkPixbuf *image,int x,int y,const tile_rectangle &r,int flags) {
	if(flags&ICON_ALIGN_CENTER) x += TILE_CENTER_X-r.center_x,y += TILE_CENTER_Y-r.center_y;
	gdk_draw_pixbuf(paint_pixmap,paint_gc,image,r.x,r.y,x,y,r.width,r.height,GDK_RGB_DITHER_NONE,0,0);
}

static void fill_pattern(GdkPixbuf *image,int x,int y,int w,int h,int sx,int sy,int sw,int sh) {
	int x1;
	for(w+=x,h+=y; y<h-1; y+=sh) {
		if(y+sh>h) sh = h-y;
		for(x1=x; x1<w-1; x1+=sw)
			draw_image(image,x1,y,x1+sw>w? w-x1 : sw,sh,sx,sy);
	}
}

static int draw_icon(int x,int y,int i,int flags) {
	int sy = 0;
	if(flags&ICON_DISABLED) sy += WORK_HEIGHT+WORK_HEIGHT;
	else if(flags&ICON_ACTIVE) sy += WORK_HEIGHT;
	gdk_draw_pixbuf(paint_pixmap,paint_gc,images.work,WORK_WIDTH*i,sy,x,y,WORK_WIDTH,WORK_HEIGHT,GDK_RGB_DITHER_NONE,0,0);
	return x+WORK_WIDTH;
}

static void draw_icon(GdkPixbuf *image,int x,int y,int w,int h,int sx,int row_h,int flags) {
	int sy = 0;
	if(flags&ICON_DISABLED) sy += row_h+row_h;
	else if(flags&ICON_ACTIVE) sy += row_h;
	gdk_draw_pixbuf(paint_pixmap,paint_gc,image,sx,sy,x,y,w,h,GDK_RGB_DITHER_NONE,0,0);
}

static inline void draw_icon(GdkPixbuf *image,int x,int y,const tile_rectangle &r,int row_h,int flags) {
	draw_icon(image,x,y,r.width,r.height,r.x,row_h,flags);
}

static inline void draw_building_icon(int x,int y,const city_building &cb,int flags) {
	draw_icon(images.building_icon,x,y,BUILDING_ICON_WIDTH,BUILDING_ICON_HEIGHT,BUILDING_ICON_WIDTH*cb.sign,BUILDING_ICON_HEIGHT,flags);
	draw_image(images.roman,x,y+BUILDING_ICON_HEIGHT-ROMAN_HEIGHT,-1,ROMAN_HEIGHT,0,cb.level*ROMAN_HEIGHT);
}

static inline void draw_unit_icon(int x,int y,const unit_type &ut,int flags) {
	draw_icon(images.unit_icon,x,y,UNIT_ICON_WIDTH,UNIT_ICON_HEIGHT,UNIT_ICON_WIDTH*ut.sign,UNIT_ICON_HEIGHT,flags);
	draw_image(images.roman,x,y+UNIT_ICON_HEIGHT-ROMAN_HEIGHT,-1,ROMAN_HEIGHT,0,ut.level*ROMAN_HEIGHT);
}

static inline void draw_resource_icon(int x,int y,int r,int flags) {
	if(r>=RES) r--;
	draw_icon(images.resource_icon,x,y,RESOURCE_WIDTH,RESOURCE_HEIGHT,RESOURCE_WIDTH*r,RESOURCE_HEIGHT,flags);
}

static int draw_unit_sprite(int x,int y,int t,int flags) {
	const tile_rectangle &r = unit_dimension[unit_types[t].sprite];
	int sy = r.y;
	if(flags&ICON_ALIGN_CENTER) x += TILE_CENTER_X-r.center_x,y += TILE_CENTER_Y-r.center_y;
	else if(flags&ICON_ALIGN_BOTTOM) y += UNIT_SPRITE_WIDTH-r.height;
	if(flags&ICON_DISABLED) sy += UNIT_SPRITE_HEIGHT+UNIT_SPRITE_HEIGHT;
	else if(flags&ICON_ACTIVE) sy += UNIT_SPRITE_HEIGHT;
	gdk_draw_pixbuf(paint_pixmap,paint_gc,images.unit_sprite,r.x,sy,x,y,r.width,r.height,GDK_RGB_DITHER_NONE,0,0);
	return x+r.width;
}


static inline int draw_attribute(int x,int y,int a,int n,int m=0) {
//	if(n==0) return x;
	if(n<0) n = 0;
	gdk_draw_pixbuf(paint_pixmap,paint_gc,images.att,a*12,0,x,y,12,12,GDK_RGB_DITHER_NONE,0,0);
	return draw_number(x+12,y+2,0,n,m);
}

static inline void draw_attribute(int x,int y,int a) {
	gdk_draw_pixbuf(paint_pixmap,paint_gc,images.att,a*12,0,x,y,12,12,GDK_RGB_DITHER_NONE,0,0);
}

static inline int draw_stock(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_STOCK,n,m); }
static inline int draw_turns(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_TURNS,n<=0? 1 : n,m); }
static inline int draw_moves(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_MOVES,n,m); }
static inline int draw_hp(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_HP,n,m); }
static inline int draw_attack(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_ATTACK,n,m); }
static inline int draw_defence(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_DEFENCE,n,m); }
static inline int draw_ranged(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_RANGED,n,m); }
static inline int draw_bombard(int x,int y,int n,int m=0) { return draw_attribute(x,y,ATT_ICON_BOMBARD,n,m); }

static int draw_rank(int x,int y,int n,int m=0) {
	if(n==0) return x;
	int r;
	if(n<4) r = ATT_ICON_LIEUTENANT;
	else if(n<16) r = ATT_ICON_CAPTAIN;
	else if(n<64) r = ATT_ICON_MAJOR;
	else if(n<256) r = ATT_ICON_COLONEL;
	else r = ATT_ICON_GENERAL;
	return draw_attribute(x,y,r,n,m);
}

static void draw_stat(int x,int y,int s,int n,int m=0) {
	if(n<=0) return;
	if(m>0) n = (16*n)/m;
	if(n>16) n = 16;
	gdk_draw_pixbuf(paint_pixmap,paint_gc,images.stat,0,s*3,x,y,2,4,GDK_RGB_DITHER_NONE,0,0);
	if(n>2) gdk_draw_pixbuf(paint_pixmap,paint_gc,images.stat,2,s*3,x+2,y,n-2,4,GDK_RGB_DITHER_NONE,0,0);
	gdk_draw_pixbuf(paint_pixmap,paint_gc,images.stat,20,s*3,x+n,y,2,4,GDK_RGB_DITHER_NONE,0,0);
}


void Game::printText(int x,int y,const char *text, ...) {
	static char buf[257];
	va_list args;
	va_start(args,text);
	vsnprintf(buf,256,text,args);
	va_end(args);
	PangoLayout *layout = gtk_widget_create_pango_layout(gui.maparea,buf);
	pango_layout_set_font_description(layout,font);
	gdk_draw_layout(paint_pixmap,paint_gc,x,y,layout);
	g_object_unref(layout);
}

void Game::printText(int x,int y,int w,PangoAlignment a,const char *text, ...) {
	static char buf[257];
	va_list args;
	va_start(args,text);
	vsnprintf(buf,256,text,args);
	va_end(args);
	PangoLayout *layout = gtk_widget_create_pango_layout(gui.maparea,buf);
	pango_layout_set_font_description(layout,font);
	pango_layout_set_width(layout,w);
	pango_layout_set_alignment(layout,a);
	gdk_draw_layout(paint_pixmap,paint_gc,x,y,layout);
	g_object_unref(layout);
}


void Game::paint() {
	static int count = 0;
	int i,x,y;

	maparea_gc = gdk_gc_new(maparea_buffer);
	set_canvas(maparea_buffer,maparea_gc);
	if(!isGameRunning()) {
		set_color(0x3a2613);
		draw_rectangle(0,0,width,height,true);
	} else {
		int j,k,x1,y1,bx,by,vx,vy;
		Unit *u;

		set_color(0x000000);
		draw_rectangle(0,0,width,height,true);

		// Paint map:
		bx = -view_x/TILE_RECT_WIDTH;
		by = -view_y/TILE_RECT_HEIGHT;
		vx = view_x%TILE_RECT_WIDTH-TILE_CORNER_WIDTH;
		vy = view_y%TILE_RECT_HEIGHT-TILE_RECT_HEIGHT;

		for(y=0,y1=view_y; y1<height; y++,y1+=TILE_RECT_HEIGHT) {
			if(y1<=-TILE_HEIGHT) continue;
			if(y<0) { if(isVWrap()) y += map.height;else continue; }
			else if(y>=map.height) { if(isVWrap()) y -= map.height;else break; }
			for(x=0,x1=view_x+(y&1)*TILE_HALF_RECT_WIDTH; x1<width; x++,x1+=TILE_RECT_WIDTH) {
				if(x1<=-TILE_WIDTH) continue;
				if(x<0) { if(isHWrap()) x += map.width;else continue; }
				else if(x>=map.width) { if(isHWrap()) x -= map.width;else break; }

if(x<0 || x>=map.width || y<0 || y>=map.height) debug_output("Game::paint(x=%d,y=%d)\n",x,y);
				if(!showFogOfWar() || !local.player || !isFogOfWar(*local.player,x,y))
					paintTile(map.tiles[y*map.width+x],x1,y1,TILE_MAP);
			}
		}

		// Paint selected Unit's trail:
		if(((u=focus_unit) || (u=getUnit())) && u->tr && u->tr->hasMoreSteps() && (u->player==local.player || !local.player)) {
//debug_output("paint(tr=%p,index=%d,length=%d)\n",u->tr,u->tr->index(),u->tr->length());fflush(stdout);
			i = u->tr->index();
			k = u->tr->getDir();
			while(u->tr->hasMoreSteps()) {
				u->tr->next();
				j = (k+3)%6;
				k = u->tr->getDir();
				if(showFogOfWar() && local.player && isFogOfWar(*local.player,u->tr->getX(),u->tr->getY())) continue;
				x1 = view_x+u->tr->getX()*TILE_RECT_WIDTH+(u->tr->getY()&1)*TILE_HALF_RECT_WIDTH;
				y1 = view_y+u->tr->getY()*TILE_RECT_HEIGHT;
				if(isHWrap()) { if(x1<0) x1 += map.width_p;else if(x1>=map.width_p) x1 -= map.width_p; }
				if(isVWrap()) { if(y1<0) y1 += map.height_p;else if(y1>=map.height_p) y1 -= map.height_p; }
				if(x1+30>=0 && y1+27>=0 && x1<width && y1<=height) {
					draw_image(images.step,x1+step[j].center_x,y1+step[j].center_y,step[j].width,step[j].height,step[j].x,step[j].y);
					if(u->tr->hasMoreSteps())
						draw_image(images.step,x1+step[k+6].center_x,y1+step[k+6].center_y,step[k+6].width,step[k+6].height,step[k+6].x,step[k+6].y);
				}
			}
			u->tr->setIndex(i);
		}

		// Paint units:
		for(y=0,y1=view_y; y1<height; y++,y1+=TILE_RECT_HEIGHT) {
			if(y1<=-TILE_HEIGHT) continue;
			if(y<0) { if(isVWrap()) y += map.height;else continue; }
			else if(y>=map.height) { if(isVWrap()) y -= map.height;else break; }
			for(x=0,x1=view_x+(y&1)*TILE_HALF_RECT_WIDTH; x1<width; x++,x1+=TILE_RECT_WIDTH) {
				if(x1<=-TILE_WIDTH) continue;
				if(x<0) { if(isHWrap()) x += map.width;else continue; }
				else if(x>=map.width) { if(isHWrap()) x -= map.width;else break; }

				if(!showFogOfWar() || !local.player || !isFogOfWar(*local.player,x,y)) {
					u = map.tiles[y*map.width+x].unit;
					if(u) paintUnit(*u,x1,y1,(u->player==local.player)|2);
				}
			}
		}

		paintCityBox();
		paintUnitBox();

		// Paint map:
		if(showMap()) {
			draw_image(mapimage.image,mapimage.x,mapimage.y,mapimage.width,mapimage.height,0,0);
			paintBox(mapimage.x-4,mapimage.y-4,mapimage.width+8,mapimage.height+8,false);
		}

		paintToolTip();
	}

	if(log.top && log.top->time<=time(0)-300) log.update = true;
	if(log.update) updateMessageLog();
	if(log.layout) gdk_draw_layout(maparea_buffer,maparea_gc,2,height-2-log.height,log.layout);

	if(message_box.text) paintMessageBox();

	set_color(0xffffff);
	printText((width-128)/2,5,_("Run: %d, View[%d,%d]"),++count,view_x,view_y);

	g_object_unref(maparea_gc);
}

void Game::paintTile(tile &t,int x,int y,int s) {
	tile **l = getLocation(t.x,t.y,1);
	int i,n = t.terrain>TER_WATER? TER_LAND : TER_WATER;
	// Paint ground:
//	draw_image(images.terrain,x,y,terrain[n],ICON_ALIGN_CENTER);
	// Paint City or terrain: (forest, mountains, cities)
	if(t.city) {
		draw_image(images.building,x,y,city_dimension[t.city->sprite],ICON_ALIGN_CENTER);
		if(t.owner==t.city && t.city->player) paintFlag(x+8,y+TILE_CENTER_Y-2,t.city->player->alliance,2);
	}
	//else if(t.construction) draw_image(images.construction,x,y,terrain[t.terrain],ICON_ALIGN_CENTER);
	else draw_image(images.terrain,x,y,terrain[t.terrain],ICON_ALIGN_CENTER);
	// Paint resource:
	if((s&TILE_RESOURCE) && t.resource>0 && !t.city) draw_image(images.resource,x+(TILE_WIDTH-12)/2,y+(TILE_HEIGHT-12)/2,12,12,12*t.resource,0);
	// Paint fog of war border:
	if((s&TILE_FOG_OF_WAR) && showFogOfWar() && local.player)
		for(i=1; i<=6; i++)
			if(!l[i] || isFogOfWar(*local.player,l[i]->x,l[i]->y))
				draw_image(images.fow_edge,x,y-10,fow_edge[i],0,0);
	// Paint player borders:
	if((s&TILE_BORDER) && n==TER_LAND && t.owner && t.owner->player) {
		for(i=1; i<=6; i++)
			if(!l[i] || l[i]->terrain==TER_WATER || !l[i]->owner || l[i]->owner->player!=t.owner->player)
				draw_image(images.border,x,y,border[i],t.owner->player->alliance*TILE_WIDTH,0);
	}
	// Paint selection mark:
	if((s&TILE_MARKER) && t.x==focus_x && t.y==focus_y) draw_image(images.marker,x,y,TILE_WIDTH,TILE_HEIGHT,0,0);
	// Paint a flag for cities that are capitals:
	if(t.city && t.owner==t.city && t.city->player) paintFlag(x+8,y+TILE_CENTER_Y-2,t.city->player->alliance,2);
}

void Game::paintUnit(Unit &u,int x,int y,int s) {
	// Paint unit:
	draw_unit_sprite(x,y,u.type,ICON_ALIGN_CENTER);
	// Paint stats (moves, hp):
	if(s&1) {
		if(u.moves>0) draw_stat(x+TILE_CENTER_X-9,y+TILE_HEIGHT-8,STAT_MOVES,u.moves,unit_types[u.type].moves);
	}
	// Paint flag:
	//if(s&2) paintFlag(x+TILE_CENTER_X+10,y+TILE_HEIGHT-13,u.player->alliance,map.tiles[u.y*map.width+u.x].units);
}

void Game::paintFlag(int x,int y,int c,int n) {
	if(n<=1) return;
	draw_image(images.flag,x-3/*+18*/,y-22/*-4*/,flag[0]);
	const GdkRectangle *r;
	if(n>30) {
		if((n%25)>5) r = &flag[n/25],n %= 25;
		else r = &flag[n/25-1],n = (n%25)+25;
		draw_image(images.flag,x-1,y-10,r->width,r->height-1,r->x,r->y+1+c*8);
	}
	if(n>5) {
		if((n%5)>0) r = &flag[n/5],n %= 5;
		else r = &flag[n/5-1],n = 5;
		draw_image(images.flag,x-1,y-16,r->width,r->height-1,r->x,r->y+1+c*8);
	}
	r = &flag[n];
	draw_image(images.flag,x-1,y-23,r->width,r->height,r->x,r->y+c*8);
}

void Game::paintBox(int x,int y,int w,int h,bool fill) {
	if(fill) {
		set_color(0x734D26);
		draw_rectangle(x+4,y+4,w-8,h-8,true);
	}
	draw_image(images.frame_edge,x,y,4,4,0,0);
	draw_image(images.frame_edge,x+w-4,y,4,4,8,0);
	draw_image(images.frame_edge,x,y+h-4,4,4,0,8);
	draw_image(images.frame_edge,x+w-4,y+h-4,4,4,8,8);
	fill_pattern(images.frame_h,x+4,y,w-8,4,0,0,256,4);
	fill_pattern(images.frame_v,x,y+4,4,h-8,0,0,4,256);
	fill_pattern(images.frame_v,x+w-4,y+4,4,h-8,0,0,4,256);
	fill_pattern(images.frame_h,x+4,y+h-4,w-8,4,0,0,256,4);
}

void Game::paintHorizontalLine(int x,int y,int w) {
	fill_pattern(images.frame_h,x,y,w,4,0,0,256,4);
}

void Game::paintVerticalLine(int x,int y,int h) {
	fill_pattern(images.frame_v,x,y,4,h,0,0,4,256);
}

void Game::paintCityBox() {
	city_box.height = 0;
	city_box.prod_rows = 0;
	if(focus_x<0 || focus_x>=map.width || focus_y<0 || focus_y>=map.height) return;
	int i = focus_y*map.width+focus_x;
	City *c = map.tiles[i].city;
	if(!c) return;
	int j,x,y;
	uint64_t b = 0;
	bool islpl = map.tiles[i].owner && map.tiles[i].owner->player==local.player;
	const city_building *cb;
	tile *l[7];
	getLocation(c->x,c->y,1,l);
	// Calculate box bounds:
	if(islpl) {
		b = c->getCanBuildBuildings();
		for(j=0,x=0; j<BLD_BUILDINGS; j++) if(b&(1<<j)) {
			cb = &city_buildings[j];
			if(x+UNIT_ICON_WIDTH>city_box.x+city_box.prod_x+CITY_BOX_WIDTH) city_box.prod_rows++,x = 0;
			x += BUILDING_ICON_WIDTH+1;
		}
		for(j=0; j<UNIT_TYPES; j++) if(c->build.unit_types&(1<<j)) {
			if(x+UNIT_ICON_WIDTH>CITY_BOX_WIDTH) city_box.prod_rows++,x = 0;
			else x += UNIT_ICON_WIDTH+1;
		}
		if(x>0 || city_box.prod_rows) city_box.prod_rows++;
	}
	city_box.prod_x = 8+67;
	city_box.x = 5;
	city_box.y = 5;
	city_box.width = city_box.prod_x+CITY_BOX_WIDTH+6;
	city_box.height = 19+WORK_HEIGHT+RES*(RESOURCE_HEIGHT+1)+city_box.prod_rows*UNIT_ICON_HEIGHT+UNIT_SPRITE_HEIGHT;
	if(city_box.height<73+TILE_HEIGHT) city_box.height = 73+TILE_HEIGHT;
	// Paint box:
	paintBox(city_box.x,city_box.y,city_box.width,city_box.height);
	// Paint tile:
	paintTile(map.tiles[i],city_box.x+10+(56-TILE_WIDTH)/2,city_box.y+15,TILE_BOX);
	// Paint economy:
	city_box.res_x = 5;
	city_box.res_y = 17+TILE_HEIGHT;
	x = city_box.x+city_box.res_x;
	y = city_box.y+city_box.res_y+3;
	set_color(0x3a2613);
	draw_line(x,y-2,x+63,y-2);
	y = paintEconomy(x,y,63,CITY_BOX_WIDTH,c);
	draw_line(x,y+1,x+63,y+1);
	// Paint city build buttons:
	x = city_box.x+city_box.prod_x;
	y = city_box.y+5;
	paintVerticalLine(x-5,city_box.y+3,city_box.height-6);
	for(j=0; j<BLD_BUILDINGS; j++) if(c->buildings&(1<<j)) {
		if(x+BUILDING_ICON_WIDTH>city_box.x+city_box.prod_x+CITY_BOX_WIDTH) x = city_box.x+city_box.prod_x,y += BUILDING_ICON_HEIGHT+1;
		draw_building_icon(x,y,city_buildings[j],mouse_over.area==AREA_CB_BUILDING && mouse_over.value==j? ICON_ACTIVE : 0);
		x += BUILDING_ICON_WIDTH+1;
	}
	x = city_box.x+city_box.prod_x;
	y += BUILDING_ICON_HEIGHT;
	// Paint build:
	if(islpl) {
		draw_line(x,y+1,x+CITY_BOX_WIDTH,y+1);
		y += 3;
		city_box.prod_y = y-city_box.y;
		for(j=0; j<BLD_BUILDINGS; j++) if(b&(1<<j)) {
			if(x+UNIT_ICON_WIDTH>city_box.x+city_box.prod_x+CITY_BOX_WIDTH) x = city_box.x+city_box.prod_x,y += BUILDING_ICON_HEIGHT+1;
			draw_building_icon(x,y,city_buildings[j],c->canBuild(j)? (mouse_over.area==AREA_CB_BLDPROD && mouse_over.value==j? ICON_ACTIVE : 0) : ICON_DISABLED);
			x += BUILDING_ICON_WIDTH+1;
		}
		for(j=0; j<UNIT_TYPES; j++) if(c->build.unit_types&(1<<j)) {
			if(x+UNIT_ICON_WIDTH>city_box.x+city_box.prod_x+CITY_BOX_WIDTH) x = city_box.x+city_box.prod_x,y += UNIT_ICON_HEIGHT+1;
			draw_unit_icon(x,y,unit_types[j],c->canProduce(j)? (mouse_over.area==AREA_CB_UNITPROD && mouse_over.value==j? ICON_ACTIVE : 0) : ICON_DISABLED);
			x += UNIT_ICON_WIDTH+1;
		}
		x = city_box.x+city_box.prod_x;
		y += UNIT_ICON_HEIGHT;
		draw_line(x,y+1,x+CITY_BOX_WIDTH,y+1);
		y += 3;
		if(c->build.type>=0) {
			if(c->build.type&BLD_BUILD_BUILDING) {
				cb = &city_buildings[c->build.type&_BLD_BUILDINGS];
				draw_building_icon(x+1,y,*cb,0);
				x += BUILDING_ICON_WIDTH+3;
				draw_turns(x,y,cb->work-c->work);
			} else {
				x = draw_unit_sprite(x+1,y,c->build.type,ICON_ALIGN_BOTTOM)+2;
				draw_turns(x,y,unit_types[c->build.type].work-c->work);
			}
			if(c->build.queue>=0) {
debug_output("Game::paint(c->build.queue=%d)\n",c->build.queue);
				if(c->build.queue&BLD_BUILD_BUILDING)
					draw_building_icon(city_box.x+city_box.width-6-BUILDING_ICON_WIDTH,y,city_buildings[c->build.queue&_BLD_BUILDINGS],0);
				else {
					draw_unit_sprite(city_box.x+city_box.width-6-UNIT_SPRITE_WIDTH,y,c->build.queue,ICON_ALIGN_BOTTOM);
				}
			}
		}
	}
}

void Game::paintUnitBox() {
	unit_box.height = 0;
	if(focus_x<0 || focus_x>=map.width || focus_y<0 || focus_y>=map.height) return;
	tile &t = map.tiles[focus_y*map.width+focus_x];
	Unit *u = t.unit;
	if(!u) return;
	int i,x,y;
	int moves = -1,m_moves = 0,attack = 0,defence = 0,xp = 0;
	int terrain = 1<<t.terrain;
	Unit *worker = 0;
	const unit_type *ut;
	const tile_rectangle *ud;
	const terrain_action *ta;

	x = 0,unit_box.unit_rows = 1;
	ut = &unit_types[u->type];
	ud = &unit_dimension[ut->sprite];
	if(x+ud->width>unit_box.x+UNIT_BOX_WIDTH) unit_box.unit_rows++,x = 0;
	x += ud->width+1;
	if(u->status&U_WORK) worker = u;

//	bool islpl = map.tiles[i].player==local.player;
	unit_box.width = 15+UNIT_BOX_WIDTH+TILE_WIDTH;
	unit_box.height = 19+unit_box.unit_rows*UNIT_SPRITE_HEIGHT+WORK_HEIGHT*3;
	if(unit_box.height<21+TILE_HEIGHT) unit_box.height = 21+TILE_HEIGHT;
	unit_box.x = width-5-unit_box.width;
	unit_box.y = 5;
	// Paint box:
	paintBox(unit_box.x,unit_box.y,unit_box.width,unit_box.height);
	// Paint tile:
	x = unit_box.x+unit_box.width-6-TILE_WIDTH;
	y = unit_box.y+15;
	paintTile(t,x,y,TILE_BOX);
	paintUnit(*u,x,y,3);
	set_color(0x3a2613);
	draw_line(x-2,unit_box.y+5,x-2,unit_box.y+unit_box.height-6);
	// Paint units:
	unit_box.unit_x = 5;
	unit_box.unit_y = 5;

	x = unit_box.unit_x,y = unit_box.unit_y;
	if(x>=unit_box.unit_x+UNIT_BOX_WIDTH) x = unit_box.unit_x,y += UNIT_SPRITE_HEIGHT;
	draw_unit_sprite(unit_box.x+x+1,unit_box.y+y,u->type,ICON_ALIGN_BOTTOM|
			((mouse_over.area==AREA_UB_UNIT && mouse_over.object==(intptr_t)u)? ICON_ACTIVE : 0));

	moves = u->moves,m_moves = unit_types[u->type].moves;
	attack += u->attack();
	defence += u->defence();
	if(xp<u->xp) xp = u->xp;

	x = unit_box.x+5;
	y += unit_box.y+UNIT_SPRITE_HEIGHT+3;
	draw_line(x,y-2,x+UNIT_BOX_WIDTH,y-2);
	x = draw_moves(x,y,moves,m_moves)+3;
	x = draw_attack(x,y,attack)+3;
	x = draw_defence(x,y,defence)+3;
	x = draw_rank(x,y,xp);
	x = unit_box.x+5;
	y += WORK_HEIGHT+3;
	unit_box.action_x = x-unit_box.x;
	unit_box.action_y = y-unit_box.y;
	draw_line(x,y-2,x+UNIT_BOX_WIDTH,y-2);
	if(focus_unit && focus_unit->player==local.player) {
		ut = &unit_types[focus_unit->type];
		if(ut->actions&U_WORK)
			for(i=0; (ta=&terrain_actions[i])->action!=-1; i++)
				if((terrain&ta->terrain) && (ut->actions&ta->action) && (!ta->owned || (t.owner && t.owner->player==local.player)))
					x = draw_icon(x,y,ta->icon,worker? ICON_DISABLED : (mouse_over.area==AREA_UB_ACTION && mouse_over.value==ta->icon? ICON_ACTIVE : 0))+1;
	}
	x = unit_box.x+5;
	draw_icon(x+UNIT_BOX_WIDTH-WORK_WIDTH,y,ICON_CANCEL,mouse_over.area==AREA_UB_ACTION && mouse_over.value==ICON_CANCEL? ICON_ACTIVE : 0);
	y += WORK_HEIGHT+3;
	draw_line(x,y-2,x+UNIT_BOX_WIDTH,y-2);
	if(worker) {
		for(i=0; (ta=&terrain_actions[i])->action!=-1 && !(ta->action&worker->status); i++);
		if(ta->action!=-1) {
			x = draw_icon(x,y,ta->icon,0)+1;
			draw_turns(x,y,getWorkTurns(1,worker->work,ta->work));
		}
	}
}

void Game::paintToolTip() {
	if(mouse_over.tooltip<=0) return;
	// Paint box:
	int v = mouse_over.value,x = mouse.x,y = mouse.y+20,w = 0,h = 0;
	const char *name = 0;
	switch(mouse_over.tooltip) {
		case TOOLTIP_BUILDING_BUILD:
			w = 120;
			h = 16+WORK_HEIGHT+3+RESOURCE_HEIGHT+1;
			name = city_buildings[v].name;
		break;
		case TOOLTIP_UNIT:
			w = 120;
			h = 16+UNIT_SPRITE_HEIGHT+3;
			name = unit_types[v].name;
			break;
		case TOOLTIP_UNIT_BUILD:
			w = 120;
			h = 16+UNIT_SPRITE_HEIGHT+5+WORK_HEIGHT+3+RESOURCE_HEIGHT+1;
			name = unit_types[v].name;
			break;
	}
	if(w==0 || h==0) return;
	set_color(0x734D26);
	draw_rectangle(x+1,y+1,w-1,h-1,true);
	set_color(0x3a2613);
	draw_rectangle(x,y,w,h,false);
	if(name) {
		printText(x+3,y+2,name);
		y += 16;
		draw_line(x+2,y-2,x+w-3,y-2);
	} else y += 2;
	switch(mouse_over.tooltip) {
		case TOOLTIP_BUILDING_BUILD:
		{
			const city_building &cb = city_buildings[v];
			draw_turns(x+2,y,cb.work-getCity()->work);
			paintCost(x+2,y,73,113,w-5,cb.cost,cb.maintain);
			break;
		}
		case TOOLTIP_UNIT:
		case TOOLTIP_UNIT_BUILD:
		{
			const unit_type &t = unit_types[v];
			const tile_rectangle &r = unit_dimension[t.sprite];
			draw_unit_sprite(x+3+(UNIT_SPRITE_WIDTH-r.width)/2,y+(UNIT_SPRITE_HEIGHT+2-r.height)/2,v,0);
			draw_line(x+5+UNIT_SPRITE_WIDTH,y,x+5+UNIT_SPRITE_WIDTH,y+UNIT_SPRITE_HEIGHT+2);
			draw_moves(x+9+UNIT_SPRITE_WIDTH,y,t.moves);
			draw_attack(x+9+UNIT_SPRITE_WIDTH+40,y,t.terrain[TER_LAND].attack);
			draw_defence(x+9+UNIT_SPRITE_WIDTH+40,y+13,t.terrain[TER_LAND].defence);
			if(mouse_over.tooltip==TOOLTIP_UNIT_BUILD) {
				y += UNIT_SPRITE_HEIGHT+5;
				draw_line(x+2,y-2,x+w-3,y-2);
				draw_turns(x+2,y,t.work-getCity()->work);
				paintCost(x+2,y,73,113,w-5,t.cost,t.maintain);
			}
			break;
		}
	}
}

int Game::paintEconomy(int x,int y,int c1,int w,City *c) {
	if(!c) return y;
	draw_resource_icon(x+1,y,RES_GOLD,0);
	draw_number(x+c1,y+3,1,c->gold);
	y += RESOURCE_HEIGHT+1;
	draw_string(x,y,0,_("Income"));
	draw_number(x+c1,y,1,c->income,0,true);
	y += FONT_HEIGHT+1;
	draw_string(x,y,0,_("Wages"));
	draw_number(x+c1,y,1,-c->wages,0,true);
	return y += FONT_HEIGHT+1;
}

int Game::paintCost(int x,int y,int c1,int c2,int w,int cost,int maintain) {
	draw_resource_icon(x+1,y,RES_GOLD,0);
	draw_number(x+c1,y+3,1,cost);
	draw_number(x+c2,y+3,1,-maintain);
	return y += RESOURCE_HEIGHT+1;
}

void Game::paintMap() {
	static int colors[] = { 0x0000ff,0x00ff00,0x009900,0x999999,0x990000 };
	int c,x,y,w,h,n = 256/map.width,xn;
	if(n<3) n = 3;
	w = n*map.width;
	h = n*(map.height-1)/4;
	if(!mapimage.image || mapimage.width!=w || mapimage.height!=h) {
		if(mapimage.image) g_object_unref(mapimage.image);
		mapimage.image = gdk_pixmap_new(gui.maparea->window,w,h,-1);
	}
	mapimage.width = w;
	mapimage.height = h;

	set_canvas(mapimage.image,gdk_gc_new(mapimage.image));
	set_color(0x000000);
	draw_rectangle(0,0,mapimage.width,mapimage.height,true);

	for(c=0; c<5; c++) {
		set_color(colors[c]);
		for(y=0; y<map.height; y++) {
			xn = (y&1)*n/2;
			for(x=0; x<map.width; x++)
				if(map.tiles[y*map.width+x].terrain==c &&
						(!showFogOfWar() || !local.player || !isFogOfWar(*local.player,x,y)))
					draw_rectangle(n*x+xn,n*y/4-n/4,n/2+1,n/2+1,true);
		}
	}
	g_object_unref(paint_gc);
	set_canvas(maparea_buffer,maparea_gc);
	updateDisplay();
}

void Game::paintMessageBox() {
	if(!message_box.text) return;
	paintBox(message_box.x,message_box.y,message_box.width,message_box.height);
	set_color(0x3a2613);
	printText(message_box.x+16,message_box.y+16,"%s",message_box.text);

	if((--message_box.timer)==0) {
		free(message_box.text);
		message_box.text = 0;
		message_box.timer = 0;
	}
}
