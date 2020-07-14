/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2020 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include "vector.hpp"

namespace mango
{

    template <>
    struct Vector<s16, 32>
    {
        using VectorType = simd::s16x32;
        using ScalarType = s16;
        enum { VectorSize = 32 };

        VectorType m;

        ScalarType& operator [] (size_t index)
        {
            assert(index < VectorSize);
            return data()[index];
        }

        ScalarType operator [] (size_t index) const
        {
            assert(index < VectorSize);
            return data()[index];
        }

        ScalarType* data()
        {
            return reinterpret_cast<ScalarType *>(this);
        }

        const ScalarType* data() const
        {
            return reinterpret_cast<const ScalarType *>(this);
        }

        explicit Vector() {}

        Vector(s16 s)
            : m(simd::s16x32_set(s))
        {
        }

        Vector(simd::s16x32 v)
            : m(v)
        {
        }

        template <typename T, int I>
        Vector& operator = (const ScalarAccessor<ScalarType, T, I>& accessor)
        {
            *this = ScalarType(accessor);
            return *this;
        }

        Vector(const Vector& v) = default;

        Vector& operator = (const Vector& v)
        {
            m = v.m;
            return *this;
        }

        Vector& operator = (simd::s16x32 v)
        {
            m = v;
            return *this;
        }

        Vector& operator = (s16 s)
        {
            m = simd::s16x32_set(s);
            return *this;
        }

        operator simd::s16x32 () const
        {
            return m;
        }

#ifdef int512_is_hardware_vector
        operator simd::s16x32::vector () const
        {
            return m.data;
        }
#endif
    };

    static inline const Vector<s16, 32> operator + (Vector<s16, 32> v)
    {
        return v;
    }

    static inline Vector<s16, 32> operator - (Vector<s16, 32> v)
    {
        return simd::neg(v);
    }

    static inline Vector<s16, 32>& operator += (Vector<s16, 32>& a, Vector<s16, 32> b)
    {
        a = simd::add(a, b);
        return a;
    }

    static inline Vector<s16, 32>& operator -= (Vector<s16, 32>& a, Vector<s16, 32> b)
    {
        a = simd::sub(a, b);
        return a;
    }

    static inline Vector<s16, 32> operator + (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::add(a, b);
    }

    static inline Vector<s16, 32> operator - (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::sub(a, b);
    }

    static inline Vector<s16, 32> unpacklo(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::unpacklo(a, b);
    }

    static inline Vector<s16, 32> unpackhi(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::unpackhi(a, b);
    }

    static inline Vector<s16, 32> abs(Vector<s16, 32> a)
    {
        return simd::abs(a);
    }

    static inline Vector<s16, 32> abs(Vector<s16, 32> a, mask16x32 mask)
    {
        return simd::abs(a, mask);
    }

    static inline Vector<s16, 32> abs(Vector<s16, 32> a, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::abs(a, mask, value);
    }

    static inline Vector<s16, 32> add(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask)
    {
        return simd::add(a, b, mask);
    }

    static inline Vector<s16, 32> add(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::add(a, b, mask, value);
    }

    static inline Vector<s16, 32> sub(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask)
    {
        return simd::sub(a, b, mask);
    }

    static inline Vector<s16, 32> sub(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::sub(a, b, mask, value);
    }

    static inline Vector<s16, 32> adds(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::adds(a, b);
    }

    static inline Vector<s16, 32> adds(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask)
    {
        return simd::adds(a, b, mask);
    }

    static inline Vector<s16, 32> adds(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::adds(a, b, mask, value);
    }

    static inline Vector<s16, 32> subs(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::subs(a, b);
    }

    static inline Vector<s16, 32> subs(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask)
    {
        return simd::subs(a, b, mask);
    }

    static inline Vector<s16, 32> subs(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::subs(a, b, mask, value);
    }

    static inline Vector<s16, 32> min(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::min(a, b);
    }

    static inline Vector<s16, 32> min(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask)
    {
        return simd::min(a, b, mask);
    }

    static inline Vector<s16, 32> min(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::min(a, b, mask, value);
    }

    static inline Vector<s16, 32> max(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::max(a, b);
    }

    static inline Vector<s16, 32> max(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask)
    {
        return simd::max(a, b, mask);
    }

    static inline Vector<s16, 32> max(Vector<s16, 32> a, Vector<s16, 32> b, mask16x32 mask, Vector<s16, 32> value)
    {
        return simd::max(a, b, mask, value);
    }

    static inline Vector<s16, 32> clamp(Vector<s16, 32> a, Vector<s16, 32> low, Vector<s16, 32> high)
    {
        return simd::clamp(a, low, high);
    }

    // ------------------------------------------------------------------
	// bitwise operators
    // ------------------------------------------------------------------

    static inline Vector<s16, 32> nand(Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::bitwise_nand(a, b);
    }

    static inline Vector<s16, 32> operator & (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::bitwise_and(a, b);
    }

    static inline Vector<s16, 32> operator | (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::bitwise_or(a, b);
    }

    static inline Vector<s16, 32> operator ^ (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::bitwise_xor(a, b);
    }

    static inline Vector<s16, 32> operator ~ (Vector<s16, 32> a)
    {
        return simd::bitwise_not(a);
    }

    // ------------------------------------------------------------------
	// compare / select
    // ------------------------------------------------------------------

    static inline mask16x32 operator > (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::compare_gt(a, b);
    }

    static inline mask16x32 operator < (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::compare_gt(b, a);
    }

    static inline mask16x32 operator == (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::compare_eq(a, b);
    }

    static inline mask16x32 operator >= (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::compare_ge(a, b);
    }

    static inline mask16x32 operator <= (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::compare_le(b, a);
    }

    static inline mask16x32 operator != (Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::compare_neq(a, b);
    }

    static inline Vector<s16, 32> select(mask16x32 mask, Vector<s16, 32> a, Vector<s16, 32> b)
    {
        return simd::select(mask, a, b);
    }

    // ------------------------------------------------------------------
	// shift
    // ------------------------------------------------------------------

    static inline Vector<s16, 32> operator << (Vector<s16, 32> a, int b)
    {
        return simd::sll(a, b);
    }

    static inline Vector<s16, 32> operator >> (Vector<s16, 32> a, int b)
    {
        return simd::sra(a, b);
    }

} // namespace mango
