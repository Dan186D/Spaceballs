#version 330 core

struct light_t{
	vec3 pos;
	vec3 colour;
	float constant;
	float linear;
	float quadratic;
	bool on;
};

#define LIGHT_NUM 5

uniform sampler2D texture0;
uniform vec3 u_clear_col;
uniform float u_fog_min;
uniform float u_fog_max;
uniform vec3 u_cam_pos;
uniform float u_cam_near;
uniform float u_cam_far;
uniform float u_time;
uniform light_t u_lights[LIGHT_NUM];
uniform float u_log_depth;
uniform bool u_drawing_far_plane;

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
	vec3 result;
	vec3 cam_to_frag;
	if(u_drawing_far_plane){
		cam_to_frag = fragPosition - u_cam_pos;
		cam_to_frag = normalize(cam_to_frag)*10000;
	}else{
		cam_to_frag = fragPosition - u_cam_pos;
	}
	vec3 view_dir = normalize(cam_to_frag);

	for(int i = 0; i < LIGHT_NUM; i++)
	{
		if(!u_lights[i].on && !u_drawing_far_plane){
			continue;
		}

		// ambient
		float ambientStrength = 1;
		vec3 lightCol = u_lights[i].colour;
		vec3 lightPosition = u_lights[i].pos;
		vec3 ambient = ambientStrength * lightCol * texture(texture0, fragTexCoord).rgb;

		//diffuse
		vec3 norm = normalize(fragNormal);
		vec3 lightDir = normalize(lightPosition - fragPosition);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightCol * texture(texture0, fragTexCoord).rgb;

		//specular
		float specularStrength = 0.5;
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(view_dir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightCol * texture(texture0, fragTexCoord).rgb;

		//attenuation
		float distance = length(lightPosition - fragPosition);
		float attenuation = 1.0 / (u_lights[i].constant + u_lights[i].linear * distance + u_lights[i].quadratic * (distance * distance)); 

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		result += (ambient + diffuse + specular);
	}

	float depth = (length(u_cam_pos - fragPosition) - 0.01)/(u_cam_far - 0.01);
#if 0	
	depth += clamp(0.5f+snoise(fragPosition/50.f + u_time/5000.), 0, 1)/30.;
	depth = clamp(depth, 0, 1);
	vec4 linear = vec4(mix(u_clear_col, result, (1-depth)), 1.0f) + random(gl_FragCoord + u_time)/255.0f;
#endif


//look_line = u_cam_pos + (fragpos - u_cam_pos)*t
// x = u_cam_pos.x + (fragpos.x - u_cam_pos.x)*t
// y_intercept = u_cam_pos.y + (fragpos.y - u_cam_pos.y)*t
// t = (y_intercept - u_cam_pos.y)/(fragpos.y - u_cam_pos.y);

// where 0 =< t =< 1



//how fog is done

//          C
/*           \
-------------- \------------------------- fog max
                 \  ''\   
			       \    \- fog ammount  
				     \  _\   
----------------------\------------------- fog min
                       \     
                        \     
                         F
*/
float max_fog_t = (u_fog_max - u_cam_pos.y)/(fragPosition.y - u_cam_pos.y);
float min_fog_t = (u_fog_min - u_cam_pos.y)/(fragPosition.y - u_cam_pos.y);

bool max_hit = max_fog_t >= 0.0 && max_fog_t <= 1.0;
bool min_hit = min_fog_t >= 0.0 && min_fog_t <= 1.0;
#if 1
//try new 
vec3 p1, p2;
if(max_hit && min_hit) {
// looking through fog volume
	vec3 max_q = u_cam_pos + (fragPosition - u_cam_pos)*max_fog_t;
	vec3 min_q = u_cam_pos + (fragPosition - u_cam_pos)*min_fog_t;
	 p1 = max_q;
	 p2 = min_q;
}else if(max_hit || min_hit) {
	if(u_cam_pos.y > u_fog_min && u_cam_pos.y < u_fog_max){
	//inside fog looking out
		vec3 q = u_cam_pos + (fragPosition - u_cam_pos)*(max(max_fog_t, min_fog_t));
	 p1 = u_cam_pos;
	 p2 = q;
	}else{
	//outside fog looking in
		vec3 q = u_cam_pos + (fragPosition - u_cam_pos)*(min(max_fog_t, min_fog_t));
		 p1 = fragPosition;
		p2 = q;
	}
}
else {
	//inside fog looking in or outside fog looking out
	 p1 = u_cam_pos;
	 p2 = fragPosition;
}
//dist_through_fog *= parabola(map(u_cam_pos.y + (fragPosition.y - u_cam_pos.y)*0.5 , u_fog_min, u_fog_max),1.0);

//fog_dist_norm += clamp(0.5f+snoise(fragPosition/50.f + u_time/10.), 0, 1)/30;
#define FOG_SAMPLE_NUM 20
#define MAX_FOG_PARTICLES 2000.0
#define MAX_FOG_NORM 0.95 //max opacity of fog
float value = 0.0;
float step_size_m = length(p2 - p1)/FOG_SAMPLE_NUM;
vec3 dir = normalize(p2 - p1);
vec3 sample_point = p1;

for(int i = 0; i < FOG_SAMPLE_NUM; i++){
	float p = clamp(parabola(map(sample_point.y, u_fog_min, u_fog_max),1),0,1)*2;// + snoise(sample_point/100 + u_time/10);// + clamp(0.5f + snoise(fragPosition + u_time/10), 0, 1)/10 + 1;
	value += p*step_size_m;
	sample_point += dir*step_size_m;
}
#endif

value += clamp(length(cam_to_frag)*4, 0, 1000);

value = clamp(value/MAX_FOG_PARTICLES, 0, MAX_FOG_NORM);

	vec3 fog_mix = mix(result, u_clear_col, value);
	if(u_drawing_far_plane){
		gl_FragColor = vec4(vec3(fog_mix) + rand(fragPosition + u_time)/255, value);
	}else{
		gl_FragColor = vec4(vec3(fog_mix) + rand(fragPosition + u_time)/255, 1.0);
	}

	//gl_FragColor = vec4(vec3(value), 1.0);
	//gl_FragColor = vec4(vec3(depth),1.0);
	//gl_FragDepth = log2(fragDepth) * u_log_depth * 0.5;
}