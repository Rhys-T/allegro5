/*         ______   ___    ___ 
 *        /\  _  \ /\_ \  /\_ \ 
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___ 
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      DirectDraw gfx drivers header.
 *
 *      By Stefan Schimanski.
 *
 *      See readme.txt for copyright information.
 */


#ifndef WDDRAW_H_INCLUDED
#define WDDRAW_H_INCLUDED

#define DIRECTDRAW_VERSION 0x0300

#include "allegro.h"
#include "allegro/internal/aintern.h"
#include "allegro/platform/aintwin.h"

#ifndef SCAN_DEPEND
   #include <ddraw.h>
#endif

#ifndef ALLEGRO_WINDOWS
#error something is wrong with the makefile
#endif


/* DirectDraw globals (from wddraw.c) */
AL_VAR(LPDIRECTDRAW2, directdraw);
AL_VAR(LPDIRECTDRAWSURFACE2, dd_prim_surface);
AL_VAR(LPDIRECTDRAWCLIPPER, dd_clipper);
AL_VAR(LPDIRECTDRAWPALETTE, dd_palette);
AL_VAR(DDCAPS, dd_caps);
AL_VAR(LPDDPIXELFORMAT, dd_pixelformat);
AL_VAR(BITMAP *, dd_frontbuffer);
AL_VAR(char *, pseudo_surf_mem);


/* driver routines */
AL_FUNC(BITMAP *, gfx_directx_init, (GFX_DRIVER *drv, int w, int h, int v_w, int v_h, int color_depth));
AL_FUNC(void, gfx_directx_exit, (BITMAP *bmp));
AL_FUNC(void, gfx_directx_sync, (void));
AL_FUNC(void, gfx_directx_set_palette, (AL_CONST RGB *p, int from, int to, int vsync));
AL_FUNC(int, gfx_directx_poll_scroll, (void));
AL_FUNC(void, gfx_directx_created_sub_bitmap, (BITMAP *bmp, BITMAP *parent));
AL_FUNC(BITMAP *, gfx_directx_create_video_bitmap, (int width, int height));
AL_FUNC(void, gfx_directx_destroy_video_bitmap, (BITMAP *bmp));
AL_FUNC(int, gfx_directx_show_video_bitmap, (BITMAP *bmp));
AL_FUNC(int, gfx_directx_request_video_bitmap, (BITMAP *bmp));
AL_FUNC(BITMAP *, gfx_directx_create_system_bitmap, (int width, int height));
AL_FUNC(void, gfx_directx_destroy_system_bitmap, (BITMAP *bmp));
AL_FUNC(void, gfx_directx_destroy_surf, (LPDIRECTDRAWSURFACE2 surf));
AL_FUNC(GFX_MODE_LIST *, gfx_directx_fetch_mode_list, (void));


/* driver initialisation and shutdown (from wddraw.c) */
AL_FUNC(int, init_directx, (void));
AL_FUNC(int, create_primary, (void));
AL_FUNC(int, create_clipper, (HWND hwnd));
AL_FUNC(int, create_palette, (LPDIRECTDRAWSURFACE2 surf));
AL_FUNC(int, setup_driver, (GFX_DRIVER * drv, int w, int h, int color_depth));
AL_FUNC(int, finalize_directx_init, (void));
AL_FUNC(int, exit_directx, (void));


/* driver initialisation and shutdown (from wddaccel.c) */
AL_FUNC(void, enable_acceleration, (GFX_DRIVER *drv));
AL_FUNC(void, enable_triple_buffering, (GFX_DRIVER *drv));


/* video mode setting (from wddmode.c) */
AL_VAR(int, desktop_depth);
AL_VAR(RGB_MAP, desktop_rgb_map);

AL_FUNC(void, build_desktop_rgb_map, (void));
AL_FUNC(int, gfx_directx_compare_color_depth, (int color_depth));
AL_FUNC(int, gfx_directx_update_color_format, (LPDIRECTDRAWSURFACE2 surf, int color_depth));
AL_FUNC(int, set_video_mode, (int w, int h, int v_w, int v_h, int color_depth));


/* bitmap definitions */
typedef struct BMP_EXTRA_INFO {
   LPDIRECTDRAWSURFACE2 surf;
   struct BMP_EXTRA_INFO *next;
   struct BMP_EXTRA_INFO *prev;
   int flags;
   int lock_nesting;
} BMP_EXTRA_INFO;

#define BMP_EXTRA(bmp) ((BMP_EXTRA_INFO *)(bmp->extra))
#define BMP_FLAG_LOST  1


/* bitmap locking (from wddlock.c and asmlock.s) */
AL_FUNC(void, gfx_directx_lock, (BITMAP *bmp));
AL_FUNC(void, gfx_directx_autolock, (BITMAP* bmp));
AL_FUNC(void, gfx_directx_unlock, (BITMAP *bmp));
AL_FUNC(void, gfx_directx_unlock_win, (BITMAP *bmp));
AL_FUNC(void, gfx_directx_release_lock, (BITMAP * bmp));
AL_FUNC(void, gfx_directx_write_bank, (void));
AL_FUNC(void, gfx_directx_unwrite_bank, (void));
AL_FUNC(void, gfx_directx_write_bank_win, (void));
AL_FUNC(void, gfx_directx_unwrite_bank_win, (void));
AL_FUNCPTR(void, ptr_gfx_directx_autolock, (BITMAP* bmp));
AL_FUNCPTR(void, ptr_gfx_directx_unlock, (BITMAP* bmp));


/* bitmap creation (from wddbmp.c) */
#define SURF_PRIMARY   1
#define SURF_OVERLAY   2
#define SURF_SYSTEM    3
#define SURF_VIDEO     4

AL_FUNC(LPDIRECTDRAWSURFACE2, gfx_directx_create_surface, (int w, int h, LPDDPIXELFORMAT pixel_format, int type));
AL_FUNC(BITMAP *, make_directx_bitmap, (LPDIRECTDRAWSURFACE2 surf, int w, int h, int id));
AL_FUNC(void, destroy_directx_bitmap, (BITMAP *bmp));


/* video bitmap list (from wddbmpl.c) */
AL_VAR(BMP_EXTRA_INFO *, directx_bmp_list);

AL_FUNC(void, register_directx_bitmap, (BITMAP *bmp));
AL_FUNC(void, unregister_directx_bitmap, (BITMAP *bmp));
AL_FUNC(void, unregister_all_directx_bitmaps, (void));


/* windowed mode (from wddwin.c) */
AL_VAR(BITMAP*, pseudo_screen);


#endif

