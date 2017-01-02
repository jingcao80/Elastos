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

#include "elastos/droid/text/utility/CLinkify.h"
#include "elastos/droid/text/utility/Linkify.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CAR_INTERFACE_IMPL(CLinkify, Singleton, ILinkify)

CAR_SINGLETON_IMPL(CLinkify)

ECode CLinkify::GetStaticUrlMatchFilter(
    /* [out] */ ILinkifyMatchFilter** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ILinkifyMatchFilter> lmf = Linkify::GetStaticUrlMatchFilter();
    *ret = lmf;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CLinkify::GetStaticPhoneNumberMatchFilter(
    /* [out] */ ILinkifyMatchFilter** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ILinkifyMatchFilter> lmf = Linkify::GetStaticPhoneNumberMatchFilter();
    *ret = lmf;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CLinkify::GetStaticPhoneNumberTransformFilter(
    /* [out] */ ILinkifyTransformFilter** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr< ILinkifyTransformFilter > ltf = Linkify::GetStaticPhoneNumberTransformFilter();
    *ret = ltf;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CLinkify::AddLinks(
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 mask,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    VALIDATE_NOT_NULL(text);
    *ret = Linkify::AddLinks(text, mask);
    return NOERROR;
}

ECode CLinkify::AddLinks(
    /* [in] */ ITextView* text,
    /* [in] */ Int32 mask,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    VALIDATE_NOT_NULL(text);
    *ret = Linkify::AddLinks(text, mask);
    return NOERROR;
}

ECode CLinkify::AddLinks(
    /* [in] */ ITextView* text,
    /* [in] */ IPattern* pattern,
    /* [in] */ const String& scheme)
{
    VALIDATE_NOT_NULL(text);
    Linkify::AddLinks(text, pattern, scheme);
    return NOERROR;
}

ECode CLinkify::AddLinks(
    /* [in] */ ITextView* text,
    /* [in] */ IPattern* p,
    /* [in] */ const String& scheme,
    /* [in] */ ILinkifyMatchFilter* matchFilter,
    /* [in] */ ILinkifyTransformFilter* transformFilter)
{
    VALIDATE_NOT_NULL(text);
    Linkify::AddLinks(text, p, scheme, matchFilter, transformFilter);
    return NOERROR;
}

ECode CLinkify::AddLinks(
    /* [in] */ ISpannable* text,
    /* [in] */ IPattern* pattern,
    /* [in] */ const String& scheme,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = FALSE;
    VALIDATE_NOT_NULL(text);
    *ret = Linkify::AddLinks(text, pattern, scheme);
    return NOERROR;
}

ECode CLinkify::AddLinks(
    /* [in] */ ISpannable* s,
    /* [in] */ IPattern* p,
    /* [in] */ const String& scheme,
    /* [in] */ ILinkifyMatchFilter* matchFilter,
    /* [in] */ ILinkifyTransformFilter* transformFilter,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    VALIDATE_NOT_NULL(s);
    *ret = Linkify::AddLinks(s, p, scheme, matchFilter, transformFilter);
    return NOERROR;
}

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos
