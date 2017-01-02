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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTADAPTER_H__

#include "elastos/droid/contacts/common/list/ContactEntryListAdapter.h"

using Elastos::Core::ICharSequence;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A cursor adapter for the {@link ContactsContract.Contacts#CONTENT_TYPE} content type.
 * Also includes support for including the {@link ContactsContract.Profile} record in the
 * list.
 */
class ContactListAdapter
    : public ContactEntryListAdapter
    , public IContactListAdapter
{
protected:
    class ContactQuery
    {
    public:
        static const Int32 CONTACT_ID               = 0;
        static const Int32 CONTACT_DISPLAY_NAME     = 1;
        static const Int32 CONTACT_PRESENCE_STATUS  = 2;
        static const Int32 CONTACT_CONTACT_STATUS   = 3;
        static const Int32 CONTACT_PHOTO_ID         = 4;
        static const Int32 CONTACT_PHOTO_URI        = 5;
        static const Int32 CONTACT_LOOKUP_KEY       = 6;
        static const Int32 CONTACT_IS_USER_PROFILE  = 7;
        static const Int32 CONTACT_SNIPPET          = 8;

    private:
        static const AutoPtr<ArrayOf<String> > CONTACT_PROJECTION_PRIMARY;
        static const AutoPtr<ArrayOf<String> > CONTACT_PROJECTION_ALTERNATIVE;
        static const AutoPtr<ArrayOf<String> > FILTER_PROJECTION_PRIMARY;
        static const AutoPtr<ArrayOf<String> > FILTER_PROJECTION_ALTERNATIVE;

        friend class ContactListAdapter;
    };

public:
    CAR_INTERFACE_DECL()

    ContactListAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(void) SetPhotoPosition(
        /* [in] */ PhotoPosition photoPosition);

    CARAPI_(PhotoPosition) GetPhotoPosition();

    CARAPI_(AutoPtr<ICharSequence>) GetUnknownNameText();

    CARAPI_(Int64) GetSelectedContactDirectoryId();

    CARAPI_(String) GetSelectedContactLookupKey();

    CARAPI_(Int64) GetSelectedContactId();

    CARAPI_(void) SetSelectedContact(
        /* [in] */ Int64 selectedDirectoryId,
        /* [in] */ const String& lookupKey,
        /* [in] */ Int64 contactId);

    // @Override
    CARAPI GetContactDisplayName(
        /* [in] */ Int32 position,
        /* [out] */ String* displayName);

    /**
     * Builds the {@link Contacts#CONTENT_LOOKUP_URI} for the given
     * {@link ListView} position.
     */
    CARAPI_(AutoPtr<IUri>) GetContactUri(
        /* [in] */ Int32 position);

    CARAPI_(AutoPtr<IUri>) GetContactUri(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor);

    /**
     * Returns true if the specified contact is selected in the list. For a
     * contact to be shown as selected, we need both the directory and and the
     * lookup key to be the same. We are paying no attention to the contactId,
     * because it is volatile, especially in the case of directories.
     */
    CARAPI_(Boolean) IsSelectedContact(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor);

    CARAPI_(Int32) GetSelectedContactPosition();

    CARAPI_(Boolean) HasValidSelection();

    CARAPI_(AutoPtr<IUri>) GetFirstContactUri();

    // @Override
    CARAPI ChangeCursor(
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor);

protected:
    static CARAPI_(AutoPtr<IUri>) BuildSectionIndexerUri(
        /* [in] */ IUri* uri);

    // @Override
    CARAPI_(AutoPtr<IView>) NewView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position,
        /* [in] */ IViewGroup* parent);

    CARAPI_(void) BindSectionHeaderAndDivider(
        /* [in] */ IContactListItemView* view,
        /* [in] */ Int32 position,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) BindPhoto(
        /* [in] */ IContactListItemView* view,
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) BindNameAndViewId(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) BindPresenceAndStatusMessage(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) BindSearchSnippet(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

    /**
     * @return Projection useful for children.
     */
    CARAPI_(AutoPtr<ArrayOf<String> >) GetProjection(
        /* [in] */ Boolean forSearch);

private:
    AutoPtr<ICharSequence> mUnknownNameText;

    Int64 mSelectedContactDirectoryId;
    String mSelectedContactLookupKey;
    Int64 mSelectedContactId;
    PhotoPosition mPhotoPosition;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTADAPTER_H__
