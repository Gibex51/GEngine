//--------------------------------------------------------------------------------------

Texture2D txDiffuse <
                    int vType = 5;
					int vID = 0;
                    >;
					
int Stage <
          int vType = 0;
		  int vID = 1;
          > = 0;
		  
float2 TexOffset <
                 int vType = 0;
		         int vID = 2;
                 > = {0,0};

float2 TexUVScale <
                  int vType = 0;
		          int vID = 3;
                  > = {1,1};	

Buffer <float4> mTransf <int vType = 0; int vID = 5;>;				  
		  
SamplerState samLinear
{
    Filter = ANISOTROPIC;//MIN_MAG_MIP_LINEAR;
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

DepthStencilState DisableDepth
{
  DepthEnable = false;
};

DepthStencilState EnableDepth
{
  DepthEnable = true;
};

struct VS_INPUT
{
    float3 Pos   : POSITION;
	float2 Tex   : TEXCOORD;	
};

struct PS_INPUT
{
    float4 Pos   : SV_POSITION;
	float2 Tex   : TEXCOORD0;
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

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0; 
	output.Pos = mul( float4(input.Pos,1), mul(ev_World, get_inst_transform(0)) );
    output.Pos = mul( output.Pos, ev_VP );
	
	output.Tex = input.Tex * TexUVScale + TexOffset;
	
    return output;
}

//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input) : SV_Target
{	
    if (Stage == 1)
	  input.Tex.x = (input.Pos.x%32)/64 + 0.5f;
	  
	return txDiffuse.Sample( samLinear, input.Tex );
}

//--------------------------------------------------------------------------------------

technique10 Render <int tType = 0;>
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		SetDepthStencilState( DisableDepth, 0 );
		SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------

technique10 Restore <int tType = 1;>
{
    pass P0
    {
        SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------
