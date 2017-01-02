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

#include "Elastos.CoreLibrary.IO.h"
#include "DocumentBuilder.h"
#include "CInputSource.h"
#include "elastosx/xml/parsers/FilePathToURI.h"
#include "utility/logging/Slogger.h"

using Org::Xml::Sax::CInputSource;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_INTERFACE_IMPL(DocumentBuilder, Object, IDocumentBuilder)

ECode DocumentBuilder::Reset()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilder::Parse(
    /* [in]*/ IInputStream* is,
    /* [out]*/ IDocument** doc)
{
    if (is == NULL) {
        // throw new IllegalArgumentException("InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> in;
    CInputSource::New(is, (IInputSource**)&in);
    return Parse(in, doc);
}

ECode DocumentBuilder::Parse(
    /* [in]*/ IInputStream* is,
    /* [in]*/ const String& systemId,
    /* [out]*/ IDocument** doc)
{
    if (is == NULL) {
        // throw new IllegalArgumentException("InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> in;
    CInputSource::New(is, (IInputSource**)&in);
    in->SetSystemId(systemId);
    return Parse(in, doc);
}

ECode DocumentBuilder::Parse(
    /* [in]*/ const String& uri,
    /* [out]*/ IDocument** doc)
{
    if (uri == NULL) {
        // throw new IllegalArgumentException("URI cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> in;
    CInputSource::New(uri, (IInputSource**)&in);
    return Parse(in, doc);
}

ECode DocumentBuilder::Parse(
    /* [in]*/ IFile* f,
    /* [out]*/ IDocument** doc)
{
    if (f == NULL) {
        // throw new IllegalArgumentException("File cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String aPath;
    f->GetAbsolutePath(&aPath);
    String escapedURI = FilePathToURI::Filepath2URI(aPath);

    // if (DEBUG) {
    //     System.out.println("Escaped URI = " + escapedURI);
    // }

    AutoPtr<IInputSource> in;
    CInputSource::New(escapedURI, (IInputSource**)&in);
    return Parse(in, doc);
}

ECode DocumentBuilder::GetSchema(
    /* [out]*/ ISchema** schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilder::IsXIncludeAware(
    /* [out]*/ Boolean* isAware)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
