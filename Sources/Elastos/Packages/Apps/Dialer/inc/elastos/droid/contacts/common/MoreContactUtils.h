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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_MORECONTACTNUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_MORECONTACTNUTILS_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Contacts::Common::Model::Account::IAccountType;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Shared static contact utility methods.
 */
class MoreContactUtils
{
public:
    /**
     * Returns true if two data with mimetypes which represent values in contact entries are
     * considered equal for collapsing in the GUI. For caller-id, use
     * {@link android.telephony.PhoneNumberUtils#compare(android.content.Context, String, String)}
     * instead
     */
    static CARAPI_(Boolean) ShouldCollapse(
        /* [in] */ ICharSequence* mimetype1,
        /* [in] */ ICharSequence* data1,
        /* [in] */ ICharSequence* mimetype2,
        /* [in] */ ICharSequence* data2);

    /**
     * Returns the {@link android.graphics.Rect} with left, top, right, and bottom coordinates
     * that are equivalent to the given {@link android.view.View}'s bounds. This is equivalent to
     * how the target {@link android.graphics.Rect} is calculated in
     * {@link android.provider.ContactsContract.QuickContact#showQuickContact}.
     */
    static CARAPI_(AutoPtr<IRect>) GetTargetRectFromView(
        /* [in] */ IView* view);

    /**
     * Returns a header view based on the R.layout.list_separator, where the
     * containing {@link android.widget.TextView} is set using the given textResourceId.
     */
    static CARAPI_(AutoPtr<ITextView>) CreateHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 textResourceId);

    /**
     * Set the top padding on the header view dynamically, based on whether the header is in
     * the first row or not.
     */
    static CARAPI_(void) SetHeaderViewBottomPadding(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView,
        /* [in] */ Boolean isFirstRow);

    /**
     * Returns the intent to launch for the given invitable account type and contact lookup URI.
     * This will return null if the account type is not invitable (i.e. there is no
     * {@link AccountType#getInviteContactActivityClassName()} or
     * {@link AccountType#syncAdapterPackageName}).
     */
    static CARAPI_(AutoPtr<IIntent>) GetInvitableIntent(
        /* [in] */ IAccountType* accountType,
        /* [in] */ IUri* lookupUri);

private:
    // TODO: Move this to PhoneDataItem.shouldCollapse override
    static CARAPI_(Boolean) ShouldCollapsePhoneNumbers(
        /* [in] */ const String& number1,
        /* [in] */ const String& number2);

private:
    static const String WAIT_SYMBOL_AS_STRING;
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_MORECONTACTNUTILS_H__
