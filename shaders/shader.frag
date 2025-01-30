#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(0.2);
// uniform vec3 LightDirection = normalize(vec3(1, 5, 2));
// uniform vec3 LightColor = vec3(1);

// added for project 2
uniform vec3 lightDirection1 = normalize(vec3(1, 5, 2));
uniform vec3 lightColor1 = vec3(1.0, 0.0, 0.0);  // red light
uniform vec3 lightDirection2 = normalize(vec3(-2, -3, 1));
uniform vec3 lightColor2 = vec3(0.0, 0.0, 1.0);  // blue light

uniform vec3 DiffuseColor;	// passed in from c++ side NOTE: you can also set the value here and then remove 
							// color from the c++ side

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{

	// Compute irradiance (sum of ambient & direct lighting)
	// vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, fragNormal));

	vec3 irradiance1 = lightColor1 * max(0, dot(lightDirection1, fragNormal));
    vec3 irradiance2 = lightColor2 * max(0, dot(lightDirection2, fragNormal));
    // Combined irradiance with ambient
    vec3 totalIrradiance = AmbientColor + irradiance1 + irradiance2;

	// Diffuse reflectance
	// vec3 reflectance = irradiance * DiffuseColor;
	vec3 reflectance = totalIrradiance * DiffuseColor;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}