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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/CPluginList.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CPluginList, Object, IPluginList);

CAR_OBJECT_IMPL(CPluginList);

ECode CPluginList::constructor()
{
    return NOERROR;
}

ECode CPluginList::GetList(
    /* [out] */ IList** plugins)
{
    VALIDATE_NOT_NULL(plugins);
    AutoLock lock(this);
    FAIL_RETURN(CArrayList::New(plugins));
    List< AutoPtr<CPlugin> >::Iterator it;
    for (it = mPlugins.Begin(); it != mPlugins.End(); ++it) {
        (*plugins)->Add((IPlugin*)it->Get());
    }
    return NOERROR;
}

ECode CPluginList::AddPlugin(
    /* [in] */ IPlugin* plugin)
{
    AutoLock lock(this);
    AutoPtr<CPlugin> ap = (CPlugin*)plugin;
    if (Find(mPlugins.Begin(), mPlugins.End(), ap) == mPlugins.End()) {
        mPlugins.PushBack(ap);
    }
    return NOERROR;
}

ECode CPluginList::RemovePlugin(
    /* [in] */ IPlugin* plugin)
{
    AutoLock lock(this);
    mPlugins.Remove((CPlugin*)plugin);
    return NOERROR;
}

ECode CPluginList::Clear()
{
    AutoLock lock(this);
    mPlugins.Clear();
    return NOERROR;
}

ECode CPluginList::PluginClicked(
    /* [in] */ IContext * context,
    /* [in] */ Int32 position)
{
    AutoLock lock(this);
    AutoPtr<CPlugin> plugin = mPlugins[position];
    if (plugin != NULL) {
        plugin->DispatchClickEvent(context);
    }
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
