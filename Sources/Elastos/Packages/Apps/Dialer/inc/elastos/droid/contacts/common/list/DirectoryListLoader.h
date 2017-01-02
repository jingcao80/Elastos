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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_DIRECTORYLISTLOADER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_DIRECTORYLISTLOADER_H__

#include "elastos/droid/content/AsyncTaskLoader.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::AsyncTaskLoader;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A specialized loader for the list of directories, see {@link Directory}.
 */
class DirectoryListLoader
    : public AsyncTaskLoader
{
private:
    class DirectoryQuery
    {
    public:
        static const AutoPtr<IUri> URI;
        static const String ORDER_BY;

        static const AutoPtr<ArrayOf<String> > PROJECTION;

        static const Int32 ID = 0;
        static const Int32 PACKAGE_NAME = 1;
        static const Int32 TYPE_RESOURCE_ID = 2;
        static const Int32 DISPLAY_NAME = 3;
        static const Int32 PHOTO_SUPPORT = 4;
    };

    class MyObserver : public ContentObserver
    {
    public:
        TO_STRING_IMPL("DirectoryListLoader::MyObserver")

        CARAPI constructor(
            /* [in] */ DirectoryListLoader* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        DirectoryListLoader* mHost;
    };

public:
    DirectoryListLoader(
        /* [in] */ IContext* ctx);

    CARAPI_(void) SetDirectorySearchMode(
        /* [in] */ Int32 mode);

    /**
     * A flag that indicates whether the {@link Directory#LOCAL_INVISIBLE} directory should
     * be included in the results.
     */
    CARAPI_(void) SetLocalInvisibleDirectoryEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI LoadInBackground(
        /* [out] */ IInterface** cursor);

protected:
    // @Override
    CARAPI OnStartLoading();

    // @Override
    CARAPI OnStopLoading();

    // @Override
    CARAPI OnReset();

private:
    CARAPI_(AutoPtr<ICursor>) GetDefaultDirectories();

public:
    static const Int32 SEARCH_MODE_NONE = 0;
    static const Int32 SEARCH_MODE_DEFAULT = 1;
    static const Int32 SEARCH_MODE_CONTACT_SHORTCUT = 2;
    static const Int32 SEARCH_MODE_DATA_SHORTCUT = 3;

    // This is a virtual column created for a MatrixCursor.
    static const String DIRECTORY_TYPE;

private:
    static const String TAG;

    static const AutoPtr<ArrayOf<String> > RESULT_PROJECTION;

    AutoPtr<MyObserver> mObserver;

    Int32 mDirectorySearchMode;
    Boolean mLocalInvisibleDirectoryEnabled;

    AutoPtr<IMatrixCursor> mDefaultDirectoryList;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_DIRECTORYLISTLOADER_H__
