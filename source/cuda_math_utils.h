//
// Created by shriram on 11/10/20.
//

#ifndef CUDA_MATH_UTILS_CUH
#define CUDA_MATH_UTILS_CUH

#include "cuda_runtime.h"


typedef unsigned int uint;
typedef unsigned short ushort;

namespace CudaMathUtils {
    #ifndef __CUDACC__
    #include <math.h>

    ////////////////////////////////////////////////////////////////////////////////
    // host implementations of CUDA functions
    ////////////////////////////////////////////////////////////////////////////////

    inline float fminf(float a, float b)
    {
        return a < b ? a : b;
    }

    inline float fmaxf(float a, float b)
    {
        return a > b ? a : b;
    }

    inline int max(int a, int b)
    {
        return a > b ? a : b;
    }

    inline int min(int a, int b)
    {
        return a < b ? a : b;
    }

    inline float rsqrtf(float x)
    {
        return 1.0f / sqrtf(x);
    }
    #endif

    ////////////////////////////////////////////////////////////////////////////////
    // negate
    ////////////////////////////////////////////////////////////////////////////////

    inline __host__ __device__ float2 operator-(float2 &a)
    {
        return make_float2(-a.x, -a.y);
    }
    inline __host__ __device__ int2 operator-(int2 &a)
    {
        return make_int2(-a.x, -a.y);
    }
    inline __host__ __device__ float3 operator-(float3 &a)
    {
        return make_float3(-a.x, -a.y, -a.z);
    }
    inline __host__ __device__ int3 operator-(int3 &a)
    {
        return make_int3(-a.x, -a.y, -a.z);
    }
    inline __host__ __device__ float4 operator-(float4 &a)
    {
        return make_float4(-a.x, -a.y, -a.z, -a.w);
    }
    inline __host__ __device__ int4 operator-(int4 &a)
    {
        return make_int4(-a.x, -a.y, -a.z, -a.w);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // equality
    ////////////////////////////////////////////////////////////////////////////////
    inline __host__ __device__ bool equalTo(float3 a, float3 b)
    {
        return (a.x == b.x && a.y == b.y && a.z == b.z);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // addition
    ////////////////////////////////////////////////////////////////////////////////

    inline __host__ __device__ float2 operator+(float2 a, float2 b)
    {
        return make_float2(a.x + b.x, a.y + b.y);
    }
    inline __host__ __device__ void operator+=(float2 &a, float2 b)
    {
        a.x += b.x; a.y += b.y;
    }
    inline __host__ __device__ float2 operator+(float2 a, float b)
    {
        return make_float2(a.x + b, a.y + b);
    }
    inline __host__ __device__ float2 operator+(float b, float2 a)
    {
        return make_float2(a.x + b, a.y + b);
    }
    inline __host__ __device__ void operator+=(float2 &a, float b)
    {
        a.x += b; a.y += b;
    }

    inline __host__ __device__ int2 operator+(int2 a, int2 b)
    {
        return make_int2(a.x + b.x, a.y + b.y);
    }
    inline __host__ __device__ void operator+=(int2 &a, int2 b)
    {
        a.x += b.x; a.y += b.y;
    }
    inline __host__ __device__ int2 operator+(int2 a, int b)
    {
        return make_int2(a.x + b, a.y + b);
    }
    inline __host__ __device__ int2 operator+(int b, int2 a)
    {
        return make_int2(a.x + b, a.y + b);
    }
    inline __host__ __device__ void operator+=(int2 &a, int b)
    {
        a.x += b; a.y += b;
    }

    inline __host__ __device__ uint2 operator+(uint2 a, uint2 b)
    {
        return make_uint2(a.x + b.x, a.y + b.y);
    }
    inline __host__ __device__ void operator+=(uint2 &a, uint2 b)
    {
        a.x += b.x; a.y += b.y;
    }
    inline __host__ __device__ uint2 operator+(uint2 a, uint b)
    {
        return make_uint2(a.x + b, a.y + b);
    }
    inline __host__ __device__ uint2 operator+(uint b, uint2 a)
    {
        return make_uint2(a.x + b, a.y + b);
    }
    inline __host__ __device__ void operator+=(uint2 &a, uint b)
    {
        a.x += b; a.y += b;
    }


    inline __host__ __device__ float3 operator+(float3 a, float3 b)
    {
        return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    inline __host__ __device__ void operator+=(float3 &a, float3 b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z;
    }
    inline __host__ __device__ float3 operator+(float3 a, float b)
    {
        return make_float3(a.x + b, a.y + b, a.z + b);
    }
    inline __host__ __device__ void operator+=(float3 &a, float b)
    {
        a.x += b; a.y += b; a.z += b;
    }

    inline __host__ __device__ int3 operator+(int3 a, int3 b)
    {
        return make_int3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    inline __host__ __device__ void operator+=(int3 &a, int3 b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z;
    }
    inline __host__ __device__ int3 operator+(int3 a, int b)
    {
        return make_int3(a.x + b, a.y + b, a.z + b);
    }
    inline __host__ __device__ void operator+=(int3 &a, int b)
    {
        a.x += b; a.y += b; a.z += b;
    }

    inline __host__ __device__ uint3 operator+(uint3 a, uint3 b)
    {
        return make_uint3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    inline __host__ __device__ void operator+=(uint3 &a, uint3 b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z;
    }
    inline __host__ __device__ uint3 operator+(uint3 a, uint b)
    {
        return make_uint3(a.x + b, a.y + b, a.z + b);
    }
    inline __host__ __device__ void operator+=(uint3 &a, uint b)
    {
        a.x += b; a.y += b; a.z += b;
    }

    inline __host__ __device__ int3 operator+(int b, int3 a)
    {
        return make_int3(a.x + b, a.y + b, a.z + b);
    }
    inline __host__ __device__ uint3 operator+(uint b, uint3 a)
    {
        return make_uint3(a.x + b, a.y + b, a.z + b);
    }
    inline __host__ __device__ float3 operator+(float b, float3 a)
    {
        return make_float3(a.x + b, a.y + b, a.z + b);
    }

    inline __host__ __device__ float4 operator+(float4 a, float4 b)
    {
        return make_float4(a.x + b.x, a.y + b.y, a.z + b.z,  a.w + b.w);
    }
    inline __host__ __device__ void operator+=(float4 &a, float4 b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
    }
    inline __host__ __device__ float4 operator+(float4 a, float b)
    {
        return make_float4(a.x + b, a.y + b, a.z + b, a.w + b);
    }
    inline __host__ __device__ float4 operator+(float b, float4 a)
    {
        return make_float4(a.x + b, a.y + b, a.z + b, a.w + b);
    }
    inline __host__ __device__ void operator+=(float4 &a, float b)
    {
        a.x += b; a.y += b; a.z += b; a.w += b;
    }

    inline __host__ __device__ int4 operator+(int4 a, int4 b)
    {
        return make_int4(a.x + b.x, a.y + b.y, a.z + b.z,  a.w + b.w);
    }
    inline __host__ __device__ void operator+=(int4 &a, int4 b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
    }
    inline __host__ __device__ int4 operator+(int4 a, int b)
    {
        return make_int4(a.x + b, a.y + b, a.z + b,  a.w + b);
    }
    inline __host__ __device__ int4 operator+(int b, int4 a)
    {
        return make_int4(a.x + b, a.y + b, a.z + b,  a.w + b);
    }
    inline __host__ __device__ void operator+=(int4 &a, int b)
    {
        a.x += b; a.y += b; a.z += b; a.w += b;
    }

    inline __host__ __device__ uint4 operator+(uint4 a, uint4 b)
    {
        return make_uint4(a.x + b.x, a.y + b.y, a.z + b.z,  a.w + b.w);
    }
    inline __host__ __device__ void operator+=(uint4 &a, uint4 b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
    }
    inline __host__ __device__ uint4 operator+(uint4 a, uint b)
    {
        return make_uint4(a.x + b, a.y + b, a.z + b,  a.w + b);
    }
    inline __host__ __device__ uint4 operator+(uint b, uint4 a)
    {
        return make_uint4(a.x + b, a.y + b, a.z + b,  a.w + b);
    }
    inline __host__ __device__ void operator+=(uint4 &a, uint b)
    {
        a.x += b; a.y += b; a.z += b; a.w += b;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // subtract
    ////////////////////////////////////////////////////////////////////////////////

    inline __host__ __device__ float2 operator-(float2 a, float2 b)
    {
        return make_float2(a.x - b.x, a.y - b.y);
    }
    inline __host__ __device__ void operator-=(float2 &a, float2 b)
    {
        a.x -= b.x; a.y -= b.y;
    }
    inline __host__ __device__ float2 operator-(float2 a, float b)
    {
        return make_float2(a.x - b, a.y - b);
    }
    inline __host__ __device__ float2 operator-(float b, float2 a)
    {
        return make_float2(b - a.x, b - a.y);
    }
    inline __host__ __device__ void operator-=(float2 &a, float b)
    {
        a.x -= b; a.y -= b;
    }

    inline __host__ __device__ int2 operator-(int2 a, int2 b)
    {
        return make_int2(a.x - b.x, a.y - b.y);
    }
    inline __host__ __device__ void operator-=(int2 &a, int2 b)
    {
        a.x -= b.x; a.y -= b.y;
    }
    inline __host__ __device__ int2 operator-(int2 a, int b)
    {
        return make_int2(a.x - b, a.y - b);
    }
    inline __host__ __device__ int2 operator-(int b, int2 a)
    {
        return make_int2(b - a.x, b - a.y);
    }
    inline __host__ __device__ void operator-=(int2 &a, int b)
    {
        a.x -= b; a.y -= b;
    }

    inline __host__ __device__ uint2 operator-(uint2 a, uint2 b)
    {
        return make_uint2(a.x - b.x, a.y - b.y);
    }
    inline __host__ __device__ void operator-=(uint2 &a, uint2 b)
    {
        a.x -= b.x; a.y -= b.y;
    }
    inline __host__ __device__ uint2 operator-(uint2 a, uint b)
    {
        return make_uint2(a.x - b, a.y - b);
    }
    inline __host__ __device__ uint2 operator-(uint b, uint2 a)
    {
        return make_uint2(b - a.x, b - a.y);
    }
    inline __host__ __device__ void operator-=(uint2 &a, uint b)
    {
        a.x -= b; a.y -= b;
    }

    inline __host__ __device__ float3 operator-(float3 a, float3 b)
    {
        return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    inline __host__ __device__ void operator-=(float3 &a, float3 b)
    {
        a.x -= b.x; a.y -= b.y; a.z -= b.z;
    }
    inline __host__ __device__ float3 operator-(float3 a, float b)
    {
        return make_float3(a.x - b, a.y - b, a.z - b);
    }
    inline __host__ __device__ float3 operator-(float b, float3 a)
    {
        return make_float3(b - a.x, b - a.y, b - a.z);
    }
    inline __host__ __device__ void operator-=(float3 &a, float b)
    {
        a.x -= b; a.y -= b; a.z -= b;
    }

    inline __host__ __device__ int3 operator-(int3 a, int3 b)
    {
        return make_int3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    inline __host__ __device__ void operator-=(int3 &a, int3 b)
    {
        a.x -= b.x; a.y -= b.y; a.z -= b.z;
    }
    inline __host__ __device__ int3 operator-(int3 a, int b)
    {
        return make_int3(a.x - b, a.y - b, a.z - b);
    }
    inline __host__ __device__ int3 operator-(int b, int3 a)
    {
        return make_int3(b - a.x, b - a.y, b - a.z);
    }
    inline __host__ __device__ void operator-=(int3 &a, int b)
    {
        a.x -= b; a.y -= b; a.z -= b;
    }

    inline __host__ __device__ uint3 operator-(uint3 a, uint3 b)
    {
        return make_uint3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    inline __host__ __device__ void operator-=(uint3 &a, uint3 b)
    {
        a.x -= b.x; a.y -= b.y; a.z -= b.z;
    }
    inline __host__ __device__ uint3 operator-(uint3 a, uint b)
    {
        return make_uint3(a.x - b, a.y - b, a.z - b);
    }
    inline __host__ __device__ uint3 operator-(uint b, uint3 a)
    {
        return make_uint3(b - a.x, b - a.y, b - a.z);
    }
    inline __host__ __device__ void operator-=(uint3 &a, uint b)
    {
        a.x -= b; a.y -= b; a.z -= b;
    }

    inline __host__ __device__ float4 operator-(float4 a, float4 b)
    {
        return make_float4(a.x - b.x, a.y - b.y, a.z - b.z,  a.w - b.w);
    }
    inline __host__ __device__ void operator-=(float4 &a, float4 b)
    {
        a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
    }
    inline __host__ __device__ float4 operator-(float4 a, float b)
    {
        return make_float4(a.x - b, a.y - b, a.z - b,  a.w - b);
    }
    inline __host__ __device__ void operator-=(float4 &a, float b)
    {
        a.x -= b; a.y -= b; a.z -= b; a.w -= b;
    }

    inline __host__ __device__ int4 operator-(int4 a, int4 b)
    {
        return make_int4(a.x - b.x, a.y - b.y, a.z - b.z,  a.w - b.w);
    }
    inline __host__ __device__ void operator-=(int4 &a, int4 b)
    {
        a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
    }
    inline __host__ __device__ int4 operator-(int4 a, int b)
    {
        return make_int4(a.x - b, a.y - b, a.z - b,  a.w - b);
    }
    inline __host__ __device__ int4 operator-(int b, int4 a)
    {
        return make_int4(b - a.x, b - a.y, b - a.z, b - a.w);
    }
    inline __host__ __device__ void operator-=(int4 &a, int b)
    {
        a.x -= b; a.y -= b; a.z -= b; a.w -= b;
    }

    inline __host__ __device__ uint4 operator-(uint4 a, uint4 b)
    {
        return make_uint4(a.x - b.x, a.y - b.y, a.z - b.z,  a.w - b.w);
    }
    inline __host__ __device__ void operator-=(uint4 &a, uint4 b)
    {
        a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
    }
    inline __host__ __device__ uint4 operator-(uint4 a, uint b)
    {
        return make_uint4(a.x - b, a.y - b, a.z - b,  a.w - b);
    }
    inline __host__ __device__ uint4 operator-(uint b, uint4 a)
    {
        return make_uint4(b - a.x, b - a.y, b - a.z, b - a.w);
    }
    inline __host__ __device__ void operator-=(uint4 &a, uint b)
    {
        a.x -= b; a.y -= b; a.z -= b; a.w -= b;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // multiply
    ////////////////////////////////////////////////////////////////////////////////

    inline __host__ __device__ float2 operator*(float2 a, float2 b)
    {
        return make_float2(a.x * b.x, a.y * b.y);
    }
    inline __host__ __device__ void operator*=(float2 &a, float2 b)
    {
        a.x *= b.x; a.y *= b.y;
    }
    inline __host__ __device__ float2 operator*(float2 a, float b)
    {
        return make_float2(a.x * b, a.y * b);
    }
    inline __host__ __device__ float2 operator*(float b, float2 a)
    {
        return make_float2(b * a.x, b * a.y);
    }
    inline __host__ __device__ void operator*=(float2 &a, float b)
    {
        a.x *= b; a.y *= b;
    }

    inline __host__ __device__ int2 operator*(int2 a, int2 b)
    {
        return make_int2(a.x * b.x, a.y * b.y);
    }
    inline __host__ __device__ void operator*=(int2 &a, int2 b)
    {
        a.x *= b.x; a.y *= b.y;
    }
    inline __host__ __device__ int2 operator*(int2 a, int b)
    {
        return make_int2(a.x * b, a.y * b);
    }
    inline __host__ __device__ int2 operator*(int b, int2 a)
    {
        return make_int2(b * a.x, b * a.y);
    }
    inline __host__ __device__ void operator*=(int2 &a, int b)
    {
        a.x *= b; a.y *= b;
    }

    inline __host__ __device__ uint2 operator*(uint2 a, uint2 b)
    {
        return make_uint2(a.x * b.x, a.y * b.y);
    }
    inline __host__ __device__ void operator*=(uint2 &a, uint2 b)
    {
        a.x *= b.x; a.y *= b.y;
    }
    inline __host__ __device__ uint2 operator*(uint2 a, uint b)
    {
        return make_uint2(a.x * b, a.y * b);
    }
    inline __host__ __device__ uint2 operator*(uint b, uint2 a)
    {
        return make_uint2(b * a.x, b * a.y);
    }
    inline __host__ __device__ void operator*=(uint2 &a, uint b)
    {
        a.x *= b; a.y *= b;
    }

    inline __host__ __device__ float3 operator*(float3 a, float3 b)
    {
        return make_float3(a.x * b.x, a.y * b.y, a.z * b.z);
    }
    inline __host__ __device__ void operator*=(float3 &a, float3 b)
    {
        a.x *= b.x; a.y *= b.y; a.z *= b.z;
    }
    inline __host__ __device__ float3 operator*(float3 a, float b)
    {
        return make_float3(a.x * b, a.y * b, a.z * b);
    }
    inline __host__ __device__ float3 operator*(float b, float3 a)
    {
        return make_float3(b * a.x, b * a.y, b * a.z);
    }
    inline __host__ __device__ void operator*=(float3 &a, float b)
    {
        a.x *= b; a.y *= b; a.z *= b;
    }

    inline __host__ __device__ int3 operator*(int3 a, int3 b)
    {
        return make_int3(a.x * b.x, a.y * b.y, a.z * b.z);
    }
    inline __host__ __device__ void operator*=(int3 &a, int3 b)
    {
        a.x *= b.x; a.y *= b.y; a.z *= b.z;
    }
    inline __host__ __device__ int3 operator*(int3 a, int b)
    {
        return make_int3(a.x * b, a.y * b, a.z * b);
    }
    inline __host__ __device__ int3 operator*(int b, int3 a)
    {
        return make_int3(b * a.x, b * a.y, b * a.z);
    }
    inline __host__ __device__ void operator*=(int3 &a, int b)
    {
        a.x *= b; a.y *= b; a.z *= b;
    }

    inline __host__ __device__ uint3 operator*(uint3 a, uint3 b)
    {
        return make_uint3(a.x * b.x, a.y * b.y, a.z * b.z);
    }
    inline __host__ __device__ void operator*=(uint3 &a, uint3 b)
    {
        a.x *= b.x; a.y *= b.y; a.z *= b.z;
    }
    inline __host__ __device__ uint3 operator*(uint3 a, uint b)
    {
        return make_uint3(a.x * b, a.y * b, a.z * b);
    }
    inline __host__ __device__ uint3 operator*(uint b, uint3 a)
    {
        return make_uint3(b * a.x, b * a.y, b * a.z);
    }
    inline __host__ __device__ void operator*=(uint3 &a, uint b)
    {
        a.x *= b; a.y *= b; a.z *= b;
    }

    inline __host__ __device__ float4 operator*(float4 a, float4 b)
    {
        return make_float4(a.x * b.x, a.y * b.y, a.z * b.z,  a.w * b.w);
    }
    inline __host__ __device__ void operator*=(float4 &a, float4 b)
    {
        a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
    }
    inline __host__ __device__ float4 operator*(float4 a, float b)
    {
        return make_float4(a.x * b, a.y * b, a.z * b,  a.w * b);
    }
    inline __host__ __device__ float4 operator*(float b, float4 a)
    {
        return make_float4(b * a.x, b * a.y, b * a.z, b * a.w);
    }
    inline __host__ __device__ void operator*=(float4 &a, float b)
    {
        a.x *= b; a.y *= b; a.z *= b; a.w *= b;
    }

    inline __host__ __device__ int4 operator*(int4 a, int4 b)
    {
        return make_int4(a.x * b.x, a.y * b.y, a.z * b.z,  a.w * b.w);
    }
    inline __host__ __device__ void operator*=(int4 &a, int4 b)
    {
        a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
    }
    inline __host__ __device__ int4 operator*(int4 a, int b)
    {
        return make_int4(a.x * b, a.y * b, a.z * b,  a.w * b);
    }
    inline __host__ __device__ int4 operator*(int b, int4 a)
    {
        return make_int4(b * a.x, b * a.y, b * a.z, b * a.w);
    }
    inline __host__ __device__ void operator*=(int4 &a, int b)
    {
        a.x *= b; a.y *= b; a.z *= b; a.w *= b;
    }

    inline __host__ __device__ uint4 operator*(uint4 a, uint4 b)
    {
        return make_uint4(a.x * b.x, a.y * b.y, a.z * b.z,  a.w * b.w);
    }

    inline __host__ __device__ void operator*=(uint4 &a, uint4 b)
    {
        a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
    }

    inline __host__ __device__ uint4 operator*(uint4 a, uint b)
    {
        return make_uint4(a.x * b, a.y * b, a.z * b,  a.w * b);
    }

    inline __host__ __device__ uint4 operator*(uint b, uint4 a)
    {
        return make_uint4(b * a.x, b * a.y, b * a.z, b * a.w);
    }

    inline __host__ __device__ void operator*=(uint4 &a, uint b)
    {
        a.x *= b; a.y *= b; a.z *= b; a.w *= b;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // divide
    ////////////////////////////////////////////////////////////////////////////////

    inline __host__ __device__ float2 operator/(float2 a, float2 b)
    {
        return make_float2(a.x / b.x, a.y / b.y);
    }
    inline __host__ __device__ void operator/=(float2 &a, float2 b)
    {
        a.x /= b.x; a.y /= b.y;
    }
    inline __host__ __device__ float2 operator/(float2 a, float b)
    {
        return make_float2(a.x / b, a.y / b);
    }
    inline __host__ __device__ void operator/=(float2 &a, float b)
    {
        a.x /= b; a.y /= b;
    }
    inline __host__ __device__ float2 operator/(float b, float2 a)
    {
        return make_float2(b / a.x, b / a.y);
    }

    inline __host__ __device__ float3 operator/(float3 a, float3 b)
    {
        return make_float3(a.x / b.x, a.y / b.y, a.z / b.z);
    }
    inline __host__ __device__ void operator/=(float3 &a, float3 b)
    {
        a.x /= b.x; a.y /= b.y; a.z /= b.z;
    }
    inline __host__ __device__ float3 operator/(float3 a, float b)
    {
        return make_float3(a.x / b, a.y / b, a.z / b);
    }
    inline __host__ __device__ void operator/=(float3 &a, float b)
    {
        a.x /= b; a.y /= b; a.z /= b;
    }
    inline __host__ __device__ float3 operator/(float b, float3 a)
    {
        return make_float3(b / a.x, b / a.y, b / a.z);
    }

    inline __host__ __device__ float4 operator/(float4 a, float4 b)
    {
        return make_float4(a.x / b.x, a.y / b.y, a.z / b.z,  a.w / b.w);
    }
    inline __host__ __device__ void operator/=(float4 &a, float4 b)
    {
        a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w;
    }
    inline __host__ __device__ float4 operator/(float4 a, float b)
    {
        return make_float4(a.x / b, a.y / b, a.z / b,  a.w / b);
    }
    inline __host__ __device__ void operator/=(float4 &a, float b)
    {
        a.x /= b; a.y /= b; a.z /= b; a.w /= b;
    }
    inline __host__ __device__ float4 operator/(float b, float4 a)
    {
        return make_float4(b / a.x, b / a.y, b / a.z, b / a.w);
    }

    //Vector operations
    ////////////////////////////////////////////////////////////////////////////////
    // cross product
    ////////////////////////////////////////////////////////////////////////////////
    inline __host__ __device__ float3 Cross(float3 a, float3 b)
    {
        return make_float3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); ;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // dot product
    ////////////////////////////////////////////////////////////////////////////////
    inline __host__ __device__ float Dot(float3 a, float3 b)
    {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // norm
    ////////////////////////////////////////////////////////////////////////////////
    inline __host__ __device__ float Norm(float3 a)
    {
        return sqrtf((a.x * a.x + a.y * a.y + a.z * a.z));
    }
    ////////////////////////////////////////////////////////////////////////////////
    // normalize
    ////////////////////////////////////////////////////////////////////////////////
    inline __host__ __device__ float3 Normalize(float3 v)
    {
        float invLen = rsqrtf(Dot(v, v));
        return v * invLen;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // reflect
    // - returns reflection of incident ray I around surface normal N
    // - N should be normalized, reflected vector's length is equal to length of I
    ////////////////////////////////////////////////////////////////////////////////
    inline __host__ __device__ float3 Reflect(float3 i, float3 n)
    {
        return i - 2.0f * n * Dot(n,i);
    }

}

#endif //CUDA_MATH_UTILS_CUH
