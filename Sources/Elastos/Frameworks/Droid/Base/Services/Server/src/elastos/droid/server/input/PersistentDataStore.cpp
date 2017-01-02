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

#include "elastos/droid/server/input/PersistentDataStore.h"
#include "elastos/droid/server/input/CInputManagerService.h"
//#include "elastos/droid/utility/CAtomicFile.h"
#include "elastos/droid/hardware/input/TouchCalibration.h"
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/droid/utility/Xml.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Hardware::Input::ITouchCalibration;
using Elastos::Droid::Hardware::Input::CTouchCalibration;
using Elastos::Droid::Hardware::Input::ITouchCalibrationHelper;
using Elastos::Droid::Hardware::Input::CTouchCalibrationHelper;
using Elastos::Droid::View::ISurface;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Org::Xml::Sax::CInputSource;
using Org::Xml::Sax::Helpers::CDefaultHandler;
using Org::Xmlpull::V1::IXmlPullParserFactory;
using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

//==============================================================================
//  PersistentDataStore::InputDeviceState
//==============================================================================
const AutoPtr<ArrayOf<String> > PersistentDataStore::InputDeviceState::CALIBRATION_NAME = INIT_CALIBRATION_NAME();

AutoPtr<ArrayOf<String> > PersistentDataStore::InputDeviceState::INIT_CALIBRATION_NAME()
{
    AutoPtr<ArrayOf<String> > strArr = ArrayOf<String>::Alloc(6);
    (*strArr)[0] = "x_scale";
    (*strArr)[1] = "x_ymix";
    (*strArr)[2] = "x_offset";
    (*strArr)[3] = "y_xmix";
    (*strArr)[4] = "y_scale";
    (*strArr)[5] = "y_offset";
    return strArr;
}

PersistentDataStore::InputDeviceState::InputDeviceState()
{
    mTouchCalibration = ArrayOf<ITouchCalibration*>::Alloc(4);
    CArrayList::New((IArrayList**)&mKeyboardLayouts);
}

AutoPtr<ITouchCalibration> PersistentDataStore::InputDeviceState::GetTouchCalibration(
    /* [in] */ Int32 surfaceRotation)
{
    if (surfaceRotation >= 0 && surfaceRotation < mTouchCalibration->GetLength()) {
        return (*mTouchCalibration)[surfaceRotation];
    }
    Slogger::W(CInputManagerService::TAG, "Cannot get touch calibration.");
    return NULL;
}

Boolean PersistentDataStore::InputDeviceState::SetTouchCalibration(
    /* [in] */ Int32 surfaceRotation,
    /* [in] */ ITouchCalibration* calibration)
{
    if (surfaceRotation >= 0 && surfaceRotation < mTouchCalibration->GetLength()) {
        Boolean isEquals;
        if (IObject::Probe(calibration)->Equals((*mTouchCalibration)[surfaceRotation], &isEquals), !isEquals) {
            mTouchCalibration->Set(surfaceRotation, calibration);
            return TRUE;
        }
        return FALSE;
    }
    Slogger::W(CInputManagerService::TAG, "Cannot set touch calibration.");
    return FALSE;
}

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

AutoPtr< ArrayOf<String> > PersistentDataStore::InputDeviceState::GetKeyboardLayouts()
{
    Boolean isEmpty;
    if (mKeyboardLayouts->IsEmpty(&isEmpty), isEmpty) {
        return ArrayOf<String>::Alloc(0);
    }

    Int32 size;
    mKeyboardLayouts->GetSize(&size);
    AutoPtr< ArrayOf<String> > keyboardLayouts = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> keyboardLayout;
        mKeyboardLayouts->Get(i, (IInterface**)&keyboardLayout);
        String keyboardLayoutStr;
        ICharSequence::Probe(keyboardLayout)->ToString(&keyboardLayoutStr);
        (*keyboardLayouts)[i] = keyboardLayoutStr;
    }
    return keyboardLayouts;
}

Boolean PersistentDataStore::InputDeviceState::AddKeyboardLayout(
    /* [in] */ const String& keyboardLayout)
{
    AutoPtr<ICharSequence> csq;
    CString::New(keyboardLayout, (ICharSequence**)&csq);
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    Int32 index;
    coll->BinarySearch(IList::Probe(mKeyboardLayouts), csq, &index);
    if (index >= 0) {
        return FALSE;
    }
    mKeyboardLayouts->Add(-index - 1, csq);
    if (mCurrentKeyboardLayout.IsNull()) {
        mCurrentKeyboardLayout = keyboardLayout;
    }
    return TRUE;
}

Boolean PersistentDataStore::InputDeviceState::RemoveKeyboardLayout(
    /* [in] */ const String& keyboardLayout)
{
    AutoPtr<ICharSequence> csq;
    CString::New(keyboardLayout, (ICharSequence**)&csq);
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    Int32 index;
    coll->BinarySearch(IList::Probe(mKeyboardLayouts), csq, &index);
    if (index < 0) {
        return FALSE;
    }
    mKeyboardLayouts->Remove(index);
    UpdateCurrentKeyboardLayoutIfRemoved(keyboardLayout, index);
    return TRUE;
}

void PersistentDataStore::InputDeviceState::UpdateCurrentKeyboardLayoutIfRemoved(
    /* [in] */ const String& removedKeyboardLayout,
    /* [in] */ Int32 removedIndex)
{
    if (mCurrentKeyboardLayout.Equals(removedKeyboardLayout)) {
        Boolean isEmpty;
        if (mKeyboardLayouts->IsEmpty(&isEmpty), !isEmpty) {
            Int32 index = removedIndex;
            Int32 size;
            mKeyboardLayouts->GetSize(&size);
            if (index == size) {
                index = 0;
            }
            AutoPtr<IInterface> keyboardLayout;
            mKeyboardLayouts->Get(index, (IInterface**)&keyboardLayout);
            ICharSequence::Probe(keyboardLayout)->ToString(&mCurrentKeyboardLayout);
        }
        else {
            mCurrentKeyboardLayout = NULL;
        }
    }
}

Boolean PersistentDataStore::InputDeviceState::SwitchKeyboardLayout(
    /* [in] */ Int32 direction)
{
    Int32 size;
    mKeyboardLayouts->GetSize(&size);
    if (size < 2) {
        return FALSE;
    }
    AutoPtr<ICharSequence> csq;
    CString::New(mCurrentKeyboardLayout, (ICharSequence**)&csq);
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    Int32 index;
    coll->BinarySearch(IList::Probe(mKeyboardLayouts), csq, &index);
    assert(index >= 0);
    if (direction > 0) {
        index = (index + 1) % size;
    }
    else {
        index = (index + size - 1) % size;
    }
    AutoPtr<IInterface> keyboardLayout;
    mKeyboardLayouts->Get(index, (IInterface**)&keyboardLayout);
    ICharSequence::Probe(keyboardLayout)->ToString(&mCurrentKeyboardLayout);
    return TRUE;

}

Boolean PersistentDataStore::InputDeviceState::RemoveUninstalledKeyboardLayouts(
    /* [in] */ ISet* availableKeyboardLayouts)
{
    Boolean changed = FALSE;
    Int32 size;
    mKeyboardLayouts->GetSize(&size);
    for (Int32 i = size; i-- > 0; ) {
        AutoPtr<IInterface> keyboardLayout;
        mKeyboardLayouts->Get(i, (IInterface**)&keyboardLayout);
        Boolean isContained;
        if (availableKeyboardLayouts->Contains(keyboardLayout, &isContained), !isContained) {
            String keyboardLayoutStr;
            ICharSequence::Probe(keyboardLayout)->ToString(&keyboardLayoutStr);
            Slogger::I(TAG, "Removing uninstalled keyboard layout %s", keyboardLayoutStr.string());
            mKeyboardLayouts->Remove(i);
            UpdateCurrentKeyboardLayoutIfRemoved(keyboardLayoutStr, i);
            changed = TRUE;
        }
    }
    return changed;
}

// throws IOException, XmlPullParserException
ECode PersistentDataStore::InputDeviceState::LoadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<ITouchCalibrationHelper> helper;
    CTouchCalibrationHelper::AcquireSingleton((ITouchCalibrationHelper**)&helper);
    AutoPtr<ITouchCalibration> IDENTITY;
    helper->GetIDENTITY((ITouchCalibration**)&IDENTITY);

    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        parser->GetName(&name);
        if (name.Equals("keyboard-layout")) {
            String descriptor;
            parser->GetAttributeValue(String(NULL), String("descriptor"), &descriptor);
            if (descriptor.IsNull()) {
                //throw new XmlPullParserException(
                //        "Missing descriptor attribute on keyboard-layout.");
                Slogger::E(TAG, "Missing descriptor attribute on keyboard-layout.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            String current;
            parser->GetAttributeValue(String(NULL), String("current"), &current);
            AutoPtr<ICharSequence> csq;
            CString::New(descriptor, (ICharSequence**)&csq);
            Boolean result;
            if (mKeyboardLayouts->Contains(csq, &result), result) {
                //throw new XmlPullParserException(
                //        "Found duplicate keyboard layout.");
                Slogger::E(TAG, "Found duplicate keyboard layout.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            mKeyboardLayouts->Add(csq);
            if (!current.IsNull() && current.Equals("TRUE")) {
                if (!mCurrentKeyboardLayout.IsNull()) {
                    //throw new XmlPullParserException(
                    //        "Found multiple current keyboard layouts.");
                    Slogger::E(TAG, "Found multiple current keyboard layouts.");
                    return E_XML_PULL_PARSER_EXCEPTION;
                }
                mCurrentKeyboardLayout = descriptor;
            }
        }
        else if (name.Equals("calibration")) {
            String format;
            parser->GetAttributeValue(String(NULL), String("format"), &format);
            String rotation;
            parser->GetAttributeValue(String(NULL), String("rotation"), &rotation);
            Int32 r = -1;

            if (format.IsNull()) {
                //throw new XmlPullParserException(
                //        "Missing format attribute on calibration.");
                Slogger::E(TAG, "Missing format attribute on calibration.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            if (!format.Equals("affine")) {
                //throw new XmlPullParserException(
                //        "Unsupported format for calibration.");
                Slogger::E(TAG, "Unsupported format for calibration.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            if (!rotation.IsNull()) {
                ECode ec = StringToSurfaceRotation(rotation, &r);
                if (FAILED(ec)) {
                    //throw new XmlPullParserException(
                    //        "Unsupported rotation for calibration.");
                    Slogger::E(TAG, "Unsupported rotation for calibration.");
                    return E_XML_PULL_PARSER_EXCEPTION;
                }
            }

            AutoPtr<ArrayOf<Float> > matrix;
            IDENTITY->GetAffineTransform((ArrayOf<Float>**)&matrix);
            Int32 depth;
            parser->GetDepth(&depth);
            while (XmlUtils::NextElementWithin(parser, depth)) {
                String tag;
                parser->GetName(&tag);
                tag = tag.ToLowerCase();
                String value;
                parser->NextText(&value);

                for (Int32 i = 0; i < matrix->GetLength() && i < CALIBRATION_NAME->GetLength(); i++) {
                    if (tag.Equals((*CALIBRATION_NAME)[i])) {
                        (*matrix)[i] = StringUtils::ParseFloat(value);
                        break;
                    }
                }

                if (r == -1) {
                    // Assume calibration applies to all rotations
                    for (r = 0; r < mTouchCalibration->GetLength(); r++) {
                        AutoPtr<ITouchCalibration> tc;
                        CTouchCalibration::New((*matrix)[0], (*matrix)[1], (*matrix)[2],
                                (*matrix)[3], (*matrix)[4], (*matrix)[5], (ITouchCalibration**)&tc);
                        mTouchCalibration->Set(r, tc);
                    }
                }
                else {
                    AutoPtr<ITouchCalibration> tc;
                    CTouchCalibration::New((*matrix)[0], (*matrix)[1], (*matrix)[2],
                            (*matrix)[3], (*matrix)[4], (*matrix)[5], (ITouchCalibration**)&tc);
                    mTouchCalibration->Set(r, tc);
                }
            }
        }
    }

    // Maintain invariant that layouts are sorted.
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(IList::Probe(mKeyboardLayouts));

    // Maintain invariant that there is always a current keyboard layout unless
    // there are none installed.
    Boolean isEmpty;
    if (mCurrentKeyboardLayout.IsNull() && (mKeyboardLayouts->IsEmpty(&isEmpty), !isEmpty)) {
        AutoPtr<IInterface> keyboardLayout;
        mKeyboardLayouts->Get(0, (IInterface**)&keyboardLayout);
        ICharSequence::Probe(keyboardLayout)->ToString(&mCurrentKeyboardLayout);
    }
    return NOERROR;
}

// throws IOException
ECode PersistentDataStore::InputDeviceState::SaveToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    Int32 size;
    mKeyboardLayouts->GetSize(&size);
    for (Int32 i = 0;  i < size;  i++) {
        AutoPtr<IInterface> keyboardLayout;
        mKeyboardLayouts->Get(i, (IInterface**)&keyboardLayout);
        String layout;
        ICharSequence::Probe(keyboardLayout)->ToString(&layout);
        serializer->WriteStartTag(String(NULL), String("keyboard-layout"));
        serializer->WriteAttribute(String(NULL), String("descriptor"), layout);
        if (layout.Equals(mCurrentKeyboardLayout)) {
            serializer->WriteAttribute(String(NULL), String("current"), String("TRUE"));
        }
        serializer->WriteEndTag(String(NULL), String("keyboard-layout"));
    }

    for (Int32 i = 0;  i < mTouchCalibration->GetLength();  i++) {
        if ((*mTouchCalibration)[i] != NULL) {
            String rotation;
            FAIL_RETURN(SurfaceRotationToString(i, &rotation));
            AutoPtr<ArrayOf<Float> > transform;
            (*mTouchCalibration)[i]->GetAffineTransform((ArrayOf<Float>**)&transform);

            serializer->WriteStartTag(String(NULL), String("calibration"));
            serializer->WriteAttribute(String(NULL), String("format"), String("affine"));
            serializer->WriteAttribute(String(NULL), String("rotation"), rotation);
            for (Int32 j = 0;  j < transform->GetLength() && j < CALIBRATION_NAME->GetLength();  j++) {
                serializer->WriteStartTag(String(NULL), (*CALIBRATION_NAME)[j]);
                serializer->WriteText(StringUtils::ToString((*transform)[j]));
                serializer->WriteEndTag(String(NULL), (*CALIBRATION_NAME)[j]);
            }
            serializer->WriteEndTag(String(NULL), String("calibration"));
        }
    }
    return NOERROR;
}

ECode PersistentDataStore::InputDeviceState::SurfaceRotationToString(
    /* [in] */ Int32 surfaceRotation,
    /* [in] */ String* str)
{
    switch (surfaceRotation) {
        case ISurface::ROTATION_0:
            *str = "0";
            return NOERROR;
        case ISurface::ROTATION_90:
            *str = "90";
            return NOERROR;
        case ISurface::ROTATION_180:
            *str = "180";
            return NOERROR;
        case ISurface::ROTATION_270:
            *str = "270";
            return NOERROR;
    }
    //throw new IllegalArgumentException("Unsupported surface rotation value" + surfaceRotation);
    Slogger::E(TAG, "Unsupported surface rotation value%d", surfaceRotation);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode PersistentDataStore::InputDeviceState::StringToSurfaceRotation(
    /* [in] */ const String& str,
    /* [in] */ Int32* r)
{
    if (str.Equals("0")) {
        *r = ISurface::ROTATION_0;
        return NOERROR;
    }
    if (str.Equals("90")) {
        *r = ISurface::ROTATION_90;
        return NOERROR;
    }
    if (str.Equals("180")) {
        *r = ISurface::ROTATION_180;
        return NOERROR;
    }
    if (str.Equals("270")) {
        *r = ISurface::ROTATION_270;
        return NOERROR;
    }
    //throw new IllegalArgumentException("Unsupported surface rotation string '" + s + "'");
    Slogger::E(TAG, "Unsupported surface rotation string '%s'", str.string());
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}


//==============================================================================
//  PersistentDataStore
//==============================================================================
const String PersistentDataStore::TAG("InputManager");

PersistentDataStore::PersistentDataStore()
    : mLoaded(FALSE)
    , mDirty(FALSE)
{
    AutoPtr<IFile> file;
    CFile::New(String("/data/system/input-manager-state.xml"), (IFile**)&file);
    CAtomicFile::New(file, (IAtomicFile**)&mAtomicFile);
}

void PersistentDataStore::SaveIfNeeded()
{
    if (mDirty) {
        Save();
        mDirty = FALSE;
    }
}

AutoPtr<ITouchCalibration> PersistentDataStore::GetTouchCalibration(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 surfaceRotation)
{
    AutoPtr<ITouchCalibrationHelper> helper;
    CTouchCalibrationHelper::AcquireSingleton((ITouchCalibrationHelper**)&helper);
    AutoPtr<ITouchCalibration> IDENTITY;
    helper->GetIDENTITY((ITouchCalibration**)&IDENTITY);

    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, FALSE);
    if (state == NULL) {
        return IDENTITY;
    }

    AutoPtr<ITouchCalibration> cal = state->GetTouchCalibration(surfaceRotation);
    if (cal == NULL) {
        return IDENTITY;
    }
    return cal;
}

Boolean PersistentDataStore::SetTouchCalibration(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 surfaceRotation,
    /* [in] */ ITouchCalibration* calibration)
{
    AutoPtr<InputDeviceState> state = GetInputDeviceState(inputDeviceDescriptor, TRUE);

    if (state->SetTouchCalibration(surfaceRotation, calibration)) {
        SetDirty();
        return TRUE;
    }

    return FALSE;
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

AutoPtr< ArrayOf<String> > PersistentDataStore::GetKeyboardLayouts(
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
    /* [in] */ ISet* availableKeyboardLayouts)
{
    Boolean changed = FALSE;
    HashMap<String, AutoPtr<InputDeviceState> >::Iterator it;
    for (it = mInputDevices.Begin(); it != mInputDevices.End(); ++it) {
        InputDeviceState* state = it->mSecond;
        if (state->RemoveUninstalledKeyboardLayouts(availableKeyboardLayouts)) {
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
    AutoPtr<InputDeviceState> state;
    HashMap<String, AutoPtr<InputDeviceState> >::Iterator it = mInputDevices.Find(inputDeviceDescriptor);
    if (it != mInputDevices.End()) {
        state = it->mSecond;
    }
    if (state == NULL && createIfAbsent) {
        state = new InputDeviceState();
        mInputDevices[inputDeviceDescriptor] = state;
        SetDirty();
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

    //try {
    AutoPtr<IFileInputStream> stream;
    ECode ec = mAtomicFile->OpenRead((IFileInputStream**)&stream);
    if (FAILED(ec)) return;
    AutoPtr<IInputStream> is = IInputStream::Probe(stream);
    //} catch (FileNotFoundException ex) {
    //    return;
    //}

    AutoPtr<IXmlPullParser> parser;
    //try {
    Xml::NewPullParser((IXmlPullParser**)&parser);
    AutoPtr<IBufferedInputStream> bis;
    CBufferedInputStream::New(is, (IBufferedInputStream**)&bis);
    parser->SetInput(IInputStream::Probe(bis), String(NULL));
    ec = LoadFromXml(parser);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(CInputManagerService::TAG, "Failed to load input manager persistent store data.");
        ClearState();
    }
    else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Slogger::W(CInputManagerService::TAG, "Failed to load input manager persistent store data.");
        ClearState();
    }
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(is));
    /*
    } catch (IOException ex) {
        Slog.w(InputManagerService.TAG, "Failed to load input manager persistent store data.", ex);
        clearState();
    } catch (XmlPullParserException ex) {
        Slog.w(InputManagerService.TAG, "Failed to load input manager persistent store data.", ex);
        clearState();
    } finally {
    */
}

void PersistentDataStore::Save()
{
    AutoPtr<IFileOutputStream> os;
    ECode ec = mAtomicFile->StartWrite((IFileOutputStream**)&os);
    if (FAILED(ec)) {
        Slogger::W(CInputManagerService::TAG, "Failed to save input manager persistent store data.");
        return;
    }
    Boolean success = FALSE;

    AutoPtr<IXmlSerializer> serializer;
    CFastXmlSerializer::New((IXmlSerializer**)&serializer);
    AutoPtr<IOutputStream> out;
    CBufferedOutputStream::New(IOutputStream::Probe(os), (IOutputStream**)&out);
    serializer->SetOutput(out, String("utf-8"));
    ec = SaveToXml(serializer);
    if (FAILED(ec)) goto EXIT;
    ec = serializer->Flush();
    if (FAILED(ec)) goto EXIT;
    success = TRUE;
EXIT:
    if (success) {
        ec = mAtomicFile->FinishWrite(os);
    }
    else {
        ec = mAtomicFile->FailWrite(os);
    }
    if (FAILED(ec)) {
        Slogger::W(CInputManagerService::TAG, "Failed to save input manager persistent store data.");
    }
}

// throws IOException, XmlPullParserException
ECode PersistentDataStore::LoadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    XmlUtils::BeginDocument(parser, String("input-manager-state"));
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        parser->GetName(&name);
        if (name.Equals("input-devices")) {
            return LoadInputDevicesFromXml(parser);
        }
    }
    return NOERROR;
}

// throws IOException, XmlPullParserException
ECode PersistentDataStore::LoadInputDevicesFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        parser->GetName(&name);
        if (name.Equals("input-devices")) {
            String descriptor;
            parser->GetAttributeValue(String(NULL), String("descriptor"), &descriptor);
            if (descriptor.IsNull()) {
                //throw new XmlPullParserException(
                //        "Missing descriptor attribute on input-device.");
                Slogger::E(TAG, "Missing descriptor attribute on input-device.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            if (mInputDevices.Find(descriptor) != mInputDevices.End()) {
                //throw new XmlPullParserException("Found duplicate input device.");
                Slogger::E(TAG, "Found duplicate input device.");
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            AutoPtr<InputDeviceState> state = new InputDeviceState();
            state->LoadFromXml(parser);
            mInputDevices[descriptor] = state;
        }
    }
    return NOERROR;
}

//  throws IOException
ECode PersistentDataStore::SaveToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    serializer->StartDocument(String(NULL), TRUE);
    serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
    serializer->WriteStartTag(String(NULL), String("input-manager-state"));
    serializer->WriteStartTag(String(NULL), String("input-devices"));
    HashMap<String, AutoPtr<InputDeviceState> >::Iterator it;
    for (it = mInputDevices.Begin(); it != mInputDevices.End(); ++it) {
        String descriptor = it->mFirst;
        InputDeviceState* state = it->mSecond;
        serializer->WriteStartTag(String(NULL), String("input-device"));
        serializer->WriteAttribute(String(NULL), String("descriptor"), descriptor);
        state->SaveToXml(serializer);
        serializer->WriteEndTag(String(NULL), String("input-device"));
    }
    serializer->WriteEndTag(String(NULL), String("input-devices"));
    serializer->WriteEndTag(String(NULL), String("input-manager-state"));
    serializer->EndDocument();
    return NOERROR;
}

} // Input
} // Server
} // Droid
} // Elastos
