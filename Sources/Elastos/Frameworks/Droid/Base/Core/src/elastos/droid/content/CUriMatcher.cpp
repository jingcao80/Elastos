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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CUriMatcher.h"

using Elastos::Utility::IList;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Content {

AutoPtr<IPattern> CUriMatcher::PATH_SPLIT_PATTERN;

CAR_INTERFACE_IMPL(CUriMatcher, Object, IUriMatcher)

CAR_OBJECT_IMPL(CUriMatcher)

AutoPtr<IPattern> CUriMatcher::GetSplitPattern()
{
    if (PATH_SPLIT_PATTERN.Get() == NULL) {
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        helper->Compile(String("/"), (IPattern**)&PATH_SPLIT_PATTERN);
    }

    return PATH_SPLIT_PATTERN;
}

CUriMatcher::CUriMatcher()
    : mCode(IUriMatcher::NO_MATCH)
    , mWhich(-1)
{
}

CUriMatcher::~CUriMatcher()
{
    mChildren.Clear();
}

ECode CUriMatcher::AddURI(
    /* [in] */ const String& authority,
    /* [in] */ const String& path,
    /* [in] */ Int32 code)
{
    if (code < 0) {
        // throw new IllegalArgumentException("code " + code + " is invalid: it must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<String> > tokens;
    if (!path.IsNull()) {
        String newPath = path;
        // Strip leading slash if present.
        if (!path.IsEmpty() && path.GetChar(0) == '/') {
            newPath = path.Substring(1);
        }
        FAIL_RETURN(GetSplitPattern()->Split(newPath, (ArrayOf<String>**)&tokens))
    }

    Int32 numTokens = tokens != NULL ? tokens->GetLength() : 0;
    AutoPtr<CUriMatcher> node = this;
    String token;

    for (Int32 i = -1; i < numTokens; i++) {
        if (i < 0) {
            token = authority;
        }
        else {
            token = (*tokens)[i];
        }

        List<AutoPtr<CUriMatcher> >& children = node->mChildren;
        AutoPtr<CUriMatcher> child;
        List<AutoPtr<CUriMatcher> >::Iterator it;
        for (it = children.Begin(); it != children.End(); it++) {
            child = *it;
            if (token.Equals(child->mText)) {
                node = child;
                break;
            }
        }

        if (it == children.End()) {
            // Child not found, create it
            child = new CUriMatcher();

            if (token.Equals("#")) {
                child->mWhich = NUMBER;
            }
            else if (token.Equals("*")) {
                child->mWhich = TEXT;
            }
            else {
                child->mWhich = EXACT;
            }

            child->mText = token;
            node->mChildren.PushBack(child);
            node = child;
        }
    }

    node->mCode = code;
    return NOERROR;
}

ECode CUriMatcher::Match(
    /* [in] */ IUri* uri,
    /* [out] */ Int32* matchCode)
{
    VALIDATE_NOT_NULL(matchCode)
    *matchCode = 0;
    VALIDATE_NOT_NULL(uri)

    AutoPtr<IList> pathSegments;
    FAIL_RETURN(uri->GetPathSegments((IList**)&pathSegments))
    Int32 length;
    pathSegments->GetSize(&length);
    AutoPtr<CUriMatcher> node = this;
    String authority;
    FAIL_RETURN(uri->GetAuthority(&authority))

    if (0 == length && authority.IsNull()) {
        *matchCode = node->mCode;
        return NOERROR;
    }

    String u;
    Char32 c;
    AutoPtr<ArrayOf<Char32> > chars;
    for (Int32 i = -1; i < length; i++) {
        if (i < 0) {
            u = authority;
        }
        else {
            AutoPtr<IInterface> obj;
            pathSegments->Get(i, (IInterface**)&obj);
            u = Object::ToString(obj);
        }

        List<AutoPtr<CUriMatcher> >& list = node->mChildren;
        if (list.IsEmpty()) break;
        node = NULL;
        List<AutoPtr<CUriMatcher> >::Iterator it;

        for (it = list.Begin(); it != list.End(); it++) {
            AutoPtr<CUriMatcher> n = *it;
            Int32 lk = 0;
            //which_switch:
            switch (n->mWhich) {
            case EXACT:
                if (n->mText.Equals(u)) {
                    node = n;
                }
                break;
            case NUMBER:
                chars = u.GetChars();
                lk = chars->GetLength();
                for (Int32 k = 0; k < lk; k++) {
                    c = (*chars)[k];
                    if (c < '0' || c > '9') {
                        //break which_switch;
                        goto which_switch;
                    }
                }
                node = n;
                break;
            case TEXT:
                node = n;
                break;
            }

which_switch:
            if (NULL != node) {
                break;
            }
        }

        if (NULL == node) {
            *matchCode = IUriMatcher::NO_MATCH;
            return NOERROR;
        }
    }

    *matchCode = node->mCode;
    return NOERROR;
}

ECode CUriMatcher::constructor(
    /* [in] */ Int32 code)
{
    mCode = code;
    return NOERROR;
}

}
}
}

