//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
