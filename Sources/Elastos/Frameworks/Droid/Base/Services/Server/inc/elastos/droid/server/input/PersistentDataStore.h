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

#ifndef __ELASTOS_DROID_Server_Input_PersistentDataStore_H__
#define __ELASTOS_DROID_Server_Input_PersistentDataStore_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Hardware::Input::ITouchCalibration;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ISet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class PersistentDataStore
    : public Object
{
private:
    class InputDeviceState
        : public Object
    {
    public:
        InputDeviceState();

        CARAPI_(AutoPtr<ITouchCalibration>) GetTouchCalibration(
            /* [in] */ Int32 surfaceRotation);

        CARAPI_(Boolean) SetTouchCalibration(
            /* [in] */ Int32 surfaceRotation,
            /* [in] */ ITouchCalibration* calibration);

        CARAPI_(String) GetCurrentKeyboardLayout();

        CARAPI_(Boolean) SetCurrentKeyboardLayout(
            /* [in] */ const String& keyboardLayout);

        CARAPI_(AutoPtr< ArrayOf<String> >) GetKeyboardLayouts();

        CARAPI_(Boolean) AddKeyboardLayout(
            /* [in] */ const String& keyboardLayout);

        CARAPI_(Boolean) RemoveKeyboardLayout(
            /* [in] */ const String& keyboardLayout);

        CARAPI_(void) UpdateCurrentKeyboardLayoutIfRemoved(
            /* [in] */ const String& removedKeyboardLayout,
            /* [in] */ Int32 removedIndex);

        CARAPI_(Boolean) SwitchKeyboardLayout(
            /* [in] */ Int32 direction);

        CARAPI_(Boolean) RemoveUninstalledKeyboardLayouts(
            /* [in] */ ISet* availableKeyboardLayouts);

        // throws IOException, XmlPullParserException
        CARAPI LoadFromXml(
            /* [in] */ IXmlPullParser* parser);

        // throws IOException
        CARAPI SaveToXml(
            /* [in] */ IXmlSerializer* serializer);

    private:
        static CARAPI SurfaceRotationToString(
            /* [in] */ Int32 surfaceRotation,
            /* [in] */ String* str);

        static CARAPI StringToSurfaceRotation(
            /* [in] */ const String& str,
            /* [in] */ Int32* r);

        static CARAPI_(AutoPtr<ArrayOf<String> >) INIT_CALIBRATION_NAME();

    private:
        static const AutoPtr<ArrayOf<String> > CALIBRATION_NAME;

        AutoPtr<ArrayOf<ITouchCalibration*> > mTouchCalibration;
        String mCurrentKeyboardLayout;
        AutoPtr<IArrayList> mKeyboardLayouts;
    };

public:
    PersistentDataStore();

    CARAPI_(void) SaveIfNeeded();

    CARAPI_(AutoPtr<ITouchCalibration>) GetTouchCalibration(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 surfaceRotation);

    CARAPI_(Boolean) SetTouchCalibration(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 surfaceRotation,
        /* [in] */ ITouchCalibration* calibration);

    CARAPI_(String) GetCurrentKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor);

    CARAPI_(Boolean) SetCurrentKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetKeyboardLayouts(
        /* [in] */ const String& inputDeviceDescriptor);

    CARAPI_(Boolean) AddKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI_(Boolean) RemoveKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI_(Boolean) SwitchKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 direction);

    CARAPI_(Boolean) RemoveUninstalledKeyboardLayouts(
        /* [in] */ ISet* availableKeyboardLayouts);

private:
    CARAPI_(AutoPtr<InputDeviceState>) GetInputDeviceState(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Boolean createIfAbsent);

    CARAPI_(void) LoadIfNeeded();

    CARAPI_(void) SetDirty();

    CARAPI_(void) ClearState();

    CARAPI_(void) Load();

    CARAPI_(void) Save();

    // throws IOException, XmlPullParserException
    CARAPI LoadFromXml(
        /* [in] */ IXmlPullParser* parser);

    // throws IOException, XmlPullParserException
    CARAPI LoadInputDevicesFromXml(
        /* [in] */ IXmlPullParser* parser);

    //  throws IOException
    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* serializer);

private:
    static const String TAG;

    // Input device state by descriptor.
    HashMap< String, AutoPtr<InputDeviceState> > mInputDevices;
    AutoPtr<IAtomicFile> mAtomicFile;

    // True if the data has been loaded.
    Boolean mLoaded;

    // True if there are changes to be saved.
    Boolean mDirty;
};

} // Input
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Input_PersistentDataStore_H__


