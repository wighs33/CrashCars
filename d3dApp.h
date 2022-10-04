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

// 필요한 d3d12 라이브러리들을 링크한다.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:

    // 멤버들을 기본 값으로 초기화 한다.
    D3DApp(HINSTANCE hInstance);
    D3DApp(const D3DApp& rhs) = delete;
    D3DApp& operator=(const D3DApp& rhs) = delete;
    // D3DApp이 획득한 COM을 해제하고 명령 대기열을 비운다.(명령처리 마칠 때까지 기다리고 파괴)
    virtual ~D3DApp();

public:

    static D3DApp* GetApp();
    // 인스턴스 핸들 복사본 반환
	HINSTANCE AppInst()const;
    // 메인 윈도우 핸들 복사본 반환
	HWND      MainWnd()const;
    // 종횡비 반환
	float     AspectRatio()const;

    // 4X MSAA 활성화여부 반환
    bool Get4xMsaaState()const;
    // 4X MSAA 활성화여부 설정
    void Set4xMsaaState(bool value);

    // 윈도우 메시지가 없을 때도 게임의 논리를 처리
	int Run();
 
    // 자원 할당, 장면 물체 초기화, 광원 설정을 넣는다.
    // 메인 윈도우와 다렉 초기화 함수 호출
    virtual bool Initialize();
    // 메인 윈도우의 메시지 처리부를 넣는다.
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    // RTV 서술자와 DSV 서술자를 응용프로그램에 맞게 생성
    virtual void CreateRtvAndDsvDescriptorHeaps();
    // 클라이언트 영역 크기에 의존하는 다렉속성 갱신(후면버퍼 조정, 깊이&스텐실 버퍼 파괴후 재생성, 뷰도 다시생성)
	virtual void OnResize(); 
    // 매 프레임마다 호출, 시간에 따른 갱신 처리
	virtual void Update(const GameTimer& gt)=0;
    // 매 프레임마다 호출 렌더링 명령 제출, 후면 버퍼 제시
    virtual void Draw(const GameTimer& gt)=0;

	// 간편한 마우스 입력 처리를 위한 가상 함수들
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:

	bool InitMainWindow();
	bool InitDirect3D();
    // 명령 대기열, 명령 목록 할당자, 명령목록 하나씩 생성
	void CreateCommandObjects();
    void CreateSwapChain();
    // 대기열의 명령 처리 끝날때 까지 CPU가 기다리게 만듦
	void FlushCommandQueue();

    // 현재 후면 버퍼에 대한 ID3D12Resource반환
	ID3D12Resource* CurrentBackBuffer()const;
    // 현재 후면 버퍼에 대한 RTV반환
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    // 현재 후면 버퍼에 대한 DSV반환
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

    // fps, mspf 계산
	void CalculateFrameStats();

    // 시스템의 모든 디스플레이 어댑터 열거
    void LogAdapters();
    // 어댑터에 연관된 출력 열거
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    // 해당 출력이과 픽셀조합이 지원하는 디스플레이 열거
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

    static D3DApp* mApp;

    HINSTANCE mhAppInst = nullptr; // 응용 프로그램 인스턴스 핸들
    HWND      mhMainWnd = nullptr; // 주 창 핸들
	bool      mAppPaused = false;  // 응용 프로그램이 일시 정지된 상태인가?
	bool      mMinimized = false;  // 응용 프로그램이 최소화된 상태인가?
	bool      mMaximized = false;  // 응용 프로그램이 최대화된 상태인가?
	bool      mResizing = false;   // 사용자가 크기 조정용 테두리를 끌고 있는 상태인가?
    bool      mFullscreenState = false;// 전체화면 활성화 여부

	// true로 설정하면 4X MSAA (4.1.8)가 적용된다.  기본은 false이다.
    bool      m4xMsaaState = false;    // 4X MSAA 활성화 여부
    UINT      m4xMsaaQuality = 0;      // 4X MSAA의 품질 수준

	// 경과 시간과 게임 전체 시간을 측정하는 데 쓰인다. (4.4).
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

	// 파생 클래스는 자신의 생성자에서 이 멤버 변수들을 자신의 목적에 맞는 초기 값들로 설정해야 한다.
	std::wstring mMainWndCaption = L"Crash Cars";
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;
};

