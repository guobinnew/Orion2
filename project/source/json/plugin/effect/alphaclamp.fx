/////////////////////////////////////////////////////////
// Grayscale effect
varying mediump vec2 vTex;
uniform lowp sampler2D samplerFront;
uniform lowp float alphathreshold;
uniform lowp float lowerclamp;
uniform lowp float upperclamp;

void main(void)
{
	// Retrieve source pixel
	lowp vec4 front = texture2D(samplerFront, vTex);
	
	// Extract alpha and unpremultiply
	lowp float a = front.a;
	lowp vec4 unpremultiplied = front / a;
	
	// Apply threshold
	a = (a < alphathreshold ? lowerclamp : upperclamp);
	
	// Premultiply and apply clamped alpha
	front *= a;
	front.a = a;
	
	gl_FragColor = front;
}
