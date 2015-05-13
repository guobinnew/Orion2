/////////////////////////////////////////////////////////
// Black & white effect
varying mediump vec2 vTex;
uniform lowp sampler2D samplerFront;
uniform lowp float threshold;

void main(void)
{
	// Retrieve source pixel
	lowp vec4 front = texture2D(samplerFront, vTex);
	
	// Calculate grayscale amount
	lowp float gray = front.r * 0.299 + front.g * 0.587 + front.b * 0.114;
	
	// Output either black or white
	if (gray < threshold)
		gl_FragColor = vec4(0.0, 0.0, 0.0, front.a);
	else
		gl_FragColor = vec4(front.a, front.a, front.a, front.a);
}
