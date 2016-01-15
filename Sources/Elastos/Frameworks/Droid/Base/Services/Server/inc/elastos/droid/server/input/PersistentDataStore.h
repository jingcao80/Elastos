
#ifndef __ELASTOS_DROID_SERVER_INPUT_INPUT_PERSISTENTDATASTORE_H__
#define __ELASTOS_DROID_SERVER_INPUT_INPUT_PERSISTENTDATASTORE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Droid::Utility::IAtomicFile;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

/**
 * Manages persistent state recorded by the input manager service as an XML file.
 * Caller must acquire lock on the data store before accessing it.
 *
 * File format:
 * <code>
 * &lt;input-mananger-state>
 *   &lt;input-devices>
 *     &lt;input-device descriptor="xxxxx" keyboard-layout="yyyyy" />
 *   &gt;input-devices>
 * &gt;/input-manager-state>
 * </code>
 */
class PersistentDataStore : public ElRefBase
{
private:
    class InputDeviceState : public ElRefBase
    {
    public:
        CARAPI_(String) GetCurrentKeyboardLayout();

        CARAPI_(Boolean) SetCurrentKeyboardLayout(
            /* [in] */ const String& keyboardLayout);

        CARAPI_(AutoPtr<ArrayOf<String> >) GetKeyboardLayouts();

        CARAPI_(Boolean) AddKeyboardLayout(
            /* [in] */ const String& keyboardLayout);

        CARAPI_(Boolean) RemoveKeyboardLayout(
            /* [in] */ const String& keyboardLayout);

        CARAPI_(Boolean) SwitchKeyboardLayout(
            /* [in] */ Int32 direction);

        CARAPI_(Boolean) RemoveUninstalledKeyboardLayouts(
            /* [in] */ const HashSet<String>& availableKeyboardLayouts);

        CARAPI LoadFromXml(
            /* [in] */ IXmlPullParser* parser);

        CARAPI SaveToXml(
            /* [in] */ IXmlSerializer* serializer);

    private:
        CARAPI_(void) UpdateCurrentKeyboardLayoutIfRemoved(
            /* [in] */ const String& removedKeyboardLayout,
            /* [in] */ List<String>::Iterator removedIter);

    private:
        String mCurrentKeyboardLayout;
        List<String> mKeyboardLayouts;
    };

public:
    PersistentDataStore();

    CARAPI_(void) SaveIfNeeded();

    CARAPI_(String) GetCurrentKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor);

    CARAPI_(Boolean) SetCurrentKeyboardLayout(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI_(AutoPtr<ArrayOf<String> >) GetKeyboardLayouts(
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
        /* [in] */ const HashSet<String>& availableKeyboardLayouts);

private:
    CARAPI_(AutoPtr<InputDeviceState>) GetInputDeviceState(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Boolean createIfAbsent);

    CARAPI_(void) LoadIfNeeded();

    CARAPI_(void) SetDirty();

    CARAPI_(void) ClearState();

    CARAPI_(void) Load();

    CARAPI_(void) Save();

    CARAPI LoadFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI LoadInputDevicesFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* serializer);

protected:
    static const char* TAG;

private:
    // Input device state by descriptor.
    HashMap<String, AutoPtr<InputDeviceState> > mInputDevices;
    AutoPtr<IAtomicFile> mAtomicFile;

    // True if the data has been loaded.
    Boolean mLoaded;

    // True if there are changes to be saved.
    Boolean mDirty;
};

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_INPUT_PERSISTENTDATASTORE_H__
