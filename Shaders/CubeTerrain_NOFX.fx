//--------------------------------------------------------------------------------------

float3 TLightDir  <
                  int vType = 0;
				  int vID = 0;
				  > = float3(1,1,1);			  
				  
struct VS_INPUT
{
    float3 Pos    : POSITION;
	uint   Color  : COLOR;
	uint   Tex    : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos    : SV_POSITION;
	float  Color  : COLOR0;
	float3 Tex    : TEXCOORD0;
    float3 PosReal: POSITION;
};

Texture2DArray txDiffuse    <
                            int vType = 5;
					        int vID = 0;
                            >;					

Buffer <uint> bFaceArray    <
                            int vType = 5;
					        int vID = 1;
                            >;	
														
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
  Filter = MIN_MAG_MIP_POINT;
  AddressU = Wrap;
  AddressV = Wrap;
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
				   
	matrix ev_LVP  <
	               int vType = 0;
				   int vID = 2;
	               >;		   
};

cbuffer cbChangesEveryFrame
{
    matrix ev_World : World <
	                int vType = 1;
	                >;
};

//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input, uint VertexID : SV_VertexID )
{
    PS_INPUT output = (PS_INPUT)0; 
	output.Pos = mul( float4(input.Pos,1), ev_World );
	output.PosReal = output.Pos;
	output.Pos = mul( output.Pos, ev_VP );
    output.Color = input.Color;

	float3 UnpackedTex;
	UnpackedTex.x = ((input.Tex % 256) - 127)/100.0f;
	UnpackedTex.y = ((floor(input.Tex/256) % 256) - 127)/100.0f;
	UnpackedTex.z = ((floor(input.Tex/65536) % 256) - 127)/100.0f;

	output.Tex = UnpackedTex;
    return output;
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
	
	float4 OutColor = saturate(saturate(dot(Norm, TLightDir)) + float4(0.3f,0.3f,0.3f,1.0f));
	
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
	
	if (modtex.a == 1)
	  return modtex*OutColor;
	else
	  return maintex*OutColor;
}

//--------------------------------------------------------------------------------------

technique10 Render
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