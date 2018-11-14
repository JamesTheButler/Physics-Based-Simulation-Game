#version 140 

in vec4 vertexPos;
in vec3 vertexNormal;

out vec3 position;
out vec3 normal;

uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 normalMatrix;

void main() {
	gl_Position = mvp * vertexPos;

	position = vec3(mv * vertexPos);
	normal = normalize(vec3(normalMatrix * vec4(vertexNormal, 0.0)));  //we could also use mat3(mv) for the normal transformation matrix as long as it does not have any non-uniform scaling (which this current implementation does not have).

	gl_PointSize = 5.0;
}

