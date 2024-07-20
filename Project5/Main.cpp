#include <d3d9.h>
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

LPDIRECT3D9             g_pD3D = NULL; // D3D ����̽��� ������ D3D��ü����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // �������� ���� D3D����̽�
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // ������ ������ ��������

// ����� ���� ����ü�� ���� ������ ��Ÿ���� FVF��. X,Y,Z,RHW���� Diffuse�������� �̷���� ������ �� �� �ִ�.
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // ������ ��ȯ�� ��ǥ. rhw���� ������ ��ȯ�� �Ϸ�� �����̴�.
	DWORD color; // ������ ����
};

// #2. Direct3D �ʱ�ȭ
HRESULT InitD3D(HWND hWnd)
{
	// ����̽��� �����ϱ����� D3D��ü ����
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp; // ����̽��� ������ ����ü : _D3DPRESENT_PARAMETERS_ ����ü�� �ش� �����Ͱ� ���� �ִ�.
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // �޸� �ʱ�ȭ
	d3dpp.Windowed = TRUE; // â ��� ����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ���� ���� ��ȯ ����� ���Ѵ�.
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // �� ������ �ȼ� ������ ����

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice))) // Direct3D ����̽� ���� : �������� ������ �� �ִ� ��ü ����. ������ �����ϸ� E_FAIL�� ��ȯ
	{
		return E_FAIL;
	}

	// ����̽� ���������� ó���Ұ�� ���⿡�� �Ѵ�.

	return S_OK;
}

// #3. ���� ���� ����
HRESULT InitVB()
{
	// �ﰢ���� �������ϱ����� ������ ������ ����
	CUSTOMVERTEX vertices[]
	{
		// x,		y,	  z,   rhw,		color
		{ 85.0f, 155.0f, 0.5f, 1.0f, 0xff1707ef, }, // ���� �ֻ���� ������ (0, 0)���� ��� x�� �������� ������, y�� �Ʒ��� ������ ��������.
		{ 145.5f, 70.0f, 0.5f, 1.0f, 0xfffbf5f9, },
		{ 200.0f, 180.0f, 0.5f, 1.0f, 0xfff50d18, },
	};

	// �������� ���� : 3���� ����������� ������ �޸𸮸� �Ҵ��Ѵ�. FVF�� �����Ͽ� ������ �������� ������ �����Ѵ�.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// �������۸� ���� �����͸� �����Ѵ�. 
	VOID* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) // ���������� Lock()�Լ��� ȣ���Ͽ� �����͸� ���´�. �̶� Lock�� �ϴ� ������ ���簡 ���������� GPU�� �����ϴ� ���� ���� �����̴�.
		return E_FAIL;

	memcpy(pVertices, vertices, sizeof(vertices)); // vertices �迭�� �����͸� ���� ���ۿ� �����Ѵ�.
	g_pVB->Unlock(); // ���簡 ������ Unlock()���� GPU�� ������ �����ϰ� �Ѵ�.

	return S_OK;
}

// #9. �ʱ�ȭ ��ü�� �Ұ�
VOID Cleanup()
{
	if (g_pVB != NULL) // ����Ͽ��� ��� ��ü�� �����Ѵ�. ��ü���� ���������� ������ ������ ������ �ִ�.
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

// #8. ȭ�� �׸���
VOID Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0); // �ĸ���۸� ������(0, 0, 0)���� �����.

	// ������ ����
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// ���������� �ﰢ���� �׸���.
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX)); // 1. ���������� ����ִ� �������۸� ��� ��Ʈ������ �Ҵ��Ѵ�.
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX); // 2. D3D���� �������̴� ������ �����Ѵ�. ��κ��� ��쿡�� FVF�� �����Ѵ�.
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1); // 3. ���� ������ ����ϱ� ���� DrawPrimitive()�Լ� ȣ��
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ������ ����
		g_pd3dDevice->EndScene();
	}

	// �ĸ���۸� ���̴� ȭ������!
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

// ���α׷� ������
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// ������ Ŭ���� ���
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Tutorial", NULL };
	RegisterClassEx(&wc);

	// #1. ������ ����
	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 02: Vertices",
		WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// Direct3D �ʱ�ȭ
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// �������� �ʱ�ȭ
		if (SUCCEEDED(InitVB()))
		{
			// #5. ������ ���2. �������� ȭ�鿡 ���̵��� �Ѵ�.
			ShowWindow(hWnd, SW_SHOWDEFAULT);

			// #4. ������ ���1. Paint �ݹ� �Լ��� ��û�Ѵ�.
			UpdateWindow(hWnd);

			// #6. �޽��� ����
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));

			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) // �޽���ť�� �޽����� ������ �޽��� ó��
				{
					TranslateMessage(&msg); // Ű���� �Է� �޽����� ���� �޽����� ��ȯ�Ѵ�.
					DispatchMessage(&msg); // �޽����� �ش� ������ ���ν����� �����Ͽ� ó���Ѵ�.
				}
				else // #7. ó���� �޽����� ������ Render()�Լ� ȣ��
					Render();
			}
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance); // ��ϵ� Ŭ���� �Ұ�
	return 0;
}
