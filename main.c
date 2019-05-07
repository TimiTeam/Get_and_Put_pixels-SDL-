#include "SDL.h"
#include <stdio.h>

int			win_x = 980;
int			win_y = 680;
SDL_Window		*win = NULL;
SDL_Surface		*surf = NULL;
SDL_Surface		*win_surf = NULL;
SDL_Renderer		*ren = NULL;
SDL_Texture		*tex = NULL;

void 			close_all()
{
	if (win)
		SDL_DestroyWindow(win);
	if (surf)
		SDL_FreeSurface(surf);
	if (win_surf)
		SDL_FreeSurface(win_surf);
//	if (tex)
//		SDL_DestroyTexture(tex);
	printf("END\n");
}

int			init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return (1);
	win = SDL_CreateWindow("pixel access", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, win_x, win_y, SDL_WINDOW_SHOWN);
	if (!win)
		return (1);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED
			| SDL_RENDERER_PRESENTVSYNC);
	if (!ren)
		return (1);
	win_surf = SDL_CreateRGBSurface(0, win_x, win_y, 32, 0, 0, 0, 255);
	if (!win_surf)
		return (1);
	return (0);
}

SDL_Surface		*load_media(char *path)
{
	SDL_Surface	*opt;
	SDL_Surface	*convert;
	SDL_Surface	*surf;
	SDL_Rect	r;

	surf = SDL_LoadBMP(path);
	opt = NULL;
	if (surf)
	{
		printf("size surf befor convertin x = %d, y = %d\n", surf->w, surf->h);
		convert = SDL_ConvertSurface(surf, win_surf->format, 0);
		if (!convert)
			return (NULL);
		SDL_FreeSurface(surf);
		opt = SDL_CreateRGBSurface(0, win_x, win_y, 32, 0, 0, 0, 255);
		r.x = 0;
		r.y = 0;
		r.w = win_x;
		r.h = win_y;
		SDL_BlitScaled(convert, NULL, opt, &r);
		SDL_FreeSurface(convert);
	}
	return (opt);
}

Uint32			get_pixel(SDL_Surface *src, int x, int y)
{
	int		bpp;
	Uint8		*p;

	bpp = src->format->BytesPerPixel;
	p = (Uint8*)src->pixels + y * src->pitch + x * bpp;
	
	switch (bpp)
	{
		case 1:
			return (*p);
		case 2:
			return (*(Uint16*)p);
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return (p[0] << 16 | p[1] << 8 | p[2]);
			else
				return (p[0] | p[1] << 8 | p[2] << 16);
		case 4:
			return (*(Uint32*)p);
		default:
			return (0);
	}
}


void			set_pixel(SDL_Surface *dst, int x, int y, Uint32 pixel)
{
	int		bpp;
	Uint8		*p;

	bpp = dst->format->BytesPerPixel;
	p = (Uint8*)dst->pixels + y * dst->pitch + x * bpp;
	switch (bpp)
	{
		case 1:
			*p = pixel;
			break ;
		case 2:
			*(Uint16*)p = pixel;
			break ;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel && 0xff;
			}
			else
			{
				p[0] = pixel && 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break ;
		case 4:
			*(Uint32*)p = pixel;
			break ;
	}
}

int			fill_win_surf(SDL_Surface *dst, SDL_Surface *src)
{
	int		x;
	int		y;
	Uint32		pixel;

	if (!dst || !src)
		return (1);
	x = 0;
	while (x < dst->w && x < src->w)
	{
		y = 0;
		while(y < dst->h && y < src->h)
		{
			pixel = get_pixel(src, x, y);
			set_pixel(dst, x, y, pixel);
			y++;
		}
		x++;
	}
	return (0);
}

int			main(int ac, char **av)
{
	SDL_Event	e;
	int		run;

	if (ac < 2)
	{
		printf("Need enter file name\n");
		exit(1);
	}
	if (init())
	{
		printf("FATAL: %s\n", SDL_GetError());
		close_all();
		SDL_Quit();
		exit(1);
	}
	if(!(surf = load_media(av[1])))
	{
		printf("FATAL: %s\n", SDL_GetError());
		close_all();
		SDL_Quit();
		exit(1);
	}
	printf("size surf x = %d, y = %d\n", surf->w, surf->h);
	run = 1;
	fill_win_surf(win_surf, surf);
	printf("size win_surf x = %d, y = %d\n", win_surf->w, win_surf->h);
	tex = SDL_CreateTextureFromSurface(ren, win_surf);
	while (run)
	{
		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);
		SDL_RenderClear(ren);
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				run = 0;
		}
//		win_surf = SDL_CreateRGBSurface(0, win_x, win_y, 32, 0, 0, 0, 255);
	}
	close_all();
	SDL_Quit();
	return (0);
}
