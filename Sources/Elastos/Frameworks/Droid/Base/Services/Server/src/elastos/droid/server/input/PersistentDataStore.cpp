
#include "input/PersistentDataStore.h"
#include "util/Xml.h"
#include "util/XmlUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Utility::IFastXmlSerializer;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

const char* PersistentDataStore::TAG = "InputManager";

String PersistentDataStore::InputDeviceState::GetCurrentKeyboardLayout()
{
    return mCurrentKeyboardLayout;
}

Boolean PersistentDataStore::InputDeviceState::SetCurrentKeyboardLayout(
    /* [in] */ const String& keyboardLayout)
{
    if (mCurrentKeyboardLayout.Equals(keyboardLayout)) {
        return FALSE;
    }
    AddKeyboardLayout(keyboardLayout);
    mCurrentKeyboardLayout = keyboardLayout;
    return TRUE;
}

AutoPtr<ArrayOf<String> > PersistentDataStore::InputDeviceState::GetKeyboardLayouts()
{
    if (mKeyboardLayouts.IsEmpty()) {
        return ArrayOf<String>::Alloc(0);
    }
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(mKeyboardLayouts.GetSize());
    List<String>::Iterator iter = mKeyboardLayouts.Begin();
    for (Int32 i = 0; iter != mKeyboardLayouts.End(); ++iter, ++i)
        (*array)[i] = *iter;

    return array;
}

Boolean PersistentDataStore::InputDeviceState::AddKeyboardLayout(
    /* [in] */ const String& keyboardLayout)
{
    List<String>::Iterator find = Find(
        mKeyboardLayouts.Begin(), mKeyboardLayouts.End(), keyboardLayout);
    if (find != mKeyboardLayouts.End()) {
        return FALSE;
    }
    // Int32 index = Collections::BinarySearch(mKeyboardLayouts, keyboardLayout);
    // if (index >= 0) {
    //     return FALSE;
    // }
    // mKeyboardLayouts.Insert(mKeyboardLayouts.Begin() - index - 1, keyboardLayout);
    mKeyboardLayouts.PushBack(keyboardLayout);
    if (mCurrentKeyboardLayout == NULL) {
        mCurrentKeyboardLayout = keyboardLayout;
    }
    return TRUE;
}

Boolean PersistentDataStore::InputDeviceState::RemoveKeyboardLayout(
    /* [in] */ const String& keyboardLayout)
{
    List<String>::Iterator find = Find(
        mKeyboardLayouts.Begin(), mKeyboardLayouts.End(), keyboardLayout);
    if (find == mKeyboardLayouts.End()) {
        return FALSE;
    }
    // Int32 index = Collections::BinarySearch(mKeyboardLayouts, keyboardLayout);
    // if (index < 0) {
    //     return FALSE;
    // }
    // mKeyboardLayouts.Erase(mKeyboardLayouts.Begin() + index);
    find = mKeyboardLayouts.Erase(find);
    UpdateCurrentKeyboardLayoutIfRemoved(keyboardLayout, find);
    return TRUE;
}

void PersistentDataStore::InputDeviceState::UpdateCurrentKeyboardLayoutIfRemoved(
    /* [in] */ const String& removedKeyboardLayout,
    /* [in] */ List<String>::Iterator removedIter)
{
    if (mCurrentKeyboardLayout.Equals(removedKeyboardLayout)) {
        if (!mKeyboardLayouts.IsEmpty()) {
            if (removedIter == mKeyboardLayouts.End()) {
                removedIter = mKeyboardLayouts.Begin();
            }
            mCurrentKeyboardLayout = *removedIter;
        }
        else {
            mCurrentKeyboardLayout = NULL;
        }
    }
}

Boolean PersistentDataStore::InputDeviceState::SwitchKeyboardLayout(
    /* [in] */ Int32 direction)
{
    Int32 size = mKeyboardLayouts.GetSize();
    if (size < 2) {
        return FALSE;
    }
    List<String>::Iterator find = Find(
        mKeyboardLayouts.Begin(), mKeyboardLayouts.End(), mCurrentKeyboardLayout);
    assert(find != mKeyboardLayouts.End());
    if (direction > 0) {
        ++find;
        if (find == mKeyboardLayouts.End())
            find = mKeyboardLayouts.Begin();
    }
    else {
        if (find == mKeyboardLayouts.Begin())
            find = mKeyboardLayouts.End();
        --find;
    }
    // Int32 index = Collections::BinarySearch(mKeyboardLayouts, mCurrentKeyboardLayout);
    // assert(index >= 0);
    // if (direction > 0) {
    //     index = (index + 1) % size;
    // } else {
    //     index = (index + size - 1) % size;
    // }
    mCurrentKeyboardLayout = *find;
    return TRUE;
}

Boolean PersistentDataStore::InputDeviceState::RemoveUninstalledKeyboardLayouts(
    /* [in] */ const HashSet<String>& availableKeyboardLayouts)
{
    Boolean changed = FALSE;
    List<String>::ReverseIterator iter = mKeyboardLayouts.RBegin();
    for (; iter != mKeyboardLayouts.REnd();) {
        String keyboardLayout = *iter;
        if (availableKeyboardLayouts.Find(keyboardLayout) == availableKeyboardLayouts.End()) {
            Slogger::I(TAG, "Removing uninstalled keyboard layout %s", keyboardLayout.string());
            UpdateCurrentKeyboardLayoutIfRemoved(keyboardLayout, --(iter.GetBase()));
            iter = List<String>::ReverseIterator(mKeyboardLayouts.Erase(--(iter.GetBase())));
            changed = TRUE;
        }
        else {
            ++iter;
        }
    }
    return changed;
}

ECode PersistentDataStore::InputDeviceState::LoadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    FAIL_RETURN(parser->GetDepth(&outerDepth));
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        FAIL_RETURN(parser->GetName(&name));
        if (name.Equals("keyboard-layout")) {
            String descriptor;
            parser->GetAttributeValue(String(NULL), String("descriptor"), &descriptor);
            if (descriptor.IsNull()) {
                Slogger::E(TAG, "Missing descriptor attribute on keyboard-layout.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            String current;
            parser->GetAttributeValue(String(NULL), String("current"), &current);
            if (Find(mKeyboardLayouts.Begin(), mKeyboardLayouts.End(), descriptor)
                != mKeyboardLayouts.End()) {
                Slogger::E(TAG, "Found duplicate keyboard layout.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            mKeyboardLayouts.PushBack(descriptor);
            if (!current.IsNull() && current.Equals("TRUE")) {
                if (mCurrentKeyboardLayout != NULL) {
                    Slogger::E(TAG, "Found multiple current keyboard layout.");
                    return E_XML_PULL_PARSER_EXCEPTION;
                }
                mCurrentKeyboardLayout = descriptor;
            }
        }
    }

    // Maintain invariant that layouts are sorted.
    //Collections::Sort(mKeyboardLayouts);
    mKeyboardLayouts.Sort();

    // Maintain invariant that there is always a current keyboard layout unless
    // there are none installed.
    if (mCurrentKeyboardLayout.IsNull() && !mKeyboardLayouts.IsEmpty()) {
        mCurrentKeyboardLayout = mKeyboardLayouts[0];
    }
    return NOERROR;
}

ECode PersistentDataStore::InputDeviceState::SaveToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    List<String>::Iterator iter = mKeyboardLayouts.Begin();
    for (; iter != mKeyboardLayouts.End(); ++iter) {
        String layout = *iter;
        FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("keyboard-layout")));
        FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("descriptor"), layout));
        if (layout.Equals(mCurrentKeyboardLayout)) {
            FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("current"), String("TRUE")));
        }
        FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("keyboard-layout")));
    }

    return NOERROR;
}

PersistentDataStore::PersistentDataStore()
    : mLoaded(FALSE)
    , mDirty(FALSE)
{
    AutoPtr<IFile> file;
    ASSERT_SUCCEEDED(CFile::New(
        String("/data/system/input-manager-state.xml"), (IFile**)&file));
    ASSERT_SUCCEEDED(CAtomicFile::New(file, (IAtomicFile**)&mAtomicFile));
}

void PersistentDataStore::SaveIfNeeded()
{
    if (mDirty) {
        Save();
        mDirty = FALSE;
    }
}

String PersistentDataStore::GetCurrentKeyboardLayout(
    /* [in] */ const String& inputDeviceDescriptor)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, FALSE);
    return state != NULL ? state->GetCurrentKeyboardLayout() : String(NULL);
}

Boolean PersistentDataStore::SetCurrentKeyboardLayout(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, TRUE);
    if (state->SetCurrentKeyboardLayout(keyboardLayoutDescriptor)) {
        SetDirty();
        return TRUE;
    }
    return FALSE;
}

AutoPtr<ArrayOf<String> > PersistentDataStore::GetKeyboardLayouts(
    /* [in] */ const String& inputDeviceDescriptor)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, FALSE);
    if (state == NULL) {
        return ArrayOf<String>::Alloc(0);
    }
    return state->GetKeyboardLayouts();
}

Boolean PersistentDataStore::AddKeyboardLayout(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, TRUE);
    if (state->AddKeyboardLayout(keyboardLayoutDescriptor)) {
        SetDirty();
        return TRUE;
    }
    return FALSE;
}

Boolean PersistentDataStore::RemoveKeyboardLayout(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, TRUE);
    if (state->RemoveKeyboardLayout(keyboardLayoutDescriptor)) {
        SetDirty();
        return TRUE;
    }
    return FALSE;
}

Boolean PersistentDataStore::SwitchKeyboardLayout(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 direction)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, FALSE);
    if (state != NULL && state->SwitchKeyboardLayout(direction)) {
        SetDirty();
        return TRUE;
    }
    return FALSE;
}

Boolean PersistentDataStore::RemoveUninstalledKeyboardLayouts(
    /* [in] */ const HashSet<String>& availableKeyboardLayouts)
{
    Boolean changed = FALSE;
    HashMap<String, AutoPtr<InputDeviceState> >::Iterator iter;
    for (iter = mInputDevices.Begin(); iter != mInputDevices.End(); ++iter) {
        if (iter->mSecond->RemoveUninstalledKeyboardLayouts(availableKeyboardLayouts)) {
            changed = TRUE;
        }
    }
    if (changed) {
        SetDirty();
        return TRUE;
    }
    return FALSE;
}

AutoPtr<PersistentDataStore::InputDeviceState> PersistentDataStore::GetInputDeviceState(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Boolean createIfAbsent)
{
    LoadIfNeeded();
    HashMap<String, AutoPtr<InputDeviceState> >::Iterator find =
        mInputDevices.Find(inputDeviceDescriptor);
    AutoPtr<InputDeviceState> state;
    if (find == mInputDevices.End()) {
        if (createIfAbsent) {
            state = new InputDeviceState();
            mInputDevices[inputDeviceDescriptor] = state;
            SetDirty();
        }
    }
    else {
        state = find->mSecond;
    }

    return state;
}

void PersistentDataStore::LoadIfNeeded()
{
    if (!mLoaded) {
        Load();
        mLoaded = TRUE;
    }
}

void PersistentDataStore::SetDirty()
{
    mDirty = TRUE;
}

void PersistentDataStore::ClearState()
{
    mInputDevices.Clear();
}

void PersistentDataStore::Load()
{
    ClearState();

    AutoPtr<IInputStream> is;
    if (FAILED(mAtomicFile->OpenRead((IFileInputStream**)&is)))
        return;

    AutoPtr<IXmlPullParser> parser;

    parser = Xml::NewPullParser();

    AutoPtr<IBufferedInputStream> bis;
    ECode ec = CBufferedInputStream::New(is, (IBufferedInputStream**)&bis);
    if (FAILED(ec))
        goto _Exit_;

    ec = parser->SetInput(bis, String(NULL));
    if (FAILED(ec))
        goto _Exit_;

    ec = LoadFromXml(parser);

_Exit_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG,
            "Failed to load input manager persistent store data.");
        ClearState();
    }
    else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION ) {
        Slogger::W(TAG,
            "Failed to load input manager persistent store data.");
        ClearState();
    }

    if (ICloseable::Probe(is))
        ICloseable::Probe(is)->Close();
}

void PersistentDataStore::Save()
{
    AutoPtr<IFileOutputStream> os;
    Boolean success = FALSE;
    AutoPtr<IXmlSerializer> serializer;
    AutoPtr<IBufferedOutputStream> bos;
    AutoPtr<IFastXmlSerializer> fs;

    ECode ec = mAtomicFile->StartWrite((IFileOutputStream**)&os);
    if (FAILED(ec))
        goto _Exit2_;

    ec = CFastXmlSerializer::New((IFastXmlSerializer**)&fs);
    if (FAILED(ec))
        goto _Exit1_;
    serializer = IXmlSerializer::Probe(fs);

    ec = CBufferedOutputStream::New(os, (IBufferedOutputStream**)&bos);
    if (FAILED(ec))
        goto _Exit1_;

    ec = serializer->SetOutput(bos, String("utf-8"));
    if (FAILED(ec))
        goto _Exit1_;

    ec = SaveToXml(serializer);
    if (FAILED(ec))
        goto _Exit1_;

    ec = serializer->Flush();
    if (FAILED(ec))
        goto _Exit1_;
    success = TRUE;

_Exit1_:
    if (success) {
        ec = mAtomicFile->FinishWrite(os);
    }
    else {
        ec = mAtomicFile->FailWrite(os);
    }

_Exit2_:
    if (FAILED(ec)) {
        Slogger::W(TAG, "Failed to save input manager persistent store data.");
    }
}

ECode PersistentDataStore::LoadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    FAIL_RETURN(XmlUtils::BeginDocument(parser, String("input-manager-state")));
    Int32 outerDepth;
    FAIL_RETURN(parser->GetDepth(&outerDepth));
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        FAIL_RETURN(parser->GetName(&name));
        if (name.Equals("input-devices")) {
            FAIL_RETURN(LoadInputDevicesFromXml(parser));
        }
    }

    return NOERROR;
}

ECode PersistentDataStore::LoadInputDevicesFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    FAIL_RETURN(parser->GetDepth(&outerDepth));
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        FAIL_RETURN(parser->GetName(&name));
        if (name.Equals("input-device")) {
            String descriptor;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("descriptor"), &descriptor));
            if (descriptor.IsNull()) {
                Slogger::E(TAG, "Missing descriptor attribute on input-device.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            if (mInputDevices.Find(descriptor) != mInputDevices.End()) {
                Slogger::E(TAG, "Found duplicate input device.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            AutoPtr<InputDeviceState> state = new InputDeviceState();
            FAIL_RETURN(state->LoadFromXml(parser));
            mInputDevices[descriptor] = state;
        }
    }

    return NOERROR;
}

ECode PersistentDataStore::SaveToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    AutoPtr<IBoolean> boolObj;
    CBoolean::New(TRUE, (IBoolean**)&boolObj);
    FAIL_RETURN(serializer->StartDocument(String(NULL), boolObj));
    FAIL_RETURN(serializer->SetFeature(
        String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE));
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("input-manager-state")));
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("input-devices")));
    HashMap<String, AutoPtr<InputDeviceState> >::Iterator iter
        = mInputDevices.Begin();
    for (; iter != mInputDevices.End(); ++iter) {
        String descriptor = iter->mFirst;
        AutoPtr<InputDeviceState> state = iter->mSecond;

        FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("input-device")));
        FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("descriptor"), descriptor));
        FAIL_RETURN(state->SaveToXml(serializer));
        FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("input-device")));
    }
    FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("input-devices")));
    FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("input-manager-state")));
    FAIL_RETURN(serializer->EndDocument());

    return NOERROR;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
