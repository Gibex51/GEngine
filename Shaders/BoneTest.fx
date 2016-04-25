//--------------------------------------------------------------------------------------
#include "DefPool.gfxp"

Texture2D txDiffuse <
                    int vType = 5;
					int vID = 0;
                    >;
	
G_BONES_SET TestBones[MAX_BONES] <
                                 int vType = 0;
					             int vID = 2;
                                 >;
							
float3 DirectLightDir = {-1,1,-0.5f};

//-----------------------------//
	
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_POINT;
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
  SrcBlendAlpha = SRC_ALPHA;
  DestBlendAlpha = INV_SRC_ALPHA;
  BlendOpAlpha = ADD;
  RenderTargetWriteMask[0] = 0x0F;
};

BlendState NoBlending
{
  BlendEnable[0] = FALSE;
  RenderTargetWriteMask[0] = 0x0F;
};

//--------------------------------------------------------------------------------------

float3 qtransform( in float3 v, in float4 q )
{ 
  return v + 2.0f*cross(cross(v, q.xyz) + q.w*v, q.xyz);
}

float3 bone_transf( int index, float3 pos )
{
  return TestBones[index].position + qtransform( pos, TestBones[index].orientation ).xyz;
}

float3 bone_transf_norm( int index, float3 norm )
{
  return qtransform( norm, TestBones[index].orientation ).xyz;
}
//--------------------------------------------------------------------------------------

PS_INPUT_LBONES VSBones( VS_INPUT_LBONES input )
{
  PS_INPUT_LBONES output = (PS_INPUT_LBONES)0; 

  float3 transf_pos = float3(0, 0, 0);
  float3 transf_norm = float3(0,0,0);
  if (input.Weights.x > EPS)
  {
    transf_pos += input.Weights.x * bone_transf( input.Bones.x, input.Pos );
	transf_norm += input.Weights.x * bone_transf_norm( input.Bones.x, input.Norm );
  }
  
  if (input.Weights.y > EPS)
  {
    transf_pos += input.Weights.y * bone_transf( input.Bones.y, input.Pos );
	transf_norm += input.Weights.y * bone_transf_norm( input.Bones.y, input.Norm );
  } 
	
  if (input.Weights.z > EPS)
  {
    transf_pos += input.Weights.z * bone_transf( input.Bones.z, input.Pos );
	transf_norm += input.Weights.z * bone_transf_norm( input.Bones.z, input.Norm );
  } 
	  
  if (input.Weights.w > EPS)
  {
    transf_pos += input.Weights.w * bone_transf( input.Bones.w, input.Pos );
	transf_norm += input.Weights.w * bone_transf_norm( input.Bones.w, input.Norm );
  }
  
  output.Pos = mul( mul( float4(transf_pos, 1.0f), ev_World ), ev_VP );
  output.Norm = normalize(mul(transf_norm, ev_World ));
  
  output.Tex = input.Tex;
  return output;
}
//--------------------------------------------------------------------------------------

float4 PSBones( PS_INPUT_LBONES input) : SV_Target
{
  //return txDiffuse.Sample( samLinear, input.Tex );
  float4 finalColor = float4(1,1,1,1);
  finalColor *= dot(-input.Norm, normalize(DirectLightDir));
  finalColor.a = 1.0f;
  return finalColor;
}

//--------------------------------------------------------------------------------------

technique10 Render <int tID = 0;>
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, VSBones() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSBones() ) );
		SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------