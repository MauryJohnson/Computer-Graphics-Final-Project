#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 color;
layout (location=2) in vec2 tex_coord;

out vec3 our_color;
out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position=transform*vec4(position,1.0f);
    our_color=color;
    TexCoord=vec2(tex_coord.x,1.0f-tex_coord.y);
}
