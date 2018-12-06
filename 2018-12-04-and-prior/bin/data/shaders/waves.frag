#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec3 u_mouse;
uniform float u_time;

// Leap uniforms
uniform vec3 rightHandPos;
uniform vec3 leftHandPos;

const vec2 center = vec2(0.5,0.5);
const float speed = 0.05;
const float minHandValue = 0.5;
const bool invert = false;
const bool symmetricRipples = false;
const bool bColorize = true;

float circleFromCenter(vec2 uv, float radius) {
    return distance(uv, vec2(0.5)) * radius;
}

vec4 computeWaves(vec2 true_uv, vec3 handPosition) {
    // Scale the cofficient here by 10 to get wild jumps in movement
    vec2 handPosModifier = (handPosition.xz * 0.001);
    // flip down y-axis
    handPosModifier.x *= -1.0;
    vec2 uv = true_uv + handPosModifier;

    vec3 texcol;
    float x = (center.x-uv.x);
    float y = (center.y-uv.y);

    float r = -(x*x + y*y);
    if (symmetricRipples) {
        r = -sqrt(x*x + y*y);
    }

    // Smaller == less intense
    float patternIntensity = 0.05;
    /* float patternIntensity = 1.5; */
    float waveDebug = (handPosition.y * patternIntensity) + 5.0;
    /* if (true_uv.x > 0.5) waveDebug *= 0.3; */
    /* float z = uv.x + 0.5*sin((r*waveDebug+u_time*speed)/0.013); */
    float z = 0.1 + 0.5*sin((r*waveDebug+u_time*speed)/0.013);

    texcol.x = z;
    texcol.y = z;
    texcol.z = z;

    /* gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) * texcol * , 0.5); */
    float alpha = 1.0;
    vec3 color = texcol;
    if (invert) {
        color = vec3(1.0) - color;
    }
    if (bColorize) {
        color *= vec4(uv,0.5+0.5*sin(u_time * 0.1),1.0).rgb;
    }

    if (false /* add weird depth thing */) {
        color *= vec3(mod(u_time * 0.001, true_uv.x));
    }
    alpha *= 1.0 - circleFromCenter(true_uv, 2.0);
    /* color *= -1.0 - circleFromCenter(true_uv, 2.0); */
    /* alpha *= true_uv.y * 2.0; */
    /* color *= 1.0 - circleFromCenter(true_uv, 1.0); */
    return vec4(color, alpha);
}

void main(){
    vec2 true_uv = gl_FragCoord.st/u_resolution;
    gl_FragColor = (computeWaves(true_uv, leftHandPos) + computeWaves(true_uv, rightHandPos)) * vec4(vec3(0.6), 1.0);
}
