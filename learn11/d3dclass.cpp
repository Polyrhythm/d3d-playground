#include "d3dclass.h"

D3DClass::D3DClass()
    : _swapChain(0)
    , _device(0)
    , _deviceContext(0)
    , _renderTargetView(0)
    , _depthStencilBuffer(0)
    , _depthStencilState(0)
    , _depthStencilView(0)
    , _rasterState(0)
{

}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass()
{

}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync,
    HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator;
    unsigned long long stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_VIEWPORT viewport;
    float fieldOfView, screenAspect;

    _vsyncEnabled = vsync;

    // Create graphics interface factory
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
    {
        return false;
    }

    // Use factory to create adapter for primary graphics interface
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        return false;
    }

    // Enumerate primary adapter output
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        return false;
    }

    // Get the number of modes that fit the display format for the monitor
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        return false;
    }

    // Create list to hold all possible display modes
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        return false;
    }

    // fill DisplayModeList
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
    {
        return false;
    }

    // Go through display modes and find match for resolution
    for (i = 0; i < numModes; i++) {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Get the GPU desc
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }

    // store gpu memory in mb
    _videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert name of gpu to character array and store it
    error = wcstombs_s(&stringLength, _videoCardDescription, 128,
        adapterDesc.Description, 128);
    if (error != 0)
    {
        return false;
    }

    // Release stuff
    delete[] displayModeList;
    displayModeList = 0;

    adapterOutput->Release();
    adapterOutput = 0;

    adapter->Release();
    adapter = 0;

    factory->Release();
    factory = 0;
    
    // Start initialization
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set one backbuffer
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    // vsync
    if (_vsyncEnabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;

    // multisampling off
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // Scan line ordering and scaling to unspecified
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard backbuffer after presenting
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set advanced flags
    swapChainDesc.Flags = 0;

    // feature level
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, d3d device, and device context
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
        NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
        &_swapChain, &_device, NULL, &_deviceContext);
    if (FAILED(result))
    {
        return false;
    }
    
    result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
    {
        return false;
    }

    // create render target view with backbuffer pointer
    result = _device->CreateRenderTargetView(backBufferPtr, NULL, &_renderTargetView);
    if (FAILED(result))
    {
        return false;
    }

    backBufferPtr->Release();
    backBufferPtr = 0;

    // depth and stencil
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    result = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // initialize stencil desc
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
    if (FAILED(result))
    {
        return false;
    }

    // set depth stencil state
    _deviceContext->OMSetDepthStencilState(_depthStencilState, 1);

    // initialize depth stencil view
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = _device->CreateDepthStencilView(_depthStencilBuffer,
        &depthStencilViewDesc, &_depthStencilView);
    if (FAILED(result))
    {
        return false;
    }

    _deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

    // set up raster state
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // create raster state
    result = _device->CreateRasterizerState(&rasterDesc, &_rasterState);
    if (FAILED(result))
    {
        return false;
    }

    _deviceContext->RSSetState(_rasterState);

    // setup viewport
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    _deviceContext->RSSetViewports(1, &viewport);

    // projection matrix
    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;
    _projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect,
        screenNear, screenDepth);

    // world matrix
    _worldMatrix = XMMatrixIdentity();

    // orthographic projection matrix
    _orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight,
        screenNear, screenDepth);

    return true;
}

void D3DClass::Shutdown()
{
    // before shutting down, set to windowed mode or when the swapchain 
    // is released, it will throw an error
    if (_swapChain)
    {
        _swapChain->SetFullscreenState(false, NULL);
    }

    if (_rasterState)
    {
        _rasterState->Release();
        _rasterState = 0;
    }

    if (_depthStencilView)
    {
        _depthStencilView->Release();
        _depthStencilView = 0;
    }

    if (_depthStencilState)
    {
        _depthStencilState->Release();
        _depthStencilState = 0;
    }

    if (_depthStencilBuffer)
    {
        _depthStencilBuffer->Release();
        _depthStencilBuffer = 0;
    }

    if (_renderTargetView)
    {
        _renderTargetView->Release();
        _renderTargetView = 0;
    }

    if (_deviceContext)
    {
        _deviceContext->Release();
        _deviceContext = 0;
    }

    if (_device)
    {
        _device->Release();
        _device = 0;
    }

    if (_swapChain)
    {
        _swapChain->Release();
        _swapChain = 0;
    }
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    _deviceContext->ClearRenderTargetView(_renderTargetView, color);
    _deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH,
        1.0f, 0);
}

void D3DClass::EndScene()
{
    if (_vsyncEnabled)
    {
        _swapChain->Present(1, 0);
    }
    else
    {
        _swapChain->Present(0, 0);
    }
}

ID3D11Device* D3DClass::GetDevice()
{
    return _device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return _deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = _projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = _worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = _orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, _videoCardDescription);
    memory = _videoCardMemory;
}