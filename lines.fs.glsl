#version 400

struct LightInfo {
  vec4 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;

//Light.Position = vec4(1, 1, 1, 1);

//Light.Intensity = vec3(1, 1, 1);

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;
//Material.Ka = vec3(1, 1, 1);
//Material.Kd = vec3(1, 1, 1);
//Material.Ks = vec3(1, 1, 1);
//Material.Shininess = 1;


vec4 LineColor = vec4(1, 0, 0, 1);

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout( location = 0 ) out vec4 FragColor;

const int levels = 3;
const float scaleFactor = 1.0 / levels;


void main() {
    if( GIsEdge == 1 )
        FragColor = LineColor;
    else
        FragColor = vec4(1, 1, 1, 1);

}