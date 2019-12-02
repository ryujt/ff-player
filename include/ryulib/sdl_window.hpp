#ifndef RYU_SDL_WINDOW_HPP
#define RYU_SDL_WINDOW_HPP

#include <string>
#include <SDL2/SDL.h>

#pragma comment(lib, "sdl2maind.lib")

using namespace std;

class WindowSDL {
public:
	/** 영상을 출력하기 위한 윈도우를 생성(오픈)합니다.
	@param caption 윈도우의 제목
	@param width 윈도우의 넓이 (가로 크기)
	@param height 윈도우의 높이 (세로 크기)
	*/
	bool open(string caption, int width, int height)
	{
		width_ = width;
		height_ = height;

		SDL_Init(SDL_INIT_EVERYTHING);
		
		if (target_handle_ != nullptr) {
			window_ = SDL_CreateWindowFrom(target_handle_);
		} else {
			window_ = SDL_CreateWindow(
				caption.c_str(),
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				width, height,
				SDL_WINDOW_RESIZABLE
			);
		}
		renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

		is_opened_ = true;
		return true;
	}

	/** 윈도우를 닫습니다. */
	void close()
	{
		if (is_opened_) {
			is_opened_ = false;
			SDL_DestroyWindow(window_);
			SDL_Quit();
		}
	}

	/** 32비트 BITMAP을 윈도우에 표시합니다.
	@param bitmap 표시할 BITMAP 데이터
	*/
	void showBitmap32(void* bitmap)
	{
		surface_ = SDL_CreateRGBSurfaceFrom(bitmap, width_, height_, 32, width_*4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		texture_ = SDL_CreateTextureFromSurface(renderer_, surface_);
		try {
			SDL_RenderCopy(renderer_, texture_, NULL, NULL);
			SDL_RenderPresent(renderer_);
		} catch (...) {
			//
		}
		SDL_FreeSurface(surface_);
		SDL_DestroyTexture(texture_);
	}

	/** YUV 포멧 이미지를 윈도우에 표시합니다. */
	void showYUV(const Uint8* y_plane, int y_pitch, const Uint8* u_plane, int u_pitch, const Uint8* v_plane, int v_pitch)
	{
		SDL_Texture* texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width_, height_);
		SDL_UpdateYUVTexture(texture_, NULL, y_plane, y_pitch, u_plane, u_pitch, v_plane, v_pitch);
		try {
			SDL_RenderCopy(renderer_, texture_, NULL, NULL);
			SDL_RenderPresent(renderer_);
		} catch (...) {
			//
		}
		SDL_DestroyTexture(texture_);
	}

	void setTargetHandle(void* handle) { target_handle_ = handle; }

private:
	bool is_opened_ = false;

	int width_;
	int height_;

	void* target_handle_ = nullptr;

	SDL_Window* window_;
	SDL_Renderer* renderer_;
	SDL_Surface* surface_;
	SDL_Texture* texture_;
};

#endif  // RYU_SDL_WINDOW_HPP