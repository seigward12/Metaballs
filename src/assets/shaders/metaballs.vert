const int MAX_BALLS=50;
const float threshold=3.4;

uniform vec2 ballPositions[MAX_BALLS];
uniform float ballRadius[MAX_BALLS];
uniform int n_balls;
uniform vec2 u_resolution;

void main() {
    vec2 vertexPos = (gl_ModelViewProjectionMatrix * gl_Vertex).xy;
    float influence = 0.0;
    
    for(int i = 0; i < n_balls; i++) {
        float dist = distance(vertexPos, ballPositions[i]);
        
        if (dist < ballRadius[i]) {
            influence += ballRadius[i] / dist;
        }
    }
    
    if (influence > 1.0) {
        // Calculer la nouvelle position sur l'isocontour
        // Simplification : déplacement proportionnel à l'influence
        vertexPos = mix(vertexPos, vec2(0.0), 0.5);
    }
    
    gl_Position = vec4(vertexPos, 0.0, 1.0);
}