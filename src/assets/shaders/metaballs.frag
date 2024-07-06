#version 330
layout(origin_upper_left) in vec4 gl_FragCoord;

const int MAX_BALLS=8;

uniform vec2 ballPositions[MAX_BALLS];
uniform float ballRadius[MAX_BALLS];
uniform int n_balls;
uniform vec2 u_resolution;

void main() {
    float total = 0.;
    for(int i = 0; i < n_balls; i++) {
        float dst = distance(ballPositions[i].xy, gl_FragCoord.xy);
        float val = ballRadius[i] / dst;
        val = val * val;
        total += val;
    }
    if(total > 1) {
        gl_FragColor = vec4(0., 1., 0., 1.);
    }
}