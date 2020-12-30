#version 410 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoord;
uniform mat4 viewMat;   //uniform은 글로벌로 사용되는 정보를 위한 변수를 지정하는 지정자.
uniform mat4 projMat;
uniform mat4 modelMat = mat4(1);
uniform mat4 shadowBiasMVP;
//attribute = vector shader로 vector information을 넘기는 변수를 지정하는 지정자.
out vec3 normal;    //pixel마다 interpolation 된 것들을 frag로 보내주는 용도
out vec3 worldPos;  //out으로 보내면 intepolation되서 in으로 들어간다.
out vec2 texCoord;
out vec4 shadowCoord;


void main(void)
{
    vec4 worldPos4 = modelMat*vec4( in_Position, 1.);
    worldPos = worldPos4.xyz;
    normal = normalize(modelMat*vec4(in_Normal, 0)).xyz;
    texCoord = vec2 ( in_TexCoord.x, 1 - in_TexCoord.y );
    shadowCoord = shadowBiasMVP * vec4(in_Position, 1.);
    gl_Position= projMat * viewMat * worldPos4;
}
