// SDL.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "SDL.h"
#include "SDL_ttf.h"


// FONT_SIZE定义的字体大小为带有margin的大小，所以在绘制的时候无需留空。字模大小为FONT_SIZE - 1
#define FONT_SIZE	10


static Uint16 glyphBuf[FONT_SIZE];


static std::map<Uint16, std::pair<int, SDL_Surface*>>	s_vImgs;
static std::list<std::pair<std::string, std::list<Uint16>>>	s_vStrings;


static char absolutePath[0x100] = {0};
static void SetAbsolutePath(_TCHAR* path){
	for(int i=0;i<wcslen(path);++i){
		absolutePath[i] = (char)path[i];
	}
	for(int i=0;i<strlen(absolutePath);++i){
		if(absolutePath[i] == '\\')
			absolutePath[i] = '/';
	}
	//for(int i=strlen(absolutePath)-1;i>=0;--i){
	//	if(absolutePath[i] == '/'){
	//		absolutePath[i] = 0;
	//		break;
	//	}
	//}
}
static const char* GetFullPath(const char* name){
	static char buf[0x1000]; 
	strcpy(buf, absolutePath);
	strcat(buf, "/");
	strcat(buf, name);
	return buf;
}

static bool IsDuplicated(Uint16 ascii){
	return s_vImgs.find(ascii) != s_vImgs.end();
}

static void SearchIndividualImages(){
	for(int hi=0x00;hi<=0xFF;++hi){
		for(int lo=0x00;lo<=0xFF;++lo){
			Uint16 key = (hi << 8) | lo;
			// 绝对不允许出现重复！
			assert(!IsDuplicated(key));

			char fileName[0x10];
			sprintf(fileName, "%02X%02X.bmp", hi, lo);
			FILE* f = fopen(GetFullPath(fileName), "r");
			if(f){
				SDL_Surface* temp = SDL_LoadBMP(GetFullPath(fileName));	// load bitmap to temp surface
				SDL_Surface* img = SDL_DisplayFormat(temp);	// convert bitmap to display format
				SDL_FreeSurface(temp);						// free the temp surface
				std::pair<Uint16, std::pair<int, SDL_Surface*>> item(key, std::pair<int, SDL_Surface*>(0, img));
				s_vImgs.insert(s_vImgs.end(), item);
				fclose(f);
			}
		}
	}
}

static char* localeToUTF8(char* src){
	static char* buf = NULL;
	if(buf){
		free(buf);
		buf = NULL;
	}
	wchar_t* unicode_buf;
	int nRetLen = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
	unicode_buf = (wchar_t*)malloc((nRetLen+1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, src, -1, unicode_buf, nRetLen);
	nRetLen = WideCharToMultiByte(CP_UTF8, 0, unicode_buf, -1, NULL, 0, NULL, NULL);
	buf = (char*)malloc(nRetLen + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode_buf, -1, buf, nRetLen, NULL, NULL);
	free(unicode_buf);
	return buf;
}

static void AddAllASCIICharImages(TTF_Font* font, SDL_Color& textColor){
	char chr[2];
	for(int i=0x20;i<=0x7F;++i){
		// 绝对不允许出现重复！
		assert(!IsDuplicated(i));

		memset(chr, 0, sizeof(chr));
		chr[0] = (char)i;
		SDL_Surface* img = TTF_RenderUTF8_Solid(font, chr, textColor);
		memmove(img->pixels, (const void*)((Uint8*)(img->pixels) + img->pitch), img->pitch * (img->h - 1));
		std::pair<Uint16, std::pair<int, SDL_Surface*>> item(i, std::pair<int, SDL_Surface*>(0, img));
		s_vImgs.insert(s_vImgs.end(), item);
	}
}

static void AddAllGBKCharImages(TTF_Font* font, SDL_Color& textColor){
	char chr[3];
	for(int hi=0x81;hi<=0xFE;++hi){
		for(int lo=0x40;lo<=0xFE;++lo){
			Uint16 key = (hi << 8) | lo;
			// 绝对不允许出现重复！
			assert(!IsDuplicated(key));

			memset(chr, 0, sizeof(chr));
			chr[0] = hi;
			chr[1] = lo;
			SDL_Surface* img = TTF_RenderUTF8_Solid(font, localeToUTF8(chr), textColor);
			memmove(img->pixels, (const void*)((Uint8*)(img->pixels) + img->pitch), img->pitch * (img->h - 1));
			std::pair<Uint16, std::pair<int, SDL_Surface*>> item(key, std::pair<int, SDL_Surface*>(0, img));
			s_vImgs.insert(s_vImgs.end(), item);
		}
	}
}

static void AddGBKCharImages(TTF_Font* font, SDL_Color& textColor){
	FILE* f = fopen(GetFullPath("text.txt"), "rb");
	if(f){
		fseek(f, 0, SEEK_END);
		int fileSize = ftell(f);
		rewind(f);
		unsigned char* buf = new unsigned char[fileSize];
		memset(buf, 0, fileSize);
		fread(buf, fileSize, 1, f);
		fclose(f);

		std::map<Uint16, int>	m_vChars;
		int p = 0;
		while(p < fileSize){
			if(buf[p] <= 0x7F){
				p++;
			}else{
				m_vChars.insert(m_vChars.begin(), std::pair<Uint16, int>((buf[p] << 8) | buf[p + 1], 0));
				p += 2;
			}
		}
		delete[] buf;

		char chr[3];
		for(std::map<Uint16, int>::iterator iter = m_vChars.begin();iter != m_vChars.end();++iter){
			memset(chr, 0, sizeof(chr));
			chr[0] = (*iter).first >> 8;
			chr[1] = (*iter).first & 0xFF;
			SDL_Surface* img = TTF_RenderUTF8_Solid(font, localeToUTF8(chr), textColor);
			memmove(img->pixels, (const void*)((Uint8*)(img->pixels) + img->pitch), img->pitch * (img->h - 1));
			std::pair<Uint16, std::pair<int, SDL_Surface*>> item((*iter).first, std::pair<int, SDL_Surface*>(0, img));
			s_vImgs.insert(s_vImgs.end(), item);
		}
	}
}

static void ResetIdxOfCharImages(){
	int idx = 0;
	for(std::map<Uint16, std::pair<int, SDL_Surface*>>::iterator iter = s_vImgs.begin();iter != s_vImgs.end();++iter){
		(*iter).second.first = idx;
		idx++;
	}
}

static void GetGlyphFromSDLSurface(const SDL_Surface* text, int width, Uint16* buf, bool isVertical = true){
	memset(buf, 0, sizeof(Uint16) * width);
	if(isVertical){
		for(int x=0;x<text->w;++x){
			for(int y=0;y<text->h;++y){
				switch(text->format->BitsPerPixel){
				case 8:{
					Uint8 p = *((Uint8*)text->pixels + x + y * text->pitch / text->format->BytesPerPixel);
					if(p)
						buf[x] |= (1 << y);
					   }break;
				case 32:{
					Uint32 p = *((Uint32*)text->pixels + x + y * text->pitch / text->format->BytesPerPixel);
					if(!(p & 0xFFFFFF))
						buf[x] |= (1 << y);
						}break;
				default:{
					assert(false);
						}break;
				}
			}
		}
	}else{
		for(int y=0;y<text->h;++y){
			for(int x=0;x<text->w;++x){
				switch(text->format->BitsPerPixel){
				case 8:{
					Uint8 p = *((Uint8*)text->pixels + x + y * text->pitch / text->format->BytesPerPixel);
					if(p)
						buf[y] |= (1 << x);
					   }break;
				case 32:{
					Uint32 p = *((Uint32*)text->pixels + x + y * text->pitch / text->format->BytesPerPixel);
					if(!(p & 0xFFFFFF))
						buf[y] |= (1 << x);
						}break;
				default:{
					assert(false);
						}break;
				}
			}
			// 注意，IST3932需要交换高低字节
			Uint8 lo = buf[y] & 0xFF;
			Uint8 hi = (buf[y] >> 8) & 0xFF;
			buf[y] = ((Uint16)lo << 8) | (Uint16)hi; 
		}
	}
}

static int hex2dec(char a){
	if(a >= '0' && a <= '9')
		return a - '0';
	else if(a >= 'a' && a <= 'f')
		return a - 'a' + 0xA;
	else if(a >= 'A' && a <= 'F')
		return a = 'A' + 0xA;
	else
		return -1;
}

static void GenerateStrings(){
	FILE* f = fopen(GetFullPath("text.txt"), "rb");
	if(f){
		std::string m_key;
		std::list<int> m_Ascii;
		m_Ascii.clear();

		bool isKey = true;
		bool isExit = false;
		while(!isExit){
			unsigned char c[8];
			memset(c, 0, sizeof(c));
			fread(&c[0], sizeof(char), 1, f);
			if(c[0] >= 0x80){
				fread(&c[1], sizeof(char), 1, f);
			}
			if(c[0] == '\\'){
				fread(&c[1], sizeof(char), 1, f);
				fread(&c[2], sizeof(char), 1, f);
				fread(&c[3], sizeof(char), 1, f);
				fread(&c[4], sizeof(char), 1, f);
				fread(&c[5], sizeof(char), 1, f);
				fread(&c[6], sizeof(char), 1, f);
				fread(&c[7], sizeof(char), 1, f);
			}
			switch(c[0]){
			case 0:{
				isExit = true;
				   }break;
			case '\n':{
					  }break;
			case '\r':{
				std::list<Uint16> stringidx;
				for(std::list<int>::iterator iter = m_Ascii.begin();iter != m_Ascii.end();++iter){
					assert(s_vImgs.find(*iter) != s_vImgs.end());
					int idx = s_vImgs[*iter].first;
					stringidx.push_back(idx);
				}
				if(stringidx.size() > 0)
					s_vStrings.push_back(std::make_pair(m_key, stringidx));
				m_key = "";
				m_Ascii.clear();
				isKey = true;
					  }break;
			case '\\':{
				assert((c[1] == 'X' || c[1] == 'x') && (c[4] == '\\') && (c[5] == 'X' || c[5] == 'x'));
				int _a = hex2dec(c[2]);
				assert(_a >= 0);
				int _b = hex2dec(c[3]);
				assert(_b >= 0);
				int _c = hex2dec(c[6]);
				assert(_c >= 0);
				int _d = hex2dec(c[7]);
				assert(_d >= 0);
				m_Ascii.push_back((_a << 12) | (_b << 8) | (_c << 4) | (_d << 0));
					  }break;
			case '\t':{
				isKey = false;
					  }break;
			default:{
				if(isKey){
					m_key += c[0];
				}else{
					if(c[0] < 0x80){
						m_Ascii.push_back(c[0]);
					}else{
						m_Ascii.push_back(((int)c[0] << 8) | c[1]);
					}
				}
					}break;
			}
		}

		fclose(f);
		f = NULL;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetAbsolutePath(argv[1]);
	printf(GetFullPath("hello.bmp"));

	/* initialize SDL */
	SDL_Init(SDL_INIT_VIDEO);
	/* set the title bar */
	SDL_WM_SetCaption("SDL Test", "SDL Test");
	/* create window */
	SDL_Surface* screen = SDL_SetVideoMode(640, 480, 0, 0);
	/* load bitmap to temp surface */
	SDL_Surface* temp = SDL_LoadBMP(GetFullPath("hello.bmp"));
	/* convert bitmap to display format */
	SDL_Surface* bg = SDL_DisplayFormat(temp);
	/* free the temp surface */
	SDL_FreeSurface(temp);

	TTF_Init();
	//TTF_Font* font = TTF_OpenFont("ZpixEX2_EX.ttf", FONT_SIZE);
	TTF_Font* font = TTF_OpenFont(GetFullPath("SIMSUN.ttc"), FONT_SIZE);
	assert(font);
	SDL_Color textColor = {0, 0, 255, 0};

	SearchIndividualImages();
	AddAllASCIICharImages(font, textColor);
	AddGBKCharImages(font, textColor);
	ResetIdxOfCharImages();

	GenerateStrings();

	FILE* f = fopen(GetFullPath("FontText.txt"), "w");

	fprintf(f, "#ifndef FONT_TEXT_H_\n#define FONT_TEXT_H_\n\n\n");
	fprintf(f, "#include \"typedef.h\"\n\n\n");

	int idx = 0;
	for(std::list<std::pair<std::string, std::list<Uint16>>>::iterator iter = s_vStrings.begin();iter != s_vStrings.end();++iter){
		fprintf(f, "const u16 code string%02X[] = {\n", idx);
		fprintf(f, "\t");
		std::list<Uint16> v = (*iter).second;
		for(std::list<Uint16>::iterator iter2 = v.begin();iter2 != v.end();++iter2){
			fprintf(f, "%d, ", *iter2);
		}
		fprintf(f, "0xFFFF");
		fprintf(f, "\n");
		fprintf(f, "};\n");
		idx++;
	}
	fprintf(f, "\n");

	fprintf(f, "const u16* code strings[] = {\n");
	fprintf(f, "\t");
	for(int i=0;i<s_vStrings.size();++i){
		fprintf(f, "string%02X, ", i);
	}
	fprintf(f, "\n");
	fprintf(f, "};\n");
	fprintf(f, "\n");

	fprintf(f, "#define CHARACTER_COUNT %d\n", s_vImgs.size() - 0x60);

	fprintf(f, "const u16 code fontIdx[CHARACTER_COUNT] = {\n");
	fprintf(f, "\t");
	for(std::map<Uint16, std::pair<int, SDL_Surface*>>::iterator iter = s_vImgs.begin();iter != s_vImgs.end();++iter){
		Uint16 ascii = (*iter).first;
		if(ascii < 0x80)
			continue;
		fprintf(f, "%d, ", (*iter).first);
	}
	fprintf(f, "\n");
	fprintf(f, "};\n");
	fprintf(f, "\n");

	fprintf(f, "#if (defined(LCM_DRIVER_UC1701X) || defined(LCM_DRIVER_SSD1306))\n");
	fprintf(f, "const u8 code fontText[(0x60 + CHARACTER_COUNT) * sizeof(u16) * %d] = {\n", FONT_SIZE);
	for(std::map<Uint16, std::pair<int, SDL_Surface*>>::iterator iter = s_vImgs.begin();iter != s_vImgs.end();++iter){
		Uint16 key = (*iter).first;
		SDL_Surface* img = (*iter).second.second;

		GetGlyphFromSDLSurface(img, FONT_SIZE, glyphBuf);
		//fwrite(glyphBuf, sizeof(Uint16), FONT_SIZE, f);
		fprintf(f, "\t");
		for(int j=0;j<FONT_SIZE;++j){
			fprintf(f, "0x%02X, ", (glyphBuf[j] >> 0) & 0xFF);	// 再写低字节
			fprintf(f, "0x%02X, ", (glyphBuf[j] >> 8) & 0xFF);	// 先写高字节
		}
		//char str[0x4];
		//memset(str, 0, sizeof(str));
		//if(key <= 0x7F){
		//	str[0] = key;
		//}else{
		//	str[0] = key >> 8;
		//	str[1] = key & 0xFF;
		//}
		//fprintf(f, "// 0x%04X %s\n", key, str);
		fprintf(f, "\n");
	}
	fprintf(f, "};\n");
	fprintf(f, "#endif\n");
	fprintf(f, "#if (defined(LCM_DRIVER_IST3932))\n");
	fprintf(f, "const u8 code fontText[(0x60 + CHARACTER_COUNT) * sizeof(u16) * %d] = {\n", FONT_SIZE);
	for(std::map<Uint16, std::pair<int, SDL_Surface*>>::iterator iter = s_vImgs.begin();iter != s_vImgs.end();++iter){
		Uint16 key = (*iter).first;
		SDL_Surface* img = (*iter).second.second;

		GetGlyphFromSDLSurface(img, FONT_SIZE, glyphBuf, false);
		//fwrite(glyphBuf, sizeof(Uint16), FONT_SIZE, f);
		fprintf(f, "\t");
		for(int j=0;j<FONT_SIZE;++j){
			fprintf(f, "0x%02X, ", (glyphBuf[j] >> 0) & 0xFF);	// 再写低字节
			fprintf(f, "0x%02X, ", (glyphBuf[j] >> 8) & 0xFF);	// 先写高字节
		}
		//char str[0x4];
		//memset(str, 0, sizeof(str));
		//if(key <= 0x7F){
		//	str[0] = key;
		//}else{
		//	str[0] = key >> 8;
		//	str[1] = key & 0xFF;
		//}
		//fprintf(f, "// 0x%04X %s\n", key, str);
		fprintf(f, "\n");
	}
	fprintf(f, "};\n");
	fprintf(f, "#endif\n\n\n");

	fprintf(f, "#endif	/* FONT_TEXT_H_ */\n");

	fclose(f);

	f = fopen(GetFullPath("enum.txt"), "w");

	fprintf(f, "typedef enum{\n");
	for(std::list<std::pair<std::string, std::list<Uint16>>>::iterator iter = s_vStrings.begin();iter != s_vStrings.end();++iter){
		std::string key = (*iter).first;
		fprintf(f, "\t");
		fprintf(f, "%s,\n", key.c_str());
	}
	fprintf(f, "}enumStrings;\n");
	fprintf(f, "\n");

	fclose(f);

	SDL_Event event;
	int gameover = 0;
	/* message pump */
	while (!gameover)
	{
		/* look for an event */
		if (SDL_PollEvent(&event)) {
			/* an event was found */
			switch (event.type) {
				/* close button clicked */
			case SDL_QUIT:
				gameover = 1;
				break;
				/* handle the keyboard */
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					gameover = 1;
					break;
				}
				break;
			}
		}
		/* draw the background */
		SDL_BlitSurface(bg, NULL, screen, NULL);
		//for(std::vector<FontInfo>::iterator iter = texts.begin();iter != texts.end();++iter){
		//	uint32_t ID = (*iter).ID;
		//	SDL_Surface* text = (*iter).text;
		//	SDL_Rect rect = {((ID & 0xFF) - 0x40) * FONT_SIZE, (((ID >> 8) & 0xFF) - 0x81) * FONT_SIZE, FONT_SIZE, FONT_SIZE};
		//	SDL_BlitSurface(text, NULL, screen, &rect);
		//}
		int x = 0, y = 0;
		for(std::map<Uint16, std::pair<int, SDL_Surface*>>::iterator iter = s_vImgs.begin();iter != s_vImgs.end();++iter){
			Uint16 key = (*iter).first;
			SDL_Surface* img = (*iter).second.second;
			if(key <= 0x7F){
				SDL_Rect rect = {x, y, FONT_SIZE >> 1, FONT_SIZE};
				SDL_BlitSurface(img, NULL, screen, &rect);
				x += (FONT_SIZE >> 1);
			}else{
				SDL_Rect rect = {x, y, FONT_SIZE, FONT_SIZE};
				SDL_BlitSurface(img, NULL, screen, &rect);
				x += FONT_SIZE;
			}
			if(x >= 0x60 * FONT_SIZE >> 1){
				x = 0;
				y += FONT_SIZE;
			}
		}
		/* update the screen */
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
	/* free the background surface */
	SDL_FreeSurface(bg);
	/* cleanup SDL */
	SDL_Quit();
	return 0;
}

