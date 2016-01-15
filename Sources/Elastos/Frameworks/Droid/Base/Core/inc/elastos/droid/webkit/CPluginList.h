
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
