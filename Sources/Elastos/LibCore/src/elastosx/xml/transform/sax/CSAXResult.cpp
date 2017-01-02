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

#include "Elastos.CoreLibrary.External.h"
#include "CSAXResult.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Sax {

CAR_OBJECT_IMPL(CSAXResult)
CAR_INTERFACE_IMPL_2(CSAXResult, Object, ISAXResult, IResult)

CSAXResult::CSAXResult()
{
}

ECode CSAXResult::constructor(
    /* [in] */ IContentHandler* handler)
{
    return SetHandler(handler);
}

ECode CSAXResult::SetHandler(
    /* [in] */ IContentHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CSAXResult::GetHandler(
    /* [out] */ IContentHandler** handler)
{
    VALIDATE_NOT_NULL(handler);
    *handler = mHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CSAXResult::SetLexicalHandler(
    /* [in] */ ILexicalHandler* handler)
{
    mLexhandler = handler;
    return NOERROR;
}

ECode CSAXResult::GetLexicalHandler(
    /* [out] */ ILexicalHandler** lexhandler)
{
    VALIDATE_NOT_NULL(lexhandler);
    *lexhandler = mLexhandler;
    REFCOUNT_ADD(*lexhandler);
    return NOERROR;
}

ECode CSAXResult::SetSystemId(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CSAXResult::GetSystemId(
    /* [out] */ String* systemId)
{
    VALIDATE_NOT_NULL(systemId);
    *systemId = mSystemId;
    return NOERROR;
}

} // namespace Sax
} // namespace Transform
} // namespace Xml
} // namespace Elastosx
