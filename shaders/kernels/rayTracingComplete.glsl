/*
 * Copyright LWJGL. All rights reserved.
 * License terms: http://lwjgl.org/license.php
 */
#version 430 core

layout(binding = 0, rgba32f) uniform image2D framebufferImage;

uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;

uniform float blendFactor;
uniform float time;
uniform int bounceCount;

struct box {
    vec3 min;
    vec3 max;
    int mat;
};

#define MAX_SCENE_BOUNDS 900.0
#define NUM_BOXES 2

//===============Random.glsl
/**
 * http://amindforeverprogramming.blogspot.de/2013/07/random-floats-in-glsl-330.html
 */
uint hash(uint x) {
  x += (x << 10u);
  x ^= (x >> 6u);
  x += (x << 3u);
  x ^= (x >> 11u);
  x += (x << 15u);
  return x;
}

/**
 * http://amindforeverprogramming.blogspot.de/2013/07/random-floats-in-glsl-330.html
 */
uint hash(uvec3 v) {
  return hash(v.x ^ hash(v.y) ^ hash(v.z));
}

/**
 * Generate a random value in [-1..+1).
 * 
 * The distribution MUST be really uniform and exhibit NO pattern at all,
 * because it is heavily used to generate random sample directions for various
 * things, and if the random function contains the slightest pattern, it will
 * be visible in the final image.
 * 
 * In the GLSL world, the function presented in the first answer to:
 * 
 *   http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
 * 
 * is often used, but that is not a good function, as it has problems with
 * floating point precision and is very sensitive to the seed value, resulting
 * in visible patterns for small and large seeds.
 * 
 * The best implementation (requiring GLSL 330, though) that I found over
 * time is actually this:
 * 
 *   http://amindforeverprogramming.blogspot.de/2013/07/random-floats-in-glsl-330.html
 */
float random(vec2 f, float time) {
  const uint mantissaMask = 0x007FFFFFu;
  const uint one = 0x3F800000u;
  uint h = hash(floatBitsToUint(vec3(f, time)));
  h &= mantissaMask;
  h |= one;
  float r2 = uintBitsToFloat(h);
  return (r2 - 1.0) * 2.0 - 1.0;
}
//====================================
//=========Random Common===========
#define PI 3.14159265359

/**
 * Generate a uniformly distributed random point on the unit disk.
 * 
 * After:
 * http://mathworld.wolfram.com/DiskPointPicking.html
 */
vec3 randomDiskPoint(vec3 rand, vec3 n, vec3 up) {
  float r = rand.x * 0.5 + 0.5; // [-1..1) -> [0..1)
  float angle = (rand.y + 1.0) * PI; // [-1..1] -> [0..2*PI)
  float sr = sqrt(r);
  vec2 p = vec2(sr * cos(angle), sr * sin(angle));
  /*
   * Compute some arbitrary tangent space for orienting
   * our disk towards the normal. We use the camera's up vector
   * to have some fix reference vector over the whole screen.
   */
  vec3 tangent = up;
  vec3 bitangent = cross(tangent, n);
  tangent = cross(bitangent, n);
  
  /* Make our disk orient towards the normal. */
  return tangent * p.x + bitangent * p.y;
}

/**
 * Generate a uniformly distributed random point on the unit-sphere.
 * 
 * After:
 * http://mathworld.wolfram.com/SpherePointPicking.html
 */
vec3 randomSpherePoint(vec3 rand) {
  float ang1 = (rand.x + 1.0) * PI; // [-1..1) -> [0..2*PI)
  float u = rand.y; // [-1..1), cos and acos(2v-1) cancel each other out, so we arrive at [-1..1)
  float u2 = u * u;
  float sqrt1MinusU2 = sqrt(1.0 - u2);
  float x = sqrt1MinusU2 * cos(ang1);
  float y = sqrt1MinusU2 * sin(ang1);
  float z = u;
  return vec3(x, y, z);
}

/**
 * Generate a uniformly distributed random point on the unit-hemisphere
 * around the given normal vector.
 * 
 * This function can be used to generate reflected rays for diffuse surfaces.
 * Actually, this function can be used to sample reflected rays for ANY surface
 * with an arbitrary BRDF correctly.
 * This is because we always need to solve the integral over the hemisphere of
 * a surface point by using numerical approximation using a sum of many
 * sample directions.
 * It is only with non-lambertian BRDF's that, in theory, we could sample them more
 * efficiently, if we knew in which direction the BRDF reflects the most energy.
 * This would be importance sampling, but care must be taken as to not over-estimate
 * those surfaces, because then our sum for the integral would be greater than the
 * integral itself. This is the inherent problem with importance sampling.
 * 
 * The points are uniform over the sphere and NOT over the projected disk
 * of the sphere, so this function cannot be used when sampling a spherical
 * light, where we need to sample the projected surface of the light (i.e. disk)!
 */
vec3 randomHemispherePoint(vec3 rand, vec3 n) {
  /**
   * Generate random sphere point and swap vector along the normal, if it
   * points to the wrong of the two hemispheres.
   * This method provides a uniform distribution over the hemisphere, 
   * provided that the sphere distribution is also uniform.
   */
  vec3 v = randomSpherePoint(rand);
  return v * sign(dot(v, n));
}

vec3 ortho(vec3 v) {
    //  See : http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
    return abs(v.x) > abs(v.z) ? vec3(-v.y, v.x, 0.0)  : vec3(0.0, -v.z, v.y);
}

vec3 getSampleBiased(vec3 r, vec3 dir, float power) {
	vec3 o1 = ortho(dir);
	vec3 o2 = cross(dir, o1);
	r.x = (r.x + 1.0) * PI;
	r.y = pow(r.y * 0.5 + 0.5, 1.0 / (power + 1.0));
	float oneminus = sqrt(1.0 - r.y * r.y);
	return cos(r.x) * oneminus * o1 + sin(r.x) * oneminus * o2 + r.y * dir;
}

vec3 randomHemisphereCosineWeightedPoint(vec3 rand, vec3 dir) {
	return getSampleBiased(rand, dir, 1.0);
}
//=============================================

const box boxes[] = {
    /* The ground */
    {vec3(-5.0, -0.1, -5.0), vec3(5.0, 0.0, 5.0), 0},
    /* Box in the middle */
    {vec3(-0.5, 0.0, -0.5), vec3(0.5, 1.0, 0.5), 1},
    ///* Left wall */
    //{vec3(-5.1, 0.0, -5.0), vec3(-5.0, 5.0, 5.0), 2},
    ///* Right wall */
    //{vec3(5.0, 0.0, -5.0), vec3(5.1, 5.0, 5.0), 3},
    ///* Back wall */
    //{vec3(-5.0, 0.0, -5.1), vec3(5.0, 5.0, -5.0), 0},
    ///* Front wall */
    //{vec3(-5.0, 0.0, 5.0), vec3(5.0, 5.0, 5.1), 0},
    ///* Top wall */
    //{vec3(-5.0, 5.0, -5.0), vec3(5.0, 5.1, 5.0), 4}
};

#define EPSILON 0.01001
#define LIGHT_RADIUS 0.4

#define LIGHT_BASE_INTENSITY 20.0
const vec3 lightCenterPosition = vec3(1.5, 2.9, 3);
const vec4 lightColor = vec4(1);

float random(vec2 f, float time);
vec3 randomDiskPoint(vec3 rand, vec3 n, vec3 up);
vec3 randomHemispherePoint(vec3 rand, vec3 n);

struct hitinfo {
    float near;
    float far;
    int bi;
};

/*
 * We need random values every now and then.
 * So, they will be precomputed for each ray we trace and
 * can be used by any function.
 */
vec3 rand;
vec3 cameraUp;

vec2 intersectBox(vec3 origin, vec3 dir, const box b) {
    vec3 tMin = (b.min - origin) / dir;
    vec3 tMax = (b.max - origin) / dir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}

vec4 colorOfBox(const box b) {
    vec4 col;
    if (b.mat == 0) {
        col = vec4(1.0, 1.0, 1.0, 1.0);
    } else if (b.mat == 1) {
        col = vec4(1.0, 0.2, 0.2, 1.0);
    } else if (b.mat == 2) {
        col = vec4(0.2, 0.2, 1.0, 1.0);
    } else if (b.mat == 3) {
        col = vec4(0.2, 1.0, 0.2, 1.0);
    } else {
        col = vec4(0.5, 0.5, 0.5, 1.0);
    }
    return col;
}

bool intersectBoxes(vec3 origin, vec3 dir, out hitinfo info) {
    float smallest = MAX_SCENE_BOUNDS;
    bool found = false;
    for (int i = 0; i < NUM_BOXES; i++) {
        vec2 lambda = intersectBox(origin, dir, boxes[i]);
        if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest) {
            info.near = lambda.x;
            info.far = lambda.y;
            info.bi = i;
            smallest = lambda.x;
            found = true;
        }
    }
    return found;
}

vec3 normalForBox(vec3 hit, const box b) {
    if (hit.x < b.min.x + EPSILON)
        return vec3(-1.0, 0.0, 0.0);
    else if (hit.x > b.max.x - EPSILON)
        return vec3(1.0, 0.0, 0.0);
    else if (hit.y < b.min.y + EPSILON)
        return vec3(0.0, -1.0, 0.0);
    else if (hit.y > b.max.y - EPSILON)
        return vec3(0.0, 1.0, 0.0);
    else if (hit.z < b.min.z + EPSILON)
        return vec3(0.0, 0.0, -1.0);
    else
        return vec3(0.0, 0.0, 1.0);
}

vec4 trace(vec3 origin, vec3 dir) {
    hitinfo i;
    vec4 accumulated = vec4(0.0);
    vec4 attenuation = vec4(1.0);
    for (int bounce = 0; bounce < bounceCount; bounce++) {
        if (intersectBoxes(origin, dir, i)) {
            box b = boxes[i.bi];
            vec3 hitPoint = origin + i.near * dir;
            vec3 normal = normalForBox(hitPoint, b);
            vec3 lightNormal = normalize(hitPoint - lightCenterPosition);
            vec3 lightPosition = lightCenterPosition + randomDiskPoint(rand, lightNormal, cameraUp) * LIGHT_RADIUS;
            vec3 shadowRayDir = lightPosition - hitPoint;
            vec3 shadowRayStart = hitPoint + normal * EPSILON;
            hitinfo shadowRayInfo;
            bool lightObstructed = intersectBoxes(shadowRayStart, shadowRayDir, shadowRayInfo);
            attenuation *= colorOfBox(b);
            if (shadowRayInfo.near >= 1.0) 
            {
                float cosineFallOff = max(0.0, dot(normal, normalize(shadowRayDir)));
                float oneOverR2 = 1.0 / dot(shadowRayDir, shadowRayDir);
                accumulated += attenuation * vec4(lightColor * LIGHT_BASE_INTENSITY * cosineFallOff * oneOverR2);
            }
            origin = shadowRayStart;
            dir = randomHemispherePoint(rand, normal);
            attenuation *= dot(normal, dir);
        } else {
                //accumulated += vec4(0.351, 0.01, 0.01, 0.0);
            break;
        }
    }
    return accumulated;
}
vec4 traceSimple(vec3 origin, vec3 dir) {
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
    ivec2 size = imageSize(framebufferImage);
    if (pix.x >= size.x || pix.y >= size.y) {
        return;
    }
    vec2 pos = (vec2(pix) + vec2(0.5, 0.5)) / vec2(size.x, size.y);
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    cameraUp = normalize(ray01 - ray00);
    for (int s = 0; s < 1; s++) {
        float rand1 = random(pix, time + float(s));
        float rand2 = random(pix + vec2(641.51224, 423.178), time + float(s));
        float rand3 = random(pix - vec2(147.16414, 363.941), time - float(s));
        rand = vec3(rand1, rand2, rand3);
        vec2 jitter = vec2(rand1, rand2) / vec2(size);
        vec2 p = pos + jitter;
        vec3 dir = mix(mix(ray00, ray01, p.y), mix(ray10, ray11, p.y), p.x);
        color += trace(eye, dir)/1;
    }
    color /= 1;
    vec4 oldColor = vec4(0.0);
    //if (blendFactor > 0.0) {
    //    /* Without explicitly disabling imageLoad for
    //     * the first frame, we get VERY STRANGE corrupted image!
    //     * 'mix' SHOULD mix oldColor out, but strangely it does not!
    //     */
    //    oldColor = imageLoad(framebufferImage, pix);
    //}
    vec4 finalColor = mix(color, oldColor, blendFactor);
    //vec4 finalColor = color;
    imageStore(framebufferImage, pix, finalColor);
}
