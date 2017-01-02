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

#include "elastos/droid/dialer/list/DialerPhoneNumberListAdapter.h"
#include "elastos/droid/contacts/common/GeoUtil.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::GeoUtil;
using Elastos::Droid::Contacts::Common::List::ContactListItemView;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::List::EIID_IDialerPhoneNumberListAdapter;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CAR_INTERFACE_IMPL(DialerPhoneNumberListAdapter, PhoneNumberListAdapter, IDialerPhoneNumberListAdapter)

DialerPhoneNumberListAdapter::DialerPhoneNumberListAdapter()
{
    mShortcutEnabled = ArrayOf<Boolean>::Alloc(IDialerPhoneNumberListAdapter::SHORTCUT_COUNT);
}

ECode DialerPhoneNumberListAdapter::constructor(
    /* [in] */ IContext* context)
{
    PhoneNumberListAdapter::constructor(context);
    mCountryIso = GeoUtil::GetCurrentCountryIso(context);

    // Enable all shortcuts by default
    for (Int32 i = 0; i < mShortcutEnabled->GetLength(); i++) {
        (*mShortcutEnabled)[i] = TRUE;
    }
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    Int32 superCount, shortcutCount;
    PhoneNumberListAdapter::GetCount(&superCount);
    GetShortcutCount(&shortcutCount);
    *count = superCount + shortcutCount;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetShortcutCount(
    /* [out] */ Int32* shortcutCount)
{
    VALIDATE_NOT_NULL(shortcutCount)

    Int32 count = 0;
    for (Int32 i = 0; i < mShortcutEnabled->GetLength(); i++) {
        if ((*mShortcutEnabled)[i]) count++;
    }
    *shortcutCount = count;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    Int32 shortcut;
    GetShortcutTypeFromPosition(position, &shortcut);
    if (shortcut >= 0) {
        // shortcutPos should always range from 1 to SHORTCUT_COUNT
        Int32 count;
        PhoneNumberListAdapter::GetViewTypeCount(&count);
        *type = count + shortcut;
        return NOERROR;
    }
    else {
        return PhoneNumberListAdapter::GetItemViewType(position, type);
    }
}

ECode DialerPhoneNumberListAdapter::GetViewTypeCount(
    /* [in] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    // Number of item view types in the super implementation + 2 for the 2 new shortcuts
    Int32 viewTypeCount;
    PhoneNumberListAdapter::GetViewTypeCount(&viewTypeCount);
    *count = viewTypeCount + SHORTCUT_COUNT;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    Int32 shortcutType;
    GetShortcutTypeFromPosition(position, &shortcutType);
    if (shortcutType >= 0) {
        if (convertView != NULL) {
            AssignShortcutToView(IContactListItemView::Probe(convertView), shortcutType);
            *view = convertView;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
        else {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ContactListItemView> v = new ContactListItemView();
            v->constructor(context, NULL);
            AssignShortcutToView(IContactListItemView::Probe(v), shortcutType);
            *view = IView::Probe(v);
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    else {
        return PhoneNumberListAdapter::GetView(position, convertView, parent, view);
    }
}

ECode DialerPhoneNumberListAdapter::GetShortcutTypeFromPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    Int32 count;
    PhoneNumberListAdapter::GetCount(&count);
    Int32 shortcutCount = position - count;
    if (shortcutCount >= 0) {
        // Iterate through the array of shortcuts, looking only for shortcuts where
        // mShortcutEnabled[i] is true
        for (Int32 i = 0; shortcutCount >= 0 && i < mShortcutEnabled->GetLength(); i++) {
            if ((*mShortcutEnabled)[i]) {
                shortcutCount--;
                if (shortcutCount < 0) {
                    *type = i;
                    return NOERROR;
                }
            }
        }
        // throw new IllegalArgumentException("Invalid position - greater than cursor count "
        //         + " but not a shortcut.");
        Logger::E(String("DialerPhoneNumberListAdapter"), "Invalid position - greater than cursor count  but not a shortcut.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *type = IDialerPhoneNumberListAdapter::SHORTCUT_INVALID;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    GetShortcutCount(&count);
    Boolean isEmpty;
    *result = count == 0 && (PhoneNumberListAdapter::IsEmpty(&isEmpty), isEmpty);
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 shortcutType;
    GetShortcutTypeFromPosition(position, &shortcutType);
    if (shortcutType >= 0) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        return PhoneNumberListAdapter::IsEnabled(position, result);
    }
}

ECode DialerPhoneNumberListAdapter::AssignShortcutToView(
    /* [in] */ IContactListItemView* _v,
    /* [in] */ Int32 shortcutType)
{
    AutoPtr<ContactListItemView> v = (ContactListItemView*)_v;
    String text;
    Int32 drawableId;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String number;
    GetFormattedQueryString(&number);
    switch (shortcutType) {
        case SHORTCUT_DIRECT_CALL: {
            AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(1);
            AutoPtr<ICharSequence> cs;
            CString::New(number, (ICharSequence**)&cs);
            attrs->Set(0, cs);
            resources->GetString(Elastos::Droid::Dialer::R::string::search_shortcut_call_number,
                    attrs, &text);
            drawableId = Elastos::Droid::Dialer::R::drawable::ic_search_phone;
            break;
        }
        case SHORTCUT_ADD_NUMBER_TO_CONTACTS:
            resources->GetString(Elastos::Droid::Dialer::R::string::search_shortcut_add_to_contacts, &text);
            drawableId = Elastos::Droid::Dialer::R::drawable::ic_search_add_contact;
            break;
        case SHORTCUT_MAKE_VIDEO_CALL:
            resources->GetString(Elastos::Droid::Dialer::R::string::search_shortcut_make_video_call, &text);
            drawableId = Elastos::Droid::Dialer::R::drawable::ic_videocam;
            break;
        default:
            Logger::E(String("DialerPhoneNumberListAdapter"), "Invalid shortcut type");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Invalid shortcut type");
    }
    v->SetDrawableResource(Elastos::Droid::Dialer::R::drawable::search_shortcut_background, drawableId);
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    v->SetDisplayName(cs);
    Int32 position;
    PhoneNumberListAdapter::GetPhotoPosition(&position);
    v->SetPhotoPosition(position);
    v->SetAdjustSelectionBoundsEnabled(FALSE);

    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::SetShortcutEnabled(
    /* [in] */ Int32 shortcutType,
    /* [in] */ Boolean visible,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    Boolean changed = (*mShortcutEnabled)[shortcutType] != visible;
    (*mShortcutEnabled)[shortcutType] = visible;
    *enabled = changed;

    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetFormattedQueryString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mFormattedQueryString;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::SetQueryString(
    /* [in] */ const String& queryString)
{
    AutoPtr<IPhoneNumberUtils> utils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    String number;
    utils->NormalizeNumber(queryString, &number);
    utils->FormatNumber(number, mCountryIso, &mFormattedQueryString);
    return PhoneNumberListAdapter::SetQueryString(queryString);
}

} // List
} // Dialer
} // Droid
} // Elastos
