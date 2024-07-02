struct Ball {
    vec2 pos;
    vec3 rgb;
    float r;
};

const int MAX_BALLS=50;
const float threshold=3.4;

uniform Ball balls[MAX_BALLS];
uniform int n_balls;
uniform vec2 u_resolution;

varying vec4 vertColor;

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution;
    gl_FragColor = vec4(st.x,st.y,0.0,1.0);
    // gl_FragColor = vec4(1.0,0.0,1.0,1.0);
    // float total=0. ;
    // int idx=0;
    // float highest=0. ;
    // for(int i=0;i<n_balls;i++) {
    //     Ball b=balls[i];
    //     float dst=distance(b.pos.xy,gl_FragCoord.xy);
    //     float val=b.r/dst;
    //     total+=val;
    //     if(val>highest) {
    //         idx=i;
    //         highest=val;
    //     }
    // }
    // if(total<threshold) {
    //     gl_FragColor=vec4(vec3(1.),1.);
    // }else if(total<threshold+.05) {
    //     gl_FragColor=vec4(vec3(0.),1.);
    // }else {
    //     gl_FragColor=vec4(balls[idx].rgb,1.);
    // }
}