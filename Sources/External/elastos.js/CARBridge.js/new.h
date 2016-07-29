#ifndef __CAR_BRIDGE_NEW_H
# define __CAR_BRIDGE_NEW_H

# include <stddef.h>



class NoThrow {};

extern NoThrow const NO_THROW;

extern void *operator new(size_t size, NoThrow const &noThrow) noexcept;
extern void operator delete(void *pointer, NoThrow const &noThrow) noexcept;

extern void *operator new [](size_t size, NoThrow const &noThrow) noexcept;
extern void operator delete [](void *pointer, NoThrow const &noThrow) noexcept;

#endif
