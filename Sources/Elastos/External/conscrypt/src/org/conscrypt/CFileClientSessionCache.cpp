#include "org/conscrypt/CFileClientSessionCache.h"
#include "org/conscrypt/FileClientSessionCache.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CFileClientSessionCache)

CAR_INTERFACE_IMPL(CFileClientSessionCache, Singleton, IFileClientSessionCache)

ECode CFileClientSessionCache::UsingDirectory(
    /* [in] */ IFile* directory,
    /* [out] */ ISSLClientSessionCache** result)
{
    return FileClientSessionCache::UsingDirectory(directory, result);
}

ECode CFileClientSessionCache::Reset()
{
    return FileClientSessionCache::Reset();
}

} // namespace Conscrypt
} // namespace Org
