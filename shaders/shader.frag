#version 460 core

in  vec3  position;    
out vec4 FragColor;

uniform vec3   cameraPos;  
uniform sampler3D volumeTex;
uniform int    maxSteps;    
uniform float  alphaScale;  

uniform vec3   lightDir;    
uniform vec3   lightColor;  
uniform float  shininess;   
uniform float  specStrength;

void main() {
    // Ray setup in object space
    vec3 rayOrigin = cameraPos;
    vec3 rayDir    = normalize(position - rayOrigin);

    // AABB intersection (-1→1 cube)
    vec3 boxMin = vec3(-1.0), boxMax = vec3(1.0);
    vec3 invDir = 1.0 / rayDir;
    vec3 t0     = (boxMin - rayOrigin) * invDir;
    vec3 t1     = (boxMax - rayOrigin) * invDir;
    vec3 tMin   = min(t0, t1), tMax = max(t0, t1);
    float tEnter = max(max(tMin.x, tMin.y), tMin.z);
    float tExit  = min(min(tMax.x, tMax.y), tMax.z);
    if (tEnter > tExit) discard;

    // Compute uniform step size
    float tStart = max(tEnter, 0.0);
    float tRange = tExit - tStart;
    float tStep  = tRange / float(maxSteps);

    // Jitter: a tiny per-pixel shift in [–0.5, +0.5]*tStep
    float hash     = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898,78.233))) * 43758.5453);
    float jitter   = (hash - 0.5) * tStep;
    // Half-step + jitter for even sampling
    float tCurrent = tStart + 0.5*tStep + jitter;

    vec4 accum = vec4(0.0);
    float s = 1.0 / 256.0;  // gradient delta

    for (int i = 0; i < maxSteps; ++i) {
        vec3 posObj = rayOrigin + rayDir * tCurrent;
        if (any(lessThan(posObj, boxMin)) || any(greaterThan(posObj, boxMax)))
            break;

        // sample density
        vec3 tex = posObj * 0.5 + 0.5;
        float d0 = texture(volumeTex, tex).r;

        // central‐difference gradient
        float gx = texture(volumeTex, tex + vec3(s,0,0)).r - texture(volumeTex, tex - vec3(s,0,0)).r;
        float gy = texture(volumeTex, tex + vec3(0,s,0)).r - texture(volumeTex, tex - vec3(0,s,0)).r;
        float gz = texture(volumeTex, tex + vec3(0,0,s)).r - texture(volumeTex, tex - vec3(0,0,s)).r;
        vec3 N = normalize(vec3(gx,gy,gz));

        // Blinn-Phong lighting
        vec3 A = 1.5 * lightColor;
        float diff = max(dot(N, lightDir), 0.0);
        vec3 D = diff * lightColor;
        vec3 V = normalize(cameraPos - posObj);
        vec3 H = normalize(lightDir + V);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        vec3 S = specStrength * spec * lightColor;
        vec3 L = A + D + S;

        // shade & composite
        vec4 col = vec4(L * d0, d0 * alphaScale);
        accum.rgb += (1.0 - accum.a) * col.a * col.rgb;
        accum.a   += (1.0 - accum.a) * col.a;
        if (accum.a >= 1.0) break;

        tCurrent += tStep;
    }

    FragColor = vec4(accum.rgb, 1.0);
}