$input v_color0, v_position

#include "../common.sh"

uniform vec4 u_time;

void main() {
	float p = ceil(v_position.x / 2.0);
	gl_FragColor = vec4(0.0, 0.0, mod(p, 2.0), 1.0);
}
