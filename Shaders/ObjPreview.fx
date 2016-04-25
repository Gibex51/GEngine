//--------------------------------------------------------------------------------------
#include "DefPool.gfxp"

Texture2DArray txDiffuse <
                    int vType = 5;
					int vID = 0;
                    >;
					
Buffer <float4> mTransf <int vType = 0; int vID = 5;>;
					
SamplerState samLinear
{
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

matrix get_inst_transform(uint inst_id)
{
  float4 row1 = mTransf.Load(inst_id*4);
  float4 row2 = mTransf.Load(inst_id*4 + 1);
  float4 row3 = mTransf.Load(inst_id*4 + 2);
  float4 row4 = mTransf.Load(inst_id*4 + 3);
        
  return float4x4( row1, row2, row3, row4 );
}

//--------------------------------------------------------------------------------------

PS_INPUT_OP VS( VS_INPUT_OP input )
{
    PS_INPUT_OP output = (PS_INPUT_OP)0; 
	output.Pos = mul( float4(input.Pos,1), mul(ev_World, get_inst_transform(0)) );
	output.PosH = output.Pos;
	output.Pos = mul( output.Pos, ev_VP );
	output.Tex = input.Tex;
	output.Col = input.Col;
    return output;
}

//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT_OP input) : SV_Target
{
	float4 modtex = float4(0,0,0,0);
	float4 maintex = float4(0,0,0,0);
	
	if (input.Col.x > 0)	
	  maintex = txDiffuse.Sample( samLinear, float3(input.Tex,input.Col.x-1));
	
	if (input.Col.y > 0) 
	  modtex = txDiffuse.Sample( samLinear, float3(input.Tex,input.Col.y-1));	
	
	if (modtex.a == 1)
	  return modtex*1.2f;
	else
	  return maintex*1.2f;
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
