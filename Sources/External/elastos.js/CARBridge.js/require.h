#ifndef __CAR_BRIDGE_REQUIRE_H
# define __CAR_BRIDGE_REQUIRE_H

# include <cstddef>
# include <cstdint>

# include <node.h>

# include <nan.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern ::v8::Local<::v8::Value> Require(char const *ecoPath,
        uint32_t major, uint32_t minor, uint32_t build, uint32_t revision,
        size_t nEntryIds, char const * const entryIds[]);

CAR_BRIDGE_NAMESPACE_END

#endif
