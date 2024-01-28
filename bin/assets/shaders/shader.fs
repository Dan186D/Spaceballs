#version 330 core

struct body_t{
	vec2 position;
	float mass;
	float radius;
};

#define BODY_NUM 20 

uniform sampler2D texture0;
uniform vec2 u_screen_dims;
uniform float u_time;
uniform body_t u_bodies[BODY_NUM];

out vec4 FragColor;

in vec3 fragPosition;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 gl_FragCoord;
in vec2 fragTexCoord;
in float fragDepth;

float rand(float x)
{
	return fract(sin(x)*1000000.0);
}

vec3 rand(vec3 x){
	return vec3(rand(x.x), rand(x.y), rand(x.z));
}

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float random (in vec3 _st) {
    return fract(sin(dot(_st,
                         vec3(12.9898,78.233,3.12323)))*
        43758.5453123);
}

float map(float value, float min1, float max1)
{
	return (value - min1)/(max1 - min1);
}

float map(float value, float min1, float max1, float min2, float max2)
{
	float perc = map(value, min1, max1);
	return perc*(max2 - min2) + min2;
}

vec3 random3(vec3 c) {
    float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
    vec3 r;
    r.z = fract(512.0*j);
    j *= .125;
    r.x = fract(512.0*j);
    j *= .125;
    r.y = fract(512.0*j);
    return r-0.5;
}

float parabola( float x, float k ){
    return pow( 4.0*x*(1.0-x), k );
}

const float F3 =  0.3333333;
const float G3 =  0.1666667;
float snoise(vec3 p) {

    vec3 s = floor(p + dot(p, vec3(F3)));
    vec3 x = p - s + dot(s, vec3(G3));

    vec3 e = step(vec3(0.0), x - x.yzx);
    vec3 i1 = e*(1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy*(1.0 - e);

    vec3 x1 = x - i1 + G3;
    vec3 x2 = x - i2 + 2.0*G3;
    vec3 x3 = x - 1.0 + 3.0*G3;

    vec4 w, d;

    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);

    w = max(0.6 - w, 0.0);

    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);

    w *= w;
    w *= w;
    d *= w;

    return dot(d, vec4(52.0));
}

void main() {
	vec2 pos = vec2(gl_FragCoord.x, u_screen_dims.y - gl_FragCoord.y );
	vec2 gravityVec = vec2(0,0);
	for(int i = 0; i < BODY_NUM; i++)
	{
		float bodyDistance = distance(u_bodies[i].position, pos);
		float gConstant = 1.0f;
		float sampleMass = 100.0f;
		float gravityStrength = (gConstant * sampleMass * u_bodies[i].mass) / (bodyDistance * bodyDistance);
		vec2 direction = normalize(u_bodies[i].position - pos);
		gravityVec += (gravityStrength * direction);
	}

	float gravityMag = length(gravityVec);
	int period = 3; // reduce this to increase the frequency of lines
	float tolerance = 0.1; // increase this to increase the thickness of lines
	float minSensitivity = 0;
	float speed = 1.0f;

	vec3 lineColor = vec3(0.0,0.0,0.0);
	if ((floor(mod(gravityMag - (u_time * speed), period)) <= tolerance) && gravityMag >= minSensitivity)
	{
		lineColor = vec3(1.0,1.0,1.0) * gravityMag * gravityMag * 0.01f;
	}

	gl_FragColor = vec4(lineColor,1.0);
}