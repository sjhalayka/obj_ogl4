#version 400


uniform vec4 LineColor = vec4(0, 0, 0, 1);

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout( location = 0 ) out vec4 FragColor;

void main()
{
    if( GIsEdge == 1 ) 
    {
        FragColor = LineColor;
    }
    else 
   {
       FragColor = vec4(1, 0, 0, 1);
   }

}
