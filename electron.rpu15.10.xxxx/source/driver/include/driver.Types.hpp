/** 
 * Driver types.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_TYPES_HPP_
#define DRIVER_TYPES_HPP_

#ifdef NULL
#undef NULL
#endif

// NULL definition
#if __cplusplus >= 201103L  // Since C++11
  typedef decltype(nullptr)  nullptr_t;
  const nullptr_t NULL = nullptr;
#elif __cplusplus           // Until C++11
//const int NULL = 0;
  #define NULL 0
#else
  #error "The source file must be compiled by C++ compiler"
#endif

// LP32 or 2/4/4 (int is 16-bit, long and pointer are 32-bit)
typedef signed   int        int16;
typedef unsigned int        uint16;
typedef signed   long       int32;
typedef unsigned long       uint32;
typedef signed   long long  int64;
typedef unsigned long long  uint64;
// Floating point types
typedef float               float32;
typedef long double         float64;
// The type returned by sizeof
typedef uint32              size_t;

#endif // DRIVER_TYPES_HPP_
