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

#include "Elastos.CoreLibrary.Net.h"
#include "CStructGroupReq.h"
#include "core/StringUtils.h"

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructGroupReq)

CAR_INTERFACE_IMPL(CStructGroupReq, Object, IStructGroupReq)

ECode CStructGroupReq::constructor(
    /* [in] */ Int32 gr_interface,
    /* [in] */ IInetAddress* gr_group)
{
    mGr_interface = gr_interface;
    mGr_group = gr_group;
    return NOERROR;
}

ECode CStructGroupReq::GetGrInterface(
    /* [out] */ Int32* grInterface)
{
    VALIDATE_NOT_NULL(grInterface)

    *grInterface = mGr_interface;
    return NOERROR;
}

ECode CStructGroupReq::GetGrGroup(
    /* [out] */ IInetAddress** grGroup)
{
    VALIDATE_NOT_NULL(grGroup)

    *grGroup = mGr_group;
    REFCOUNT_ADD(*grGroup)
    return NOERROR;
}

ECode CStructGroupReq::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    String outstr;
    IObject* obj = IObject::Probe(mGr_group);
    obj->ToString(&outstr);
    *str = String("StructGroupReq[gr_interface=")
            + StringUtils::ToString(mGr_interface)
            + String(",gr_group=")
            + outstr
            + String("]");
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
