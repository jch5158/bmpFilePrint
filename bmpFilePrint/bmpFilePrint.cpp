// bmpFilePrint.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "bmpFilePrint.h"


#define BMP_WIDTH 1920
#define BMP_HEIGHT 1080

void BitMap();

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.

HDC hdc;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BMPFILEPRINT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BMPFILEPRINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = L"bmpFilePrint";
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(L"bmpFilePrint", L"bmpTitle", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1980, 1080, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
           
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
           
            BitMap();
            
            
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void BitMap() {

    FILE* fp;

    _wfopen_s(&fp, L"photo3.bmp", L"rb");
    // 헤더 구조체 선언
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;

    
    // bmp 헤더 정보 읽기
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);


    // 한 줄당 실제 바이트 갯수
    LONG lWidth = bitmapInfoHeader.biWidth * (bitmapInfoHeader.biBitCount / 8);

    //--------------------------------------------------------------------------------
    // pitch 계산법
    // 32bit -> 4byte 정렬
    // 64bit -> 생각 좀 해봐야 한다
    // 3(이진수 011) 올려서 4byte 정렬을 해준 다음 올려준 3(이진수 011)를 날림
    //--------------------------------------------------------------------------------
    int iPitch = (lWidth + 3) & ~3;

    // 이미지 크기
    int iImageSize = iPitch * bitmapInfoHeader.biHeight;

    //---------------------------------------------------------
    // DIB(BMP)는 이미지가 뒤집혀져 있다
    // 뒤집혀진 이미지가 정상 DIB구조 이므로, 
    // 뒤집혀진 채로 읽어서 DIB 출력 해주면 그림이 출력된다
    //---------------------------------------------------------

    // 이미지 생성
    BYTE* pImage = new BYTE[iImageSize];

    fseek(fp, bitmapFileHeader.bfOffBits, SEEK_SET);

    // bmp 파일 읽기
    fread(pImage, iImageSize, 1, fp);

    // 파일 닫기
    fclose(fp);

    
    // DIB -> DC 출력 함수
    // DC 연결 후 출력
    StretchDIBits
    (
        hdc,                                    // 목적지 DC
        0,                                      // x 좌표
        0,                                      // y 좌표
        iPitch,                                 // 목적지 너비
        bitmapInfoHeader.biHeight,              // 목적지 높이
        0,                                      // 출력소스 x 좌표
        0,                                      // 출력소스 y 좌표
        iPitch,                                 // 출력소스 너비
        bitmapInfoHeader.biHeight,              // 출력소스 높이
        pImage,                                 // 이미지가 위치한 포인터
        (LPBITMAPINFO)(&bitmapInfoHeader),      // BITMAPINFO = (BITMAPINFOHEADER + RGBQUAD) -> type캐스팅 해야한다. 
        DIB_RGB_COLORS,                         // DIB_RGB_COLORS
        SRCCOPY                                 // 출력모드
    );

    // 동적 할당 해제
    delete[] pImage;

    return;

}