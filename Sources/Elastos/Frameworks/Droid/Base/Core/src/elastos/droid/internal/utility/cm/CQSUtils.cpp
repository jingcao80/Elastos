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

#include "elastos/droid/internal/utility/cm/QSUtils.h"
#include "elastos/droid/internal/utility/cm/CQSUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CQSUtils, Singleton, IQSUtils)

CAR_SINGLETON_IMPL(CQSUtils)

ECode CQSUtils::GetAvailableTiles(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
	return QSUtils::GetAvailableTiles(context, result);
}

ECode CQSUtils::GetDefaultTiles(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
	return QSUtils::GetDefaultTiles(context, result);
}

ECode CQSUtils::GetDefaultTilesAsString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
	return QSUtils::GetDefaultTilesAsString(context, result);
}

ECode CQSUtils::DeviceSupportsLte(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsLte(context, result);
}

ECode CQSUtils::DeviceSupportsDdsSupported(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsDdsSupported(context, result);
}

ECode CQSUtils::DeviceSupportsMobileData(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsMobileData(context, result);
}

ECode CQSUtils::DeviceSupportsBluetooth(
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsBluetooth(result);
}

ECode CQSUtils::DeviceSupportsNfc(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsNfc(context, result);
}

ECode CQSUtils::DeviceSupportsFlashLight(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsFlashLight(context, result);
}

ECode CQSUtils::DeviceSupportsCompass(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsCompass(context, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
