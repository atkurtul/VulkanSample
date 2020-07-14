#pragma once
#include "immintrin.h"
#include "cmath"
typedef unsigned uint;
typedef unsigned long long uint64;
typedef long long int64;

struct vec2 {
	float x, y;

	vec2 operator * (vec2 b) { return { x * b.x, y * b.y }; }
	vec2 operator - (vec2 b) { return { x - b.x, y - b.y }; }
	vec2 operator + (vec2 b) { return { x + b.x, y + b.y }; }
	vec2 operator / (vec2 b) { return { x / b.x, y / b.y }; }
	vec2 operator * (float b) { return { x * b, y * b }; }
	vec2 operator - (float b) { return { x - b, y - b }; }
	vec2 operator + (float b) { return { x + b, y + b }; }
	vec2 operator / (float b) { return { x / b, y / b }; }
};

struct vec3 {
	float x, y, z;

	vec3 operator * (vec3 b) const { return { x * b.x, y * b.y, z * b.z }; }
	vec3 operator - (vec3 b) const { return { x - b.x, y - b.y, z - b.z }; }
	vec3 operator + (vec3 b) const { return { x + b.x, y + b.y, z + b.z }; }
	vec3 operator / (vec3 b) const { return { x / b.x, y / b.y, z / b.z }; }
	vec3 operator * (float b) const { return { x * b, y * b, z * b }; }
	vec3 operator - (float b) const { return { x - b, y - b, z - b }; }
	vec3 operator + (float b) const { return { x + b, y + b, z + b }; }
	vec3 operator / (float b) const { return { x / b, y / b, z / b }; }

	vec3& operator += (vec3 b) { return *this = *this + b; }
	vec3 normalize() const
	{
		return operator*(1.f / len());
	}

	float angle(vec3 b) const
	{
		return acosf(dot(b) / (len() * b.len()));
	}

	float dot(vec3 b) const
	{
		return x * b.x + y * b.y + z * b.z;
	}

	float len() const
	{
		return sqrtf(dot(*this));
	}
};

inline vec3 cross(vec3 a, vec3 b)
{
	return
	{	
		a.y * b.z - b.y * a.z,
		a.z * b.x - b.z * a.x,
		a.x * b.y - b.x * a.y
	};
}



struct vec3u {uint x, y, z;};
struct vec4u {uint x, y, z, w;};


static constexpr float rad = 0.01745329251f;
static constexpr float pi  = 3.14159265359f;
union vec4;
union vec8;
template<int mask>
struct proxy128
{
	__m128 xmm;
	operator vec4() const;
};
template<int mask>
struct proxy256
{
	__m256 ymm;
	operator vec8() const;
};

union vec4
{
	__m128 xmm;
	struct { float x, y, z, w; };

	vec4() = default;
	vec4(__m128 xm) { xmm = xm; }
	vec4(float x) { xmm = _mm_set_ps1(x); }
	vec4(float x, float y, float z = 0, float w = 0) { xmm = _mm_setr_ps(x, y, z, w); }

	static vec4 load(const void* ptr)
	{
		return _mm_loadu_ps((float*)ptr);
	}

	template<int x, int y, int z, int w>
	using proxy = proxy128<x | (y << 2) | (z << 4) | (w << 6)>;
	proxy<0, 0, 0, 0> xxxx;
	proxy<0, 0, 0, 1> xxxy;
	proxy<0, 0, 0, 2> xxxz;
	proxy<0, 0, 0, 3> xxxw;
	proxy<0, 0, 1, 0> xxyx;
	proxy<0, 0, 1, 1> xxyy;
	proxy<0, 0, 1, 2> xxyz;
	proxy<0, 0, 1, 3> xxyw;
	proxy<0, 0, 2, 0> xxzx;
	proxy<0, 0, 2, 1> xxzy;
	proxy<0, 0, 2, 2> xxzz;
	proxy<0, 0, 2, 3> xxzw;
	proxy<0, 0, 3, 0> xxwx;
	proxy<0, 0, 3, 1> xxwy;
	proxy<0, 0, 3, 2> xxwz;
	proxy<0, 0, 3, 3> xxww;
	proxy<0, 1, 0, 0> xyxx;
	proxy<0, 1, 0, 1> xyxy;
	proxy<0, 1, 0, 2> xyxz;
	proxy<0, 1, 0, 3> xyxw;
	proxy<0, 1, 1, 0> xyyx;
	proxy<0, 1, 1, 1> xyyy;
	proxy<0, 1, 1, 2> xyyz;
	proxy<0, 1, 1, 3> xyyw;
	proxy<0, 1, 2, 0> xyzx;
	proxy<0, 1, 2, 1> xyzy;
	proxy<0, 1, 2, 2> xyzz;
	proxy<0, 1, 2, 3> xyzw;
	proxy<0, 1, 3, 0> xywx;
	proxy<0, 1, 3, 1> xywy;
	proxy<0, 1, 3, 2> xywz;
	proxy<0, 1, 3, 3> xyww;
	proxy<0, 2, 0, 0> xzxx;
	proxy<0, 2, 0, 1> xzxy;
	proxy<0, 2, 0, 2> xzxz;
	proxy<0, 2, 0, 3> xzxw;
	proxy<0, 2, 1, 0> xzyx;
	proxy<0, 2, 1, 1> xzyy;
	proxy<0, 2, 1, 2> xzyz;
	proxy<0, 2, 1, 3> xzyw;
	proxy<0, 2, 2, 0> xzzx;
	proxy<0, 2, 2, 1> xzzy;
	proxy<0, 2, 2, 2> xzzz;
	proxy<0, 2, 2, 3> xzzw;
	proxy<0, 2, 3, 0> xzwx;
	proxy<0, 2, 3, 1> xzwy;
	proxy<0, 2, 3, 2> xzwz;
	proxy<0, 2, 3, 3> xzww;
	proxy<0, 3, 0, 0> xwxx;
	proxy<0, 3, 0, 1> xwxy;
	proxy<0, 3, 0, 2> xwxz;
	proxy<0, 3, 0, 3> xwxw;
	proxy<0, 3, 1, 0> xwyx;
	proxy<0, 3, 1, 1> xwyy;
	proxy<0, 3, 1, 2> xwyz;
	proxy<0, 3, 1, 3> xwyw;
	proxy<0, 3, 2, 0> xwzx;
	proxy<0, 3, 2, 1> xwzy;
	proxy<0, 3, 2, 2> xwzz;
	proxy<0, 3, 2, 3> xwzw;
	proxy<0, 3, 3, 0> xwwx;
	proxy<0, 3, 3, 1> xwwy;
	proxy<0, 3, 3, 2> xwwz;
	proxy<0, 3, 3, 3> xwww;
	proxy<1, 0, 0, 0> yxxx;
	proxy<1, 0, 0, 1> yxxy;
	proxy<1, 0, 0, 2> yxxz;
	proxy<1, 0, 0, 3> yxxw;
	proxy<1, 0, 1, 0> yxyx;
	proxy<1, 0, 1, 1> yxyy;
	proxy<1, 0, 1, 2> yxyz;
	proxy<1, 0, 1, 3> yxyw;
	proxy<1, 0, 2, 0> yxzx;
	proxy<1, 0, 2, 1> yxzy;
	proxy<1, 0, 2, 2> yxzz;
	proxy<1, 0, 2, 3> yxzw;
	proxy<1, 0, 3, 0> yxwx;
	proxy<1, 0, 3, 1> yxwy;
	proxy<1, 0, 3, 2> yxwz;
	proxy<1, 0, 3, 3> yxww;
	proxy<1, 1, 0, 0> yyxx;
	proxy<1, 1, 0, 1> yyxy;
	proxy<1, 1, 0, 2> yyxz;
	proxy<1, 1, 0, 3> yyxw;
	proxy<1, 1, 1, 0> yyyx;
	proxy<1, 1, 1, 1> yyyy;
	proxy<1, 1, 1, 2> yyyz;
	proxy<1, 1, 1, 3> yyyw;
	proxy<1, 1, 2, 0> yyzx;
	proxy<1, 1, 2, 1> yyzy;
	proxy<1, 1, 2, 2> yyzz;
	proxy<1, 1, 2, 3> yyzw;
	proxy<1, 1, 3, 0> yywx;
	proxy<1, 1, 3, 1> yywy;
	proxy<1, 1, 3, 2> yywz;
	proxy<1, 1, 3, 3> yyww;
	proxy<1, 2, 0, 0> yzxx;
	proxy<1, 2, 0, 1> yzxy;
	proxy<1, 2, 0, 2> yzxz;
	proxy<1, 2, 0, 3> yzxw;
	proxy<1, 2, 1, 0> yzyx;
	proxy<1, 2, 1, 1> yzyy;
	proxy<1, 2, 1, 2> yzyz;
	proxy<1, 2, 1, 3> yzyw;
	proxy<1, 2, 2, 0> yzzx;
	proxy<1, 2, 2, 1> yzzy;
	proxy<1, 2, 2, 2> yzzz;
	proxy<1, 2, 2, 3> yzzw;
	proxy<1, 2, 3, 0> yzwx;
	proxy<1, 2, 3, 1> yzwy;
	proxy<1, 2, 3, 2> yzwz;
	proxy<1, 2, 3, 3> yzww;
	proxy<1, 3, 0, 0> ywxx;
	proxy<1, 3, 0, 1> ywxy;
	proxy<1, 3, 0, 2> ywxz;
	proxy<1, 3, 0, 3> ywxw;
	proxy<1, 3, 1, 0> ywyx;
	proxy<1, 3, 1, 1> ywyy;
	proxy<1, 3, 1, 2> ywyz;
	proxy<1, 3, 1, 3> ywyw;
	proxy<1, 3, 2, 0> ywzx;
	proxy<1, 3, 2, 1> ywzy;
	proxy<1, 3, 2, 2> ywzz;
	proxy<1, 3, 2, 3> ywzw;
	proxy<1, 3, 3, 0> ywwx;
	proxy<1, 3, 3, 1> ywwy;
	proxy<1, 3, 3, 2> ywwz;
	proxy<1, 3, 3, 3> ywww;
	proxy<2, 0, 0, 0> zxxx;
	proxy<2, 0, 0, 1> zxxy;
	proxy<2, 0, 0, 2> zxxz;
	proxy<2, 0, 0, 3> zxxw;
	proxy<2, 0, 1, 0> zxyx;
	proxy<2, 0, 1, 1> zxyy;
	proxy<2, 0, 1, 2> zxyz;
	proxy<2, 0, 1, 3> zxyw;
	proxy<2, 0, 2, 0> zxzx;
	proxy<2, 0, 2, 1> zxzy;
	proxy<2, 0, 2, 2> zxzz;
	proxy<2, 0, 2, 3> zxzw;
	proxy<2, 0, 3, 0> zxwx;
	proxy<2, 0, 3, 1> zxwy;
	proxy<2, 0, 3, 2> zxwz;
	proxy<2, 0, 3, 3> zxww;
	proxy<2, 1, 0, 0> zyxx;
	proxy<2, 1, 0, 1> zyxy;
	proxy<2, 1, 0, 2> zyxz;
	proxy<2, 1, 0, 3> zyxw;
	proxy<2, 1, 1, 0> zyyx;
	proxy<2, 1, 1, 1> zyyy;
	proxy<2, 1, 1, 2> zyyz;
	proxy<2, 1, 1, 3> zyyw;
	proxy<2, 1, 2, 0> zyzx;
	proxy<2, 1, 2, 1> zyzy;
	proxy<2, 1, 2, 2> zyzz;
	proxy<2, 1, 2, 3> zyzw;
	proxy<2, 1, 3, 0> zywx;
	proxy<2, 1, 3, 1> zywy;
	proxy<2, 1, 3, 2> zywz;
	proxy<2, 1, 3, 3> zyww;
	proxy<2, 2, 0, 0> zzxx;
	proxy<2, 2, 0, 1> zzxy;
	proxy<2, 2, 0, 2> zzxz;
	proxy<2, 2, 0, 3> zzxw;
	proxy<2, 2, 1, 0> zzyx;
	proxy<2, 2, 1, 1> zzyy;
	proxy<2, 2, 1, 2> zzyz;
	proxy<2, 2, 1, 3> zzyw;
	proxy<2, 2, 2, 0> zzzx;
	proxy<2, 2, 2, 1> zzzy;
	proxy<2, 2, 2, 2> zzzz;
	proxy<2, 2, 2, 3> zzzw;
	proxy<2, 2, 3, 0> zzwx;
	proxy<2, 2, 3, 1> zzwy;
	proxy<2, 2, 3, 2> zzwz;
	proxy<2, 2, 3, 3> zzww;
	proxy<2, 3, 0, 0> zwxx;
	proxy<2, 3, 0, 1> zwxy;
	proxy<2, 3, 0, 2> zwxz;
	proxy<2, 3, 0, 3> zwxw;
	proxy<2, 3, 1, 0> zwyx;
	proxy<2, 3, 1, 1> zwyy;
	proxy<2, 3, 1, 2> zwyz;
	proxy<2, 3, 1, 3> zwyw;
	proxy<2, 3, 2, 0> zwzx;
	proxy<2, 3, 2, 1> zwzy;
	proxy<2, 3, 2, 2> zwzz;
	proxy<2, 3, 2, 3> zwzw;
	proxy<2, 3, 3, 0> zwwx;
	proxy<2, 3, 3, 1> zwwy;
	proxy<2, 3, 3, 2> zwwz;
	proxy<2, 3, 3, 3> zwww;
	proxy<3, 0, 0, 0> wxxx;
	proxy<3, 0, 0, 1> wxxy;
	proxy<3, 0, 0, 2> wxxz;
	proxy<3, 0, 0, 3> wxxw;
	proxy<3, 0, 1, 0> wxyx;
	proxy<3, 0, 1, 1> wxyy;
	proxy<3, 0, 1, 2> wxyz;
	proxy<3, 0, 1, 3> wxyw;
	proxy<3, 0, 2, 0> wxzx;
	proxy<3, 0, 2, 1> wxzy;
	proxy<3, 0, 2, 2> wxzz;
	proxy<3, 0, 2, 3> wxzw;
	proxy<3, 0, 3, 0> wxwx;
	proxy<3, 0, 3, 1> wxwy;
	proxy<3, 0, 3, 2> wxwz;
	proxy<3, 0, 3, 3> wxww;
	proxy<3, 1, 0, 0> wyxx;
	proxy<3, 1, 0, 1> wyxy;
	proxy<3, 1, 0, 2> wyxz;
	proxy<3, 1, 0, 3> wyxw;
	proxy<3, 1, 1, 0> wyyx;
	proxy<3, 1, 1, 1> wyyy;
	proxy<3, 1, 1, 2> wyyz;
	proxy<3, 1, 1, 3> wyyw;
	proxy<3, 1, 2, 0> wyzx;
	proxy<3, 1, 2, 1> wyzy;
	proxy<3, 1, 2, 2> wyzz;
	proxy<3, 1, 2, 3> wyzw;
	proxy<3, 1, 3, 0> wywx;
	proxy<3, 1, 3, 1> wywy;
	proxy<3, 1, 3, 2> wywz;
	proxy<3, 1, 3, 3> wyww;
	proxy<3, 2, 0, 0> wzxx;
	proxy<3, 2, 0, 1> wzxy;
	proxy<3, 2, 0, 2> wzxz;
	proxy<3, 2, 0, 3> wzxw;
	proxy<3, 2, 1, 0> wzyx;
	proxy<3, 2, 1, 1> wzyy;
	proxy<3, 2, 1, 2> wzyz;
	proxy<3, 2, 1, 3> wzyw;
	proxy<3, 2, 2, 0> wzzx;
	proxy<3, 2, 2, 1> wzzy;
	proxy<3, 2, 2, 2> wzzz;
	proxy<3, 2, 2, 3> wzzw;
	proxy<3, 2, 3, 0> wzwx;
	proxy<3, 2, 3, 1> wzwy;
	proxy<3, 2, 3, 2> wzwz;
	proxy<3, 2, 3, 3> wzww;
	proxy<3, 3, 0, 0> wwxx;
	proxy<3, 3, 0, 1> wwxy;
	proxy<3, 3, 0, 2> wwxz;
	proxy<3, 3, 0, 3> wwxw;
	proxy<3, 3, 1, 0> wwyx;
	proxy<3, 3, 1, 1> wwyy;
	proxy<3, 3, 1, 2> wwyz;
	proxy<3, 3, 1, 3> wwyw;
	proxy<3, 3, 2, 0> wwzx;
	proxy<3, 3, 2, 1> wwzy;
	proxy<3, 3, 2, 2> wwzz;
	proxy<3, 3, 2, 3> wwzw;
	proxy<3, 3, 3, 0> wwwx;
	proxy<3, 3, 3, 1> wwwy;
	proxy<3, 3, 3, 2> wwwz;
	proxy<3, 3, 3, 3> wwww;
};

union vec8
{
	__m256 ymm;
	struct { vec4 lo, hi; };
	vec8() = default;
	vec8(__m256 xm) { ymm = xm; }
	vec8(float x) { ymm = _mm256_set1_ps(x); }
	vec8(float x, float y, float z = 0, float w = 0, float x0 = 0, float y0 = 0, float z0 = 0, float w0 = 0)
	{
		ymm = _mm256_setr_ps(x, y, z, w, x0, y0, z0, w0);
	}

	template<int x, int y, int z, int w>
	using proxy = proxy256<x | (y << 2) | (z << 4) | (w << 6)>;
	proxy<0, 0, 0, 0> xxxx;
	proxy<0, 0, 0, 1> xxxy;
	proxy<0, 0, 0, 2> xxxz;
	proxy<0, 0, 0, 3> xxxw;
	proxy<0, 0, 1, 0> xxyx;
	proxy<0, 0, 1, 1> xxyy;
	proxy<0, 0, 1, 2> xxyz;
	proxy<0, 0, 1, 3> xxyw;
	proxy<0, 0, 2, 0> xxzx;
	proxy<0, 0, 2, 1> xxzy;
	proxy<0, 0, 2, 2> xxzz;
	proxy<0, 0, 2, 3> xxzw;
	proxy<0, 0, 3, 0> xxwx;
	proxy<0, 0, 3, 1> xxwy;
	proxy<0, 0, 3, 2> xxwz;
	proxy<0, 0, 3, 3> xxww;
	proxy<0, 1, 0, 0> xyxx;
	proxy<0, 1, 0, 1> xyxy;
	proxy<0, 1, 0, 2> xyxz;
	proxy<0, 1, 0, 3> xyxw;
	proxy<0, 1, 1, 0> xyyx;
	proxy<0, 1, 1, 1> xyyy;
	proxy<0, 1, 1, 2> xyyz;
	proxy<0, 1, 1, 3> xyyw;
	proxy<0, 1, 2, 0> xyzx;
	proxy<0, 1, 2, 1> xyzy;
	proxy<0, 1, 2, 2> xyzz;
	proxy<0, 1, 2, 3> xyzw;
	proxy<0, 1, 3, 0> xywx;
	proxy<0, 1, 3, 1> xywy;
	proxy<0, 1, 3, 2> xywz;
	proxy<0, 1, 3, 3> xyww;
	proxy<0, 2, 0, 0> xzxx;
	proxy<0, 2, 0, 1> xzxy;
	proxy<0, 2, 0, 2> xzxz;
	proxy<0, 2, 0, 3> xzxw;
	proxy<0, 2, 1, 0> xzyx;
	proxy<0, 2, 1, 1> xzyy;
	proxy<0, 2, 1, 2> xzyz;
	proxy<0, 2, 1, 3> xzyw;
	proxy<0, 2, 2, 0> xzzx;
	proxy<0, 2, 2, 1> xzzy;
	proxy<0, 2, 2, 2> xzzz;
	proxy<0, 2, 2, 3> xzzw;
	proxy<0, 2, 3, 0> xzwx;
	proxy<0, 2, 3, 1> xzwy;
	proxy<0, 2, 3, 2> xzwz;
	proxy<0, 2, 3, 3> xzww;
	proxy<0, 3, 0, 0> xwxx;
	proxy<0, 3, 0, 1> xwxy;
	proxy<0, 3, 0, 2> xwxz;
	proxy<0, 3, 0, 3> xwxw;
	proxy<0, 3, 1, 0> xwyx;
	proxy<0, 3, 1, 1> xwyy;
	proxy<0, 3, 1, 2> xwyz;
	proxy<0, 3, 1, 3> xwyw;
	proxy<0, 3, 2, 0> xwzx;
	proxy<0, 3, 2, 1> xwzy;
	proxy<0, 3, 2, 2> xwzz;
	proxy<0, 3, 2, 3> xwzw;
	proxy<0, 3, 3, 0> xwwx;
	proxy<0, 3, 3, 1> xwwy;
	proxy<0, 3, 3, 2> xwwz;
	proxy<0, 3, 3, 3> xwww;
	proxy<1, 0, 0, 0> yxxx;
	proxy<1, 0, 0, 1> yxxy;
	proxy<1, 0, 0, 2> yxxz;
	proxy<1, 0, 0, 3> yxxw;
	proxy<1, 0, 1, 0> yxyx;
	proxy<1, 0, 1, 1> yxyy;
	proxy<1, 0, 1, 2> yxyz;
	proxy<1, 0, 1, 3> yxyw;
	proxy<1, 0, 2, 0> yxzx;
	proxy<1, 0, 2, 1> yxzy;
	proxy<1, 0, 2, 2> yxzz;
	proxy<1, 0, 2, 3> yxzw;
	proxy<1, 0, 3, 0> yxwx;
	proxy<1, 0, 3, 1> yxwy;
	proxy<1, 0, 3, 2> yxwz;
	proxy<1, 0, 3, 3> yxww;
	proxy<1, 1, 0, 0> yyxx;
	proxy<1, 1, 0, 1> yyxy;
	proxy<1, 1, 0, 2> yyxz;
	proxy<1, 1, 0, 3> yyxw;
	proxy<1, 1, 1, 0> yyyx;
	proxy<1, 1, 1, 1> yyyy;
	proxy<1, 1, 1, 2> yyyz;
	proxy<1, 1, 1, 3> yyyw;
	proxy<1, 1, 2, 0> yyzx;
	proxy<1, 1, 2, 1> yyzy;
	proxy<1, 1, 2, 2> yyzz;
	proxy<1, 1, 2, 3> yyzw;
	proxy<1, 1, 3, 0> yywx;
	proxy<1, 1, 3, 1> yywy;
	proxy<1, 1, 3, 2> yywz;
	proxy<1, 1, 3, 3> yyww;
	proxy<1, 2, 0, 0> yzxx;
	proxy<1, 2, 0, 1> yzxy;
	proxy<1, 2, 0, 2> yzxz;
	proxy<1, 2, 0, 3> yzxw;
	proxy<1, 2, 1, 0> yzyx;
	proxy<1, 2, 1, 1> yzyy;
	proxy<1, 2, 1, 2> yzyz;
	proxy<1, 2, 1, 3> yzyw;
	proxy<1, 2, 2, 0> yzzx;
	proxy<1, 2, 2, 1> yzzy;
	proxy<1, 2, 2, 2> yzzz;
	proxy<1, 2, 2, 3> yzzw;
	proxy<1, 2, 3, 0> yzwx;
	proxy<1, 2, 3, 1> yzwy;
	proxy<1, 2, 3, 2> yzwz;
	proxy<1, 2, 3, 3> yzww;
	proxy<1, 3, 0, 0> ywxx;
	proxy<1, 3, 0, 1> ywxy;
	proxy<1, 3, 0, 2> ywxz;
	proxy<1, 3, 0, 3> ywxw;
	proxy<1, 3, 1, 0> ywyx;
	proxy<1, 3, 1, 1> ywyy;
	proxy<1, 3, 1, 2> ywyz;
	proxy<1, 3, 1, 3> ywyw;
	proxy<1, 3, 2, 0> ywzx;
	proxy<1, 3, 2, 1> ywzy;
	proxy<1, 3, 2, 2> ywzz;
	proxy<1, 3, 2, 3> ywzw;
	proxy<1, 3, 3, 0> ywwx;
	proxy<1, 3, 3, 1> ywwy;
	proxy<1, 3, 3, 2> ywwz;
	proxy<1, 3, 3, 3> ywww;
	proxy<2, 0, 0, 0> zxxx;
	proxy<2, 0, 0, 1> zxxy;
	proxy<2, 0, 0, 2> zxxz;
	proxy<2, 0, 0, 3> zxxw;
	proxy<2, 0, 1, 0> zxyx;
	proxy<2, 0, 1, 1> zxyy;
	proxy<2, 0, 1, 2> zxyz;
	proxy<2, 0, 1, 3> zxyw;
	proxy<2, 0, 2, 0> zxzx;
	proxy<2, 0, 2, 1> zxzy;
	proxy<2, 0, 2, 2> zxzz;
	proxy<2, 0, 2, 3> zxzw;
	proxy<2, 0, 3, 0> zxwx;
	proxy<2, 0, 3, 1> zxwy;
	proxy<2, 0, 3, 2> zxwz;
	proxy<2, 0, 3, 3> zxww;
	proxy<2, 1, 0, 0> zyxx;
	proxy<2, 1, 0, 1> zyxy;
	proxy<2, 1, 0, 2> zyxz;
	proxy<2, 1, 0, 3> zyxw;
	proxy<2, 1, 1, 0> zyyx;
	proxy<2, 1, 1, 1> zyyy;
	proxy<2, 1, 1, 2> zyyz;
	proxy<2, 1, 1, 3> zyyw;
	proxy<2, 1, 2, 0> zyzx;
	proxy<2, 1, 2, 1> zyzy;
	proxy<2, 1, 2, 2> zyzz;
	proxy<2, 1, 2, 3> zyzw;
	proxy<2, 1, 3, 0> zywx;
	proxy<2, 1, 3, 1> zywy;
	proxy<2, 1, 3, 2> zywz;
	proxy<2, 1, 3, 3> zyww;
	proxy<2, 2, 0, 0> zzxx;
	proxy<2, 2, 0, 1> zzxy;
	proxy<2, 2, 0, 2> zzxz;
	proxy<2, 2, 0, 3> zzxw;
	proxy<2, 2, 1, 0> zzyx;
	proxy<2, 2, 1, 1> zzyy;
	proxy<2, 2, 1, 2> zzyz;
	proxy<2, 2, 1, 3> zzyw;
	proxy<2, 2, 2, 0> zzzx;
	proxy<2, 2, 2, 1> zzzy;
	proxy<2, 2, 2, 2> zzzz;
	proxy<2, 2, 2, 3> zzzw;
	proxy<2, 2, 3, 0> zzwx;
	proxy<2, 2, 3, 1> zzwy;
	proxy<2, 2, 3, 2> zzwz;
	proxy<2, 2, 3, 3> zzww;
	proxy<2, 3, 0, 0> zwxx;
	proxy<2, 3, 0, 1> zwxy;
	proxy<2, 3, 0, 2> zwxz;
	proxy<2, 3, 0, 3> zwxw;
	proxy<2, 3, 1, 0> zwyx;
	proxy<2, 3, 1, 1> zwyy;
	proxy<2, 3, 1, 2> zwyz;
	proxy<2, 3, 1, 3> zwyw;
	proxy<2, 3, 2, 0> zwzx;
	proxy<2, 3, 2, 1> zwzy;
	proxy<2, 3, 2, 2> zwzz;
	proxy<2, 3, 2, 3> zwzw;
	proxy<2, 3, 3, 0> zwwx;
	proxy<2, 3, 3, 1> zwwy;
	proxy<2, 3, 3, 2> zwwz;
	proxy<2, 3, 3, 3> zwww;
	proxy<3, 0, 0, 0> wxxx;
	proxy<3, 0, 0, 1> wxxy;
	proxy<3, 0, 0, 2> wxxz;
	proxy<3, 0, 0, 3> wxxw;
	proxy<3, 0, 1, 0> wxyx;
	proxy<3, 0, 1, 1> wxyy;
	proxy<3, 0, 1, 2> wxyz;
	proxy<3, 0, 1, 3> wxyw;
	proxy<3, 0, 2, 0> wxzx;
	proxy<3, 0, 2, 1> wxzy;
	proxy<3, 0, 2, 2> wxzz;
	proxy<3, 0, 2, 3> wxzw;
	proxy<3, 0, 3, 0> wxwx;
	proxy<3, 0, 3, 1> wxwy;
	proxy<3, 0, 3, 2> wxwz;
	proxy<3, 0, 3, 3> wxww;
	proxy<3, 1, 0, 0> wyxx;
	proxy<3, 1, 0, 1> wyxy;
	proxy<3, 1, 0, 2> wyxz;
	proxy<3, 1, 0, 3> wyxw;
	proxy<3, 1, 1, 0> wyyx;
	proxy<3, 1, 1, 1> wyyy;
	proxy<3, 1, 1, 2> wyyz;
	proxy<3, 1, 1, 3> wyyw;
	proxy<3, 1, 2, 0> wyzx;
	proxy<3, 1, 2, 1> wyzy;
	proxy<3, 1, 2, 2> wyzz;
	proxy<3, 1, 2, 3> wyzw;
	proxy<3, 1, 3, 0> wywx;
	proxy<3, 1, 3, 1> wywy;
	proxy<3, 1, 3, 2> wywz;
	proxy<3, 1, 3, 3> wyww;
	proxy<3, 2, 0, 0> wzxx;
	proxy<3, 2, 0, 1> wzxy;
	proxy<3, 2, 0, 2> wzxz;
	proxy<3, 2, 0, 3> wzxw;
	proxy<3, 2, 1, 0> wzyx;
	proxy<3, 2, 1, 1> wzyy;
	proxy<3, 2, 1, 2> wzyz;
	proxy<3, 2, 1, 3> wzyw;
	proxy<3, 2, 2, 0> wzzx;
	proxy<3, 2, 2, 1> wzzy;
	proxy<3, 2, 2, 2> wzzz;
	proxy<3, 2, 2, 3> wzzw;
	proxy<3, 2, 3, 0> wzwx;
	proxy<3, 2, 3, 1> wzwy;
	proxy<3, 2, 3, 2> wzwz;
	proxy<3, 2, 3, 3> wzww;
	proxy<3, 3, 0, 0> wwxx;
	proxy<3, 3, 0, 1> wwxy;
	proxy<3, 3, 0, 2> wwxz;
	proxy<3, 3, 0, 3> wwxw;
	proxy<3, 3, 1, 0> wwyx;
	proxy<3, 3, 1, 1> wwyy;
	proxy<3, 3, 1, 2> wwyz;
	proxy<3, 3, 1, 3> wwyw;
	proxy<3, 3, 2, 0> wwzx;
	proxy<3, 3, 2, 1> wwzy;
	proxy<3, 3, 2, 2> wwzz;
	proxy<3, 3, 2, 3> wwzw;
	proxy<3, 3, 3, 0> wwwx;
	proxy<3, 3, 3, 1> wwwy;
	proxy<3, 3, 3, 2> wwwz;
	proxy<3, 3, 3, 3> wwww;
};

__forceinline vec4 __vectorcall operator + (vec4 a, vec4 b) { return _mm_add_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall operator - (vec4 a, vec4 b) { return _mm_sub_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall operator * (vec4 a, vec4 b) { return _mm_mul_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall operator / (vec4 a, vec4 b) { return _mm_div_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall operator & (vec4 a, vec4 b) { return _mm_and_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall operator ^ (vec4 a, vec4 b) { return _mm_xor_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall fmadd(vec4 a, vec4 b, vec4 c) { return _mm_fmadd_ps(a.xmm, b.xmm, c.xmm); }
__forceinline vec4 __vectorcall fmsub(vec4 a, vec4 b, vec4 c) { return _mm_fmadd_ps(a.xmm, b.xmm, c.xmm); }
__forceinline vec4 __vectorcall fmsubadd(vec4 a, vec4 b, vec4 c) { return _mm_fmsubadd_ps(a.xmm, b.xmm, c.xmm); }
__forceinline vec4 __vectorcall fnmadd(vec4 a, vec4 b, vec4 c) { return _mm_fnmadd_ps(a.xmm, b.xmm, c.xmm); }
__forceinline vec4 __vectorcall fnmsub(vec4 a, vec4 b, vec4 c) { return _mm_fnmadd_ps(a.xmm, b.xmm, c.xmm); }
__forceinline vec4 __vectorcall zerow(vec4 a) { return a & _mm_castsi128_ps(_mm_setr_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0)); }
__forceinline vec4 __vectorcall cross(vec4 a, vec4 b) { return fmsub(a, b.yzxw, b * a.yzxw).yzxw; }
__forceinline vec4 __vectorcall dot(vec4 a, vec4 b) { return _mm_dp_ps(a.xmm, b.xmm, 255); }
__forceinline vec4 __vectorcall sqrt(vec4 a) { return _mm_sqrt_ps(a.xmm); }
__forceinline vec4 __vectorcall len(vec4 a)  { return sqrt(dot(a, a)); }
__forceinline vec4 __vectorcall norm(vec4 a) { return a / sqrt(dot(a, a)); }
__forceinline vec4 __vectorcall norm3(vec4 a) { return norm(zerow(a)); }
__forceinline vec4 __vectorcall sincos(vec4* cos, vec4 ang) { return _mm_sincos_ps(&cos->xmm, ang.xmm); }
__forceinline vec4 __vectorcall unpacklo(vec4 a, vec4 b) { return _mm_unpacklo_ps(a.xmm, b.xmm); }
__forceinline vec4 __vectorcall unpackhi(vec4 a, vec4 b) { return _mm_unpackhi_ps(a.xmm, b.xmm); }

template<int x, int y, int z, int w>
__forceinline vec4 __vectorcall sign_mask() {
	const int i = 0x80000000;
	return _mm_castsi128_ps(_mm_setr_epi32(x ? i : 0, y ? i : 0, z ? i : 0, w ? i : 0));
}

template<int x, int y, int z, int w>
__forceinline vec4 __vectorcall zero_mask() {
	const int i = 0xffffffff;
	return _mm_castsi128_ps(_mm_setr_epi32(x ? i : 0, y ? i : 0, z ? i : 0, w ? i : 0));
}

__forceinline vec8 __vectorcall operator + (vec8 a, vec8 b) { return _mm256_add_ps(a.ymm, b.ymm); }
__forceinline vec8 __vectorcall operator - (vec8 a, vec8 b) { return _mm256_sub_ps(a.ymm, b.ymm); }
__forceinline vec8 __vectorcall operator * (vec8 a, vec8 b) { return _mm256_mul_ps(a.ymm, b.ymm); }
__forceinline vec8 __vectorcall operator / (vec8 a, vec8 b) { return _mm256_div_ps(a.ymm, b.ymm); }
__forceinline vec8 __vectorcall fmadd(vec8 a, vec8 b, vec8 c) { return _mm256_fmadd_ps(a.ymm, b.ymm, c.ymm); }
__forceinline vec8 __vectorcall fmsub(vec8 a, vec8 b, vec8 c) { return _mm256_fmadd_ps(a.ymm, b.ymm, c.ymm); }
__forceinline vec8 __vectorcall cross(vec8 a, vec8 b) { return fmsub(a, b.yzxw, b * a.yzxw).yzxw; }
__forceinline vec8 __vectorcall dot(vec8 a, vec8 b) { return _mm256_dp_ps(a.ymm, b.ymm, 255); }
#define SHUFFLE(a, b, x, y, z, w) (_mm_shuffle_ps(a.xmm, b.xmm, (x | (y << 2) | (z << 4) | (w << 6))))
union mat
{
	struct { vec4 x, y, z, w; };
	struct { vec8 xy, zw; };
	__m256 ymm[2];
	__m128 xmm[4];
	mat tpos() const {

		__m256i mask = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
		mat re;
		re.xy = _mm256_unpacklo_ps(ymm[0], ymm[1]);
		re.zw = _mm256_unpackhi_ps(ymm[0], ymm[1]);
		re.xy = _mm256_permutevar8x32_ps(re.ymm[0], mask);
		re.zw = _mm256_permutevar8x32_ps(re.ymm[1], mask);
		return re;
	}
	mat inv() const {
		vec4 m0 = _mm_movelh_ps(xmm[0], xmm[1]);
		vec4 m1 = _mm_movehl_ps(xmm[1], xmm[0]);
		vec4 m2 = _mm_movelh_ps(xmm[2], xmm[3]);
		vec4 m3 = _mm_movehl_ps(xmm[3], xmm[2]);
		vec4 det = fmsub(SHUFFLE(x, z, 0, 2, 0, 2), SHUFFLE(y, w, 1, 3, 1, 3), SHUFFLE(x, z, 1, 3, 1, 3) * SHUFFLE(y, w, 0, 2, 0, 2));
		vec4 det0 = det.xxxx;
		vec4 det1 = det.yyyy;
		vec4 det2 = det.zzzz;
		vec4 det3 = det.wwww;
		vec4 D_C = fmsub(m3.wwxx, m2, m3.yyzz * m2.zwxy);
		vec4 A_B = fmsub(m0.wwxx, m1, m0.yyzz * m1.zwxy);
		vec4 X_ = fmsub(det3, m0, fmadd(m1, D_C.xwxw, m1.yxwz * D_C.zyzy));
		vec4 W_ = fmsub(det0, m3, fmadd(m2, A_B.xwxw, m2.yxwz * A_B.zyzy));
		vec4 Y_ = fmsub(det1, m2, fmsub(m3, A_B.wxwx, m3.yxwz * A_B.zyzy));
		vec4 Z_ = fmsub(det2, m1, fmsub(m0, D_C.wxwx, m0.yxwz * D_C.zyzy));
		vec4 detM = fmadd(det0, det3, det1 * det2);
		detM = detM - dot(A_B, D_C.xzyw);
		vec4 mask = { 1.f, -1.f, -1.f, 1.f };
		detM = mask / detM;
		X_ = X_ * detM;
		Y_ = Y_ * detM;
		Z_ = Z_ * detM;
		W_ = W_ * detM;
		mat re;
		re.xmm[0] = SHUFFLE(X_, Y_, 3, 1, 3, 1);
		re.xmm[1] = SHUFFLE(X_, Y_, 2, 0, 2, 0);
		re.xmm[2] = SHUFFLE(Z_, W_, 3, 1, 3, 1);
		re.xmm[3] = SHUFFLE(Z_, W_, 2, 0, 2, 0);
		return re;
	}

	static mat load(const void* ptr)
	{
		mat re;
		re.xy = _mm256_loadu_ps((float*)ptr + 0);
		re.zw = _mm256_loadu_ps((float*)ptr + 8);
		return re;
	}

	static mat id()
	{
		mat id;
		id.x = vec4(1, 0, 0, 0);
		id.y = vec4(0, 1, 0, 0);
		id.z = vec4(0, 0, 1, 0);
		id.w = vec4(0, 0, 0, 1);
		return id;
	}

	static mat scale(float x, float y, float z)
	{
		mat id;
		id.x = vec4(x, 0, 0, 0);
		id.y = vec4(0, y, 0, 0);
		id.z = vec4(0, 0, z, 0);
		id.w = vec4(0, 0, 0, 1);
		return id;
	}

	static mat xlate(float x, float y, float z)
	{
		mat id;
		id.x = vec4(1, 0, 0, 0);
		id.y = vec4(0, 1, 0, 0);
		id.z = vec4(0, 0, 1, 0);
		id.w = vec4(x, y, z, 1);
		return id;
	}
};

__forceinline vec4 __vectorcall operator * (vec4 v, mat m)
{
	return fmadd(m.x, v.xxxx, fmadd(m.y, v.yyyy, fmadd(m.z, v.zzzz, m.w * v.wwww)));
}
__forceinline vec4 operator * (mat m, vec4 v) {
	return v * m.tpos();
}
__forceinline mat __vectorcall operator* (mat a, mat b)
{
	mat re;
	vec8 r0 = _mm256_broadcast_ps(&b.xmm[0]);
	re.xy = r0 * a.xy.xxxx;;
	re.zw = r0 * a.zw.xxxx;
	vec8 r1 = _mm256_broadcast_ps(&b.xmm[1]);
	re.xy = fmadd(r1, a.xy.yyyy, re.ymm[0]);
	re.zw = fmadd(r1, a.zw.yyyy, re.ymm[1]);
	vec8 r2 = _mm256_broadcast_ps(&b.xmm[2]);
	re.xy = fmadd(r2, a.xy.zzzz, re.ymm[0]);
	re.zw = fmadd(r2, a.zw.zzzz, re.ymm[1]);
	vec8 r3 = _mm256_broadcast_ps(&b.xmm[3]);
	re.xy = fmadd(r3, a.xy.wwww, re.ymm[0]);
	re.zw = fmadd(r3, a.zw.wwww, re.ymm[1]);
	return re;
}

__forceinline mat __vectorcall axang(vec4 ax, float ang) {
	vec4 axis = norm3(ax);
	vec4 cos;
	vec4 sin = sincos(&cos, ang) * axis;
	sin = _mm_blend_ps(sin.xmm, cos.xmm, 8);
	vec4 v = fnmadd(axis, cos, axis);
	mat re;
	re.x = zerow(fmadd(axis.xxxx, v, sin.wzyw ^ sign_mask<0, 0, 1, 0>()));
	re.y = zerow(fmadd(axis.yyyy, v, sin.zwxw ^ sign_mask<1, 0, 0, 0>()));
	re.z = zerow(fmsubadd(axis.zzzz, v, sin.yxww));
	re.w = vec4(0, 0, 0, 1);
	return re;
}

__forceinline mat roty(float ang) {
	vec4 cos;
	vec4 sin = sincos(&cos, ang);
	vec4 row = zerow(unpacklo(sin, cos)) ^ sign_mask<1, 0, 0, 0>();
	mat re;
	re.x = row.ywzw;
	re.y = vec4(0, 1, 0, 0);
	re.z = row.xwyw;
	re.w = vec4(0, 0, 0, 1);
	return re;
}

__forceinline mat rotx(float ang) {
	vec4 cos;
	vec4 sin = sincos(&cos, ang);
	vec4 row = zerow(unpacklo(sin, cos)) ^ sign_mask<1, 0, 0, 0>();
	mat re;
	re.x = vec4(1, 0, 0, 0);
	re.y = row.wyzw;
	re.z = row.wxyw;
	re.w = vec4(0, 0, 0, 1);
	return re;
}
__forceinline mat rotz(float ang) {
	vec4 cos;
	vec4 sin = sincos(&cos, ang);
	vec4 row = zerow(unpacklo(sin, cos)) ^ sign_mask<1, 0, 0, 0>();
	mat re;
	re.x = row.yzww;
	re.y = row.xyww;
	re.z = vec4(0, 0, 1, 0);
	re.w = vec4(0, 0, 0, 1);
	return re;
}

__forceinline vec4 MkQuat(vec4 ax, float ang)
{
	vec4 cos;
	vec4 sin = sincos(&cos, ang * 0.5f) * norm3(ax);

	return _mm_blend_ps(sin.xmm, cos.xmm, 8);
}

__forceinline mat __vectorcall compose(vec4 q, vec4 v = vec4(0, 0, 0, 1))
{
	vec4 q2 = q + q;
	vec4 qq = q2 * q;
	qq = 1.f - qq - qq.yzxw;
	vec4 qx = q2 * q.yzxw;
	vec4 qw = q2 * q.wwww;
	vec4 uu = qx.xzyw;
	vec4 vv = qw.zyxw;
	vec4 uv = uu + vv;
	vec4 vu = uu - vv;
	vec4 r0 = SHUFFLE(uv, vu, 0, 0, 1, 1);
	vec4 r1 = SHUFFLE(vu, uv, 0, 0, 2, 2);
	vec4 r2 = SHUFFLE(uv, vu, 1, 1, 2, 2);
	r2 = r2.xzyw;
	mat re;
	re.x = _mm_insert_ps(r0.xmm, qq.xmm, 8 | (0 << 4) | (1 << 6));
	re.y = _mm_insert_ps(r1.xmm, qq.xmm, 8 | (1 << 4) | (2 << 6));
	re.z = _mm_insert_ps(r2.xmm, qq.xmm, 8 | (2 << 4) | (0 << 6));
	re.w = v;
	return re;
}

__forceinline mat perspective(float fov, float x, float y, float n = .01f, float f = 2048.f)
{
	vec4 c;
	vec4 s = sincos(&c, fov * 0.5f);
	vec4 Height = c / s;
	vec4 Width = Height * y / x;
	float fRange = f / (n - f);

	mat re;
	re.x = Width & zero_mask<1, 0, 0, 0>();
	re.y = Height & zero_mask<0, 1, 0, 0>();
	re.z = vec4(0, 0, -fRange, 1);
	re.w = vec4(0, 0, fRange * n, 0);
	return re;
}

template<int mask>
proxy128<mask>::operator vec4() const
{
	return _mm_permute_ps(xmm, mask);
}

template<int mask>
proxy256<mask>::operator vec8() const
{
	return _mm256_permute_ps(ymm, mask);
}

__forceinline mat rotxyz(vec4 rot, vec4 pos = { 0, 0, 0, 1 })
{
	vec4 c;
	vec4 s = sincos(&c, rot);
	mat re;
	re.x = { c.y * c.z, c.y * s.z, s.y };
	re.y = { -s.x * s.y * c.z - s.z * c.x, c.z * c.x - s.x * s.y * s.z, s.x * c.y };
	re.z = { s.z * s.x - c.z * c.x * s.y, -s.y * s.z * c.x - s.x * c.z, c.x * c.y };
	re.w = pos;
	return re;
}

__forceinline vec4 vlerp(vec4 a, vec4 b, vec4 t) { return fmadd(b - a, t, a); }

__forceinline vec4 qlerp(vec4 a, vec4 b, vec4 t)
{
	if (_mm_movemask_ps(_mm_cmp_ps(dot(a, b).xmm, _mm_setzero_ps(), _CMP_GE_OS)))
		return norm(fmadd(t, b - a, a));
	return norm(fnmadd(t, a + b, a));
}

__forceinline vec4 qinv(vec4 q) { return  q ^ sign_mask<1, 1, 1, 0>(); }
__forceinline vec4 qinv2(vec4 q) { return  q ^ sign_mask<0, 0, 0, 1>(); }

__forceinline vec4 qmul(vec4 a, vec4 b)
{
	return norm(fmadd(vec4(1, 1, 1, -1), fmadd(b.yyxy, a.zwyy, b.xzzz * a.wxwz), fmsub(b.wwww, a, b.zxyx * a.yzxx)));
}


#include "stdio.h"

inline void printv(vec3 v)
{
	printf("%10f %10f %10f\n", v.x, v.y, v.z);
}

inline void printv(vec4 v)
{
	printf("%10f %10f %10f %10f\n", v.x, v.y, v.z, v.w);
}

inline void printv(mat v)
{
	printv(v.x);
	printv(v.y);
	printv(v.z);
	printv(v.w);
	printf("-----------\n");
}