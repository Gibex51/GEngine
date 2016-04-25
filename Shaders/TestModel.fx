//--------------------------------------------------------------------------------------
#include "DefPool.gfxp"

Texture2D txDiffuse    <int vType = VARTYPE_RESOURCE; int vID = 0;>;	
Texture2D txNormal     <int vType = VARTYPE_RESOURCE; int vID = 1;>;				
						
float3 DirectLightDir  <int vType = VARTYPE_CUSTOM; int vID = 10;> = {-1,1,-0.5f};

//----- Instance Variables -----//
uint nBones                     <int vType = VARTYPE_CUSTOM; int vID = 3;>;
uint nInst                      <int vType = VARTYPE_CUSTOM; int vID = 4;>;

Buffer <float4> BonesAndTransf  <int vType = VARTYPE_CUSTOM; int vID = 5;>;
Buffer <uint> TexIndex          <int vType = VARTYPE_CUSTOM; int vID = 6;>;
//------------------------------//
	
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbNeverChanges
{
    matrix ev_View <
	               int vType = VARTYPE_MVIEW;
	               >;
};

cbuffer cbChangeOnResize
{
    matrix ev_Projection <
	                     int vType = VARTYPE_MPROJ;
	                     >;
};

cbuffer cbChangesEveryFrame
{
    matrix ev_World <
	                int vType = VARTYPE_MWORLD;
	                >;
					
	matrix ev_VP   <
	               int vType = VARTYPE_MVIEWPROJ;
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

float3 bone_transf(int pindex, int qindex, float3 pos )
{
  return BonesAndTransf.Load(pindex).xyz + qtransform(pos, BonesAndTransf.Load(qindex));
}

float3 bone_transf_norm(int qindex, float3 norm )
{
  return qtransform(norm, BonesAndTransf.Load(qindex));
}

matrix get_inst_transform(uint inst_id)
{
  float4 row1 = BonesAndTransf.Load(inst_id*4);
  float4 row2 = BonesAndTransf.Load(inst_id*4 + 1);
  float4 row3 = BonesAndTransf.Load(inst_id*4 + 2);
  float4 row4 = BonesAndTransf.Load(inst_id*4 + 3);
        
  return float4x4( row1, row2, row3, row4 );
}

//--------------------------------------------------------------------------------------

PS_INPUT_LBONES VSBones( VS_INPUT_LBONES input , uint inst_id : SV_InstanceID)
{
  PS_INPUT_LBONES output = (PS_INPUT_LBONES)0; 

  float3 transf_pos = float3(0,0,0);
  float3 transf_norm = float3(0,0,0);
  
  if (nBones == 0) 
  {
    transf_pos = input.Pos;
	transf_norm = input.Norm;
  }
  
  uint   p_offs = nInst * 4 + inst_id * nBones;
  uint   q_offs = p_offs + nInst * nBones;
  
  if (input.Weights.x > EPS)
  {
    transf_pos += input.Weights.x * bone_transf(p_offs + input.Bones.x, q_offs + input.Bones.x, input.Pos);
	transf_norm += input.Weights.x * bone_transf_norm(q_offs + input.Bones.x, input.Norm);
  }
  
  if (input.Weights.y > EPS)
  {
    transf_pos += input.Weights.y * bone_transf(p_offs + input.Bones.y, q_offs + input.Bones.y, input.Pos);
	transf_norm += input.Weights.y * bone_transf_norm(q_offs + input.Bones.y, input.Norm);
  } 
	
  if (input.Weights.z > EPS)
  {
    transf_pos += input.Weights.z * bone_transf(p_offs + input.Bones.z, q_offs + input.Bones.z, input.Pos);
	transf_norm += input.Weights.z * bone_transf_norm(q_offs + input.Bones.z, input.Norm);
  } 
	  
  if (input.Weights.w > EPS)
  {
    transf_pos += input.Weights.w * bone_transf(p_offs + input.Bones.w, q_offs + input.Bones.w, input.Pos);
	transf_norm += input.Weights.w * bone_transf_norm(q_offs + input.Bones.w, input.Norm);
  }
  
  output.Pos = mul(mul( float4(transf_pos, 1.0f) , mul(ev_World, get_inst_transform(inst_id)) ), ev_VP );;
  output.Norm = normalize(mul(float4(transf_norm, 1.0f), ev_World ));
  output.Tex = input.Tex;
  output.TexIndex = TexIndex.Load(inst_id);
  return output;
}

//--------------------------------------------------------------------------------------

float4 PSBones( PS_INPUT_LBONES input) : SV_Target
{
  //return txDiffuse.Sample( samLinear, input.Tex );
  //float4 finalColor = float4(1,1,1,1);
  //float3 fNormal = input.Norm;
  
  float4 finalColor = txDiffuse.Sample( samLinear, input.Tex );
  float3 fNormal = txNormal.Sample( samLinear, input.Tex ).xyz * input.Norm; 
  finalColor *= dot(fNormal, normalize(DirectLightDir));
  finalColor.a = 1.0f;
  return saturate(finalColor*2) * float4(1.0f*input.TexIndex, 1, 1, 1);
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