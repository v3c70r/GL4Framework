#version 430
layout (binding = 0, rgba32f) uniform image2D framebuffer;

layout (local_size_x = 8, local_size_y = 8) in;

layout(std140) uniform CameraMats
{
    mat4 projMat;
    mat4 viewMat;
    mat4 invViewMat;
}camMats;


struct box {
  vec3 min;
  vec3 max;
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

#define NUM_BOXES 2
const box boxes[] = {
  /* The ground */
  {vec3(-5.0, -0.6, -5.0), vec3(5.0, -0.5, 5.0)},
  /* Box in the middle */
  {vec3(-0.5, -0.5, -0.5), vec3(0.5, 0.5, 0.5)}
};

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
  if (intersectBoxes(origin, dir, i)) {
    vec4 gray = vec4(i.bi / 10.0 + 0.8);
    return vec4(gray.rgb, 1.0);
  }
  return vec4(0.0, 0.0, 0.0, 1.0);
}

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

    vec4 color = trace(cameraPos.xyz, worldRay.xyz);
    imageStore(framebuffer, pix, color);
}

