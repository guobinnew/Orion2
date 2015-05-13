/////////////////////////////////////////////////////////
// Blur horizontal effect
varying mediump vec2 vTex;
uniform mediump sampler2D samplerFront;

uniform mediump float pixelWidth;
uniform mediump float intensity;

void main(void)
{
	mediump vec4 sum = vec4(0.0);
	mediump float halfPixelWidth = pixelWidth / 2.0;
	
	// Add nine horizontal samples, with mid-pixel sampling to get two pixels in one
	sum += texture2D(samplerFront, vTex - vec2(pixelWidth * 7.0 + halfPixelWidth, 0.0)) * 0.06;
	sum += texture2D(samplerFront, vTex - vec2(pixelWidth * 5.0 + halfPixelWidth, 0.0)) * 0.10;
	sum += texture2D(samplerFront, vTex - vec2(pixelWidth * 3.0 + halfPixelWidth, 0.0)) * 0.13;
	sum += texture2D(samplerFront, vTex - vec2(pixelWidth * 1.0 + halfPixelWidth, 0.0)) * 0.16;
	mediump vec4 front = texture2D(samplerFront, vTex);
	sum += front * 0.10;
	sum += texture2D(samplerFront, vTex + vec2(pixelWidth * 1.0 + halfPixelWidth, 0.0)) * 0.16;
	sum += texture2D(samplerFront, vTex + vec2(pixelWidth * 3.0 + halfPixelWidth, 0.0)) * 0.13;
	sum += texture2D(samplerFront, vTex + vec2(pixelWidth * 5.0 + halfPixelWidth, 0.0)) * 0.10;
	sum += texture2D(samplerFront, vTex + vec2(pixelWidth * 7.0 + halfPixelWidth, 0.0)) * 0.06;
	
	gl_FragColor = mix(front, sum, intensity);
}
