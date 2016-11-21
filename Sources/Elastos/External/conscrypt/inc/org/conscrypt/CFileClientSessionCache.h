
#ifndef __ORG_CONSCRYPT_CFILECLIENTSESSIONCACHE_H__
#define __ORG_CONSCRYPT_CFILECLIENTSESSIONCACHE_H__

#include "_Org_Conscrypt_CFileClientSessionCache.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Org {
namespace Conscrypt {

CarClass(CFileClientSessionCache)
    , public Singleton
    , public IFileClientSessionCache
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns a cache backed by the given directory. Creates the directory
     * (including parent directories) if necessary. This cache should have
     * exclusive access to the given directory.
     *
     * @param directory to store files in
     * @return a cache backed by the given directory
     * @throws IOException if the file exists and is not a directory or if
     *  creating the directories fails
     */
    CARAPI UsingDirectory(
        /* [in] */ IFile* directory,
        /* [out] */ ISSLClientSessionCache** result);

    /** For testing. */
    CARAPI Reset();
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CFILECLIENTSESSIONCACHE_H__
