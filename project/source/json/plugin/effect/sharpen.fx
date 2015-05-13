/////////////////////////////////////////////////////////
// Sharpen effect
// Based on code from GPUImage: https://github.com/BradLarson/GPUImage
varying mediump vec2 vTex;
uniform lowp sampler2D samplerFront;
uniform mediump float pixelWidth;
uniform mediump float pixelHeight;
uniform lowp float intensity;

void main(void)
{
	mediump float centerMultiplier = 1.0 + 4.0 * intensity;
	
	mediump vec4 front = texture2D(samplerFront, vTex);
	mediump vec3 textureColor = front.rgb;
	mediump vec3 leftTextureColor = texture2D(samplerFront, vTex - vec2(pixelWidth, 0.0)).rgb;
	mediump vec3 rightTextureColor = texture2D(samplerFront, vTex + vec2(pixelWidth, 0.0)).rgb;
	mediump vec3 topTextureColor = texture2D(samplerFront, vTex - vec2(0.0, pixelHeight)).rgb;
	mediump vec3 bottomTextureColor = texture2D(samplerFront, vTex + vec2(0.0, pixelHeight)).rgb;

	gl_FragColor = vec4((textureColor * centerMultiplier - (leftTextureColor * intensity + rightTextureColor * intensity + topTextureColor * intensity + bottomTextureColor * intensity)), front.a);
}
