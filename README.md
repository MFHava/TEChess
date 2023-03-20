# Type-erased Chess

© Michael Florian Hava


# Introduction
Proof of concept implementation of chess based on type-erasure.

C++ type-erasure is an **alternative representation of polymorphism** that does not rely on inheritance and virtual member functions.
Instead it is based on the observation every functor can be split into representation and behavior:
```
struct functor {
  int operator()();
};

functor func;

void * rep = std::addressof(func);
auto beh = +[](void * rep) { return (*reinterpret_cast<functor *>(rep))(); };

assert(func() == beh(rep));
```

Based on this observation it is possible to construct generic wrappers to represent independent classes with a common interface.
Contrary to inheritance based designs, type-erasure allows **polymorphic** behavior with **value types**.

Examples from the C++ standard library include:
 * `std::any`
 * `std::function`
 * `std::function_ref`
 * `std::move_only_function`
 * `std::copyable_function`


# Requirements
 * C++20
 * CMake
 * Catch2 (for unit tests only)
