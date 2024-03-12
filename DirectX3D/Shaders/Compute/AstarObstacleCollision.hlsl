//AstarObstacleCollision.hlsl
cbuffer RayBuffer : register(b0)
{
	float3 pos;	
	float3 dir;
	
    float distance;
    uint obstacleSize;
}

struct InputDesc
{
    float3 min;
    float3 max;
	
    float3 boxPos;
	
    float3 axisX;
    float3 axisY;
    float3 axisZ;
};
StructuredBuffer<InputDesc> input : register(t0);

struct OutputDesc
{
	float distance;
    float3 hitPoint;
    bool flag;
};
RWStructuredBuffer<OutputDesc> output : register(u0);

void Intersection(uint index)
{
    
    float3 delta = input[index].boxPos - pos;
    
    float3 x = input[index].axisX;
    float3 y = input[index].axisY;
    float3 z = input[index].axisZ;
	
    float tMin = 0.0f;
    float tMax = 99999999.f;
    
    output[index].flag = false;
    
    {
        float e = dot(x, delta);
        float f = dot(dir, x);
        
        if (f <= 0.0001f && f >= -0.0001f)
        {
            if (input[index].min.x > e || input[index].max.x < e)
            {
                output[index].distance = tMin;
                output[index].hitPoint = pos + dir * tMin;
                output[index].flag = true;
            }
                
        }
        else
        {
            float t1 = (e + input[index].min.x) / f;
            float t2 = (e + input[index].max.x) / f;

            if (t1 > t2)
            {
                float temp = t2;
                t2 = t1;
                t1 = temp;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
            {
                output[index].distance = tMin;
                output[index].hitPoint = pos + dir * tMin;
                output[index].flag = true;
            }
                
        }
    }
    
    {
        float e = dot(y, delta);
        float f = dot(dir, y);
        
        if (f <= 0.0001f && f >= -0.0001f)
        {
            if (input[index].min.y > e || input[index].max.y < e)
            {
                output[index].distance = tMin;
                output[index].hitPoint = pos + dir * tMin;
                output[index].flag = true;
            }
                
        }
        else
        {
            float t1 = (e + input[index].min.y) / f;
            float t2 = (e + input[index].max.y) / f;

            if (t1 > t2)
            {
                float temp = t2;
                t2 = t1;
                t1 = temp;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
            {
                output[index].distance = tMin;
                output[index].hitPoint = pos + dir * tMin;
                output[index].flag = true;
            }
                
        }
    }
    
    {
        float e = dot(z, delta);
        float f = dot(dir, z);
        
        if (f <= 0.0001f && f >= -0.0001f)
        {
            if (input[index].min.z > e || input[index].max.z < e)
            {
                output[index].distance = tMin;
                output[index].hitPoint = pos + dir * tMin;
                output[index].flag = true;
            }
                
        }
        else
        {
            float t1 = (e + input[index].min.z) / f;
            float t2 = (e + input[index].max.z) / f;

            if (t1 > t2)
            {
                float temp = t2;
                t2 = t1;
                t1 = temp;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
            {
                output[index].distance = tMin;
                output[index].hitPoint = pos + dir * tMin;
                output[index].flag = true;
            }
                
        }
    }
    
}

[numthreads(1, 1, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x;
	
    if (obstacleSize > index)
		Intersection(index);
}