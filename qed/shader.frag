#version 130

uniform sampler2D tex;
uniform sampler2D texNorms;

//uniform int frameNum;

in vec2 tCoord;
out vec4 outColor;

void main ()
{
  // hard code light dir for now
  //float p = 1.0/(float)mod(frameNum, 600);
  //  float p = mod(frameNum,600)/600.0;
  //vec3 lightDir = vec3(sin(p), cos(2.0*p), sin(5.0*p));
  vec3 lightDir = vec3(0.0, 10.0, 3.0);
  vec2 coord = vec2(tCoord.s, 1.0-tCoord.t);
  vec3 norm = texture2D (texNorms, coord).xyz;
  
  // this is stupid and nothing
  //outColor = dot(norm, normalize(lightDir)) * texture2D (tex, coord);
  
  // just the texture
  outColor = texture2D (tex, coord);
  
  // I want to do bump mapping. TODO
  //outColor = dot(norm, normalize(lightDir)) * vec4(1.0, 0.0, 0.0, 1.0);
}
