#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 texcoord;
layout (location = 0) out vec4 outColor;
layout (binding = 0) uniform sampler2D tex;

layout(std140, set=0, binding=1) uniform ParameterS {vec4 g_param1;};
layout(std430, set=1, binding=1) buffer ParameterS2 { vec4 g_Color1;};

void main() {
   vec4 color1 = color;
   color1 = color1 * g_Color1;   
   outColor = color1 * texture(tex, texcoord);
   //outColor = vec4(0, 1, 0, 1);
}

