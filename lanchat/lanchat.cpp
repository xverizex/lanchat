// lanchat.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lanchat.h"
#include "Users.h"
#include "Chat.h"
#include <Windowsx.h>
#include "Network.h"
#include "Idle.h"
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
Users* UsersList;
Chat* chat;
NetworkChat* networkChat;
Idle* idle;

int pos_line = 0;
HWND mainHWND;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.



    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LANCHAT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LANCHAT));

    MSG msg;

    idle = new Idle();

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LANCHAT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_BORDER,
      CW_USEDEFAULT, 0, 1024, 512 + 16, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   mainHWND = hWnd;

   SetWindowLongW(hWnd, GWL_STYLE, 0);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//

bool moving = false;
int x = 0;
int y = 0;

void threadc(void *param);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        UsersList = new Users();
        chat = new Chat();
        networkChat = new NetworkChat();
        networkChat->chat = chat;
        networkChat->users = UsersList;
        int ret = networkChat->init();
        if (ret == -1) exit(0);
    }
    break;
    case WM_COMMAND:
        {
        
        }
        break;

    break;
    case WM_SETFOCUS:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        SIZE size;
        chat->carret = chat->textInput.left + 4 + chat->inter_cmd.cx;
        bool found = false;
        for (int i = 0; i < chat->max_buf; i++) {
            GetTextExtentPoint32(hdc, &chat->buffer_input[i], 1, &size);
            chat->carret += size.cx;
            if (chat->carret > x) {
                chat->carret -= size.cx;
                SetCaretPos(chat->carret, chat->textInput.top);
                found = true;
                break;
            }
        }
        CreateCaret(hWnd, (HBITMAP)0, 1, 14);
        if (found == false)
            SetCaretPos(chat->carret, chat->textInput.top);
        ShowCaret(hWnd);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (x >= chat->textInput.left && x <= chat->textInput.right) {
            if (y >= chat->textInput.top && y <= chat->textInput.bottom) {
                HCURSOR cursor = LoadCursor(NULL, IDC_IBEAM);
                SetCursor(cursor);
                SendMessage(hWnd, WM_SETFOCUS, 0, lParam);
                return 0;
            }
        }
        HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
        SetCursor(cursor);
        
        ReleaseCapture();
        SendMessageW(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        
    }
    break;
    case WM_LBUTTONUP:
    {

    }
    break;
    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (x >= chat->textInput.left && x <= chat->textInput.right) {
            if (y >= chat->textInput.top && y <= chat->textInput.bottom) {
                HCURSOR cursor = LoadCursor(NULL, IDC_IBEAM);
                SetCursor(cursor);
                return 0;
            }
        }
    }
    break;
    case WM_KEYDOWN:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        switch (wParam) {
        case VK_LEFT:
            chat->carretLeft(hdc);
            break;
        case VK_RIGHT:
            chat->carretRight(hdc);
            break;
        case VK_RETURN:
            chat->enter(hdc);
            //InvalidateRect(hWnd, &chat->rect, TRUE);
            //SendMessage(hWnd, WM_PAINT, 0, 0);
            EndPaint(hWnd, &ps);
            return 1;
            break;
        default:
            return 0;
            break;
        }

        InvalidateRect(hWnd, &chat->textInput, TRUE);
        //SendMessage(hWnd, WM_PAINT, 0, 0);


        EndPaint(hWnd, &ps);
        return 1;

    }
    break;
    case WM_CHAR:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (wParam == 0x08) {
            chat->backspace(hdc);
        }
        else {
            chat->addChar(hdc, wParam);
        }

        
        //SendMessage(hWnd, WM_PAINT, 0, 0);
        InvalidateRect(hWnd, &chat->textInput, TRUE);

        EndPaint(hWnd, &ps);
        return 1;
    }
    break;
    case WM_USER:
        RECT rect;

        GetClientRect(hWnd, &rect);
        InvalidateRect(hWnd, &rect, TRUE);
        break;

    case WM_PAINT:
        {
        if (chat->initialize == false) {
            RECT r;
            r.left = UsersList->rect.left + UsersList->rect.right + 4;
            r.top = 16;
            r.right = 1024 - 16;
            r.bottom = 512;
            chat->setRect(&r);
        }            
        
            RECT rect;

            GetClientRect(hWnd, &rect);    

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HBRUSH brush = CreateSolidBrush(RGB(0x71, 0xbc, 0xbf));
            FillRect(hdc, &rect, brush);


            UsersList->DrawUsersLists(hdc);

            chat->draw(hdc);
            CreateCaret(hWnd, (HBITMAP)0, 1, 14);
            ShowCaret(hWnd);

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


