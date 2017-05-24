#include "modelclass.h"

ModelClass::ModelClass()
    : _vertexBuffer(0)
    , _indexBuffer(0)
{

}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
    bool result;

    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }

    return true;
}

void ModelClass::Shutdown()
{
    ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
    return _indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // MAKE A FUCKING TRIANGLE
    _vertexCount = 3;
    _indexCount = 3;
    vertices = new VertexType[_vertexCount];
    if (!vertices)
    {
        return false;
    }

    indices = new unsigned long[_indexCount];
    if (!indices)
    {
        return false;
    }

    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // give the subresource a pointer to the vertex data
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData,
        &_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData,
        &_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // release!
    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;

    return true;
}

void ModelClass::ShutdownBuffers()
{
    if (_indexBuffer)
    {
        _indexBuffer->Release();
        _indexBuffer = 0;
    }

    if (_vertexBuffer)
    {
        _vertexBuffer->Release();
        _vertexBuffer = 0;
    }
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VertexType);
    offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride,
        &offset);
    deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}