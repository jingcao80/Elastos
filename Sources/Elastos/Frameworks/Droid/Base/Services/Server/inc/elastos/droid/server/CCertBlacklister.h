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

#ifndef __ELASTOS_DROID_SERVER_CCERTBLACKLISTER_H__
#define __ELASTOS_DROID_SERVER_CCERTBLACKLISTER_H__

#include "_Elastos_Droid_Server_CCertBlacklister.h"
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/core/Thread.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::Binder;
using Elastos::Core::Thread;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CCertBlacklister)
    , public Binder
{
private:
    class BlacklistObserver
        : public ContentObserver
    {
    private:
        class WriteBlacklistThread
            : public Thread
        {
        public:
            WriteBlacklistThread(
                /* [in] */ BlacklistObserver* host);

            CARAPI Run();
        private:
            BlacklistObserver* mHost;
        };

    public:
        TO_STRING_IMPL("CCertBlacklister::BlacklistObserver")

        BlacklistObserver(
            /* [in] */ const String& key,
            /* [in] */ const String& name,
            /* [in] */ const String& path,
            /* [in] */ IContentResolver* cr);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI_(String) GetValue();

    private:
        CARAPI_(void) WriteBlacklist();

    private:
        String mKey;
        String mName;
        String mPath;
        AutoPtr<IFile> mTmpDir;
        AutoPtr<IContentResolver> mContentResolver;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

private:
    AutoPtr<IContentObserver> BuildPubkeyObserver(
        /* [in] */ IContentResolver* cr);

    AutoPtr<IContentObserver> BuildSerialObserver(
        /* [in] */ IContentResolver* cr);

    void RegisterObservers(
        /* [in] */ IContentResolver* cr);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    static const String TAG;
    static const String BLACKLIST_ROOT;

public:
    static const String PUBKEY_PATH;
    static const String SERIAL_PATH;

    static const String PUBKEY_BLACKLIST_KEY;
    static const String SERIAL_BLACKLIST_KEY;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CCERTBLACKLISTER_H__
