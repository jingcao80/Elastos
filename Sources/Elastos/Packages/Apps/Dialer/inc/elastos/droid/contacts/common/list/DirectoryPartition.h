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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_DIRECTORYPARTITION_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_DIRECTORYPARTITION_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/common/widget/CompositeCursorAdapter.h"

using Elastos::Droid::Common::Widget::CompositeCursorAdapter;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Model object for a {@link Directory} row.
 */
class DirectoryPartition
    : public CompositeCursorAdapter::Partition
    , public IDirectoryPartition
{
public:
    DirectoryPartition(
        /* [in] */ Boolean showIfEmpty,
        /* [in] */ Boolean hasHeader);

    CAR_INTERFACE_DECL()

    /**
     * Directory ID, see {@link Directory}.
     */
    CARAPI_(Int64) GetDirectoryId();

    CARAPI_(void) SetDirectoryId(
        /* [in] */ Int64 directoryId);

    /**
     * Directory type resolved from {@link Directory#PACKAGE_NAME} and
     * {@link Directory#TYPE_RESOURCE_ID};
     */
    CARAPI_(String) GetDirectoryType();

    CARAPI_(void) SetDirectoryType(
        /* [in] */ const String& directoryType);

    /**
     * See {@link Directory#DISPLAY_NAME}.
     */
    CARAPI_(String) GetDisplayName();

    CARAPI_(void) SetDisplayName(
        /* [in] */ const String& displayName);

    CARAPI_(Int32) GetStatus();

    CARAPI_(void) SetStatus(
        /* [in] */ Int32 status);

    CARAPI_(Boolean) IsLoading();

    /**
     * Returns true if this directory should be loaded before non-priority directories.
     */
    CARAPI_(Boolean) IsPriorityDirectory();

    CARAPI_(void) SetPriorityDirectory(
        /* [in] */ Boolean priorityDirectory);

    /**
     * Returns true if this directory supports photos.
     */
    CARAPI_(Boolean) IsPhotoSupported();

    CARAPI_(void) SetPhotoSupported(
        /* [in] */ Boolean flag);

    /**
     * Max number of results for this directory. Defaults to {@link #RESULT_LIMIT_DEFAULT} which
     * implies using the adapter's
     * {@link com.android.contacts.common.list.ContactListAdapter#getDirectoryResultLimit()}
     */
    CARAPI_(Int32) GetResultLimit();

    CARAPI_(void) SetResultLimit(
        /* [in] */ Int32 resultLimit);

    /**
     * Used by extended directories to specify a custom content URI. Extended directories MUST have
     * a content URI
     */
    CARAPI_(String) GetContentUri();

    CARAPI_(void) SetContentUri(
        /* [in] */ const String& contentUri);

    /**
     * A label to display in the header next to the display name.
     */
    CARAPI_(String) GetLabel();

    CARAPI_(void) SetLabel(
        /* [in] */ const String& label);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Whether or not to display the phone number in app that have that option - Dialer. If false,
     * Phone Label should be used instead of Phone Number.
     */
    CARAPI_(Boolean) IsDisplayNumber();

    CARAPI_(void) SetDisplayNumber(
        /* [in] */ Boolean displayNumber);

private:
    Int64 mDirectoryId;
    String mContentUri;
    String mDirectoryType;
    String mDisplayName;
    Int32 mStatus;
    Boolean mPriorityDirectory;
    Boolean mPhotoSupported;
    Int32 mResultLimit;
    Boolean mDisplayNumber;

    String mLabel;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_DIRECTORYPARTITION_H__
