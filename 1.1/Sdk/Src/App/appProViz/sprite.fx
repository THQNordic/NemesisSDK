//--------------------------------------------------------------------------------------
//	global
//--------------------------------------------------------------------------------------
float4x4 g_mWorldViewProj : WORLDVIEWPROJ;

struct VS_IN
{
	float4 Position	: POSITION;
	float4 Color0	: COLOR0;
	float4 Color1	: COLOR1;
    float2 UV		: TEXCOORD0;
};

struct VS_OUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR0;
    float2 UV		: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
//	sprite
//--------------------------------------------------------------------------------------
texture g_texSprite;
float4	g_vecTexSize; // x,y = texsize, z,w = 1/texsize

sampler2D g_samSprite = sampler_state
{
	Texture = <g_texSprite>;
	AddressU = Clamp;
	AddressV = Clamp;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
};

//--------------------------------------------------------------------------------------
VS_OUT VS_Sprite( VS_IN IN, uniform const bool bUseColor1 = false )
{
	VS_OUT OUT;
	OUT.Position = mul( IN.Position, g_mWorldViewProj );
	OUT.Color	 = bUseColor1 ? IN.Color1 : IN.Color0;
	OUT.UV		 = IN.UV;
	return OUT;
}

//--------------------------------------------------------------------------------------
float4 PS_Sprite( VS_OUT IN ) : COLOR 
{
	return tex2D( g_samSprite, IN.UV ) * IN.Color;
}

//--------------------------------------------------------------------------------------
float4 PS_SpriteOutline( VS_OUT IN ) : COLOR 
{
	// offset
	float2 offset = 1.0f * g_vecTexSize.zw;

	// outline
	float4 outline = 0;
	outline += tex2D( g_samSprite, IN.UV + float2(-1,-1)*offset );
	outline += tex2D( g_samSprite, IN.UV + float2( 0,-1)*offset );
	outline += tex2D( g_samSprite, IN.UV + float2( 1,-1)*offset );
																		
	outline += tex2D( g_samSprite, IN.UV + float2(-1, 0)*offset );
	outline += tex2D( g_samSprite, IN.UV + float2( 1, 0)*offset );
																		
	outline += tex2D( g_samSprite, IN.UV + float2(-1, 1)*offset );
	outline += tex2D( g_samSprite, IN.UV + float2( 0, 1)*offset );
	outline += tex2D( g_samSprite, IN.UV + float2( 1, 1)*offset );

	outline = saturate(outline);
	
	return float4( IN.Color.rgb, IN.Color.a*outline.a );
}

//--------------------------------------------------------------------------------------
float4 PS_SpriteShadow( VS_OUT IN, uniform const int offset ) : COLOR 
{
	int thickness = 4;
	float4 shadow = 0;
	for ( int i = offset;i<thickness;++i)
		shadow += tex2D( g_samSprite, IN.UV + float2( -1.0, -1.0 ) * g_vecTexSize.zw * i );

	shadow /= (thickness-offset);
	return float4( IN.Color.rgb, IN.Color.a*shadow.a );
}

//-----------------------------------
float4 PS_SpriteEmboss( VS_OUT IN): COLOR
{
	// derevation
	float4 ddx = tex2D( g_samSprite, IN.UV+float2(1,0)*g_vecTexSize.zw ) - tex2D( g_samSprite, IN.UV+float2(-1,0)*g_vecTexSize.zw );
	float4 ddy = tex2D( g_samSprite, IN.UV+float2(0,1)*g_vecTexSize.zw ) - tex2D( g_samSprite, IN.UV+float2(0,-1)*g_vecTexSize.zw );

	float4 Result = tex2D( g_samSprite, IN.UV );
	Result.rgb *= saturate((ddx.a+ddy.a)* 0.5 + 1.0);
	return IN.Color * Result;

}

//--------------------------------------------------------------------------------------
technique Sprite
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_Sprite();
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Outline
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteOutline();
	
		CullMode = None;
		FogEnable = False;
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_Sprite();
	
		CullMode = None;
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Shadow
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteShadow(0);
	
		CullMode = None;
		FogEnable = False;
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_Sprite();
	
		CullMode = None;
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Emboss
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_SpriteEmboss();
	
		CullMode = None;
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Outline_Shadow
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteOutline();
	
		CullMode = None;
		FogEnable = False;
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteShadow(1);
	
		CullMode = None;
		FogEnable = False;
	}
		
	pass p2
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_Sprite();
	
		CullMode = None;
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Outline_Emboss
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteOutline();
	
		CullMode = None;
		FogEnable = False;
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_SpriteEmboss();
	
		CullMode = None;
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Shadow_Emboss
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteShadow(0);
	
		CullMode = None;
		FogEnable = False;
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_SpriteEmboss();
	
		CullMode = None;
		FogEnable = False;
	}
}

//--------------------------------------------------------------------------------------
technique Sprite_Outline_Shadow_Emboss
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteOutline();
	
		CullMode = None;
		FogEnable = False;
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 VS_Sprite(true);
		PixelShader = compile ps_2_0 PS_SpriteShadow(1);
	
		CullMode = None;
		FogEnable = False;
	}
		
	pass p2
	{
		VertexShader = compile vs_2_0 VS_Sprite();
		PixelShader = compile ps_2_0 PS_SpriteEmboss();
	
		CullMode = None;
		FogEnable = False;
	}
}
