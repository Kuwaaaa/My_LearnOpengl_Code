#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 instancingMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float det;

// ���С������������ת֮�������
vec4 revolution(vec4 pos)
{
	// ����ת��Բ�İ뾶
	float r = sqrt(pos.x * pos.x + pos.z * pos.z);
	// ��ǰλ�õ� cos sin ֵ
	float posCos = pos.x / r;
	float posSin = pos.z / r;
	// ��Ҫ�ı�ĽǶ�ֵ
	float detCos = cos(det);
	float detSin = sin(det);
	// ����ı�֮��� cos sin ֵ
	float newPosCos = posCos * detCos - posSin * detSin;
	float newPosSin = posSin * detCos + posCos * detSin;
	// ������ת֮�������
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
