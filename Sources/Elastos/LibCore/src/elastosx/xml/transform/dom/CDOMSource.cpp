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
#include "CDOMSource.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Dom {

CAR_OBJECT_IMPL(CDOMSource)
CAR_INTERFACE_IMPL_2(CDOMSource, Object, IDOMSource, ISource)

ECode CDOMSource::constructor()
{
    return NOERROR;
}

ECode CDOMSource::constructor(
    /* [in] */ INode* n)
{
    return SetNode(n);
}

ECode CDOMSource::constructor(
    /* [in] */ INode* node,
    /* [in] */ const String& systemID)
{
    SetNode(node);
    return SetSystemId(systemID);
}

ECode CDOMSource::SetNode(
    /* [in] */ INode* node)
{
    mNode = node;
    return NOERROR;
}

ECode CDOMSource::GetNode(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = mNode;
    REFCOUNT_ADD(*node);
    return NOERROR;
}

ECode CDOMSource::SetSystemId(
    /* [in] */ const String& systemID)
{
    mSystemID = systemID;
    return NOERROR;
}

ECode CDOMSource::GetSystemId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSystemID;
    return NOERROR;
}

} // namespace Dom
} // namespace Transform
} // namespace Xml
} // namespace Elastosx
