/*
*  Jogo da vida - por Carlos Augusto
*  07/10/2015
*/

/*

The Rules

For a space that is 'populated':
    Each cell with one or no neighbors dies, as if by solitude. 
    Each cell with four or more neighbors dies, as if by overpopulation. 
    Each cell with two or three neighbors survives. 
For a space that is 'empty' or 'unpopulated'
    Each cell with three neighbors becomes populated. 
	*/

//references http://www.programming-techniques.com/2012/01/setpixel-function-on-windowsh-how-to.html
//			 http://gamedev.stackexchange.com/questions/59857/game-loop-on-windows
//			 http://groups.engin.umd.umich.edu/CIS/course.des/cis577/lecture/win.htm
//			 http://stackoverflow.com/questions/3379139/sleep-function-in-windows-using-c
#include <time.h>
#include <stdio.h>
#include <windows.h>

static HWND sHwnd;
static COLORREF redColor=RGB(255,0,0);
static COLORREF blueColor=RGB(0,0,255);
static COLORREF greenColor=RGB(0,255,0);
char buffer[80];

void Update();
void Render();
int contandoVizinhos(int i, int j);
void drawLine(int x1, int y1, int x2, int y2);
void sleep(unsigned int mseconds);

int windows_width, windows_height;
int widthSpace = 10;
bool initialized = false;

const int SIZE_LIN = 32;
const int SIZE_COL = 32;
int m[SIZE_LIN][SIZE_COL];
int mbkp[SIZE_LIN][SIZE_COL];

 
void SetWindowHandle(HWND hwnd)
{
    sHwnd=hwnd;
}
void setPixel(int x,int y,COLORREF& color=redColor)
{
    if(sHwnd==NULL)
    {
        MessageBox(NULL,"sHwnd was not initialized !","Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
    HDC hdc=GetDC(sHwnd);
    SetPixel(hdc,x,y,color);
    ReleaseDC(sHwnd,hdc);
    return;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message)
    {
    case WM_PAINT:
        SetWindowHandle(hwnd);
		//Render();
        //drawLine();
        break;
    case WM_CLOSE: // FAIL THROUGH to call DefWindowProc
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break; // FAIL to call DefWindowProc //
    }
    return DefWindowProc(hwnd,message,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("Test");
    WNDCLASS wndclass;
    wndclass.style         = CS_HREDRAW|CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;
// Register the window //
    if(!RegisterClass(& wndclass))
    {
        MessageBox(NULL,"Registering the class failled","Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
// CreateWindow //
    HWND hwnd=CreateWindow(szAppName,"Game of life",
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, //x
                           CW_USEDEFAULT, //y
                           800,//CW_USEDEFAULT,
                           800,//CW_USEDEFAULT,
                           NULL,
                           NULL,
                           hInstance,
                           NULL);
    if(!hwnd)
    {
        MessageBox(NULL,"Falhou ao criar a janela!","Erro",MB_OK);
        exit(0);
    }
// ShowWindow and UpdateWindow //
    ShowWindow(hwnd,iCmdShow);
    UpdateWindow(hwnd);
// Message Loop //
    MSG msg;
    //while(GetMessage(& msg,NULL,0,0))
	bool firstTime=true;
	
	while(true)//GetMessage(& msg,NULL,0,0))
    {
		//UpdateWindow(sHwnd);
		if(!firstTime)
			sleep(100);
		firstTime= false;
		RECT rect;
		if(GetWindowRect(hwnd, &rect))
		{
			windows_width = rect.right - rect.left;
			windows_height = rect.bottom - rect.top;
		}
		
		while(PeekMessage(&msg, 0,0,0,PM_REMOVE))
		{
			TranslateMessage(& msg);
			DispatchMessage(& msg);
		}
		
		if ( WM_QUIT == msg.message )
            break;
		
		Update();
	    
		// *Draw*
		Render();
    }
    return 0;
}

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void drawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	//criando linhas
	HPEN red_pen = CreatePen(0,0,RGB(0,0,0)); 
	 //drawing lines 
	SelectObject(hdc,red_pen); 
	MoveToEx(hdc,x1,y1, NULL); 
	LineTo(hdc,x2,y2);
	DeleteObject(red_pen);
}

void drawBox(HDC hdc, int i, int j, int status)
{
	//x+=1;
	//desenhando retangulos
	HBRUSH blue_brush = CreateSolidBrush(RGB(0,0,255)); 
	HBRUSH white_brush = CreateSolidBrush(RGB(255,255,255)); 
	
	//SelectObject(hdc,blue_brush); 
	SelectObject(hdc,blue_brush); 
	MoveToEx(hdc,i,j, NULL); 
	RECT rect = {j*widthSpace,i*widthSpace,j*widthSpace+widthSpace,i*widthSpace+widthSpace};
	//FrameRect(hdc,&rect,blue_brush);
	if(status == 1)
		FillRect(hdc,&rect,blue_brush);
	else
		FillRect(hdc,&rect,white_brush);
	//InvalidateRect(sHwnd , &rect, false);
    UpdateWindow(sHwnd);
	DeleteObject(blue_brush);
	DeleteObject(white_brush);
}

void DrawBoard(HDC hdc)
{
	int i,j;
	i=j=0;

	for(i =0;i<windows_width/widthSpace;i++){
		drawLine(hdc, i*widthSpace, 0, i*widthSpace, windows_height);
	}
	for(i =0;i<windows_height/widthSpace;i++){
		drawLine(hdc, 0, i*widthSpace, windows_width, i*widthSpace);
	}
	
	//desenhando linhas
	 //drawing lines 
	// SelectObject(hdc,red_pen); 
	// MoveToEx(hdc,10,20, NULL); 
	// LineTo(hdc,10,40); 
	// //LineTo(hdc,50,150); //can be continued to draw shapes 
	
	//desenhando retangulos
	// HBRUSH blue_brush = CreateSolidBrush(RGB(0,0,255)); 
	// static RECT rect = {100,100,200,200};
	// FrameRect(hdc,&rect,blue_brush);
	
	//desenhando pixels
	// setPixel(0, 100, blueColor);
	// for(i=0; windows_width/widthSpace; i+=10)
		// for(j=0;j<windows_height;j++)
			// setPixel(i, j, blueColor);
}

void copyBkp(){
	for(int i=0;i<SIZE_LIN;i++){
		for(int j=0;j<SIZE_COL;j++){
			mbkp[i][j]=m[i][j];
		}
	}
}

void init()
{
	for(int i=0;i<SIZE_LIN;i++){
		for(int j=0;j<SIZE_COL;j++){
			m[i][j]=0;
		}
	}
	//glider
	m[0][1] = 1;
	m[1][2] = 1;
	m[2][2] = 1;
	m[2][1] = 1;
	m[2][0] = 1;
	
	int menosFixo = 3;
	
	m[SIZE_LIN-menosFixo][SIZE_LIN-menosFixo] = 1;
	m[SIZE_LIN-menosFixo-1][SIZE_LIN-menosFixo-1] = 1;
	m[SIZE_LIN-menosFixo-2][SIZE_LIN-menosFixo-1] = 1;
	m[SIZE_LIN-menosFixo-2][SIZE_LIN-menosFixo] = 1;
	m[SIZE_LIN-menosFixo-1][SIZE_LIN-menosFixo-2] = 1;
	
	copyBkp();
}

//cima em j = -1
//baixo em j = 1
int contandovizinhos(int i, int j){
	int qtd = 0;
	
	//olhando os seus 8 lados
	//esquerdo
	if(i-1>=0 && mbkp[i-1][j] == 1)
		qtd++;
	//cima e esquerdo
	if(i-1>=0 && j-1>=0 && mbkp[i-1][j-1]  == 1)
		qtd++;
	//cima
	if(j-1 >= 0 && mbkp[i][j-1] == 1)
		qtd++;
	//cima direita
	if(mbkp[i+1][j-1] == 1 && i+1 < SIZE_LIN && j-1>=0)
		qtd++;
	//direita
	if(mbkp[i+1][j] == 1 && i+1 < SIZE_LIN)
		qtd++;
	//embaixo direita
	if(mbkp[i+1][j+1] == 1 && i+1 < SIZE_LIN && j+1 <SIZE_COL)
		qtd++;
	//embaixo
	if(mbkp[i][j+1] == 1 && j+1 < SIZE_COL)
		qtd++;
	//embaixo e esqueda
	if(mbkp[i-1][j+1] == 1 && i-1 >= 0 && j+1 < SIZE_COL)
		qtd++;
	
	return qtd;
}

void gameLogic()
{
	// //aplicando a regra do jogo
	// //For a space that is 'populated':
	int qtdvizinhos=0;
	for(int i=0;i<SIZE_LIN;i++){
		for(int j=0;j<SIZE_COL;j++){
			qtdvizinhos = contandovizinhos(i,j);
			//for a space that is 'empty' or 'unpopulated'
			//each cell with three neighbors becomes populated. 
			if(qtdvizinhos == 3){
					m[i][j] = 1;
			}
			else if(m[i][j]==1){
				//each cell with one or no neighbors dies, as if by solitude. 
				if(qtdvizinhos < 2)
					m[i][j]=0;
				//each cell with four or more neighbors dies, as if by overpopulation. 
				if(qtdvizinhos > 3)
					m[i][j]=0;
				//Each cell with two or three neighbors survives. 
				if(qtdvizinhos == 2
					|| qtdvizinhos == 3)
					m[i][j]=1;
			}
		}
	}
	copyBkp();
}

void Update()
{
	if(!initialized){
		init();
	}
	
	//sleep(3000);
	
	if(!initialized)
		return;
	
	gameLogic();
	
	//k+=10;
}
void Render()
{
	HDC hdc=GetDC(sHwnd);	
	int i,j;
	i=j=0;
	
	
	//Desenhando os com 1
	for(i=0;i<SIZE_LIN;i++){
		for(j=0;j<SIZE_COL;j++){
			//if(m[i][j]==1){
			drawBox(hdc, i, j, m[i][j]);
			//}
			int qtdvizinhos = contandovizinhos(i,j);
			//sprintf(buffer,"%i", qtdvizinhos);
			//TextOut(hdc,j*widthSpace,i*widthSpace,buffer,strlen(buffer));
			UpdateWindow(sHwnd);
		}
	}
	
	DrawBoard(hdc);
	
	initialized = true;
	//drawLine();
}