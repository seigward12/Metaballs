const int MAX_BALLS=50;
const float threshold=3.4;

uniform vec2 ballPositions[MAX_BALLS];
uniform float ballRadius[MAX_BALLS];
uniform int n_balls;
uniform vec2 u_resolution;

void main() {
    // float total = 0.;
    // for(int i = 0; i < n_balls; i++) {
    //     float dst = distance(ballPositions[i].xy, gl_FragCoord.xy);
    //     float val=ballRadius[i] / dst;
    //     total += val;
    // }
    // if(total > threshold) {
    vec2 st = gl_FragCoord.xy/u_resolution;
    gl_FragColor = vec4(st.x,st.y,0.0,1.0);
    // gl_FragColor = vec4(0., 1., 0., 1.);
    // }
}