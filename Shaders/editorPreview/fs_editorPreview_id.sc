$input v_pos, v_view, v_normal, v_color0

#include "../common.sh"

uniform vec4 u_id;

void main() {
	gl_FragColor.xyz = u_id.xyz; // This is dumb, should use u8 texture
	gl_FragColor.w = 1.0;
}