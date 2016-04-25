//--------------------------------------------------------------------------------------
#include "DefPool.gfxp"

Texture2D txDiffuse <
                    int vType = 5;
					int vID = 0;
                    >;
					
Texture2D NormMap   <
                    int vType = 5;
					int vID = 1;
                    >;
					
//#define RGB_TO_fRGB 1.0f/255.0f
										
float3 DirectLightDir = {1,1,1};
float4 DirectLightColor = {1,1,1,1};

Buffer <float4> mTransf <int vType = 0; int vID = 5;>;

//Buffer <uint> VideoBuf <int vType = 0; int vID = 10;>;
//uint vHeight <int vType = 0; int vID = 11;> = 1;
//uint vWidth  <int vType = 0; int vID = 12;> = 1;

SamplerState samLinear
{
    //Filter = MIN_MAG_MIP_LINEAR;
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
  uint offset = inst_id*4;   
  return float4x4(mTransf.Load(offset), 
                  mTransf.Load(offset+1), 
				  mTransf.Load(offset+2), 
				  mTransf.Load(offset+3));
}

/*
float4 getVideoPixel(float2 tex_coord)
{
  uint offset = (uint(vHeight*tex_coord.x)*vWidth + uint(vWidth*tex_coord.y))*3;
  return float4(VideoBuf.Load(offset)*RGB_TO_fRGB, 
                VideoBuf.Load(offset+1)*RGB_TO_fRGB, 
				VideoBuf.Load(offset+2)*RGB_TO_fRGB, 
				1.0f);
}
*/

//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0; 
	matrix mWorldTransf = mul(ev_World, get_inst_transform(0));
	output.Pos = mul( float4(input.Pos,1), mWorldTransf );
	output.PosH = output.Pos;
	output.Pos = mul( output.Pos, ev_VP );
	output.Norm = mul( input.Norm, mWorldTransf );
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

float4 PS( PS_INPUT input) : SV_Target
{
	/*float4 finalColor = input.Color;
	
	float3 vNormalTS = normalize(normalize(NormMap.Sample( samLinear, input.Tex )*2-1) + input.Norm);
	
	for (int i=0; i<nLights; i++)
	{
	  if (TestLight[i].enabled == 1)
	  {
		float Dist = pow(distance(input.PosH, float4(TestLight[i].pos,1)),3);
		float4 vDir = input.PosH - float4(TestLight[i].pos,1);    
	    finalColor += ((dot(-(float3)vDir,vNormalTS)/Dist)) * TestLight[i].diffuse * 1500; 
	  }
	}
	
	return txDiffuse.Sample( samLinear, input.Tex ) * finalColor;*/
	
	return txDiffuse.Sample( samLinear, input.Tex );
	
	//return getVideoPixel(input.Tex);
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