#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = texture(diffuseTex, IN.texCoord);

	//Changing Texture colour
	//gl_FragColor *= vec4(1.0f, 0.0f, 0.0f, 1.0f);
}