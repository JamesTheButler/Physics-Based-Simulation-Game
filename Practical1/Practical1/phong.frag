#version 140 

in vec3 position;
in vec3 normal;

out vec4 fragOut;

uniform vec4 color;

void main() {	
	vec3 n = normal;

	if (!gl_FrontFacing) // is the fragment part of a front face?
		n = -n;

	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);
	vec4 ambient = vec4(0.125, 0.125, 0.125, 1);
  	
	vec4 kd = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 ks = vec4(.650, .650, .650, 1.0);

	vec3 lightPos = vec3(-1.0, 1.0, 0.5);
	vec3 lightDir = normalize(lightPos -position);

	float NdotL = dot(n, lightDir);

	if (NdotL > 0.0)
		diffuse = kd * NdotL;

	vec3 rVector =  normalize (2.0 * n * dot(n, lightDir) - lightDir);
	vec3 viewVector = normalize(-position);
	float RdotV = dot (rVector, viewVector);

	if (RdotV > 0.0)
		specular = ks * pow(RdotV, 2.0f) * 0.25;

	vec4 front_color = color * (diffuse + specular) + ambient;

	if (gl_FrontFacing) { // is the fragment part of a front face?
		fragOut = front_color;
    }
	else { // fragment is part of a back face
		fragOut = vec4(front_color.z,front_color.y,front_color.x,1);
    }
}

