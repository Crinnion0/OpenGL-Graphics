#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;


uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;


in Vertex{
vec3 colour;
vec2 texCoord;
vec3 normal;
vec3 worldPos;
} IN;

out vec4 gl_FragColor;

void main(void) {

	vec3 normal = new vec3(0, 0, 0);
	
	//vec4 diffuse = texture (diffuseTex, IN.texCoord);


	//TEXTURE BLENDING
	vec4 diffuse;
	if (IN.worldPos.y > 700) {
		diffuse = texture(diffuseTex, IN.texCoord);
	}
	else if (IN.worldPos.y > 500) {
		float n = 3.1416 * (IN.worldPos.y - 500.0) / 400.0;
		diffuse = (cos(n)*cos(n) * texture(diffuseTex2, IN.texCoord)) + (sin(n)*sin(n) * texture(diffuseTex, IN.texCoord));
	}
	else {
		diffuse = texture(diffuseTex2, IN.texCoord);
	}
	

	vec3 incident = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(incident, IN.normal));

	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 50.0);
	colour += (lightColour.rgb * sFactor) * 0.33;
	gl_FragColor = vec4(colour * atten * lambert, diffuse.a);
	gl_FragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;

}