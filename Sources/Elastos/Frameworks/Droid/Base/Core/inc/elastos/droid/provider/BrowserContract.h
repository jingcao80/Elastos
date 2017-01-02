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

#ifndef __ELASTOS_DROID_PROVIDER_BROWSERCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_BROWSERCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IPair;

namespace Elastos {
namespace Droid {
namespace Provider {

class BrowserContract
{
public:
    class ChromeSyncColumns
    {
    private:
        ChromeSyncColumns() {}

    public:
        static const String SERVER_UNIQUE;
        static const String CLIENT_UNIQUE;
    };

    class Bookmarks
    {
    public:
        static CARAPI BuildFolderUri(
            /* [in] */ Int64 folderId,
            /* [out] */ IUri** uri);

    private:
        Bookmarks() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
        static const AutoPtr<IUri> CONTENT_URI_DEFAULT_FOLDER;
    };

    class Accounts
    {
    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class History
    {
    private:
        History() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class Searches
    {
    private:
        Searches() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class SyncState
    {
    public:
        static CARAPI Get(
            /* [in] */ IContentProviderClient* provider,
            /* [in] */ IAccount* account,
            /* [out, callee] */ ArrayOf<Byte>** value);

        static CARAPI GetWithUri(
            /* [in] */ IContentProviderClient* provider,
            /* [in] */ IAccount* account,
            /* [out] */ IPair** value);

        static CARAPI Set(
            /* [in] */ IContentProviderClient* provider,
            /* [in] */ IAccount* account,
            /* [in] */ ArrayOf<Byte>* data);

        static CARAPI NewSetOperation(
            /* [in] */ IAccount* account,
            /* [in] */ ArrayOf<Byte>* data,
            /* [out] */ IContentProviderOperation** operation);

    private:
        SyncState() {}

    public:
        static const String CONTENT_DIRECTORY;
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class Images
    {
    private:
        Images() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class ImageMappings
    {
    private:
        ImageMappings() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class Combined
    {
    private:
        Combined() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class Settings
    {
    private:
        Settings() {}

    public:
        static CARAPI IsSyncEnabled(
            /* [in] */ IContext* context,
            /* [out] */ Boolean* res);

        static CARAPI SetSyncEnabled(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled);

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

public:
    static const AutoPtr<IUri> AUTHORITY_URI;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_BROWSERCONTRACT_H__