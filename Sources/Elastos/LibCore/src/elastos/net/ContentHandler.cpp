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

#include "ContentHandler.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(ContentHandler, Object, IContentHandler)

ContentHandler::ContentHandler()
{
}

ContentHandler::~ContentHandler()
{
}

ECode ContentHandler::GetContent(
    /* [in] */ IURLConnection* uConn,
    /* [in] */ ArrayOf<InterfaceID> * types,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;
    VALIDATE_NOT_NULL(types)

    AutoPtr<IInterface> content;
    FAIL_RETURN(GetContent(uConn, (IInterface**)&content));
    for (Int32 i = 0; i < types->GetLength(); i++) {
        if (content->Probe((*types)[i]) != NULL) {
            *obj = content;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
    }
    *obj = NULL;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
