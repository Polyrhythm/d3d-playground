cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_IN
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_IN ColorVertexShader(VS_IN input)
{
    PS_IN output;

    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.color = input.color;

    return output;
}