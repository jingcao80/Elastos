
#include <stddef.h>

#include "new.h"



NoThrow const NO_THROW;

void *operator new(size_t size, NoThrow const &noThrow) noexcept
{
    try {
        return operator new(size);
    } catch (...) {
        return nullptr;
    }
}

void operator delete(void *pointer, NoThrow const &noThrow) noexcept
{
    operator delete(pointer);
}

void *operator new [](size_t size, NoThrow const &noThrow) noexcept
{
    try {
        return operator new [](size);
    } catch (...) {
        return nullptr;
    }
}

void operator delete [](void *pointer, NoThrow const &noThrow) noexcept
{
    operator delete [](pointer);
}

