//***************************************************************************************
// d3dApp.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "d3dUtil.h"
#include "GameTimer.h"

// �ʿ��� d3d12 ���̺귯������ ��ũ�Ѵ�.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:

    // ������� �⺻ ������ �ʱ�ȭ �Ѵ�.
    D3DApp(HINSTANCE hInstance);
    D3DApp(const D3DApp& rhs) = delete;
    D3DApp& operator=(const D3DApp& rhs) = delete;
    // D3DApp�� ȹ���� COM�� �����ϰ� ���� ��⿭�� ����.(����ó�� ��ĥ ������ ��ٸ��� �ı�)
    virtual ~D3DApp();

public:

    static D3DApp* GetApp();
    // �ν��Ͻ� �ڵ� ���纻 ��ȯ
	HINSTANCE AppInst()const;
    // ���� ������ �ڵ� ���纻 ��ȯ
	HWND      MainWnd()const;
    // ��Ⱦ�� ��ȯ
	float     AspectRatio()const;

    // 4X MSAA Ȱ��ȭ���� ��ȯ
    bool Get4xMsaaState()const;
    // 4X MSAA Ȱ��ȭ���� ����
    void Set4xMsaaState(bool value);

    // ������ �޽����� ���� ���� ������ ������ ó��
	int Run();
 
    // �ڿ� �Ҵ�, ��� ��ü �ʱ�ȭ, ���� ������ �ִ´�.
    // ���� ������� �ٷ� �ʱ�ȭ �Լ� ȣ��
    virtual bool Initialize();
    // ���� �������� �޽��� ó���θ� �ִ´�.
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    // RTV �����ڿ� DSV �����ڸ� �������α׷��� �°� ����
    virtual void CreateRtvAndDsvDescriptorHeaps();
    // Ŭ���̾�Ʈ ���� ũ�⿡ �����ϴ� �ٷ��Ӽ� ����(�ĸ���� ����, ����&���ٽ� ���� �ı��� �����, �䵵 �ٽû���)
	virtual void OnResize(); 
    // �� �����Ӹ��� ȣ��, �ð��� ���� ���� ó��
	virtual void Update(const GameTimer& gt)=0;
    // �� �����Ӹ��� ȣ�� ������ ���� ����, �ĸ� ���� ����
    virtual void Draw(const GameTimer& gt)=0;

	// ������ ���콺 �Է� ó���� ���� ���� �Լ���
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:

	bool InitMainWindow();
	bool InitDirect3D();
    // ���� ��⿭, ���� ��� �Ҵ���, ���ɸ�� �ϳ��� ����
	void CreateCommandObjects();
    void CreateSwapChain();
    // ��⿭�� ���� ó�� ������ ���� CPU�� ��ٸ��� ����
	void FlushCommandQueue();

    // ���� �ĸ� ���ۿ� ���� ID3D12Resource��ȯ
	ID3D12Resource* CurrentBackBuffer()const;
    // ���� �ĸ� ���ۿ� ���� RTV��ȯ
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    // ���� �ĸ� ���ۿ� ���� DSV��ȯ
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

    // fps, mspf ���
	void CalculateFrameStats();

    // �ý����� ��� ���÷��� ����� ����
    void LogAdapters();
    // ����Ϳ� ������ ��� ����
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    // �ش� ����̰� �ȼ������� �����ϴ� ���÷��� ����
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

    static D3DApp* mApp;

    HINSTANCE mhAppInst = nullptr; // ���� ���α׷� �ν��Ͻ� �ڵ�
    HWND      mhMainWnd = nullptr; // �� â �ڵ�
	bool      mAppPaused = false;  // ���� ���α׷��� �Ͻ� ������ �����ΰ�?
	bool      mMinimized = false;  // ���� ���α׷��� �ּ�ȭ�� �����ΰ�?
	bool      mMaximized = false;  // ���� ���α׷��� �ִ�ȭ�� �����ΰ�?
	bool      mResizing = false;   // ����ڰ� ũ�� ������ �׵θ��� ���� �ִ� �����ΰ�?
    bool      mFullscreenState = false;// ��üȭ�� Ȱ��ȭ ����

	// true�� �����ϸ� 4X MSAA (4.1.8)�� ����ȴ�.  �⺻�� false�̴�.
    bool      m4xMsaaState = false;    // 4X MSAA Ȱ��ȭ ����
    UINT      m4xMsaaQuality = 0;      // 4X MSAA�� ǰ�� ����

	// ��� �ð��� ���� ��ü �ð��� �����ϴ� �� ���δ�. (4.4).
	GameTimer mTimer;
	
    Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    UINT64 mCurrentFence = 0;
	
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    D3D12_VIEWPORT mScreenViewport; 
    D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	// �Ļ� Ŭ������ �ڽ��� �����ڿ��� �� ��� �������� �ڽ��� ������ �´� �ʱ� ����� �����ؾ� �Ѵ�.
	std::wstring mMainWndCaption = L"Crash Cars";
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;
};
