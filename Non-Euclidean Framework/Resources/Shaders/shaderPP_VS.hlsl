// Vertex Shader
struct VS_INPUT
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS( VS_INPUT input )
{   
    VS_OUTPUT output;
    output.Position = float4( input.Position, 0.0f, 1.0f );
    output.TexCoord = input.TexCoord;
    return output;
}