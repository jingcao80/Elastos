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

#include "CDefaultHandler2.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Ext {

CAR_INTERFACE_IMPL_3(CDefaultHandler2, DefaultHandler, ILexicalHandler, IDeclHandler, IEntityResolver2)

CAR_OBJECT_IMPL(CDefaultHandler2)

ECode CDefaultHandler2::StartDTD(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    return NOERROR;
}

ECode CDefaultHandler2::EndDTD()
{
    return NOERROR;
}

ECode CDefaultHandler2::StartEntity(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode CDefaultHandler2::EndEntity(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode CDefaultHandler2::StartCDATA()
{
    return NOERROR;
}

ECode CDefaultHandler2::EndCDATA()
{
    return NOERROR;
}

ECode CDefaultHandler2::Comment(
    /* [out] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode CDefaultHandler2::ElementDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& model)
{
    return NOERROR;
}

ECode CDefaultHandler2::AttributeDecl(
    /* [in] */ const String& eName,
    /* [in] */ const String& aName,
    /* [in] */ const String& type,
    /* [in] */ const String& mode,
    /* [in] */ const String& value)
{
    return NOERROR;
}

ECode CDefaultHandler2::InternalEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return NOERROR;
}

ECode CDefaultHandler2::ExternalEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    return NOERROR;
}

ECode CDefaultHandler2::ResolveEntity(
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [out] */ IInputSource** src)
{
    return ResolveEntity(String(NULL), publicId, String(NULL), systemId, src);
}

ECode CDefaultHandler2::GetExternalSubset(
    /* [in] */ const String& name,
    /* [in] */ const String& baseURI,
    /* [out] */ IInputSource** src)
{
    VALIDATE_NOT_NULL(src);

    *src = NULL;

    return NOERROR;
}

ECode CDefaultHandler2::ResolveEntity(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& baseURI,
    /* [in] */ const String& systemId,
    /* [out] */ IInputSource** src)
{
    VALIDATE_NOT_NULL(src);

    *src = NULL;

    return NOERROR;
}

} // namespace Ext
} // namespace Sax
} // namespace Xml
} // namespace Org
