struct GStoPS
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct VStoGS
{
	float3 position : POSITION0;
	float4 color : COLOR;
	float size : SIZE;
};

cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

[maxvertexcount(4)]
void main(point VStoGS input[1],inout TriangleStream< GStoPS > output)
{
	matrix viewProj = mul(view, projection);

	float2 offsets[4];
	offsets[0] = float2(-0.5f, -1);
	offsets[1] = float2(-0.5f, 1);
	offsets[2] = float2(0.5f, -1);
	offsets[3] = float2(0.5f, 1);

	float2 uvs[4];
	uvs[0] = float2(0, 0);
	uvs[1] = float2(0, 1);
	uvs[2] = float2(1, 0);
	uvs[3] = float2(1, 1);

	GStoPS element[4];

	// Point quad expansion and billboarding
	for (uint i = 0; i < 4; i++)
	{
		element[i].position = mul(float4(input[0].position.x, input[0].position.y, input[0].position.z, 1.0f), viewProj);
		element[i].position.xy += offsets[i] * input[0].size;
		element[i].uv = uvs[i];
		element[i].color = input[0].color;
		output.Append(element[i]);
	}

}