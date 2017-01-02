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

#include "elastos/droid/text/LoginFilter.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {

/*****************************LoginFilter*****************************/
CAR_INTERFACE_IMPL_2(LoginFilter, Object, ILoginFilter, IInputFilter)

LoginFilter::LoginFilter()
    : mAppendInvalid(FALSE)
{
}

LoginFilter::~LoginFilter()
{
}

ECode LoginFilter::constructor()
{
    return NOERROR;
}

ECode LoginFilter::constructor(
    /* [in] */ Boolean appendInvalid)
{
    mAppendInvalid = appendInvalid;
    return NOERROR;
}

ECode LoginFilter::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret)
    OnStart();

    ICharSequence* d = ICharSequence::Probe(dest);
    // Scan through beginning characters in dest, calling onInvalidCharacter()
    // for each invalid character.
    Char32 c;
    for (Int32 i = 0; i < dstart; i++) {
        d->GetCharAt(i, &c);
        Boolean bAllowed;
        if (!(IsAllowed(c, &bAllowed), bAllowed)) {
            OnInvalidCharacter(c);
        }
    }

    // Scan through changed characters rejecting disallowed chars
    AutoPtr<ISpannableStringBuilder> modification;
    IEditable* editable = NULL;
    Int32 modoff = 0;

    for (Int32 i = start; i < end; i++) {
        source->GetCharAt(i, &c);
        Boolean bAllowed;
        if ((IsAllowed(c, &bAllowed), bAllowed)) {
            // Character allowed.
            modoff++;
        }
        else {
            if (mAppendInvalid) {
                modoff++;
            }
            else {
                if (modification == NULL) {
                    CSpannableStringBuilder::New(source, start, end, (ISpannableStringBuilder**)&modification);
                    modoff = i - start;
                    editable = IEditable::Probe(modification);
                }

                editable->Delete(modoff, modoff + 1);
            }

            OnInvalidCharacter(c);
        }
    }

    // Scan through remaining characters in dest, calling onInvalidCharacter()
    // for each invalid character.
    Int32 cLen;
    for (Int32 i = dend; i < (d->GetLength(&cLen), cLen); i++) {
        d->GetCharAt(i, &c);
        Boolean bAllowed;
        if (!(IsAllowed(c, &bAllowed), bAllowed)) {
            OnInvalidCharacter(c);
        }
    }

    OnStop();

    // Either returns null if we made no changes,
    // or what we wanted to change it to if there were changes.
    *ret = ICharSequence::Probe(modification);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode LoginFilter::OnStart()
{
    return NOERROR;
}

ECode LoginFilter::OnInvalidCharacter(
    /* [in] */ Char32 c)
{
    return NOERROR;
}

ECode LoginFilter::OnStop()
{
    return NOERROR;
}

/*****************************UsernameFilterGMail*****************************/
CAR_INTERFACE_IMPL(UsernameFilterGMail, LoginFilter, IUsernameFilterGMail)

ECode UsernameFilterGMail::IsAllowed(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)

    // Allow [a-zA-Z0-9@.]
    if ('0' <= c && c <= '9') {
        *ret = TRUE;
        return NOERROR;
    }
    if ('a' <= c && c <= 'z') {
        *ret = TRUE;
        return NOERROR;
    }
    if ('A' <= c && c <= 'Z') {
        *ret = TRUE;
        return NOERROR;
    }
    if ('.' == c) {
        *ret = TRUE;
        return NOERROR;
    }
    *ret = FALSE;
    return NOERROR;
}

/*****************************UsernameFilterGeneric*****************************/
String UsernameFilterGeneric::mAllowed("@_-+."); // Additional characters

CAR_INTERFACE_IMPL(UsernameFilterGeneric, LoginFilter, IUsernameFilterGeneric)

ECode UsernameFilterGeneric::IsAllowed(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)

    // Allow [a-zA-Z0-9@.]
    if ('0' <= c && c <= '9') {
        *ret = TRUE;
        return NOERROR;
    }
    if ('a' <= c && c <= 'z') {
        *ret = TRUE;
        return NOERROR;
    }
    if ('A' <= c && c <= 'Z') {
        *ret = TRUE;
        return NOERROR;
    }
    if ((String(mAllowed).IndexOf(c)) != -1) {
        *ret = TRUE;
        return NOERROR;
    }
    *ret = FALSE;
    return NOERROR;
}

/*****************************PasswordFilterGMail*****************************/
CAR_INTERFACE_IMPL(PasswordFilterGMail, LoginFilter, IPasswordFilterGMail)

ECode PasswordFilterGMail::IsAllowed(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)

    if (32 <= c && c <= 127) {
        *ret = TRUE; // standard charset
        return NOERROR;
    }
    // if (128 <= c && c <= 159) return true;  // nonstandard (Windows(TM)(R)) charset
    if (160 <= c && c <= 255) {
        *ret = TRUE; // extended charset
        return NOERROR;
    }
    *ret = FALSE;
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos