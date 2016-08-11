#ifndef __CAR_BRIDGE_LIBC_EXT_H
# define __CAR_BRIDGE_LIBC_EXT_H

# include <cstddef>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern char *strdup(char const *s) noexcept;

extern char *strndup(char const *s, size_t n) noexcept;

CAR_BRIDGE_NAMESPACE_END

#endif
