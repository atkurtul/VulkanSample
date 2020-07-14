/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2019 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include <cassert>
#include <cmath>
#include "math.hpp"

namespace mango
{

    // ------------------------------------------------------------------
    // Quadratic
    // ------------------------------------------------------------------

    struct Quadratic
    {
        float t0;
        float t1;

        Quadratic(float a, float b, float c)
        {
            float x = std::sqrt(solve(a * 4.0f, b, c));
            x = b + b < 0 ? -x : x;
            t0 = (-2.0f * c) / x;
            t1 = x / (-2.0f * a);
        }

        float solve(float a, float b, float c) const
        {
            float t = a * c;
            return (b * b - t) - (a * c - t);
        }
    };

    // ------------------------------------------------------------------
    // LineSegment
    // ------------------------------------------------------------------

    struct LineSegment
    {
        float3 position[2];

        LineSegment()
        {
        }

        LineSegment(const float3& position0, const float3& position1)
        {
            position[0] = position0;
            position[1] = position1;
        }

        ~LineSegment()
        {
        }

        float3 closest(const float3& point) const;
        float distance(const float3& point) const;
    };

    // ------------------------------------------------------------------
    // Ray
    // ------------------------------------------------------------------

    struct Ray
    {
        float3 origin;
        float3 direction;

        Ray()
        {
        }

        Ray(const float3& origin, const float3& direction)
            : origin(origin)
            , direction(direction)
        {
        }

        ~Ray()
        {
        }

        float distance(const float3& point) const;
    };

    // ------------------------------------------------------------------
    // FastRay
    // ------------------------------------------------------------------

    struct FastRay : Ray
    {
        float dotod;
        float dotoo;
        float3 invdir;
        int3 sign;

        FastRay(const Ray& ray);
        ~FastRay()
        {
        }
    };

    // ------------------------------------------------------------------
    // Rectangle
    // ------------------------------------------------------------------

    struct Rectangle
    {
        float2 position;
        float2 size;

        Rectangle()
        {
        }

        Rectangle(float x, float y, float width, float height)
            : position(x, y)
            , size(width, height)
        {
        }

        Rectangle(const float2& position, const float2& size)
            : position(position)
            , size(size)
        {
        }

        ~Rectangle()
        {
        }

        float aspect() const;
        bool inside(const float2& point) const;
    };

    // ------------------------------------------------------------------
    // Plane
    // ------------------------------------------------------------------

    struct Plane
    {
        float3 normal;
        float dist;

        Plane()
        {
        }

        Plane(const float3& normal, float dist)
            : normal(normal)
            , dist(dist)
        {
        }

        Plane(const float3& normal, const float3& point)
            : normal(normal)
        {
            dist = dot(normal, point);
        }

        Plane(const float3& point0, const float3& point1, const float3& point2)
        {
            normal = normalize(cross(point1 - point0, point2 - point0));
            dist = dot(point0, normal);
        }

        Plane(float x, float y, float z, float w)
            : normal(x, y, z)
            , dist(w)
        {
        }

        ~Plane()
        {
        }

        operator float4 () const
        {
            return float4(normal, dist);
        }

        float distance(const float3& point) const
        {
            return dot(normal, point) - dist;
        }
    };

    // ------------------------------------------------------------------
    // Box
    // ------------------------------------------------------------------

    struct Box
    {
        float3 corner[2];

        Box()
        {
            const float s = std::numeric_limits<float>::max();
            corner[0] = float3(s, s, s);
            corner[1] = float3(-s, -s, -s);
        }

        Box(const float3& point, float size)
        {
            corner[0] = point - size * 0.5f;
            corner[1] = point + size * 0.5f;
        }

        Box(const float3& point0, const float3& point1)
        {
            corner[0] = min(point0, point1);
            corner[1] = max(point0, point1);
        }

        Box(const Box& box0, const Box& box1)
        {
            corner[0] = min(box0.corner[0], box1.corner[0]);
            corner[1] = max(box0.corner[1], box1.corner[1]);
        }

        ~Box()
        {
        }

        float3 center() const;
        float3 size() const;
        void extend(const float3& point);
        void extend(const Box& box);
        bool inside(const float3& point) const;
        float3 vertex(int index) const;
        void vertices(float3 vertex[]) const;
    };

    // ------------------------------------------------------------------
    // Sphere
    // ------------------------------------------------------------------

    struct Sphere
    {
        float3 center;
        float radius;

        Sphere()
        {
        }

        Sphere(const float3& center, float radius)
            : center(center)
            , radius(radius)
        {
        }

        ~Sphere()
        {
        }

        void circumscribe(const Box& box);
        bool inside(const float3& point) const;
    };

    // ------------------------------------------------------------------
    // Cone
    // ------------------------------------------------------------------

    struct Cone
    {
        float3 origin;
        float3 target;
        float angle;

        Cone()
        {
        }

        Cone(const float3& origin, const float3& target, float angle)
            : origin(origin)
            , target(target)
            , angle(angle)
        {
        }

        ~Cone()
        {
        }
    };

    // ------------------------------------------------------------------
    // Triangle
    // ------------------------------------------------------------------

    struct Triangle
    {
        float3 position[3];

        Triangle()
        {
        }

        Triangle(const float3& point0, const float3& point1, const float3& point2)
        {
            position[0] = point0;
            position[1] = point1;
            position[2] = point2;
        }

        ~Triangle()
        {
        }

        float3 normal() const;
        bool barycentric(float3& result, const float3& point) const;
    };

    // ------------------------------------------------------------------
    // TexTriangle
    // ------------------------------------------------------------------

    struct TexTriangle : Triangle
    {
        float2 texcoord[3];

        TexTriangle()
        {
        }

        ~TexTriangle()
        {
        }

        float3x3 tbn() const;
    };

    // ------------------------------------------------------------------
    // Frustum
    // ------------------------------------------------------------------

    struct Frustum
    {
        float3 point[4]; // 0: top_left, 1: top_right, 2: bottom_left, 3: bottom_right
        float3 origin;

        Frustum() = default;
        Frustum(const float4x4& m);
        ~Frustum() = default;

        Ray ray(float x, float y) const;
    };

    // ------------------------------------------------------------------
    // Intersect
    // ------------------------------------------------------------------

    // Intersect implements non-solid, one-sided primitives so any intersection from
    // the back of the primitive will be culled. Intersection behind the ray origin will be false.

    struct Intersect
    {
        float t0;

	    bool intersect(const Ray& ray, const Plane& plane);
	    bool intersect(const Ray& ray, const Sphere& sphere);
	    bool intersect(const Ray& ray, const Triangle& triangle);
    };

    // IntersectRange implements non-solid primitives and the intersections are at the boundaries.
    // Negative sign of either intersection (t0, t1) means that intersection was behind the ray origin.
    // If both intersections are behind the ray origin, the intersection result is false.

    struct IntersectRange
    {
        float t0;
        float t1;

	    bool intersect(const Ray& ray, const Box& box);
	    bool intersect(const FastRay& ray, const Box& box);
	    bool intersect(const FastRay& ray, const Sphere& sphere);
    };

    // Intersection inside the primitive will intersect at ray origin

    struct IntersectSolid
    {
        float t0;

	    bool intersect(const Ray& ray, const Box& box);
	    bool intersect(const FastRay& ray, const Box& box);
	    bool intersect(const FastRay& ray, const Sphere& sphere);
    };

    struct IntersectBarycentric
    {
        float t0;
        float u, v, w;

        bool intersect(const Ray& ray, const Triangle& triangle);
    };

    struct IntersectBarycentricTwosided
    {
        float t0;
        float u, v, w;

        bool intersect(const Ray& ray, const Triangle& triangle);
    };

    bool intersect(Rectangle& result, const Rectangle& rect0, const Rectangle& rect1);
    bool intersect(Ray& result, const Plane& plane0, const Plane& plane1);
    bool intersect(float3& result, const Plane& plane0, const Plane& plane1, const Plane& plane2);

    bool intersect(const Sphere& sphere, const Box& box);
    bool intersect(const Cone& cone, const Sphere& sphere);

} // namespace mango
