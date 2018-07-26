
struct VSinput
{
	float3 position : POSITION0;
	float3 startPosition : POSITION1;
	float4 color : COLOR;
	float age : AGE;
	float size : SIZE;
	float4 angleOffset : ANGLE;
};

struct VStoGS
{
	float3 position : POSITION0;
	float4 color : COLOR;
	float size : SIZE;
};

VStoGS main(VSinput input)
{
	VStoGS output;

	output.color = input.color;
	output.position = input.position;
	output.size = input.size;
	


	return output;
}