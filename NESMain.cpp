#include <SDL2/SDL.h>
#include <windows.h>
#include <cassert>
#include <chrono>

#include "BUS.h"
#include "RP6502.h"

float scale = 1;
uint16_t offset_x;
const unsigned palette[64] = {
    0x747474, 0x24188C, 0x0000A8, 0x44009C, 0x8C0074, 0xA80010, 0xA40000, 0x7C0800,
    0x402C00, 0x004400, 0x005000, 0x003C14, 0x183C5C, 0x000000, 0x000000, 0x000000,
    0xBCBCBC, 0x0070EC, 0x2038EC, 0x8000F0, 0xBC00BC, 0xE40058, 0xD82800, 0xC84C0C,
    0x887000, 0x009400, 0x00A800, 0x009038, 0x008088, 0x000000, 0x000000, 0x000000,
    0xFCFCFC, 0x3CBCFC, 0x5C94FC, 0xCC88FC, 0xF478FC, 0xFC74B4, 0xFC7460, 0xFC9838,
    0xF0BC3C, 0x80D010, 0x4CDC48, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
    0xFCFCFC, 0xA8E4FC, 0xC4D4FC, 0xD4C8FC, 0xFCC4FC, 0xFCC4D8, 0xFCBCB0, 0xFCD8A8,
    0xFCE4A0, 0xE0FCA0, 0xA8F0BC, 0xB0FCCC, 0x9CFCF0, 0xC4C4C4, 0x000000, 0x000000
};

const unsigned SoundSamplesPerSec = 48000;
const unsigned SkeepBuffer = 1789773 / SoundSamplesPerSec;
const float FPS = 59.5;
BUS NES;
CARTRIDGE * cart;
SDL_Joystick *JOY[4];

unsigned char fnt_data[0x800] = {};

HBITMAP hbrBackground;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
TCHAR szTitle[] = "NES";     
TCHAR szWindowClass[] = "WinMain";
HANDLE hThread;
DWORD threadID;
HWND hWnd;
HDC hdc;
int8_t hReduction = 8;
int8_t wReduction = 0;

LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
    int retval;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
    if (!SUCCEEDED(retval))
        return NULL;
    LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
    if (!SUCCEEDED(retval))
    {
        free(lpWideCharStr);
        return NULL;
    }
    int numArgs;
    LPWSTR* args;
    args = CommandLineToArgvW(lpWideCharStr, &numArgs);
    free(lpWideCharStr);
    if (args == NULL)
        return NULL;
    int storage = numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(args);
            return NULL;
        }
        storage += retval;
    }
    LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
    if (result == NULL)
    {
        LocalFree(args);
        return NULL;
    }
    int bufLen = storage - numArgs * sizeof(LPSTR);
    LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        assert(bufLen > 0);
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(result);
            LocalFree(args);
            return NULL;
        }
        result[i] = buffer;
        buffer += retval;
        bufLen -= retval;
    }
    LocalFree(args);
    *pNumArgs = numArgs;
    return result;
}
/*---------------*/

LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            CloseHandle(hThread);
            ReleaseDC(hWnd, hdc);
            DeleteDC(hdc);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



namespace ENGINE {
    SDL_AudioDeviceID adid;
    SDL_AudioSpec wS;
    SDL_Rect FrameRect;
    uint16_t W = 256;
    uint16_t H = 240;
    uint32_t * scr_buff;
    bool running = false;
    int pitch = 0;

    std::string hex(uint32_t n, uint8_t d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    void Init() {
        scr_buff = NES.PPU.GetFrameBuffer();
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
        uint8_t JOY_Count = SDL_NumJoysticks();
        uint8_t JC = 0;
        for(int i = 0; i < JOY_Count; i++)
        {
            if (JC < 4) {
                SDL_Joystick *joy = SDL_JoystickOpen(i);
                if (joy) {
                    JOY[JC++] = joy;
                }
            }
        }
        scale = (float) GetSystemMetrics(SM_CYSCREEN) / (float) H;
        offset_x = (float) (GetSystemMetrics(SM_CXSCREEN) - (W * scale)) / 2.0f;
        FrameRect.x = 0;//offset_x;
        FrameRect.y = 0;
        FrameRect.w = W*scale*scale;// * fullscreen_scale;
        FrameRect.h = H*scale*scale;// * fullscreen_scale;
        
        SDL_zero(wS);
        wS.freq =   SoundSamplesPerSec*2;
        wS.format = AUDIO_S16SYS;
        wS.channels = 1;

        adid = SDL_OpenAudioDevice(NULL, 0, &wS, NULL, 0);
        if (adid) SDL_PauseAudioDevice(adid, 0);
        running = true;
    }

    void FlushScanline() {
            HDC src_hdc = CreateCompatibleDC(hdc);
            HBITMAP frm_bmp = CreateBitmap(W,H,1,32, scr_buff);
            SelectObject(src_hdc, frm_bmp);
            StretchBlt(hdc, 
                            offset_x + wReduction*scale, 
                            0 + hReduction*scale, 
                            (W*scale) - (2*wReduction*scale),
                            (H*scale) - (2*hReduction*scale),
                        src_hdc, 0,0,W,H, SRCCOPY);
            DeleteDC(src_hdc);
            DeleteObject(frm_bmp);
        }

    void cls(uint8_t color) {
        for (uint16_t y = 0; y < W; y++)
            for (uint16_t x = 0; x < H; x++)   
                scr_buff[y * H + x] = palette[color] ;
    }

    void pset(unsigned px,unsigned py, unsigned color) {
        if (px < 0 || py < 0 || px >= W || py >= H) return;
        scr_buff[py * W + px] = palette[color];
    }

    void print(int16_t x, int16_t y, const std::string& text, uint8_t color = 0x20) {
        for (int16_t l = 0; l<=text.length()-1; l++)  {
            for (int16_t h = 0; h<8; h++)
                for (int16_t w = 0; w<8; w++) 
                    if (fnt_data[(uint8_t)text[l]*8+h] >> (7-w) & 1) {
                        int16_t _x1 = x+w;
                        int16_t _y = y+h;
                        pset(_x1, _y , color);
                    }
            x += 8;          
        }
    }


}
//-------------------------------------------------------------------------------

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
    bool& running = *((bool*)lpParameter);
    auto lastTime = std::chrono::steady_clock::now();
    const double maxPeriod = 1.0 / FPS;
    ENGINE::cls(0x0F);   
    while( running && !GetAsyncKeyState(VK_ESCAPE)) {
        auto time = std::chrono::steady_clock::now();
        if( std::chrono::duration<double> (time - lastTime).count() >= maxPeriod ) {
            for (uint8_t i = 0; i<4; i++) {
                const uint8_t dp = SDL_JoystickGetHat(JOY[i], 0);
                uint8_t J = NES.CPU.controller[i];
                NES.CPU.controller[i] = 0x00;
                NES.CPU.controller[i] |= SDL_JoystickGetButton(JOY[i], 1) ? 0x01 : !i && GetAsyncKeyState(0x58) ? 0x01 : 0x00; // A Button
                NES.CPU.controller[i] |= SDL_JoystickGetButton(JOY[i], 0) ? 0x02 : !i && GetAsyncKeyState(0x5A) ? 0x02 : 0x00; // B Button 2
                NES.CPU.controller[i] |= SDL_JoystickGetButton(JOY[i], 6) ? 0x04 : !i && GetAsyncKeyState(VK_RSHIFT) ? 0x04 : 0x00; // Select
                NES.CPU.controller[i] |= SDL_JoystickGetButton(JOY[i], 7) ? 0x08 : !i && GetAsyncKeyState(VK_RETURN) ? 0x08 : 0x00; // Start
                NES.CPU.controller[i] |= (dp & SDL_HAT_UP) ? 0x10 : !i && GetAsyncKeyState(VK_UP) ? 0x10 : 0x00;
                NES.CPU.controller[i] |= (dp & SDL_HAT_DOWN) ? 0x20 : !i && GetAsyncKeyState(VK_DOWN) ? 0x20 : 0x00;
                NES.CPU.controller[i] |= (dp & SDL_HAT_LEFT) ? 0x40 : !i && GetAsyncKeyState(VK_LEFT) ? 0x40 : 0x00;
                NES.CPU.controller[i] |= (dp & SDL_HAT_RIGHT) ? 0x80 : !i && GetAsyncKeyState(VK_RIGHT) ? 0x80 : 0x00;
                
                if (SDL_JoystickGetButton(JOY[i], 2)) {
                    if (J & 0x02) NES.CPU.controller[i] &= 0xFD;
                    else NES.CPU.controller[i] = NES.CPU.controller[i] |= 0x02;
                }
                if (SDL_JoystickGetButton(JOY[i], 3)) {
                    if (J & 0x01) NES.CPU.controller[i] &= 0xFE;
                    else NES.CPU.controller[i] = NES.CPU.controller[i] |= 0x01;
                }  
            }

            //SDL_ClearQueuedAudio(ENGINE::adid);
            do { NES.Clock();
                if (NES.APU.getAudioReady(SkeepBuffer)) {
                    short sample = NES.APU.sample;
                    SDL_QueueAudio(ENGINE::adid, &sample, sizeof(sample));
                }
            } while (!NES.PPU.frame_complete);

            NES.PPU.frame_complete = false;
            ENGINE::FlushScanline(); 
            lastTime = time;
        }
    }
    return 0;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    FILE* fp = fopen("font.ram", "rb");
    fread(&fnt_data[0], 0x800, 1, fp);
    fclose(fp);

    LPSTR *argv;
    int argc;
    argv = CommandLineToArgvA(GetCommandLineA(), &argc);
    for (int i = 1; i<argc; i++) {
        if ((strcmp(argv[i], "--background") == 0 || strcmp(argv[i], "-b") == 0) && argc > i+1) hbrBackground = (HBITMAP) LoadImage( NULL, argv[i+1], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if ((strcmp(argv[i], "--reduction") == 0 || strcmp(argv[i], "-r") == 0) && argc > i+2) {
            wReduction = atoi(argv[i+1]);
            hReduction = atoi(argv[i+2]);
        }
    }
    cart = new CARTRIDGE(argv[argc-1]);
    if (!cart->ImageValid()) return false;
    NES.ConnectCartridge(cart);
    NES.Reset();
    MSG msg;
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)WindowProcedure;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor        = NULL;
    wcex.hbrBackground  = CreateSolidBrush(0x00);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wcex)) { MessageBox(hWnd, "Register class error", "Error", IDI_ERROR || MB_OK); return 1; }
    hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP  & ~(WS_EX_DLGMODALFRAME |
                  WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE), CW_USEDEFAULT, 0, ENGINE::W*scale, ENGINE::H*scale, NULL, NULL, hInstance, NULL);
    if(!hWnd) { MessageBox(hWnd, "Error create window", "Error", IDI_ERROR || MB_OK);   return 1; }
    ShowWindow(hWnd, nCmdShow);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
    ShowCursor(FALSE);
    hdc = GetDC(hWnd);
    ENGINE::Init();

    if (hbrBackground) {
        BITMAPINFO bmiInfo;
        GetObject(hbrBackground, sizeof(bmiInfo), &bmiInfo);
        float sc = (float) GetSystemMetrics(SM_CYSCREEN) / (float) bmiInfo.bmiHeader.biHeight;
        int   ox = (float) (GetSystemMetrics(SM_CXSCREEN) - (bmiInfo.bmiHeader.biWidth * sc)) / 2.0f;
        HDC back_hdc = CreateCompatibleDC(hdc);
        SelectObject(back_hdc, hbrBackground);
        StretchBlt(hdc,ox,0,bmiInfo.bmiHeader.biWidth * sc,bmiInfo.bmiHeader.biHeight * sc,back_hdc,0,0,bmiInfo.bmiHeader.biWidth,bmiInfo.bmiHeader.biHeight, SRCCOPY);
        DeleteDC(back_hdc);
        DeleteObject(hbrBackground);
    }

    ENGINE::FlushScanline();

    hThread = CreateThread(NULL, 0, &ThreadProc, &ENGINE::running, 0, &threadID);
    while(GetMessage(&msg, NULL, 0, 0) && !GetAsyncKeyState(VK_ESCAPE)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return msg.wParam;
}