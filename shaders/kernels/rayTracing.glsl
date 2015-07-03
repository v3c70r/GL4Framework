#version 430
layout (binding = 0, rgba32f) uniform image2D framebuffer;

uniform vec3 eye;

//these rays are in world coord
uniform vec3 ray00;
uniform vec3 ray10;
uniform vec3 ray01;
uniform vec3 ray11;

struct box{
    vec3 min;
    vec3 max;
};
#define NUM_BOXES 3

const box boxes[] = {
    /* The ground */
    {vec3(-5.0, -0.1, -5.0), vec3(5.0, 0.0, 5.0)},
    /* Box in the middle */
    {vec3(-0.5, 0.0, -0.5), vec3(0.5, 1.0, 0.5)},
    /*Room*/
    {vec3(-6.0, -0.2, -5.0), vec3(6.0, 1.1, 5.0)}
};

#define MAX_SCENE_BOUNDS 1000.0

struct hitinfo {
    vec2 lambda;
    int bi;
};

vec3 normalBox(vec3 hit, float epsilon ,const box b)
{
    if (hit.x < b.min.x + epsilon) return vec3 (-1.0, 0.0, 0.0);
    else if (hit.x < b.max.x - epsilon) return vec3 (+1.0, 0.0, 0.0);
    else if (hit.x < b.min.y + epsilon) return vec3 (0.0, -1.0, 0.0);
    else if (hit.x < b.max.y - epsilon) return vec3 (0.0, +1.0, 0.0);
    else if (hit.x < b.min.z + epsilon) return vec3 (0.0, -1.0, 0.0);
    else if (hit.x < b.max.z - epsilon) return vec3 (0.0, +1.0, 0.0);
}
vec2 intersectBox(vec3 origin, vec3 dir, const box b) {
  vec3 tMin = (b.min - origin) / dir;
  vec3 tMax = (b.max - origin) / dir;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar = min(min(t2.x, t2.y), t2.z);
  return vec2(tNear, tFar);
}


bool intersectBoxes(vec3 origin, vec3 dir, out hitinfo info) {
    float smallest = MAX_SCENE_BOUNDS;
    bool found = false;
    for (int i = 0; i < NUM_BOXES; i++) {
        vec2 lambda = intersectBox(origin, dir, boxes[i]);
        if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest) {
            info.lambda = lambda;
            info.bi = i;
            smallest = lambda.x;
            found = true;
        }
    }
    return found;
}
float random(vec2 scale, float seed) {
  return fract(sin(dot(vec2(gl_GlobalInvocationID.xy)/vec2(imageSize(framebuffer)) + seed, scale)) * 43758.5453 + seed);
}

vec3 cosineWeightedDirection(float seed, vec3 normal)
{
    float u = random(vec2(12.9898, 78.233), seed);
    float v = random(vec2(63.7264, 10.873), seed);
    float r = sqrt(u);
    float angle = 6.283185307179586 * v;
    // compute basis from normal
    vec3 sdir, tdir;
    if (abs(normal.x)<.5) {
        sdir = cross(normal, vec3(1,0,0));
    } else {
        sdir = cross(normal, vec3(0,1,0));
    }
    tdir = cross(normal, sdir);
    return r*cos(angle)*sdir + r*sin(angle)*tdir + sqrt(1.-u)*normal;
    
}

vec3 calculateColor(vec3 origin, vec3 ray, vec3 light)
{
    vec3 colorMask = vec3(1.0);
    vec3 accumulateColor = vec3(0.0);
    for (int bounce = 0; bounce < 14; bounce++)
    {
        vec2 tRoom = intersectBox(origin, ray, boxes[2]);
        //TODO: Add other objects here

        float t = 999.0;
        vec3 normal;
        if (tRoom.x < tRoom.y) t = tRoom.y;
        //TODO: Add other objects here

        vec3 hit = origin + ray * t;
        vec3 surfaceColor = vec3(0.75);
        float specularHeighlight = 0.0;
        if ( abs(t -tRoom.y) < 0.0000001)
        {
            normal = normalBox(hit, 0.67 ,boxes[2]);
            //TODO: Set room colors
            if(hit.x < boxes[2].min.x) surfaceColor = vec3(0.1, 0.5, 1.0);
            else if(hit.x > boxes[2].max.x) surfaceColor = vec3(1.0, 0.9, 0.1);
            ray = cosineWeightedDirection(110 + float(bounce), normal);
        }
        else if (abs(t - 999.0) < 0.0001)
            break;
        else{
            if (false);
            //TODO: Add other stuffs
        }

        //compute diffuse lighting contribution
        vec3 toLight = light  - hit;
        float diffuse = max(0.0, dot(normalize(toLight), normal));
        float shadowIntensity = 1.0;    //TODO: Add shadow functions
        colorMask *= surfaceColor;
        accumulateColor += colorMask * (0.1 * diffuse * shadowIntensity);
        accumulateColor += colorMask * specularHeighlight * shadowIntensity;
        origin = hit;
    }
    return accumulateColor;
}
//vec3 uniformlyRandomVector(float seed) {
//    return uniformlyRandomDirection(seed) * sqrt(random(vec3(36.7539, 50.3658, 306.2759), seed));' +
//}


vec4 trace(vec3 origin, vec3 dir) {
    hitinfo i;
    if (intersectBoxes(origin, dir, i)) {
        vec4 gray = vec4(i.bi / 10.0 + 0.8);
        return vec4(gray.rgb, 1.0);
    }
    return vec4(0.0, 0.0, 0.0, 1.0);
}

layout (local_size_x = 8, local_size_y = 8) in;

void main(void) {
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(framebuffer);
    if (pix.x >= size.x || pix.y >= size.y) {
        return;
    }
    vec2 pos = vec2(pix) / vec2(size.x, size.y);
    vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
    //vec3 dir = vec3(mix(ray00.x, ray10.x, pos.x), mix(ray01.y, ray00.y, pos.y), ray00.z);
    //vec4 color = trace(eye, dir);
    vec3 lightPos = vec3(1.0, 0.0, 0.0);
    vec3 color = calculateColor(eye, dir, lightPos);
    imageStore(framebuffer, pix, vec4(color,1.0));
}
