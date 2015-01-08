#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

void VS(float4 iPos : POSITION, float2 iTexCoord: TEXCOORD0,
    out float4 oPos : POSITION,
    out float2 oTexCoord : TEXCOORD0)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    
    oPos.z = oPos.w;
    oTexCoord = iTexCoord;
}

void PS(float3 iTexCoord : TEXCOORD0,
    out float4 oColor : COLOR0)
{
    oColor = cMatDiffColor * tex2D(sDiffMap, iTexCoord);
}
