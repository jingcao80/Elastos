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

#ifndef __ELASTOS_DROID_WEBKIT_CPLUGINLIST_H__
#define __ELASTOS_DROID_WEBKIT_CPLUGINLIST_H__

#include "_Elastos_Droid_Webkit_CPluginList.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/webkit/CPlugin.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {

class CPlugin;

CarClass(CPluginList)
    , public Object
    , public IPluginList
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
    * Returns the list of plugins as a java.util.List.
    *
    * @hide
    * @deprecated This interface was intended to be used by Gears. Since Gears was
    * deprecated, so is this class.
    */
    //synchronized
    CARAPI GetList(
        /* [out] */ IList** plugins);

    /**
    * Adds a plugin to the list.
    *
    * @hide
    * @deprecated This interface was intended to be used by Gears. Since Gears was
    * deprecated, so is this class.
    */
    //synchronized
    CARAPI AddPlugin(
        /* [in] */ IPlugin* plugin);

    /**
    * Removes a plugin from the list.
    *
    * @hide
    * @deprecated This interface was intended to be used by Gears. Since Gears was
    * deprecated, so is this class.
    */
    //synchronized
    CARAPI RemovePlugin(
        /* [in] */ IPlugin* plugin);

    /**
    * Clears the plugin list.
    *
    * @hide
    * @deprecated This interface was intended to be used by Gears. Since Gears was
    * deprecated, so is this class.
    */
    //synchronized
    CARAPI Clear();

    /**
    * Dispatches the click event to the appropriate plugin.
    *
    * @hide
    * @deprecated This interface was intended to be used by Gears. Since Gears was
    * deprecated, so is this class.
    */
    //synchronized
    CARAPI PluginClicked(
        /* [in] */ IContext* context,
        /* [in] */ Int32 position);

private:
    List< AutoPtr<CPlugin> > mPlugins;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CPLUGINLIST_H__
