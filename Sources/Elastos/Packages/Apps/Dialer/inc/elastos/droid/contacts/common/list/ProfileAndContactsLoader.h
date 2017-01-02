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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_PROFILEANDCONTACTSLOADER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_PROFILEANDCONTACTSLOADER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/content/CursorLoader.h"
#include "elastos/droid/database/MergeCursor.h"

using Elastos::Droid::Content::CursorLoader;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::MergeCursor;
using Elastos::Droid::Database::IMatrixCursor;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A loader for use in the default contact list, which will also query for the user's profile
 * if configured to do so.
 */
class ProfileAndContactsLoader
    : public CursorLoader
{
private:
    class MyMergeCursor
        : public MergeCursor
    {
    public:
        MyMergeCursor(
            /* [in] */ ICursor* contactsCursor)
            : mContactsCursor(contactsCursor)
        {}

        CARAPI constructor(
            /* [in] */ ArrayOf<ICursor*>* cursors);

        // @Override
        CARAPI GetExtras(
            /* [out] */ IBundle** extras);

    private:
        AutoPtr<ICursor> mContactsCursor;
    };

public:
    ProfileAndContactsLoader();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(void) SetLoadProfile(
        /* [in] */ Boolean flag);

    CARAPI SetProjection(
        /* [in] */ ArrayOf<String>* projection);

    // @Override
    CARAPI LoadInBackground(
        /* [out] */ IInterface** result);

private:
    /**
     * Loads the profile into a MatrixCursor. On failure returns null, which
     * matches the behavior of CursorLoader.loadInBackground().
     *
     * @return MatrixCursor containing profile or null on query failure.
     */
    CARAPI_(AutoPtr<IMatrixCursor>) LoadProfile();

private:
    Boolean mLoadProfile;
    AutoPtr<ArrayOf<String> > mProjection;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_PROFILEANDCONTACTSLOADER_H__
