struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 ColorPixelShader(PS_IN input)
{
    return input.color;
}