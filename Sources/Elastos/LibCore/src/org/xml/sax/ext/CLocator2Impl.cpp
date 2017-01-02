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

#include "CLocator2Impl.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Ext {

CAR_INTERFACE_IMPL_2(CLocator2Impl, LocatorImpl, ILocator2, ILocator2Impl)
CAR_OBJECT_IMPL(CLocator2Impl)

ECode CLocator2Impl::constructor()
{
    return LocatorImpl::constructor();
}

ECode CLocator2Impl::constructor(
    /* [in] */ ILocator* locator)
{
    FAIL_RETURN(LocatorImpl::constructor(locator));

    ILocator2* locator2 = ILocator2::Probe(locator);

    if (locator2 != NULL) {
        locator2->GetXMLVersion(&mVersion);
        locator2->GetEncoding(&mEncoding);
    }

    return NOERROR;
}

ECode CLocator2Impl::SetXMLVersion(
    /* [in] */ const String& xmlVersion)
{
    mVersion = xmlVersion;
    return NOERROR;
}

ECode CLocator2Impl::SetEncoding(
    /* [in] */ const String& encoding)
{
    mEncoding = encoding;
    return NOERROR;
}

ECode CLocator2Impl::GetXMLVersion(
    /* [out] */ String* xmlVersion)
{
    VALIDATE_NOT_NULL(xmlVersion);

    *xmlVersion = mVersion;;

    return NOERROR;
}

ECode CLocator2Impl::GetEncoding(
    /* [out] */ String* encoding)
{
    VALIDATE_NOT_NULL(encoding);

    *encoding = mEncoding;

    return NOERROR;
}

} // namespace Ext
} // namespace Sax
} // namespace Xml
} // namespace Org
