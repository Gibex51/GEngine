//--------------------------------------------------------------------------------------

float3 TLightDir  <
                  int vType = 0;
				  int vID = 0;
				  > = float3(-300,-30,-300);			  
				  
struct VS_INPUT
{
    float3 Pos    : POSITION;
	uint   Color  : COLOR;
	float3 Tex    : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos    : SV_POSITION;
	float  Color  : COLOR0;
	float3 Tex    : TEXCOORD0;
	float4 lp[3]  : TEXCOORD1;
    float3 PosReal: POSITION;
	float  Fog    : COLOR1;
};

struct SM_VS_OUT
{
    float4 Pos    : SV_POSITION;
	float3 Tex    : TEXCOORD0;
};


Texture2DArray txDiffuse    <
                            int vType = 5;
					        int vID = 0;
                            >;					

Buffer <uint> bFaceArray    <
                            int vType = 5;
					        int vID = 1;
                            >;	

							
Texture2D txShadowMap       <
                            int vType = 5;
					        int vID = 2;
                            >;
													
bool sm_enabled             <
                            int vType = 0;
					        int vID = 3;
                            > = false;
							
float3 sm_lightdir          <
                            int vType = 0;
					        int vID = 4;
                            > = float3(0,0,0);	

float3 view_point           <
                            int vType = 0;
					        int vID = 6;
                            > = float3(0,0,0);

bool fog_enabled            <
                            int vType = 0;
					        int vID = 7;
                            > = true;							
							
float shadowMapBias[3] = {0.00005f, 0.0001f, 0.0002f};
uint2 shadowMapSize = uint2(2048,6144);

float4 fogColor = float4(0.63f, 0.35f, 0.10f, 1);
float fogDensity = 0.00075f;

uint Offset	<
            int vType = 0;
			int vID = 1;
            >;		
			
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samLinear
{
  Filter = MIN_MAG_MIP_LINEAR;
  AddressU = Wrap;
  AddressV = Wrap;
};

SamplerState samPoint
{
  Filter = MIN_MAG_MIP_POINT;
  AddressU = Wrap;
  AddressV = Wrap;
};

SamplerComparisonState samCMP
{
  Filter = COMPARISON_MIN_MAG_MIP_POINT;
  AddressU = MIRROR;
  AddressV = MIRROR;
  ComparisonFunc = LESS_EQUAL;   
};

BlendState BlendEnable
{
  AlphaToCoverageEnable = TRUE;
  BlendEnable[0] = TRUE;
  SrcBlend = SRC_ALPHA;
  DestBlend = INV_SRC_ALPHA;
  BlendOp = ADD;
  SrcBlendAlpha = ZERO;
  DestBlendAlpha = ONE;
  BlendOpAlpha = ADD;
  RenderTargetWriteMask[0] = 0x0F;
};

cbuffer cbChangeOnResize
{
	matrix ev_VP : ViewProjection  <
	               int vType = 6;
	               >; 					 
};

cbuffer cbChangesEveryFrame
{
    matrix ev_World : World <
	                        int vType = 1;
	                        >;
							
	matrix ev_LV     <
	                 int vType = 0;
				     int vID = 2;
	                 >;
					 
	matrix ev_LP[3]  <
	                 int vType = 0;
				     int vID = 5;
	                 >;
};

//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input, uint VertexID : SV_VertexID )
{
    PS_INPUT output = (PS_INPUT)0; 
	
	output.Pos = mul( float4(input.Pos,1), ev_World );
	output.PosReal = output.Pos;
	
	float4 wvpos = mul(output.Pos, ev_LV); 
	for (int i = 0; i < 3; i++)
	  output.lp[i] = mul(wvpos, ev_LP[i]);
	  
	output.Pos = mul( output.Pos, ev_VP );
    output.Color = input.Color;

	output.Tex = input.Tex;
		
	output.Fog = clamp(exp(-distance(view_point, output.PosReal)*fogDensity), 0.0f, 1.0f);
	
    return output;
}

//--------------------------------------------------------------------------------------

float2 texOffset( int u, int v, int level)
{
  return float2( u * 1.0f/shadowMapSize.x, v * 1.0f/shadowMapSize.y + level*(1.0f/3.0f));
}

//--------------------------------------------------------------------------------------

float GenNoise(int x, int y)
{
   int n = x + y * 57;
   return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

//--------------------------------------------------------------------------------------

float4 FogIt(float4 inColor, float inFogFactor)
{
  if (!fog_enabled) return inColor;
  
  float4 fogged = inFogFactor * inColor + (1.0f - inFogFactor) * fogColor;
  if (inColor.a < 0.6f) fogged.a = 0; else fogged.a = 1;
  return fogged;
}

//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT input, uint FaceID : SV_PrimitiveID) : SV_Target
{
	uint FAValue = bFaceArray.Load(Offset + floor(FaceID/2));
    uint Dir = floor(FAValue/262144);
	uint TexType1 = FAValue%512;
	uint TexType2 = floor(FAValue/512)%512;
	
	float3 Norm = float3(0,0,0);
	Norm.x = (Dir % 25)/12.0f - 1.0f;
	Norm.y = (floor(Dir/25) % 25)/12.0f - 1.0f;
	Norm.z = (floor(Dir/625) % 25)/12.0f - 1.0f;
	
	float4 OutColor = saturate(saturate(dot(Norm,float3(1,1,1))) + float4(0.3f,0.3f,0.3f,1.0f));
	OutColor.a = 1.0f;
	
    float4 modtex = float4(0,0,0,0);
	float4 maintex = float4(0,0,0,0);
	
	float2 texcoord = float2(0,0);
	
	if ((abs(Norm.x) >= abs(Norm.z)) && (abs(Norm.x) >= abs(Norm.y)))
	  texcoord = float2(input.Tex.z,-input.Tex.y);
    if ((abs(Norm.y) >= abs(Norm.x)) && (abs(Norm.y) >= abs(Norm.z)))
	  texcoord = input.Tex.xz;
	if ((abs(Norm.z) >= abs(Norm.x)) && (abs(Norm.z) >= abs(Norm.y)))
	  texcoord = -input.Tex.xy;
	  
	  
	if (TexType1 > 0)	
	  maintex = txDiffuse.Sample( samAnisotropic, float3(texcoord,TexType1-1));
	
	if (TexType2 > 0) 
	  modtex = txDiffuse.Sample( samAnisotropic, float3(texcoord,TexType2-1));	
		
	if (!sm_enabled)
	{
	  if (modtex.a == 1)
	    return FogIt(modtex*OutColor, input.Fog);
	  else
	    return FogIt(maintex*OutColor, input.Fog);
	}
	
	//-----------------------------------------------------------------------------
	float4 ShadowColor = float4(0.4f,0.4f,0.4f,1.0f);
	float4 LightColor = float4(0.6f,0.6f,0.6f,1.0f);
	
	int level = 0;
	input.lp[0].xyz /= input.lp[0].w;
	if( input.lp[level].x < -1.0f || input.lp[level].x > 1.0f || 
	    input.lp[level].y < -1.0f || input.lp[level].y > 1.0f ||
	    input.lp[level].z < 0.0f  || input.lp[level].z > 1.0f )
	{	  
	  level++;
	  input.lp[1].xyz /= input.lp[1].w;
	  if( input.lp[level].x < -1.0f || input.lp[level].x > 1.0f || 
	      input.lp[level].y < -1.0f || input.lp[level].y > 1.0f ||
	      input.lp[level].z < 0.0f  || input.lp[level].z > 1.0f )
	  { 
	    level++;
		input.lp[2].xyz /= input.lp[2].w;
	    if( input.lp[level].x < -1.0f || input.lp[level].x > 1.0f || 
	        input.lp[level].y < -1.0f || input.lp[level].y > 1.0f ||
	        input.lp[level].z < 0.0f  || input.lp[level].z > 1.0f )
	    { 
	      if (modtex.a == 1)
	        return FogIt(modtex, input.Fog);
	      else
	        return FogIt(maintex, input.Fog);
	    }
	  }
	}
	
	float4 lpos = input.lp[level];
	
	//transform clip space coords to texture space coords (-1:1 to 0:1)	
	lpos.x = lpos.x/2.0f + 0.5f;
	lpos.y = (lpos.y/-2.0f + 0.5f)/3.0f;

	//shadow map bias
	lpos.z -= shadowMapBias[level];
    //lpos.z -= 0.0000003;
	//lpos.z -= 0.0001f;

	//sample shadow map - point sampler
	//float shadowMapDepth = txShadowMap.Sample(samLinear, lpos.xy).r;
	//float shadowFactor = lpos.z <= shadowMapDepth;

    //PCF sampling for shadow map
	float sum = 0;
	float x, y;	

	//perform PCF filtering on a 4 x 4 texel neighborhood
	for (y = -1.5; y <= 1.5; y += 1.0)
	{
	  for (x = -1.5; x <= 1.5; x += 1.0)
	  {
        sum += txShadowMap.SampleCmpLevelZero(samCMP, lpos.xy + texOffset(x,y,level), lpos.z);			
	  }
	}

	float shadowFactor = sum / 16.0;		
	
	//calculate ilumination at fragment	
	//float ndotl = dot(Norm, sm_lightdir);
	//if (ndotl > 0) ndotl = 1; else ndotl = 0;

	if (modtex.a == 1)
	  return FogIt(modtex*(ShadowColor + (shadowFactor*LightColor)), input.Fog);
	else
	  return FogIt(maintex*(ShadowColor + (shadowFactor*LightColor)), input.Fog);
}

//--------------------------------------------------------------------------------------

SM_VS_OUT SM_VS( VS_INPUT input)
{
    SM_VS_OUT output = (SM_VS_OUT)0; 
	
	//output.Pos = mul(float4(input.Pos,1), mul(ev_World, mul(ev_LV, ev_LP[0])));
	output.Pos = mul(mul(float4(input.Pos,1), ev_World), ev_VP);
	output.Tex = input.Tex;
	
    return output;
}

//--------------------------------------------------------------------------------------

float4 SM_PS( SM_VS_OUT input , uint FaceID : SV_PrimitiveID) : SV_Target
{
	uint FAValue = bFaceArray.Load(Offset + floor(FaceID/2));
    uint Dir = floor(FAValue/262144);
	uint TexType1 = FAValue%512;
	uint TexType2 = floor(FAValue/512)%512;
	
	float3 Norm = float3(0,0,0);
	Norm.x = (Dir % 25)/12.0f - 1.0f;
	Norm.y = (floor(Dir/25) % 25)/12.0f - 1.0f;
	Norm.z = (floor(Dir/625) % 25)/12.0f - 1.0f;

	float4 maintex = float4(0, 0, 0, 1.0f);
	float2 texcoord = float2(0, 0);
	
	if ((abs(Norm.x) >= abs(Norm.z)) && (abs(Norm.x) >= abs(Norm.y)))
	  texcoord = float2(input.Tex.z,-input.Tex.y);
    if ((abs(Norm.y) >= abs(Norm.x)) && (abs(Norm.y) >= abs(Norm.z)))
	  texcoord = input.Tex.xz;
	if ((abs(Norm.z) >= abs(Norm.x)) && (abs(Norm.z) >= abs(Norm.y)))
	  texcoord = -input.Tex.xy;
	  
	if (TexType1 > 0)	
	  maintex = txDiffuse.Sample( samPoint, float3(texcoord,TexType1-1));
	
	return maintex;
}

//--------------------------------------------------------------------------------------

technique10 Render <int tID = 0;>
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetBlendState( BlendEnable, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------

technique10 SM_Render <int tID = 1;>
{
    pass P0
    {
		SetVertexShader(   CompileShader( vs_4_0, SM_VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader(    CompileShader( ps_4_0, SM_PS() ) );
		
		SetBlendState( BlendEnable, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

//--------------------------------------------------------------------------------------
