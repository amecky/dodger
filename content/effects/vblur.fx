uniform extern float4x4 gWVP;
uniform extern texture gTex;
float texelSize = 1.0 / 720.0;
float weight0 = 1.0f;
float weight1 = 0.9f;
float weight2 = 0.55f;
float weight3 = 0.18f;
float weight4 = 0.1f;

sampler TexS = sampler_state {
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	//AddressU  = WRAP;
    //AddressV  = WRAP;
};

struct OutputVS {
	float4 posH   : POSITION0;
	float2 tex0   : TEXCOORD0;
	float4 color0 : COLOR0;
	float2 texCoord1 : TEXCOORD1;
    float2 texCoord2 : TEXCOORD2;
    float2 texCoord3 : TEXCOORD3;
    float2 texCoord4 : TEXCOORD4;
    float2 texCoord5 : TEXCOORD5;
    float2 texCoord6 : TEXCOORD6;
    float2 texCoord7 : TEXCOORD7;
};

OutputVS VBlurVS(float3 posL : POSITION0, float2 tex : TEXCOORD0, float4 color : COLOR0) {
	OutputVS outVS = (OutputVS)0;
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	outVS.tex0 = tex;
	outVS.color0 = color;
    outVS.texCoord1 = tex + float2(0.0f, texelSize * -3.0f);
    outVS.texCoord2 = tex + float2(0.0f, texelSize * -2.0f);
    outVS.texCoord3 = tex + float2(0.0f, texelSize * -1.0f);
    outVS.texCoord4 = tex + float2(0.0f, texelSize *  0.0f);
    outVS.texCoord5 = tex + float2(0.0f, texelSize *  1.0f);
    outVS.texCoord6 = tex + float2(0.0f, texelSize *  2.0f);
    outVS.texCoord7 = tex + float2(0.0f, texelSize *  3.0f);
	return outVS;
}

float4 VBlurPS(OutputVS input) : COLOR{
	float normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3));
    // Normalize the weights.
    float w0 = weight0 / normalization;
    float w1 = weight1 / normalization;
    float w2 = weight2 / normalization;
    float w3 = weight3 / normalization;
    // Initialize the color to black.
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // Add the nine vertical pixels to the color by the specific weight of each.
    color += tex2D(TexS, input.texCoord1) * w3;
    color += tex2D(TexS, input.texCoord2) * w2;
    color += tex2D(TexS, input.texCoord3) * w1;
    color += tex2D(TexS, input.texCoord4) * w0;
    color += tex2D(TexS, input.texCoord5) * w1;
    color += tex2D(TexS, input.texCoord6) * w2;
    color += tex2D(TexS, input.texCoord7) * w3;
    // Set the alpha channel to one.
    color.a = tex2D(TexS,input.tex0).a;
	return color; 
	//return tex2D(TexS,input.tex0);
}

technique VBlurTech {
	pass P0 {
		vertexShader = compile vs_3_0 VBlurVS();
		pixelShader = compile ps_3_0 VBlurPS();
	}
};