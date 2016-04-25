//--------------------------------------------------------------------------------------
#include "DefPool.gfxp"

Texture2D txDiffuse <
                    int vType = 5;
					int vID = 0;
                    >;
					
Texture2D txDiffuse2 <
                     int vType = 5;
					 int vID = 2;
                     >;
					
Texture2D NormMap   <
                    int vType = 5;
					int vID = 1;
                    >;
					
float3 DirectLightDir = {1,1,1};
float4 DirectLightColor = {1,1,1,1};

SamplerState samLinear
{
    //Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbNeverChanges
{
    matrix ev_View <
	               int vType = 3;
	               >;
};

cbuffer cbChangeOnResize
{
    matrix ev_Projection <
	                     int vType = 2;
	                     >;
};

cbuffer cbChangesEveryFrame
{
    matrix ev_World <
	                int vType = 1;
	                >;
					
	matrix ev_VP   <
	               int vType = 6;
	               >;
};

BlendState UIBlend
{
  AlphaToCoverageEnable = FALSE;
  BlendEnable[0] = TRUE;
  SrcBlend = SRC_ALPHA;
  DestBlend = INV_SRC_ALPHA;
  BlendOp = ADD;
  SrcBlendAlpha = SRC_ALPHA;//ONE;
  DestBlendAlpha = INV_SRC_ALPHA;//ZERO;
  BlendOpAlpha = ADD;
  RenderTargetWriteMask[0] = 0x0F;
};

BlendState NoBlending
{
  BlendEnable[0] = FALSE;
  RenderTargetWriteMask[0] = 0x0F;
};

//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0; 
	output.Pos = mul( float4(input.Pos,1), ev_World );
	output.PosH = output.Pos;
	output.Pos = mul( output.Pos, ev_VP );
	output.Norm = mul( input.Norm, ev_World );
    output.Color = input.Color;
	output.Tex = input.Tex;
    return output;
}

//--------------------------------------------------------------------------------------

PS_INPUT_BONES VSBones( VS_INPUT_BONES input )
{
    PS_INPUT_BONES output = (PS_INPUT_BONES)0; 

	output.Pos = mul( float4(input.Pos,1), ev_World );
	output.PosH = output.Pos;
	output.Pos = mul( output.Pos, ev_VP );
	output.Norm = mul( input.Norm, ev_World );
	
    output.Color = input.Color;
	output.Tex = input.Tex;
    return output;
}
//--------------------------------------------------------------------------------------

float4 PSBones( PS_INPUT_BONES input) : SV_Target
{
  float4 finalColor = input.Color;
  finalColor *= saturate(dot(input.Norm,normalize(DirectLightDir)))*DirectLightColor;
  
  return finalColor;
}

//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input, uint FaceID : SV_PrimitiveID) : SV_Target
{
	float4 finalColor = input.Color;
	
	if ((FaceID == 2) || (FaceID == 3))
	{
	  finalColor = txDiffuse.Sample( samLinear, float2(-input.Tex.y,input.Tex.x));
	}
	else
	  finalColor = txDiffuse2.Sample( samLinear, input.Tex);
	
	return finalColor;
}

//--------------------------------------------------------------------------------------

technique10 Render <int tID = 0;>
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------

technique10 RenderBones <int tID = 1;>
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, VSBones() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSBones() ) );
    }
}

//--------------------------------------------------------------------------------------