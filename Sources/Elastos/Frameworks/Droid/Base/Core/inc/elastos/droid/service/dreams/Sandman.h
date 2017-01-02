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

#ifndef __ELASTOS_DROID_SERVICE_SANDMAN_H__
#define __ELASTOS_DROID_SERVICE_SANDMAN_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

class Sandman
    : public Object
{
public:
    /**
     * Returns true if the specified dock app intent should be started.
     * False if we should dream instead, if appropriate.
     */
    static CARAPI ShouldStartDockApp(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Starts a dream manually.
     */
    /*static*/
    static CARAPI StartDreamByUserRequest(
        /* [in] */ IContext* context);

    /**
     * Starts a dream when docked if the system has been configured to do so,
     * otherwise does nothing.
     */
    /*static*/
    static CARAPI StartDreamWhenDockedIfAppropriate(
        /* [in] */ IContext* context);

private:
    // The sandman is eternal.  No one instantiates him.
    Sandman();

    static CARAPI StartDream(
        /* [in] */ IContext* context,
        /* [in] */ Boolean docked);

    static CARAPI_(Boolean) IsScreenSaverEnabled(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsScreenSaverActivatedOnDock(
        /* [in] */ IContext* context);

private:
    const static String TAG;

    // The component name of a special dock app that merely launches a dream.
    // We don't want to launch this app when docked because it causes an unnecessary
    // activity transition.  We just want to start the dream.
    static AutoPtr<IComponentName> SOMNAMBULATOR_COMPONENT;
};

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_SANDMAN_H__
