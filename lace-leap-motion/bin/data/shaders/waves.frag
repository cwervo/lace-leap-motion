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
const bool invert = true;
const bool symmetricRipples = false;
const bool bColorize = true;

float circleFromCenter(vec2 uv, float radius) {
    return distance(uv, vec2(0.5)) * radius;
}

void main(){
    vec2 true_uv = gl_FragCoord.st/u_resolution;

    // TODO:
    // TODO:
    // TODO:
    // TODO:
    // create a function that takes in true_uv & a hand position & returns the color (vec3? 4?) for that one wave stack
    // then add them together, average the color, apply to circular alpha fade at the bottom, and actually set gl_FragColor

    // Scale the cofficient here by 10 to get wild jumps in movement
    vec2 handPosModifier = (rightHandPos.xz * 0.001);
    // flip down y-axis
    handPosModifier.x *= -1.0;
    vec2 uv = true_uv + handPosModifier;

    vec3 col = vec3(uv,0.5+0.25*sin(u_time));
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
    float waveDebug = (rightHandPos.y * patternIntensity) + 5.0;
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
    alpha *= true_uv.y * 1.5;
    /* color *= 1.0 - circleFromCenter(true_uv, 1.0); */
    gl_FragColor = vec4(color, alpha);
}
