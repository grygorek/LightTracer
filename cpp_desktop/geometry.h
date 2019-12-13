#ifndef VECTOR3_H
#define VECTOR3_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <corecrt_math_defines.h>

// remove windows crap
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

constexpr float kInfinity = std::numeric_limits<float>::max();
constexpr float kEpsilon  = 1e-8f;

template <typename T>
class Vec2
{
public:
  Vec2()
      : x(0)
      , y(0)
  {
  }
  Vec2(T xx)
      : x(xx)
      , y(xx)
  {
  }
  Vec2(T xx, T yy)
      : x(xx)
      , y(yy)
  {
  }
  Vec2 operator+(const Vec2 &v) const { return Vec2(x + v.x, y + v.y); }
  Vec2 operator/(const T &r) const { return Vec2(x / r, y / r); }
  Vec2 operator*(const T &r) const { return Vec2(x * r, y * r); }
  Vec2 &operator/=(const T &r)
  {
    x /= r, y /= r;
    return *this;
  }
  Vec2 &operator*=(const T &r)
  {
    x *= r, y *= r;
    return *this;
  }
  friend std::ostream &operator<<(std::ostream &s, const Vec2<T> &v)
  {
    return s << '[' << v.x << ' ' << v.y << ']';
  }
  friend Vec2 operator*(T r, const Vec2<T> &v)
  {
    return Vec2(v.x * r, v.y * r);
  }
  T x, y;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

template <typename T>
class Vec3
{
public:
  using type = T;

  T x, y, z;
  Vec3()
      : x(T(0))
      , y(T(0))
      , z(T(0))
  {
  }
  Vec3(T xx)
      : x(xx)
      , y(xx)
      , z(xx)
  {
  }
  Vec3(T xx, T yy, T zz)
      : x(xx)
      , y(yy)
      , z(zz)
  {
  }
  Vec3 &normalize()
  {
    T nor2 = length2();
    if (nor2 > 0)
    {
      T invNor = 1 / sqrt(nor2);
      x *= invNor, y *= invNor, z *= invNor;
    }
    return *this;
  }
  Vec3 normalize() const
  {
    Vec3 copy{*this};
    return copy.normalize();
  }

  T operator[](int i) const
  {
    // check if the structure is alligned
    if (sizeof(Vec3) == sizeof(T) * 3)
    {
      const T *t = &x;
      return t[i];
    }
    else
    {
      switch (i)
      {
      case 0:
        return x;
      case 1:
        return y;
      case 2:
        return z;
      }
    }
  }

  T &operator[](int i)
  {
    // check if the structure is alligned
    if (sizeof(Vec3) == sizeof(T) * 3)
    {
      const T *t = &x;
      return t[i];
    }
    else
    {
      switch (i)
      {
      case 0:
        return x;
      case 1:
        return y;
      case 2:
        return z;
      }
    }
  }

  Vec3<T> operator*(T f) const { return Vec3<T>(x * f, y * f, z * f); }
  Vec3<T> operator*(const Vec3<T> &v) const
  {
    return Vec3<T>(x * v.x, y * v.y, z * v.z);
  }

  Vec3 operator/(T f) const { return Vec3(x / f, y / f, z / f); }
  Vec3 operator/(const Vec3 &f) const
  {
    return Vec3(x / f.x, y / f.y, z / f.z);
  }

  T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
  Vec3 cross(const Vec3<T> &v) const
  {
    return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }
  Vec3<T> operator-(const Vec3<T> &v) const
  {
    return Vec3<T>(x - v.x, y - v.y, z - v.z);
  }
  Vec3<T> operator+(const Vec3<T> &v) const
  {
    return Vec3<T>(x + v.x, y + v.y, z + v.z);
  }
  Vec3<T> &operator+=(const Vec3<T> &v)
  {
    x += v.x, y += v.y, z += v.z;
    return *this;
  }
  Vec3<T> &operator*=(const Vec3<T> &v)
  {
    x *= v.x, y *= v.y, z *= v.z;
    return *this;
  }
  Vec3<T> operator-() const { return Vec3<T>(-x, -y, -z); }
  T length2() const { return x * x + y * y + z * z; }
  T length() const { return sqrt(length2()); }

  friend std::ostream &operator<<(std::ostream &os, const Vec3<T> &v)
  {
    os << "[" << v.x << " " << v.y << " " << v.z << "]";
    return os;
  }
};

inline
Vec3<float> operator*(float f, const Vec3<float> &v)
{
  return Vec3<float>(f) * v;
}

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

//[comment]
// Implementation of a generic 4x4 Matrix class - Same thing here than with the
// Vec3 class. It uses a template which is maybe less useful than with vectors
// but it can be used to define the coefficients of the matrix to be either
// floats (the most case) or doubles depending on our needs.
//
// To use you can either write: Matrix44<float> m; or: Matrix44f m;
//[/comment]
template <typename T>
class Matrix44
{
public:
  T x[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

  Matrix44() = default;

  Matrix44(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n,
           T o, T p)
  {
    x[0][0] = a;
    x[0][1] = b;
    x[0][2] = c;
    x[0][3] = d;
    x[1][0] = e;
    x[1][1] = f;
    x[1][2] = g;
    x[1][3] = h;
    x[2][0] = i;
    x[2][1] = j;
    x[2][2] = k;
    x[2][3] = l;
    x[3][0] = m;
    x[3][1] = n;
    x[3][2] = o;
    x[3][3] = p;
  }

  const T *operator[](int i) const { return x[i]; }
  T *operator[](int i) { return x[i]; }

  // Multiply the current matrix with another matrix (rhs)
  Matrix44 operator*(const Matrix44 &v) const
  {
    Matrix44 tmp;
    multiply(*this, v, tmp);

    return tmp;
  }

  //[comment]
  // To make it easier to understand how a matrix multiplication works, the
  // fragment of code included within the #if-#else statement, show how this
  // works if you were to iterate over the coefficients of the resulting matrix
  // (a). However you will often see this multiplication being done using the
  // code contained within the #else-#end statement. It is exactly the same as
  // the first fragment only we have litteraly written down as a series of
  // operations what would actually result from executing the two for() loops
  // contained in the first fragment. It is supposed to be faster, however
  // considering matrix multiplicatin is not necessarily that common, this is
  // probably not super useful nor really necessary (but nice to have -- and it
  // gives you an example of how it can be done, as this how you will this
  // operation implemented in most libraries).
  //[/comment]
  static void multiply(const Matrix44<T> &a, const Matrix44 &b, Matrix44 &c)
  {
#if 0
        for (uint8_t i = 0; i < 4; ++i) {
            for (uint8_t j = 0; j < 4; ++j) {
                c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] +
                    a[i][2] * b[2][j] + a[i][3] * b[3][j];
            }
        }
#else
    // A restric qualified pointer (or reference) is basically a promise
    // to the compiler that for the scope of the pointer, the target of the
    // pointer will only be accessed through that pointer (and pointers
    // copied from it.
    const T *__restrict ap = &a.x[0][0];
    const T *__restrict bp = &b.x[0][0];
    T *__restrict cp       = &c.x[0][0];

    T a0, a1, a2, a3;

    a0 = ap[0];
    a1 = ap[1];
    a2 = ap[2];
    a3 = ap[3];

    cp[0] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
    cp[1] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
    cp[2] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
    cp[3] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

    a0 = ap[4];
    a1 = ap[5];
    a2 = ap[6];
    a3 = ap[7];

    cp[4] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
    cp[5] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
    cp[6] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
    cp[7] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

    a0 = ap[8];
    a1 = ap[9];
    a2 = ap[10];
    a3 = ap[11];

    cp[8]  = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
    cp[9]  = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
    cp[10] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
    cp[11] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

    a0 = ap[12];
    a1 = ap[13];
    a2 = ap[14];
    a3 = ap[15];

    cp[12] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
    cp[13] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
    cp[14] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
    cp[15] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];
#endif
  }

  // \brief return a transposed copy of the current matrix as a new matrix
  Matrix44 transposed() const
  {
#if 0
        Matrix44 t;
        for (uint8_t i = 0; i < 4; ++i) {
            for (uint8_t j = 0; j < 4; ++j) {
                t[i][j] = x[j][i];
            }
        }

        return t;
#else
    return Matrix44(x[0][0], x[1][0], x[2][0], x[3][0], x[0][1], x[1][1],
                    x[2][1], x[3][1], x[0][2], x[1][2], x[2][2], x[3][2],
                    x[0][3], x[1][3], x[2][3], x[3][3]);
#endif
  }

  // \brief transpose itself
  Matrix44 &transpose()
  {
    Matrix44 tmp(x[0][0], x[1][0], x[2][0], x[3][0], x[0][1], x[1][1], x[2][1],
                 x[3][1], x[0][2], x[1][2], x[2][2], x[3][2], x[0][3], x[1][3],
                 x[2][3], x[3][3]);
    *this = tmp;

    return *this;
  }

  //[comment]
  // This method needs to be used for point-matrix multiplication. Keep in mind
  // we don't make the distinction between points and vectors at least from
  // a programming point of view, as both (as well as normals) are declared as
  // Vec3. However, mathematically they need to be treated differently. Points
  // can be translated when translation for vectors is meaningless. Furthermore,
  // points are implicitly be considered as having homogeneous coordinates. Thus
  // the w coordinates needs to be computed and to convert the coordinates from
  // homogeneous back to Cartesian coordinates, we need to divided x, y z by w.
  //
  // The coordinate w is more often than not equals to 1, but it can be
  // different than 1 especially when the matrix is projective matrix
  // (perspective projection matrix).
  //[/comment]
  template <typename S>
  void multVecMatrix(const Vec3<S> &src, Vec3<S> &dst) const
  {
    S a, b, c, w;

    a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0] + x[3][0];
    b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1] + x[3][1];
    c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2] + x[3][2];
    w = src[0] * x[0][3] + src[1] * x[1][3] + src[2] * x[2][3] + x[3][3];

    dst.x = a / w;
    dst.y = b / w;
    dst.z = c / w;
  }

  //[comment]
  // This method needs to be used for vector-matrix multiplication. Look at the
  // differences with the previous method (to compute a point-matrix
  // multiplication). We don't use the coefficients in the matrix that account
  // for translation (x[3][0], x[3][1], x[3][2]) and we don't compute w.
  //[/comment]
  template <typename S>
  void multDirMatrix(const Vec3<S> &src, Vec3<S> &dst) const
  {
    S a, b, c;

    a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0];
    b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1];
    c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2];

    dst.x = a;
    dst.y = b;
    dst.z = c;
  }

  //[comment]
  // Compute the inverse of the matrix using the Gauss-Jordan (or reduced row)
  // elimination method. We didn't explain in the lesson on Geometry how the
  // inverse of matrix can be found. Don't worry at this point if you don't
  // understand how this works. But we will need to be able to compute the
  // inverse of matrices in the first lessons of the "Foundation of 3D
  // Rendering" section, which is why we've added this code. For now, you can
  // just use it and rely on it for doing what it's supposed to do. If you want
  // to learn how this works though, check the lesson on called Matrix Inverse
  // in the "Mathematics and Physics of Computer Graphics" section.
  //[/comment]
  Matrix44 inverse() const
  {
    int i, j, k;
    Matrix44 s;
    Matrix44 t(*this);

    // Forward elimination
    for (i = 0; i < 3; i++)
    {
      int pivot = i;

      T pivotsize = t[i][i];

      if (pivotsize < 0)
        pivotsize = -pivotsize;

      for (j = i + 1; j < 4; j++)
      {
        T tmp = t[j][i];

        if (tmp < 0)
          tmp = -tmp;

        if (tmp > pivotsize)
        {
          pivot     = j;
          pivotsize = tmp;
        }
      }

      if (pivotsize == 0)
      {
        // Cannot invert singular matrix
        return Matrix44();
      }

      if (pivot != i)
      {
        for (j = 0; j < 4; j++)
        {
          T tmp;

          tmp         = t[i][j];
          t[i][j]     = t[pivot][j];
          t[pivot][j] = tmp;

          tmp         = s[i][j];
          s[i][j]     = s[pivot][j];
          s[pivot][j] = tmp;
        }
      }

      for (j = i + 1; j < 4; j++)
      {
        T f = t[j][i] / t[i][i];

        for (k = 0; k < 4; k++)
        {
          t[j][k] -= f * t[i][k];
          s[j][k] -= f * s[i][k];
        }
      }
    }

    // Backward substitution
    for (i = 3; i >= 0; --i)
    {
      T f;

      if ((f = t[i][i]) == 0)
      {
        // Cannot invert singular matrix
        return Matrix44();
      }

      for (j = 0; j < 4; j++)
      {
        t[i][j] /= f;
        s[i][j] /= f;
      }

      for (j = 0; j < i; j++)
      {
        f = t[j][i];

        for (k = 0; k < 4; k++)
        {
          t[j][k] -= f * t[i][k];
          s[j][k] -= f * s[i][k];
        }
      }
    }

    return s;
  }

  // \brief set current matrix to its inverse
  const Matrix44 &invert()
  {
    *this = inverse();
    return *this;
  }

  friend std::ostream &operator<<(std::ostream &s, const Matrix44 &m)
  {
    std::ios_base::fmtflags oldFlags = s.flags();
    int width                        = 12; // total with of the displayed number
    s.precision(5); // control the number of displayed decimals
    s.setf(std::ios_base::fixed);

    s << "[" << std::setw(width) << m[0][0] << " " << std::setw(width)
      << m[0][1] << " " << std::setw(width) << m[0][2] << " "
      << std::setw(width) << m[0][3] << "\n"
      <<

        " " << std::setw(width) << m[1][0] << " " << std::setw(width) << m[1][1]
      << " " << std::setw(width) << m[1][2] << " " << std::setw(width)
      << m[1][3] << "\n"
      <<

        " " << std::setw(width) << m[2][0] << " " << std::setw(width) << m[2][1]
      << " " << std::setw(width) << m[2][2] << " " << std::setw(width)
      << m[2][3] << "\n"
      <<

        " " << std::setw(width) << m[3][0] << " " << std::setw(width) << m[3][1]
      << " " << std::setw(width) << m[3][2] << " " << std::setw(width)
      << m[3][3] << "]";

    s.flags(oldFlags);
    return s;
  }
};

typedef Matrix44<float> Matrix44f;
constexpr Matrix44f IdentityMtx44f;

template <class V3>
Matrix44<typename V3::type_name> lookAt(const V3 &from, const V3 &to,
                                        const V3 &tmp = V3(0, 1, 0))
{
  V3 forward = (from - to).normalize();
  V3 right   = tmp.normalize().cross(forward);
  V3 up      = forward.cross(right);

  Matrix44<typename V3::type_name> camToWorld;

  camToWorld[0][0] = right.x;
  camToWorld[0][1] = right.y;
  camToWorld[0][2] = right.z;
  camToWorld[1][0] = up.x;
  camToWorld[1][1] = up.y;
  camToWorld[1][2] = up.z;
  camToWorld[2][0] = forward.x;
  camToWorld[2][1] = forward.y;
  camToWorld[2][2] = forward.z;

  camToWorld[3][0] = from.x;
  camToWorld[3][1] = from.y;
  camToWorld[3][2] = from.z;

  return camToWorld;
}

template <class Numeric>
Numeric deg2rad(Numeric deg)
{
  return static_cast<Numeric>(deg * M_PI / 180);
}

template <class Numeric>
float clamp(Numeric lo, Numeric hi, Numeric v)
{
  return std::max(lo, std::min(hi, v));
}

#endif
