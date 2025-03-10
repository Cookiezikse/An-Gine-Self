#pragma once

#include <UDP/Types.h>

#include <iostream>
#ifdef _MSC_VER
#define DEBUGBREAK() __debugbreak()
#else
// TODO
#define DEBUGBREAK() 
#endif

#define TO_STRING_INTERNAL(...) #__VA_ARGS__
#define TO_STRING(...) TO_STRING_INTERNAL(__VA_ARGS__)
#define CHECK_SUCCESS(expr) do { /*std::cout << "[OK] " #expr << std::endl;*/ } while(0)
#define CHECK_FAILURE(expr) do { std::cout << "[FAILURE][" __FILE__ ":" TO_STRING(__LINE__) "] " #expr << std::endl; DEBUGBREAK(); } while(0)
// variadic arguments but is supposed to be used with 1 single check. This to prevent issues "too many arguments" when using multiple template parameters hence using a , ...
#define CHECK(...) do { if ((__VA_ARGS__)) { CHECK_SUCCESS((__VA_ARGS__)); } else { CHECK_FAILURE((__VA_ARGS__)); } } while(0)
#define STATIC_CHECK(...) static_assert(__VA_ARGS__, TO_STRING(__VA_ARGS__))

#define CHECK_FLOATS_EQUAL(type, a, b, nbdecimals) CHECK(std::abs((a) - (b)) <= 1. / Bousk::Pow<10, nbdecimals + 1>::Value)

