#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec3 u_mouse;
uniform float u_time;

// Leap uniforms
uniform vec3 rightHandPos;

const vec2 center = vec2(0.5,0.5);
const float speed = 0.05;
const float minHandValue = 0.5;

void main(){
    vec2 uv = gl_FragCoord.st/u_resolution;

    vec3 col = vec3(uv,0.5+0.25*sin(u_time));
    vec3 texcol;
    float x = (center.x-uv.x);
    float y = (center.y-uv.y);
    /* float r = -sqrt(x*x + y*y); //uncoment this line to symmetric ripples */
    float r = -(x*x + y*y);
    float waveDebug = (rightHandPos.z * 0.9) + 20.;
    /* if (uv.x > 0.25) waveDebug = 1.0; */
    /* if (uv.x > 0.5) waveDebug = 1.5; */
    /* float z = uv.x + 0.5*sin((r*waveDebug+u_time*speed)/0.013); */
    float z = 1.0 + 0.5*sin((r*waveDebug+u_time*speed)/0.013);

    texcol.x = z;
    texcol.y = z;
    texcol.z = z;

    /* gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) * texcol * vec3(mod(u_time * 0.001, uv.x)), 0.5); */
    gl_FragColor = vec4(vec3(1.0) - texcol, uv.y);
}
