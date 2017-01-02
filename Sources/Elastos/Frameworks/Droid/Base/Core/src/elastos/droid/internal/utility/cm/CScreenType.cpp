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

#include "elastos/droid/internal/utility/cm/ScreenType.h"
#include "elastos/droid/internal/utility/cm/CScreenType.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CScreenType, Singleton, IScreenType)

CAR_SINGLETON_IMPL(CScreenType)

ECode CScreenType::IsPhone(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return ScreenType::IsPhone(context, result);
}

ECode CScreenType::IsHybrid(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return ScreenType::IsHybrid(context, result);
}

ECode CScreenType::IsTablet(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return ScreenType::IsTablet(context, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
