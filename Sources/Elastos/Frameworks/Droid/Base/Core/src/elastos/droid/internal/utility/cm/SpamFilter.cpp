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

#include "elastos/droid/internal/utility/cm/SpamFilter.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

ECode SpamFilter::GetUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUriBuilder> uriBuilder;
    CUriBuilder::New((IUriBuilder**)&uriBuilder);
    uriBuilder->Scheme(IContentResolver::SCHEME_CONTENT);
    uriBuilder->Authority(ISpamFilter::AUTHORITY);
    return uriBuilder->Build(result);
}

ECode SpamFilter::GetNormalizedContent(
    /* [in] */ const String& msg,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringUtils::ReplaceAll(msg.ToLowerCase(),
            String("[^\\p{L}\\p{Nd}]+"), String(""), result);
    return NOERROR;
}

ECode SpamFilter::GetNotificationContent(
    /* [in] */ INotification* notification,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBundle> extras;
    notification->GetExtras((IBundle**)&extras);
    Boolean b;
    extras->ContainsKey(INotification::EXTRA_TITLE_BIG, &b);
    String titleExtra = b ? INotification::EXTRA_TITLE_BIG : INotification::EXTRA_TITLE;
    AutoPtr<ICharSequence> notificationTitle;
    extras->GetCharSequence(titleExtra, (ICharSequence**)&notificationTitle);
    AutoPtr<ICharSequence> notificationMessage;
    extras->GetCharSequence(INotification::EXTRA_TEXT, (ICharSequence**)&notificationMessage);

    if (TextUtils::IsEmpty(notificationMessage)) {
        AutoPtr<ArrayOf<ICharSequence*> > inboxLines;
        extras->GetCharSequenceArray(INotification::EXTRA_TEXT_LINES,
                (ArrayOf<ICharSequence*>**)&inboxLines);
        if (inboxLines == NULL || inboxLines->GetLength() == 0) {
            notificationMessage = NULL;
            CString::New(String(""), (ICharSequence**)&notificationMessage);
        }
        else {
            AutoPtr<ICharSequence> tmpCS;
            CString::New(String("\n"), (ICharSequence**)&tmpCS);
            AutoPtr<ArrayOf<IInterface*> > array =
                    ArrayOf<IInterface*>::Alloc(inboxLines->GetLength());
            for (Int32 i = 0; i < inboxLines->GetLength(); i++) {
                array->Set(i, (*inboxLines)[i]);
            }
            String tmpStr = TextUtils::Join(tmpCS, array);
            notificationMessage = NULL;
            CString::New(tmpStr, (ICharSequence**)&notificationMessage);
        }
    }
    String str1, str2;
    if (notificationTitle != NULL)
        notificationTitle->ToString(&str1);
    if (notificationMessage != NULL)
        notificationMessage->ToString(&str2);
    *result = str1 + "\n" + str2;
    return NOERROR;
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
