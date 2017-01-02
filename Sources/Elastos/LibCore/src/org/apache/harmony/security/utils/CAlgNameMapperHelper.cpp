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

#include "CAlgNameMapperHelper.h"
#include "AlgNameMapper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CAR_SINGLETON_IMPL(CAlgNameMapperHelper)
CAR_INTERFACE_IMPL(CAlgNameMapperHelper, Singleton, IAlgNameMapperHelper)

ECode CAlgNameMapperHelper::Map2OID(
    /* [in] */ const String& algName,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::Map2OID(algName);
    return NOERROR;
}

ECode CAlgNameMapperHelper::Map2AlgName(
    /* [in] */ const String& oid,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::Map2AlgName(oid);
    return NOERROR;
}

ECode CAlgNameMapperHelper::GetStandardName(
    /* [in] */ const String& algName,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::GetStandardName(algName);
    return NOERROR;
}

ECode CAlgNameMapperHelper::IsOID(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::IsOID(alias);
    return NOERROR;
}

ECode CAlgNameMapperHelper::Normalize(
    /* [in] */ const String& oid,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::Normalize(oid);
    return NOERROR;
}

}
}
}
}
}
