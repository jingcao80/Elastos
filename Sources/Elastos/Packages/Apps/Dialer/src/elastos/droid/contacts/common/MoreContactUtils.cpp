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

#include "elastos/droid/contacts/common/MoreContactUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/View.h"
#include <elastos/core/StringUtils.h>
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

static String InitString()
{
    String str("");
    str.Append(IPhoneNumberUtils::WAIT);
    return str;
}
const String WAIT_SYMBOL_AS_STRING = InitString();

Boolean MoreContactUtils::ShouldCollapse(
    /* [in] */ ICharSequence* mimetype1,
    /* [in] */ ICharSequence* data1,
    /* [in] */ ICharSequence* mimetype2,
    /* [in] */ ICharSequence* data2)
{
    // different mimetypes? don't collapse
    if (!TextUtils::Equals(mimetype1, mimetype2)) return FALSE;

    // exact same string? good, bail out early
    if (TextUtils::Equals(data1, data2)) return TRUE;

    // so if either is null, these two must be different
    if (data1 == NULL || data2 == NULL) return FALSE;

    // if this is not about phone numbers, we know this is not a match (of course, some
    // mimetypes could have more sophisticated matching is the future, e.g. addresses)
    String str;
    mimetype1->ToString(&str);
    if (!TextUtils::Equals(IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE,
            str)) {
        return FALSE;
    }

    String str1, str2;
    data1->ToString(&str1);
    data2->ToString(&str2);
    return ShouldCollapsePhoneNumbers(str1, str2);
}

Boolean MoreContactUtils::ShouldCollapsePhoneNumbers(
    /* [in] */ const String& number1,
    /* [in] */ const String& number2)
{
    // Now do the full phone number thing. split into parts, separated by waiting symbol
    // and compare them individually
    AutoPtr<ArrayOf<String> > dataParts1;
    StringUtils::Split(number1, WAIT_SYMBOL_AS_STRING, (ArrayOf<String>**)&dataParts1);
    AutoPtr<ArrayOf<String> > dataParts2;
    StringUtils::Split(number2, WAIT_SYMBOL_AS_STRING, (ArrayOf<String>**)&dataParts2);
    if (dataParts1->GetLength() != dataParts2->GetLength()) return FALSE;
    // TODO:
    // final PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    for (Int32 i = 0; i < dataParts1->GetLength(); i++) {
        // Match phone numbers represented by keypad letters, in which case prefer the
        // phone number with letters.
        AutoPtr<IPhoneNumberUtils> utils;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
        String dataPart1;
        utils->ConvertKeypadLettersToDigits((*dataParts1)[i], &dataPart1);
        String dataPart2 = (*dataParts2)[i];

        // substrings equal? shortcut, don't parse
        if (TextUtils::Equals(dataPart1, dataPart2)) continue;

        // do a full parse of the numbers
        // TODO:
        // final PhoneNumberUtil.MatchType result = util.isNumberMatch(dataPart1, dataPart2);
        // switch (result) {
        //     case NOT_A_NUMBER:
        //         // don't understand the numbers? let's play it safe
        //         return false;
        //     case NO_MATCH:
        //         return false;
        //     case EXACT_MATCH:
        //         break;
        //     case NSN_MATCH:
        //         try {
        //             // For NANP phone numbers, match when one has +1 and the other does not.
        //             // In this case, prefer the +1 version.
        //             if (util.parse(dataPart1, null).getCountryCode() == 1) {
        //                 // At this point, the numbers can be either case 1 or 2 below....
        //                 //
        //                 // case 1)
        //                 // +14155551212    <--- country code 1
        //                 //  14155551212    <--- 1 is trunk prefix, not country code
        //                 //
        //                 // and
        //                 //
        //                 // case 2)
        //                 // +14155551212
        //                 //   4155551212
        //                 //
        //                 // From b/7519057, case 2 needs to be equal.  But also that bug, case 3
        //                 // below should not be equal.
        //                 //
        //                 // case 3)
        //                 // 14155551212
        //                 //  4155551212
        //                 //
        //                 // So in order to make sure transitive equality is valid, case 1 cannot
        //                 // be equal.  Otherwise, transitive equality breaks and the following
        //                 // would all be collapsed:
        //                 //   4155551212  |
        //                 //  14155551212  |---->   +14155551212
        //                 // +14155551212  |
        //                 //
        //                 // With transitive equality, the collapsed values should be:
        //                 //   4155551212  |         14155551212
        //                 //  14155551212  |---->   +14155551212
        //                 // +14155551212  |

        //                 // Distinguish between case 1 and 2 by checking for trunk prefix '1'
        //                 // at the start of number 2.
        //                 if (dataPart2.trim().charAt(0) == '1') {
        //                     // case 1
        //                     return false;
        //                 }
        //                 break;
        //             }
        //         } catch (NumberParseException e) {
        //             // This is the case where the first number does not have a country code.
        //             // examples:
        //             // (123) 456-7890   &   123-456-7890  (collapse)
        //             // 0049 (8092) 1234   &   +49/80921234  (unit test says do not collapse)

        //             // Check the second number.  If it also does not have a country code, then
        //             // we should collapse.  If it has a country code, then it's a different
        //             // number and we should not collapse (this conclusion is based on an
        //             // existing unit test).
        //             try {
        //                 util.parse(dataPart2, null);
        //             } catch (NumberParseException e2) {
        //                 // Number 2 also does not have a country.  Collapse.
        //                 break;
        //             }
        //         }
        //         return false;
        //     case SHORT_NSN_MATCH:
        //         return false;
        //     default:
        //         throw new IllegalStateException("Unknown result value from phone number " +
        //                 "library");
        // }
    }
    return TRUE;
}

AutoPtr<IRect> MoreContactUtils::GetTargetRectFromView(
    /* [in] */ IView* view)
{
    AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
    view->GetLocationOnScreen(pos);

    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    rect->SetLeft((*pos)[0]);
    rect->SetTop((*pos)[1]);
    Int32 width, heigth;
    view->GetWidth(&width);
    rect->SetRight((*pos)[0] + width);
    view->GetHeight(&heigth);
    rect->SetBottom((*pos)[1] + heigth);
    return rect;
}

AutoPtr<ITextView> MoreContactUtils::CreateHeaderView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textResourceId)
{
    AutoPtr<IView> textView;
    Elastos::Droid::View::View::Inflate(context, Elastos::Droid::Dialer::R::layout::list_separator, NULL, (IView**)&textView);
    String str;
    context->GetString(textResourceId, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    AutoPtr<ITextView> tv = ITextView::Probe(textView);
    tv->SetText(cs);
    return tv;
}

void MoreContactUtils::SetHeaderViewBottomPadding(
    /* [in] */ IContext* context,
    /* [in] */ ITextView* textView,
    /* [in] */ Boolean isFirstRow)
{
    Float fval = 0;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    if (isFirstRow) {
        resources->GetDimension(
                Elastos::Droid::Dialer::R::dimen::frequently_contacted_title_top_margin_when_first_row, &fval);
    }
    else {
        resources->GetDimension(
                Elastos::Droid::Dialer::R::dimen::frequently_contacted_title_top_margin, &fval);
    }

    Int32 topPadding = fval;
    AutoPtr<IView> view = IView::Probe(textView);
    Int32 start, end, bottom;
    view->GetPaddingStart(&start);
    view->GetPaddingEnd(&end);
    view->GetPaddingBottom(&bottom);
    view->SetPaddingRelative(start, topPadding, end, bottom);
}

AutoPtr<IIntent> MoreContactUtils::GetInvitableIntent(
    /* [in] */ IAccountType* accountType,
    /* [in] */ IUri* lookupUri)
{
    assert(0);
    return NULL;
    // String syncAdapterPackageName = ((AccountType*)accountType)->mSyncAdapterPackageName;
    // String className;
    // accountType->GetInviteContactActivityClassName(&className);
    // if (TextUtils::IsEmpty(syncAdapterPackageName) || TextUtils::IsEmpty(className)) {
    //     return NULL;
    // }
    // AutoPtr<IIntent> intent;
    // CIntent::New((IIntent**)&intent);
    // intent->SetClassName(syncAdapterPackageName, className);

    // intent->SetAction(IContactsContractIntents::INVITE_CONTACT);

    // // Data is the lookup URI.
    // intent->SetData(lookupUri);
    // return intent;
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
