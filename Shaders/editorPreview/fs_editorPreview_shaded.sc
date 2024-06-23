$input v_pos, v_view, v_normal, v_color0

#include "../common.sh"

vec2 blinn(vec3 lightDir, vec3 normal, vec3 viewDir) {
	float ndotl = dot(normal, lightDir);
	vec3 reflected = lightDir - 2.0 * ndotl * normal;
	float rdotv = dot(reflected, viewDir);
	return vec2(ndotl, rdotv);
}

float fresnel(float ndotl, float bias, float pow_amt) {
	float facing = (1.0 - ndotl);
	return max(bias + (1.0 - bias) * pow(facing, pow_amt), 0.0);
}

vec4 lit(float ndotl, float rdotv, float m) {
	float diff = max(0.0, ndotl);
	float spec = step(0.0, ndotl) * max(0.0, rdotv * m);
	return vec4(1.0, diff, spec, 1.0);
}

void main() {
	vec3 lightDir = vec3(0.0, 0.0, -1.0);
	vec3 normal = normalize(v_normal);
	vec3 view = normalize(v_view);
	vec2 bln = blinn(lightDir, normal, view);
	vec4 lc = lit(bln.x, bln.y, 1.0);
	float fres = fresnel(bln.x, 0.2, 5.0);

	vec3 color = v_color0.xyz;

	gl_FragColor.xyz = pow(vec3(0.07, 0.06, 0.08) + color * lc.y + fres * pow(lc.z, 128.0), vec3_splat(1.0 / 2.2));
	gl_FragColor.w = 1.0;
}