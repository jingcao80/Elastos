
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "macros.h"

#include "parse-uri.h"

#include "error.h"



CAR_BRIDGE_NAMESPACE_BEGIN

static char const *_Trim(char const *s)
{
    return nullptr;
}

ParseURI::ParseURI(char const *uri)
{
}

char const *ParseURI::ecoPath() const noexcept
{
    return nullptr;
}

uint32_t ParseURI::major() const noexcept
{
    return 0;
}

uint32_t ParseURI::minor() const noexcept
{
    return 0;
}

uint32_t ParseURI::build() const noexcept
{
    return 0;
}

uint32_t ParseURI::revision() const noexcept
{
    return 0;
}

size_t ParseURI::nEntryIds() const noexcept
{
    return 0;
}

char const **ParseURI::entryIds() const noexcept
{
    return nullptr;
}

CAR_BRIDGE_NAMESPACE_END

