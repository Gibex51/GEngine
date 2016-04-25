//--------------------------------------------------------------------------------------

Texture2D txDiffuse <
                    int vType = 5;
					int vID = 0;
                    >;
								
struct GUI_CONTROL_SET
{
  float2 tex_offset;
  float2 tex_scale;
  uint2  tex_size;
  uint2  ctrl_size;
  uint4  borders;
};

int2 ExtraState <
                int vType = 0;
                int vID = 8;
                > = {0,0};

GUI_CONTROL_SET CtrlSet <
                        int vType = 0;
		                int vID = 9;
                        > = {{0,0},{1,1},{64,64},{64,64},{0,0,0,0}};
	
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

float border_tex(uint bord_l, uint bord_r, uint tex_sz, uint scaled_sz, float in_tex)
{
    float rpos          = float(in_tex) * scaled_sz; 
    float scale_offs    = scaled_sz / float(tex_sz) - 1.0f;
    float new_tex_coord = rpos / float(tex_sz);
 
    if (rpos < bord_l)
	  return new_tex_coord;
	
    if (rpos >= scaled_sz - bord_r)
      return new_tex_coord - scale_offs;
	
    return new_tex_coord - (rpos - bord_l)*scale_offs/(scaled_sz - bord_r - bord_l);
}

//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0; 
	output.Pos = mul( float4(input.Pos,1), mul(ev_World, get_inst_transform(0)) );
    output.Pos = mul( output.Pos, ev_VP );
	
	output.Tex = input.Tex;
	
    return output;
}

//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input) : SV_Target
{	
	if (ExtraState.x == 0)
	  input.Tex.x = border_tex(CtrlSet.borders.x, CtrlSet.borders.z, CtrlSet.tex_size.x, CtrlSet.ctrl_size.x, 
	                           input.Tex.x) * CtrlSet.tex_scale.x + CtrlSet.tex_offset.x;
	if (ExtraState.y == 0)					 
	  input.Tex.y = border_tex(CtrlSet.borders.y, CtrlSet.borders.w, CtrlSet.tex_size.y, CtrlSet.ctrl_size.y, 
	                           input.Tex.y) * CtrlSet.tex_scale.y + CtrlSet.tex_offset.y;
							 
	if (ExtraState.x == 1)
	  input.Tex.x = (input.Pos.x%(CtrlSet.tex_size.x * CtrlSet.tex_scale.x))/CtrlSet.tex_size.x + CtrlSet.tex_offset.x;
	  
	if (ExtraState.y == 1)
	  input.Tex.y = (input.Pos.y%(CtrlSet.tex_size.y * CtrlSet.tex_scale.y))/CtrlSet.tex_size.y + CtrlSet.tex_offset.y;
		  
	return txDiffuse.Sample( samLinear, input.Tex );
}

//--------------------------------------------------------------------------------------

technique10 Render <int tID = 0;>
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

technique10 Restore <int tID = 1;>
{
    pass P0
    {
        SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------
