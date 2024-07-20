#include <d3d9.h>
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

LPDIRECT3D9             g_pD3D = NULL; // D3D 디바이스를 생성할 D3D객체변수
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // 렌더링에 사용될 D3D디바이스
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // 정점을 보관할 정점버퍼

// 사용자 정점 구조체에 관한 정보를 나타내는 FVF값. X,Y,Z,RHW값과 Diffuse색깔값으로 이루어져 있음을 알 수 있다.
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // 정점의 변환된 좌표. rhw값이 있으면 변환이 완료된 정점이다.
	DWORD color; // 정점의 색깔
};

// #2. Direct3D 초기화
HRESULT InitD3D(HWND hWnd)
{
	// 디바이스를 생성하기위한 D3D객체 생성
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp; // 디바이스를 생성할 구조체 : _D3DPRESENT_PARAMETERS_ 구조체에 해당 데이터가 전부 있다.
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // 메모리 초기화
	d3dpp.Windowed = TRUE; // 창 모드 설정
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 버퍼 간의 교환 방식을 정한다.
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // 백 버퍼의 픽셀 형식을 정의

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice))) // Direct3D 디바이스 생성 : 렌더링을 수행할 수 있는 객체 생성. 생성에 실패하면 E_FAIL을 반환
	{
		return E_FAIL;
	}

	// 디바이스 상태정보를 처리할경우 여기에서 한다.

	return S_OK;
}

// #3. 정점 버퍼 생성
HRESULT InitVB()
{
	// 삼각형을 렌더링하기위해 세개의 정점을 선언
	CUSTOMVERTEX vertices[]
	{
		// x,		y,	  z,   rhw,		color
		{ 85.0f, 155.0f, 0.5f, 1.0f, 0xff1707ef, }, // 좌측 최상단을 기준점 (0, 0)으로 잡고 x는 우측으로 갈수록, y는 아래로 갈수록 더해진다.
		{ 145.5f, 70.0f, 0.5f, 1.0f, 0xfffbf5f9, },
		{ 200.0f, 180.0f, 0.5f, 1.0f, 0xfff50d18, },
	};

	// 정점버퍼 생성 : 3개의 사용자정점을 보관할 메모리를 할당한다. FVF를 지정하여 보관할 데이터의 형식을 지정한다.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// 정점버퍼를 정점 데이터를 복사한다. 
	VOID* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) // 정점버퍼의 Lock()함수를 호출하여 포인터를 얻어온다. 이때 Lock을 하는 이유는 복사가 끝날때까지 GPU가 접근하는 것을 막기 위함이다.
		return E_FAIL;

	memcpy(pVertices, vertices, sizeof(vertices)); // vertices 배열의 데이터를 정점 버퍼에 복사한다.
	g_pVB->Unlock(); // 복사가 끝나면 Unlock()으로 GPU가 접근이 가능하게 한다.

	return S_OK;
}

// #9. 초기화 객체들 소거
VOID Cleanup()
{
	if (g_pVB != NULL) // 사용하였던 모든 객체를 해제한다. 객체들을 전역변수로 지정한 이유와 관련이 있다.
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

// #8. 화면 그리기
VOID Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0); // 후면버퍼를 검정색(0, 0, 0)으로 지운다.

	// 렌더링 시작
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// 정점버퍼의 삼각형을 그린다.
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX)); // 1. 정점정보가 담겨있는 정점버퍼를 출력 스트림으로 할당한다.
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX); // 2. D3D에게 정점쉐이더 정보를 지정한다. 대부분의 경우에는 FVF만 지정한다.
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1); // 3. 기하 정보를 출력하기 위한 DrawPrimitive()함수 호출
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// 렌더링 종료
		g_pd3dDevice->EndScene();
	}

	// 후면버퍼를 보이는 화면으로!
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 프로그램 시작점
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// 윈도우 클래스 등록
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Tutorial", NULL };
	RegisterClassEx(&wc);

	// #1. 윈도우 생성
	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 02: Vertices",
		WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// Direct3D 초기화
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// 정점버퍼 초기화
		if (SUCCEEDED(InitVB()))
		{
			// #5. 윈도우 출력2. 윈도우의 화면에 보이도록 한다.
			ShowWindow(hWnd, SW_SHOWDEFAULT);

			// #4. 윈도우 출력1. Paint 콜백 함수를 요청한다.
			UpdateWindow(hWnd);

			// #6. 메시지 루프
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));

			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) // 메시지큐에 메시지가 있으면 메시지 처리
				{
					TranslateMessage(&msg); // 키보드 입력 메시지를 문자 메시지로 변환한다.
					DispatchMessage(&msg); // 메시지를 해당 윈도우 프로시저로 전달하여 처리한다.
				}
				else // #7. 처리할 메시지가 없으면 Render()함수 호출
					Render();
			}
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance); // 등록된 클래스 소거
	return 0;
}
