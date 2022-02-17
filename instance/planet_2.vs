#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 instancingMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float det;

// 获得小行星绕中心旋转之后的坐标
vec4 revolution(vec4 pos)
{
	// 所公转的圆的半径
	float r = sqrt(pos.x * pos.x + pos.z * pos.z);
	// 当前位置的 cos sin 值
	float posCos = pos.x / r;
	float posSin = pos.z / r;
	// 所要改变的角度值
	float detCos = cos(det);
	float detSin = sin(det);
	// 计算改变之后的 cos sin 值
	float newPosCos = posCos * detCos - posSin * detSin;
	float newPosSin = posSin * detCos + posCos * detSin;
	// 计算旋转之后的坐标
	pos.x = r * newPosCos;
	pos.z = r * newPosSin;

	return pos;
}

void main()
{
	vec4 pos = instancingMatrix * vec4(aPos,1.0f);
	pos = revolution(pos);
	TexCoords = aTexCoords;
	gl_Position = projection * view * pos;
}
