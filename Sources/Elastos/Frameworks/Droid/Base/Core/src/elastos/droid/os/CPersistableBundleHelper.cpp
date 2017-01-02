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

#include "elastos/droid/os/CPersistableBundleHelper.h"
#include "elastos/droid/os/CPersistableBundle.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CPersistableBundleHelper, Singleton, IPersistableBundleHelper)

CAR_SINGLETON_IMPL(CPersistableBundleHelper)

ECode CPersistableBundleHelper::GetEMPTY(
    /* [out] */ IPersistableBundle** pb)
{
    VALIDATE_NOT_NULL(pb)
    *pb = CPersistableBundle::EMPTY;
    REFCOUNT_ADD(*pb);
    return NOERROR;
}

ECode CPersistableBundleHelper::ForPair(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [out] */ IPersistableBundle** pb)
{
    VALIDATE_NOT_NULL(pb)
    AutoPtr<IPersistableBundle> b = CPersistableBundle::ForPair(key, value);
    *pb = b;
    REFCOUNT_ADD(*pb)
    return NOERROR;
}

ECode CPersistableBundleHelper::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [out] */ IPersistableBundle** pb)
{
    return CPersistableBundle::RestoreFromXml(in, pb);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
