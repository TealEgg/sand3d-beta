#version 430

//layout(std430, binding = 1) buffer predictionBuffer
//{
//	float depth[];
//};

layout(std430, binding = 3) readonly buffer voxelData
{
	uint data[];
};

uniform float iTime;
uniform vec2 iResolution;
uniform vec3 position;

//uniform vec2 iComputeResolution;
//uniform uint computePixelSize;

uniform vec3 front;
uniform vec3 right;
uniform vec3 up;

uniform uint bounds;
uniform uint chunkExp;
uniform uint chunkBounds;
uniform uint chunkLength;
uniform uint chunkVoxelNum;

//TODO: Add the uniforms that you wrote on the paper. Take out that above voxelBounds var

out vec4 outColor;

const bool BACKGROUND_COLOR_DISPLAY = false;
const vec3 BACKGROUND_COLOR = vec3(0.0,0.585,1.0);

const int MAX_RAY_STEPS = 128;

uint index3dA(uvec3 pos)
{
	uint chunkX = pos.x >> chunkExp;
	uint chunkY = pos.y >> chunkExp;
	uint chunkZ = pos.z >> chunkExp;

	uint dX = pos.x - (chunkX << chunkExp);
	uint dY = pos.y - (chunkY << chunkExp);
	uint dZ = pos.z - (chunkZ << chunkExp);
//
//	uint dX = pos.x ^ (chunkX << chunkExp);
//	uint dY = pos.y ^ (chunkY << chunkExp);
//	uint dZ = pos.z ^ (chunkZ << chunkExp);

	//return chunkVoxelNum * (chunkX + chunkLength * (chunkY + chunkLength * chunkZ))
	//		+ (dX + chunkBounds * (dY + chunkBounds * dZ));

	return chunkVoxelNum * (chunkX + chunkLength * (chunkY + chunkLength * chunkZ))
			+ (dX + ((dY + (dZ << chunkExp)) << chunkExp));

//return chunkVoxelNum * (chunkX + chunkLength * (chunkY + chunkLength * chunkZ))
//			| (dX + ((dY + (dZ << chunkExp)) << chunkExp));
}

void main()
{
	outColor = vec4(0.0, 0.0, 0.0, 1.0);

	// Bounds Intersection check
	vec3 boxMin = vec3(0), boxMax = vec3(bounds);

	// Set ray direction and ray position
	vec2 screenPos = (gl_FragCoord.xy / iResolution.xy) * 2.0 - 1.0;
	vec3 rayPos = position;
	vec3 rayDir = normalize(front + screenPos.x * right + screenPos.y * up * iResolution.y / iResolution.x);

	// Bounding box
	ivec3 mapMax;

	vec3 t1 = (boxMin - rayPos) / rayDir;
	vec3 t2 = (boxMax - rayPos) / rayDir;
	
	vec3 tminV = min(t1,t2);
	float tmin = max(max(tminV.x, tminV.y), tminV.z);
	vec3 tmaxV = max(t1,t2);
	float tmax = min(min(tmaxV.x, tmaxV.y), tmaxV.z);

	mapMax = ivec3(floor(rayPos + rayDir * tmax));
	
    if (tmax < tmin || tmax < 0.0)
    {
		//To change background Color
		if (BACKGROUND_COLOR_DISPLAY) outColor.rgb = BACKGROUND_COLOR;
        return;
    }
	else if (tmin > 0.0)
	{
		rayPos = rayPos + rayDir * (tmin - 0.001);
	}

	// Initialize variables for voxel traversal
	ivec3 mapPos = ivec3(floor(rayPos));
	ivec3 rayStep = ivec3(sign(rayDir));
	vec3 tDelta = 1.0 / rayDir * rayStep;
	vec3 tMax = (rayStep * ((mapPos - rayPos) + 0.5) + 0.5) * tDelta;

	// Color and shading of the voxels
	bvec3 mask = bvec3(false);
	vec3 color;

	if (BACKGROUND_COLOR_DISPLAY) color.rgb = BACKGROUND_COLOR;

	// voxMap needs to be called before the bounding box so the area is a perfect circle and not look weird at corners
	// Though if we do this, then there is an infinite while loop error
	ivec3 voxMap = ivec3(floor(rayPos + rayDir * MAX_RAY_STEPS));

	//bool exit = false;
	//vec3 fog = vec3(1.0);
	// for loop is better then while(true) because of loop expansion
	for (int i = 0; i < MAX_RAY_STEPS * 3; i++)
	{
		// Get Voxel data
        uint dat = 0;
//		if (mapPos.x >= 0 && mapPos.x < bounds && mapPos.y >= 0 && mapPos.y < bounds && mapPos.z >= 0 && mapPos.z < bounds)
//		{
//			dat = data[index3dA(mapPos)];
//		}

		if (uint(mapPos.x) < bounds && uint(mapPos.y) < bounds && uint(mapPos.z) < bounds)
		{
			dat = data[index3dA(mapPos)];
		}

        if (dat > 0)
		{
			color = unpackUnorm4x8(dat).xyz;

			if (mask.x) { color *= 0.5; }
			else if (mask.z) { color *= 0.75; }

			break;
			//exit = true;
		}

		//fog -= vec3(1.0/128.0);

		if (mapPos == mapMax || mapPos == voxMap) { break; }

		//exit = exit || mapPos == mapMax || mapPos == voxMap;
		
		 mask.x = (tMax.x <= tMax.y) && (tMax.x <= tMax.z);
		 mask.y = (tMax.y <= tMax.x) && (tMax.y <= tMax.z);
		 mask.z = (tMax.z <= tMax.x) && (tMax.z <= tMax.y);

		// This might be faster? (it could also not be)
//		mask.x = tMax.x <= min(tMax.y,tMax.z);
//		mask.y = tMax.y <= min(tMax.x,tMax.z);
//		mask.z = tMax.z <= min(tMax.x,tMax.y);

		tMax += ivec3(mask) * tDelta;
		mapPos += ivec3(mask) * rayStep;
	}

	outColor.rgb = color; // * fog;
}