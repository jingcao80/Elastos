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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CDATAUSAGEDETAILVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CDATAUSAGEDETAILVIEW_H__

#include "_Elastos_Droid_SystemUI_Qs_Tiles_CDataUsageDetailView.h"
#include "Elastos.CoreLibrary.Text.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerDataUsageInfo;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Text::IDecimalFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/**
 * Layout for the data usage detail in quick settings.
 */
CarClass(CDataUsageDetailView)
    , public LinearLayout
    , public IDataUsageDetailView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDataUsageDetailView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Bind(
        /* [in] */ INetworkControllerDataUsageInfo* info);

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(String) FormatBytes(
        /* [in] */ Int64 bytes);

private:
    static const Double KB;
    static const Double MB;
    static const Double GB;

    AutoPtr<IDecimalFormat> FORMAT;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CDATAUSAGEDETAILVIEW_H__
