
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "macros.h"

#include "libc-ext.h"

#include "parse-uri.h"

#include "error.h"
#include "unique-ptr.h"
#include "vector.h"



CAR_BRIDGE_NAMESPACE_BEGIN

static char const *_Trim(char const *s)
{
    return nullptr;
}

ParseURI::ParseURI(char const *uri)
{
    _buf = strdup(uri);
    if (_buf == nullptr)
        throw Error(Error::NO_MEMORY, "");

}

char const *ParseURI::ecoPath() const noexcept
{
    return _ecoPath;
}

char const *ParseURI::version() const noexcept
{
    return _version;
}

uint32_t ParseURI::major() const noexcept
{
    return _major;
}

uint32_t ParseURI::minor() const noexcept
{
    return _minor;
}

uint32_t ParseURI::build() const noexcept
{
    return _build;
}

uint32_t ParseURI::revision() const noexcept
{
    return _revision;
}

size_t ParseURI::nEntryIds() const noexcept
{
    return _entryIds.Size();
}

char const * const *ParseURI::entryIds() const noexcept
{
    return _entryIds.Data();
}

CAR_BRIDGE_NAMESPACE_END

