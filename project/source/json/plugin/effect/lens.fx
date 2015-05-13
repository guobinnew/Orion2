/////////////////////////////////////////////////////////
// Lens effect
varying mediump vec2 vTex;
uniform lowp sampler2D samplerFront;
uniform lowp sampler2D samplerBack;
uniform mediump vec2 destStart;
uniform mediump vec2 destEnd;
uniform mediump float pixelWidth;
uniform mediump float pixelHeight;
uniform mediump float magnification;

void main(void)
{	
	lowp vec4 front = texture2D(samplerFront, vTex);
	
	// Lens distort
	mediump float zoomFactor = (0.5 - (front.r * 0.299 + front.g * 0.587 + front.b * 0.114) * magnification) * front.a;
	mediump vec2 center = (destStart + destEnd) / 2.0;
	
	mediump vec2 p = vTex;
	p += (vTex - center) * zoomFactor;

	gl_FragColor = texture2D(samplerBack, mix(destStart, destEnd, p));
}
