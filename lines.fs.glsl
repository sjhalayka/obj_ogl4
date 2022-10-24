#version 430

vec4 u_color = vec4(1, 0, 1, 1);

layout(location = 0) out vec4 frag_colour;

in vec2 ftexcoord;

uniform int img_width;
uniform int img_height;
vec2 iResolution = vec2(img_width, img_height);





struct Vertex {
    vec2 p;
    vec4 c;
};





vec4 drawLineB(Vertex v1, Vertex v2, vec2 pos) {
    vec2 a = v1.p;
    vec2 b = v2.p;
    
    vec2 l = b - a;
    vec2 r = pos - a;
    float h = dot(l,r) / dot (l,l);
    
    vec2 eC = a + h * l;
    
    if (floor(pos) == floor(eC) && 0.0 <= h && h <= 1.0 ) {
       return vec4((1.0 - h) * v1.c + h * v2.c); 
    }
    
    return vec4(0,0,0,0);
}

vec4 overlay(vec4 c1, vec4 c2) {
    return vec4((1.0 - c2.w) * c1.xyz + c2.w * c2.xyz, 1.0);
}


void main()
{
    frag_colour = vec4(1.0, 0.0, 0.0, 1.0);
    return;
      


    float t = 1;
    float r = 300.0;
    Vertex v1 = Vertex(vec2(400,225), vec4(1,0,0,1));
    Vertex v2 = Vertex(vec2(410,225), vec4(1,0,0,1));
    
    vec4 col = vec4(0,0,0,1);
 
    col = overlay(col, drawLineB(v1, v2, ftexcoord));
    // Output to screen
    frag_colour = col;


/*
vec2 uv = VertexIn.mTexCoord.xy;// / iResolution.xy;
    float t = 1.0; //Pro Tip™: multipluy time to go faster!
    float pi = 4.0*atan(1.0);
    //in uv space
    vec2 p1 = vec2(0, 0);//vec2(0.5 + sin(t)*.2, 0.25 + cos(t+pi)*.1);

    vec2 p2 = vec2(1, 1);//vec2(0.5 + sin(t)*.2, 0.75 + cos(t+pi)*.1);

    float lines = drawLine(p1, p2, uv, 1.);
*/


//    frag_colour = vec4(lines, lines, lines, 1.0);

/*
     vec2 uv = VertexIn.mTexCoord;// (VertexIn.mTexCoord * 2.0 - iResolution.xy) / iResolution.y;
    vec2 mo = vec2(1, 1);//(0.5 * 2.0 - iResolution.xy) / iResolution.y;
    float thickness = 1.0;
    vec3 color = backColor;

    // draw endless line
    float intensity = line(uv, vec2(0.0), mo, thickness*0.5);
    color = mix(color, lineColor1, intensity);

    // draw line segment
    intensity = segment(uv, vec2(0.0), mo, thickness);
    color = mix(color, lineColor2, intensity);
    
    frag_colour = vec4(color, 1.0);
    */

}

