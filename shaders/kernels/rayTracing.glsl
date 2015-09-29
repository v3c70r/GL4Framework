#version 430


layout (binding = 0, rgba32f) uniform image2D framebuffer;

layout (local_size_x = 8, local_size_y = 8) in;

layout(std140) uniform CameraMats
{
    mat4 projMat;
    mat4 viewMat;
    mat4 invViewMat;
}camMats;

layout(std140) uniform Time
{
    float time;     //For random function
}time;


#define EPSILON 0.00001
#define LIGHT_RADIUS 0.4

#define LIGHT_BASE_INTENSITY 20.0
const vec3 lightCenterPosition = vec3(1.5, 2.9, 3);
const vec4 lightColor = vec4(1);
vec3 rand;
vec3 cameraUp = (camMats.viewMat * vec4(0, 1, 0, 0)).xyz;

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

struct box {
  vec3 min;
  vec3 max;
  int mat;
};

vec4 uvToEye(vec2 texCoord, float z)    //screen to camera coordinate
{
    const vec4 viewport = vec4(0, 0, 1, 1);
    vec3 ndcPos;
    //ndcPos.xy = ((2.0 * texCoord.xy)-(2.0*viewport.xy))/(viewport.zw)-1;
    ndcPos.xy = 2 *  (texCoord.xy) - vec2(1.0, 1.0);
    ndcPos.z = (2.0 * z - 1.0);
    vec4 clipPos;
    //clipPos.w = camMats.projMat[3][2]/ (ndcPos.z - (camMats.projMat[2][2]/camMats.projMat[2][3]));
    clipPos.w =  camMats.projMat[3][2] / ( ndcPos.z + camMats.projMat[2][2]);
    clipPos.xyz = ndcPos*clipPos.w;
    
    return inverse(camMats.projMat) * clipPos;
}

#define NUM_BOXES 7
const box boxes[] = {
  /* The ground */
  {vec3(-5.0, -0.1, -5.0), vec3(5.0, 0.0, 5.0), 0},
  /* Box in the middle */
  {vec3(-0.5, 0.0, -0.5), vec3(0.5, 1.0, 0.5), 1},
  /* Left wall */
  {vec3(-5.1, 0.0, -5.0), vec3(-5.0, 5.0, 5.0), 2},
  /* Right wall */
  {vec3(5.0, 0.0, -5.0), vec3(5.1, 5.0, 5.0), 3},
  /* Back wall */
  {vec3(-5.0, 0.0, -5.1), vec3(5.0, 5.0, -5.0), 0},
  /* Front wall */
  {vec3(-5.0, 0.0, 5.0), vec3(5.0, 5.0, 5.1), 0},
  /* Top wall */
  {vec3(-5.0, 5.0, -5.0), vec3(5.0, 5.1, 5.0), 4}
};

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


vec2 intersectBox(vec3 origin, vec3 dir, const box b) {
  vec3 tMin = (b.min - origin) / dir;
  vec3 tMax = (b.max - origin) / dir;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar = min(min(t2.x, t2.y), t2.z);
  return vec2(tNear, tFar);
}
#define MAX_SCENE_BOUNDS 1000.0

struct hitinfo {
  vec2 lambda;
  int bi;
};
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

vec4 trace(vec3 origin, vec3 dir) {
  hitinfo i;
  vec4 accumulated = vec4(0.0);
  vec4 attenuation = vec4(1.0);
  int bounceCount = 4;
  for (int bounce = 0; bounce < bounceCount; bounce++) {
    if (intersectBoxes(origin, dir, i)) {
      box b = boxes[i.bi];
      vec3 hitPoint = origin + i.lambda.x* dir;
      vec3 normal = normalForBox(hitPoint, b);
      vec3 lightNormal = normalize(hitPoint - lightCenterPosition);
      vec3 lightPosition = lightCenterPosition + randomDiskPoint(rand, lightNormal, cameraUp) * LIGHT_RADIUS;
      vec3 shadowRayDir = lightPosition - hitPoint;
      vec3 shadowRayStart = hitPoint + normal * EPSILON;
      hitinfo shadowRayInfo;
      bool lightObstructed = intersectBoxes(shadowRayStart, shadowRayDir, shadowRayInfo);
      attenuation *= colorOfBox(b);
      if (shadowRayInfo.lambda.x >= 1.0) {
        float cosineFallOff = max(0.0, dot(normal, normalize(shadowRayDir)));
        float oneOverR2 = 1.0 / dot(shadowRayDir, shadowRayDir);
        accumulated += attenuation * vec4(lightColor * LIGHT_BASE_INTENSITY * cosineFallOff * oneOverR2);
      }
      origin = shadowRayStart;
      dir = randomHemispherePoint(rand, normal);
      attenuation *= dot(normal, dir);
    } else {
      break;
    }
  }
  return accumulated;
}

/*
vec4 trace(vec3 origin, vec3 dir) {
  hitinfo i;
  if (intersectBoxes(origin, dir, i)) {
    vec4 gray = vec4(i.bi / 10.0 + 0.8);
    return vec4(gray.rgb, 1.0);
  }
  return vec4(0.0, 0.0, 0.0, 1.0);
}
*/

void main(void) {
    //Rays 

    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(framebuffer);
    if (pix.x >= size.x || pix.y >= size.y) {
        return;
    }

    vec4 cameraPos = (camMats.invViewMat * vec4(0.0, 0.0, 0.0, 1.0));
    cameraPos /= cameraPos.w;

    vec2 screenRay = vec2( vec2(pix)/vec2(size));
    vec4 pCam = uvToEye(screenRay, 0.0);
    vec4 worldRay = camMats.invViewMat * pCam;
    worldRay /= worldRay.w;
    worldRay = (worldRay - cameraPos);

    //vec4 color = trace(cameraPos.xyz, worldRay.xyz);
    vec4 color = vec4(0.0,0.0, 0.0,1.0);
    for (int s = 0; s < 1; s++) {
        float rand1 = random(pix, time.time + float(s));
        float rand2 = random(pix + vec2(641.51224, 423.178), time.time + float(s));
        float rand3 = random(pix - vec2(147.16414, 363.941), time.time - float(s));
        rand = vec3(rand1, rand2, rand3);
        vec2 jitter = vec2(rand1, rand2) / vec2(size);

        //vec2 p = pos + jitter;
        //vec3 dir = mix(mix(ray00, ray01, p.y), mix(ray10, ray11, p.y), p.x);

        cameraPos.y += jitter.y;
        cameraPos.x += jitter.y;

        color += trace(cameraPos.xyz, worldRay.xyz);
    }

    imageStore(framebuffer, pix, color);
}

