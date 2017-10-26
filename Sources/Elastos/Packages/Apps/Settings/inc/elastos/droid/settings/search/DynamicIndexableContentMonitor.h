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

#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_DYNAMICINDEXABLECONTENTMONITOR_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_DYNAMICINDEXABLECONTENTMONITOR_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

class DynamicIndexableContentMonitor
    : public PackageMonitor
    , public IInputDeviceListener
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("DynamicIndexableContentMonitor::MyHandler")

        MyHandler(
            /* [in] */ DynamicIndexableContentMonitor* host);

        ~MyHandler();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        DynamicIndexableContentMonitor* mHost;
    };

    class UserDictionaryContentObserver
        : public ContentObserver
    {
    public:
        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ DynamicIndexableContentMonitor* host);

        ~UserDictionaryContentObserver();

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        DynamicIndexableContentMonitor* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    DynamicIndexableContentMonitor();

    ~DynamicIndexableContentMonitor();

    using PackageMonitor::Register;

    CARAPI Register(
        /* [in] */ IContext* context);

    CARAPI Unregister();

    // Covers installed, appeared external storage with the package, upgraded.
    //@Override
    CARAPI OnPackageAppeared(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid);

    // Covers uninstalled, removed external storage with the package.
    //@Override
    CARAPI OnPackageDisappeared(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid);

    // Covers enabled, disabled.
    //@Override
    CARAPI OnPackageModified(
        /* [in] */ const String& packageName);

    //@Override
    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId);

private:
    static CARAPI_(AutoPtr<IIntent>) GetAccessibilityServiceIntent(
        /* [in] */ const String& packageName);

    static CARAPI_(AutoPtr<IIntent>) GetPrintServiceIntent(
        /* [in] */ const String& packageName);

    static CARAPI_(AutoPtr<IIntent>) GetIMEServiceIntent(
        /* [in] */ const String& packageName);

    CARAPI_(void) PostMessage(
        /* [in] */ Int32 what,
        /* [in] */ const String& packageName);

    CARAPI_(void) HandlePackageAvailable(
        /* [in] */ const String& packageName);

    CARAPI_(void) HandlePackageUnavailable(
        /* [in] */ const String& packageName);

private:
    static const Int64 DELAY_PROCESS_PACKAGE_CHANGE;

    static const Int32 MSG_PACKAGE_AVAILABLE = 1;
    static const Int32 MSG_PACKAGE_UNAVAILABLE = 2;

    AutoPtr<IList> mAccessibilityServices;
    AutoPtr<IList> mPrintServices;
    AutoPtr<IList> mImeServices;

    AutoPtr<MyHandler> mHandler;

    AutoPtr<UserDictionaryContentObserver> mUserDictionaryContentObserver;

    AutoPtr<IContext> mContext;
    Boolean mHasFeaturePrinting;
    Boolean mHasFeatureIme;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_DYNAMICINDEXABLECONTENTMONITOR_H__
