
// Scalable Ambient Obscurance (SAO) screen-space ambient obscurance algorithm
// by Morgan McGuire and Michael Mara, NVIDIA Research. See license_sao.txt
// Urho3D adaption by reattiva.

//#extension GL_EXT_gpu_shader4: require

#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

#line 13

varying vec2 vScreenPos;
varying vec3 vFarRay;

#ifdef COMPILEVS

// Gets a vector in view space pointing to 'clipPos' of the far plane.
// We can use this to calculate the view space position of the fragment
// at vScreenPos only.
vec3 GetViewFarRay(vec4 clipPos)
{
    return vec3(cFrustumSize.x * clipPos.x,
                cFrustumSize.y * clipPos.y,
                cFrustumSize.z);
}

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
    vFarRay = GetViewFarRay(gl_Position);
}

#endif


#ifdef COMPILEPS

// Total number of direct samples to take at each pixel
#define NUM_SAMPLES 9.0

// This is the number of turns around the circle that the spiral pattern makes.  
// This should be prime to prevent taps from lining up.  This particular choice 
// was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS 7.0

// Far plane informations
uniform vec3 cFrustumSize;
// Viewport inverse sizes
uniform vec4 cGBufferOffsets;

// To transform screen pos to view space 
uniform vec4 cProjInfo;
// View matrix
uniform mat3 cView;

// Multiplier of the noise textures, often you see = sizeof(viewport) / sizeof(noise)
uniform float cNoiseMult;

// The height in pixels of a 1m object if viewed from 1m away.
// You can compute it from your projection matrix.  The actual value is just
// a scale factor on radius; you can simply hardcode this to a constant (~500)
// and make your radius value unitless (...but resolution dependent.)
uniform float cProjScale;
uniform float cProjScale2;

// World-space AO radius in scene units (r).  e.g., 1.0m
uniform float cRadius;

// Bias to avoid AO in smooth corners, e.g., 0.01m
uniform float cBias;

// Intensity / radius^6
uniform float cIntensityDivR6;

vec3 GetViewPosition(vec2 ssPos, float depth)
{
    // eye_z = depth(0=camera .. 1=far) * far
    float eye_z = depth * cFrustumSize.z;

    return vec3(ssPos * cProjInfo.xy + cProjInfo.zw, 1.0) * eye_z;
}

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

void PS()
{
    // View space position
    vec3 edC = texture2D(sDepthBuffer, vScreenPos).rgb;
    float depthC = DecodeDepth(edC);
    vec3 vsC = GetViewPosition(vScreenPos, depthC);

#if 0
    // For testing position reconstruction
    float originDepth = DecodeDepth(texture2D(sDepthBuffer, vScreenPos).rgb);
    vec3 viewPos = vFarRay * originDepth;
    float lf = length(viewPos);
    if (vScreenPos.x > 0.3 && vScreenPos.x < 0.7)
        lf = length(vsC);
    gl_FragColor = vec4(vec3(lf/100.0), 1.0);
    return;
#endif

#if 1
    // Decrease intensity in the distance (TODO: the blur makes occlusion too big)
    float fadeStart = 20.0;
    float fadeEnd = 60.0;    
    if (vsC.z > fadeEnd)
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    float intensity = clamp((fadeEnd - vsC.z) / (fadeEnd - fadeStart), 0.0, 1.0) * cIntensityDivR6;
#else
    float intensity = cIntensityDivR6;
#endif

    float randomAngle = fract( pow(vScreenPos.x, vScreenPos.y) * 43758.5453 ) / cGBufferInvSize.x;
    //vec2 ss = gl_FragCoord.xy;
    //vec2 ss = vScreenPos / cGBufferInvSize;
    //float randomAngle = float((rand(ss) * ss.x * ss.y) * 10);
    // Hash function used in the HPG12 AlchemyAO paper
    //float randomAngle = float((3 * ss.x ^ ss.y + ss.x * ss.y) * 10);    

#if 1
    vec3 normal = DecodeNormal(texture2D(sNormalMap, vScreenPos));
    vec3 vsN = cView * normal;
#else
    // Reconstruct normals from positions. These will lead to 1-pixel black lines
	// at depth discontinuities, however the blur will wipe those out so they are 
    // not visible in the final image.
	vec3 vsN = normalize( cross(dFdy(vsC), dFdx(vsC)) );
#endif
    
    // Choose the screen-space sample radius
	// proportional to the projected area of the sphere
	float ssDiskRadius = cProjScale2 * cProjScale * cRadius / vsC.z; 
        
    float sum = 0.0;
    float radius2 = cRadius * cRadius;
    const float epsilon = 0.01;
    
	for (float i = 0.0; i < NUM_SAMPLES; ++i)
	{
        // Screen-space radius for the tap on a unit disk
        float alpha = (i + 0.5) * (1.0 / NUM_SAMPLES);
        float angle = randomAngle + alpha * (NUM_SPIRAL_TURNS * 6.28);
        
        vec2 ssOffset = floor( alpha * ssDiskRadius * vec2(cos(angle), sin(angle)) );

        // Sample screen space position in pixels
        vec2 ssP = vScreenPos + ssOffset * cGBufferInvSize;

        // Sample view space position
        float depthP = DecodeDepth(texture2D(sDepthBuffer, ssP).rgb);
        vec3 vsP = GetViewPosition(ssP, depthP);
            
        // Dal sample all'origin
        vec3 v = vsP - vsC;
        
        float vv = dot(v, v);
        float vn = dot(v, vsN);

        // A: From the HPG12 paper
        // Note large epsilon to avoid overdarkening within cracks
        // sum += float(vv < radius2) * max((vn - cBias) / (epsilon + vv), 0.0) * radius2 * 0.6;

        // B: Smoother transition to zero (lowers contrast, smoothing out corners). [Recommended]
        float f = max(radius2 - vv, 0.0);
        sum += f * f * f * max((vn - cBias) / (epsilon + vv), 0.0);

        // C: Medium contrast (which looks better at high radii), no division.  Note that the
        // contribution still falls off with radius^2, but we've adjusted the rate in a way that is
        // more computationally efficient and happens to be aesthetically pleasing.
        // sum += 4.0 * max(1.0 - vv * invRadius2, 0.0) * max(vn - cBias, 0.0);

        // D: Low contrast, no division operation
        // sum += 2.0 * float(vv < radius2) * max(vn - cBias, 0.0);
	}

	float occlusion = max(0.0, 1.0 - sum * intensity * (5.0 / NUM_SAMPLES));

#if 0
	// Bilateral box-filter over a quad for free, respecting depth edges
	// (the difference that this makes is subtle)
	if (abs(dFdx(vsC.z)) < 0.02)
	{
		occlusion -= dFdx(occlusion) * ((ssC.x & 1) - 0.5);
	}
	if (abs(dFdy(vsC.z)) < 0.02)
	{
		occlusion -= dFdy(occlusion) * ((ssC.y & 1) - 0.5);
	}
#endif
    
    gl_FragColor = vec4(occlusion, edC.rg, 1.0);
    //gl_FragColor = vec4(vec3(occlusion), 1.0);
}
#endif
