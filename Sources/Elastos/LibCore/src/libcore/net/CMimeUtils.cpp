#include "CMimeUtils.h"
#include "MimeUtils.h"

namespace Libcore {
namespace Net {

CAR_INTERFACE_IMPL(CMimeUtils, Singleton, IMimeUtils)

CAR_SINGLETON_IMPL(CMimeUtils)

ECode CMimeUtils::HasMineType(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean *result)
{
    return MimeUtils::HasMineType(mimeType, result);
}

ECode CMimeUtils::GuessMimeTypeFromExtension(
    /* [in] */ const String& extension,
    /* [out] */ String* result)
{
    return MimeUtils::GuessMimeTypeFromExtension(extension, result);
}

ECode CMimeUtils::HasExtension(
    /* [in] */ const String& extension,
    /* [out] */ Boolean* result)
{
    return MimeUtils::HasExtension(extension, result);
}

ECode CMimeUtils::GuessExtensionFromMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ String* result)
{
    return MimeUtils::GuessExtensionFromMimeType(mimeType, result);
}

} // namespace Net
} // namespace Libcore