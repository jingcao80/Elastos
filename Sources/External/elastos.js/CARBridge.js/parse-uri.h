#ifndef __CAR_BRIDGE_PARSE_URI_H
# define __CAR_BRIDGE_PARSE_URI_H

# include <cstddef>
# include <cstdint>

# include <memory>
# include <vector>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class ParseURI {
public:
    ParseURI(char const *uri);

    char const *ecoPath() const noexcept;

    char const *version() const noexcept;

    uint32_t major() const noexcept;

    uint32_t minor() const noexcept;

    uint32_t build() const noexcept;

    uint32_t revision() const noexcept;

    size_t nEntryIds() const noexcept;

    char const * const *entryIds() const noexcept;

private:
    ::std::unique_ptr<char> _buf;

    char const *_ecoPath;

    char const *_version;

    uint32_t _major, _minor, _build, _revision;

    ::std::vector<char const *> _entryIds;

    ParseURI(ParseURI const &parseURI) = delete;

    ParseURI(ParseURI &&parseURI) = delete;

    ParseURI &operator=(ParseURI const &parseURI) = delete;

    ParseURI &operator=(ParseURI &&parseURI) = delete;
};

CAR_BRIDGE_NAMESPACE_END

#endif
