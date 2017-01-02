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

#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "R.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telecom.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/R.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Telecom::CPhoneAccountBuilder;
using Elastos::Droid::Telecom::CPhoneAccountHandle;
using Elastos::Droid::Telecom::IConnectionService;
using Elastos::Droid::Telecom::IPhoneAccountBuilder;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Objects;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// PhoneAccountRegistrar::Listener
//=============================================================================
ECode PhoneAccountRegistrar::Listener::OnAccountsChanged(
    /* [in] */ PhoneAccountRegistrar* registrar)
{
    return NOERROR;
}

ECode PhoneAccountRegistrar::Listener::OnDefaultOutgoingChanged(
    /* [in] */ PhoneAccountRegistrar* registrar)
{
    return NOERROR;
}

ECode PhoneAccountRegistrar::Listener::OnSimCallManagerChanged(
    /* [in] */ PhoneAccountRegistrar* registrar)
{
    return NOERROR;
}

//=============================================================================
// PhoneAccountRegistrar::State
//=============================================================================
PhoneAccountRegistrar::State::State()
    : mVersionNumber(0)
{
    CArrayList::New((IList**)&mAccounts);
}

//=============================================================================
// PhoneAccountRegistrar::XmlSerialization
//=============================================================================
const String PhoneAccountRegistrar::XmlSerialization::LENGTH_ATTRIBUTE("length");
const String PhoneAccountRegistrar::XmlSerialization::VALUE_TAG("value");

//ECode PhoneAccountRegistrar::XmlSerialization::ReadFromXml(
//    /* [in] */ IXmlPullParser* parser,
//    /* [in] */ Int32 version,
//    /* [in] */ IContext* context,
//    /* [out] */ IInterface** result)
//{
//    VALIDATE_NOT_NULL(result)
//    *result = NULL;
//
//    return NOERROR;
//}

ECode PhoneAccountRegistrar::XmlSerialization::WriteTextSafely(
    /* [in] */ const String& tagName,
    /* [in] */ IInterface* value,
    /* [in] */ IXmlSerializer* serializer)
{
    if (value != NULL) {
        serializer->WriteStartTag(String(NULL), tagName);
        serializer->WriteText(TO_STR(value));
        serializer->WriteEndTag(String(NULL), tagName);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::XmlSerialization::WriteStringList(
    /* [in] */ const String& tagName,
    /* [in] */ IList* values,
    /* [in] */ IXmlSerializer* serializer)
{
    serializer->WriteStartTag(String(NULL), tagName);
    if (values != NULL) {
        Int32 valuesSize;
        values->GetSize(&valuesSize);
        serializer->WriteAttribute(String(NULL), LENGTH_ATTRIBUTE, StringUtils::ToString(valuesSize));
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String toSerialize;
            IObject::Probe(obj)->ToString(&toSerialize);
            serializer->WriteStartTag(String(NULL), VALUE_TAG);
            if (toSerialize != NULL ){
                serializer->WriteText(toSerialize);
            }
            serializer->WriteEndTag(String(NULL), VALUE_TAG);
        }
    } else {
        serializer->WriteAttribute(String(NULL), LENGTH_ATTRIBUTE, String("0"));
    }
    serializer->WriteEndTag(String(NULL), tagName);
    return NOERROR;
}

ECode PhoneAccountRegistrar::XmlSerialization::ReadStringList(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    String attrValue;
    parser->GetAttributeValue(String(NULL), LENGTH_ATTRIBUTE, &attrValue);
    Int32 length = StringUtils::ParseInt32(attrValue);
    AutoPtr<IList> arrayEntries;
    CArrayList::New(length, (IList**)&arrayEntries);
    String value(NULL);
    if (length == 0) {
        *result = arrayEntries;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        parser->GetName(&name);
        if (name.Equals(VALUE_TAG)) {
            Int32 next;
            parser->Next(&next);
            parser->GetText(&value);
            arrayEntries->Add(StringUtils::ParseCharSequence(value));
        }
    }
    *result = arrayEntries;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=============================================================================
// PhoneAccountRegistrar::SubStateXmlSerialization
//=============================================================================
const String PhoneAccountRegistrar::SubStateXmlSerialization::CLASS_STATE("phone_account_registrar_state");
const String PhoneAccountRegistrar::SubStateXmlSerialization::DEFAULT_OUTGOING("default_outgoing");
const String PhoneAccountRegistrar::SubStateXmlSerialization::SIM_CALL_MANAGER("sim_call_manager");
const String PhoneAccountRegistrar::SubStateXmlSerialization::ACCOUNTS("accounts");
const String PhoneAccountRegistrar::SubStateXmlSerialization::VERSION("version");

ECode PhoneAccountRegistrar::SubStateXmlSerialization::WriteToXml(
    /* [in] */ IInterface* _o,
    /* [in] */ IXmlSerializer* serializer)
{
    AutoPtr<State> o = (State*) IObject::Probe(_o);
    if (o != NULL) {
        serializer->WriteStartTag(String(NULL), CLASS_STATE);
        serializer->WriteAttribute(String(NULL), VERSION, StringUtils::ToString(EXPECTED_STATE_VERSION));
        if (o->mDefaultOutgoing != NULL) {
            serializer->WriteStartTag(String(NULL), DEFAULT_OUTGOING);
            sPhoneAccountHandleXml->WriteToXml(o->mDefaultOutgoing, serializer);
            serializer->WriteEndTag(String(NULL), DEFAULT_OUTGOING);
        }
        if (o->mSimCallManager != NULL) {
            serializer->WriteStartTag(String(NULL), SIM_CALL_MANAGER);
            sPhoneAccountHandleXml->WriteToXml(o->mSimCallManager, serializer);
            serializer->WriteEndTag(String(NULL), SIM_CALL_MANAGER);
        }
        serializer->WriteStartTag(String(NULL), ACCOUNTS);
        AutoPtr<IIterator> it;
        o->mAccounts->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPhoneAccount> m = IPhoneAccount::Probe(obj);
            sPhoneAccountXml->WriteToXml(m, serializer);
        }
        serializer->WriteEndTag(String(NULL), ACCOUNTS);
        serializer->WriteEndTag(String(NULL), CLASS_STATE);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::SubStateXmlSerialization::ReadFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 version,
    /* [in] */ IContext* context,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    String name;
    parser->GetName(&name);
    if (name.Equals(CLASS_STATE)) {
        AutoPtr<State> s = new State();
        String rawVersion;
        parser->GetAttributeValue(String(NULL), VERSION, &rawVersion);
        s->mVersionNumber = TextUtils::IsEmpty(rawVersion) ? 1 :
                StringUtils::ParseInt32(rawVersion);
        Int32 outerDepth;
        parser->GetDepth(&outerDepth);
        while (XmlUtils::NextElementWithin(parser, outerDepth)) {
            parser->GetName(&name);
            if (name.Equals(DEFAULT_OUTGOING)) {
                Int32 nextTag;
                parser->NextTag(&nextTag);
                AutoPtr<IInterface> obj;
                sPhoneAccountHandleXml->ReadFromXml(parser,
                        s->mVersionNumber, context, (IInterface**)&obj);
                s->mDefaultOutgoing = IPhoneAccountHandle::Probe(obj);
            } else if (name.Equals(SIM_CALL_MANAGER)) {
                Int32 nextTag;
                parser->NextTag(&nextTag);
                AutoPtr<IInterface> obj;
                sPhoneAccountHandleXml->ReadFromXml(parser,
                        s->mVersionNumber, context, (IInterface**)&obj);
                s->mSimCallManager = IPhoneAccountHandle::Probe(obj);
            } else if (name.Equals(ACCOUNTS)) {
                Int32 accountsDepth;
                parser->GetDepth(&accountsDepth);
                while (XmlUtils::NextElementWithin(parser, accountsDepth)) {
                    AutoPtr<IInterface> obj;
                    sPhoneAccountXml->ReadFromXml(parser,
                            s->mVersionNumber, context, (IInterface**)&obj);
                    AutoPtr<IPhoneAccount> account = IPhoneAccount::Probe(obj);
                    if (account != NULL && s->mAccounts != NULL) {
                        s->mAccounts->Add(account);
                    }
                }
            }
        }
        *result = TO_IINTERFACE(s);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

//=============================================================================
// PhoneAccountRegistrar::SubPhoneAccountXmlSerialization
//=============================================================================
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::CLASS_PHONE_ACCOUNT("phone_account");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::ACCOUNT_HANDLE("account_handle");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::ADDRESS("handle");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::SUBSCRIPTION_ADDRESS("subscription_number");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::CAPABILITIES("capabilities");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::ICON_RES_ID("icon_res_id");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::LABEL("label");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::SHORT_DESCRIPTION("short_description");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::SUPPORTED_URI_SCHEMES("supported_uri_schemes");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::sTRUE("true");
const String PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::sFALSE("false");

ECode PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::WriteToXml(
    /* [in] */ IInterface* _o,
    /* [in] */ IXmlSerializer* serializer)
{
    AutoPtr<IPhoneAccount> o = IPhoneAccount::Probe(_o);
    if (o != NULL) {
        serializer->WriteStartTag(String(NULL), CLASS_PHONE_ACCOUNT);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        o->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        if (phoneAccountHandle != NULL) {
            serializer->WriteStartTag(String(NULL), ACCOUNT_HANDLE);
            sPhoneAccountHandleXml->WriteToXml(phoneAccountHandle, serializer);
            serializer->WriteEndTag(String(NULL), ACCOUNT_HANDLE);
        }
        AutoPtr<IUri> address;
        o->GetAddress((IUri**)&address);
        WriteTextSafely(ADDRESS, address, serializer);
        AutoPtr<IUri> subscriptionAddress;
        o->GetSubscriptionAddress((IUri**)&subscriptionAddress);
        WriteTextSafely(SUBSCRIPTION_ADDRESS, (subscriptionAddress), serializer);
        Int32 capabilities;
        o->GetCapabilities(&capabilities);
        AutoPtr<IInteger32> i32;
        CInteger32::New(capabilities, (IInteger32**)&i32);
        WriteTextSafely(CAPABILITIES, i32, serializer);
        Int32 iconResId;
        o->GetIconResId(&iconResId);
        i32 = NULL;
        CInteger32::New(iconResId, (IInteger32**)&i32);
        WriteTextSafely(ICON_RES_ID, i32, serializer);
        AutoPtr<ICharSequence> label;
        o->GetLabel((ICharSequence**)&label);
        WriteTextSafely(LABEL, label, serializer);
        AutoPtr<ICharSequence> shortDescription;
        o->GetShortDescription((ICharSequence**)&shortDescription);
        WriteTextSafely(SHORT_DESCRIPTION, shortDescription, serializer);
        AutoPtr<IList> supportedUriSchemes;
        o->GetSupportedUriSchemes((IList**)&supportedUriSchemes);
        WriteStringList(SUPPORTED_URI_SCHEMES, supportedUriSchemes, serializer);
        serializer->WriteEndTag(String(NULL), CLASS_PHONE_ACCOUNT);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::ReadFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 version,
    /* [in] */ IContext* context,
    /* [out] */ IInterface** result)// IPhoneAccount
{
    VALIDATE_NOT_NULL(result)

    String name;
    parser->GetName(&name);
    if (name.Equals(CLASS_PHONE_ACCOUNT)) {
        Int32 outerDepth;
        parser->GetDepth(&outerDepth);
        AutoPtr<IPhoneAccountHandle> accountHandle;
        AutoPtr<IUri> address;
        AutoPtr<IUri> subscriptionAddress;
        Int32 capabilities = 0;
        Int32 iconResId = 0;
        String label(NULL);
        String shortDescription(NULL);
        AutoPtr<IList> supportedUriSchemes = NULL;
        while (XmlUtils::NextElementWithin(parser, outerDepth)) {
            parser->GetName(&name);
            if (name.Equals(ACCOUNT_HANDLE)) {
                Int32 nextTag;
                parser->NextTag(&nextTag);
                AutoPtr<IInterface> obj;
                sPhoneAccountHandleXml->ReadFromXml(parser, version,
                        context, (IInterface**)&obj);
                accountHandle = IPhoneAccountHandle::Probe(obj);
            } else if (name.Equals(ADDRESS)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                String text;
                parser->GetText(&text);
                Uri::Parse(text, (IUri**)&address);
            } else if (name.Equals(SUBSCRIPTION_ADDRESS)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                String nextText;
                parser->GetText(&nextText);
                if (nextText != NULL) {
                    Uri::Parse(nextText, (IUri**)&subscriptionAddress);
                }
            } else if (name.Equals(CAPABILITIES)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                String text;
                parser->GetText(&text);
                capabilities = StringUtils::ParseInt32(text);
            } else if (name.Equals(ICON_RES_ID)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                String text;
                parser->GetText(&text);
                iconResId = StringUtils::ParseInt32(text);
            } else if (name.Equals(LABEL)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                String text;
                parser->GetText(&text);
                label = text;
            } else if (name.Equals(SHORT_DESCRIPTION)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                String text;
                parser->GetText(&text);
                shortDescription = text;
            } else if (name.Equals(SUPPORTED_URI_SCHEMES)) {
                ReadStringList(parser, (IList**)&supportedUriSchemes);
            }
        }
        // Upgrade older phone accounts to specify the supported URI schemes.
        if (version < 2) {
            AutoPtr<IComponentName> sipComponentName;
            CComponentName::New(String("com.android.phone"),
                    String("com.android.services.telephony.sip.SipConnectionService"), (IComponentName**)&sipComponentName);
            CArrayList::New((IList**)&supportedUriSchemes);
            // Handle the SIP connection service.
            // Check the system settings to see if it also should handle "tel" calls.
            AutoPtr<IComponentName> componentName;
            accountHandle->GetComponentName((IComponentName**)&componentName);
            Boolean isEquals = Object::Equals(componentName, sipComponentName);
            if (isEquals) {
                Boolean useSipForPstn;
                UseSipForPstnCalls(context, &useSipForPstn);
                supportedUriSchemes->Add(StringUtils::ParseCharSequence(IPhoneAccount::SCHEME_SIP));
                if (useSipForPstn) {
                    supportedUriSchemes->Add(StringUtils::ParseCharSequence(IPhoneAccount::SCHEME_TEL));
                }
            } else {
                supportedUriSchemes->Add(StringUtils::ParseCharSequence(IPhoneAccount::SCHEME_TEL));
                supportedUriSchemes->Add(StringUtils::ParseCharSequence(IPhoneAccount::SCHEME_VOICEMAIL));
            }
        }
        AutoPtr<IPhoneAccountBuilder> builder;
        CPhoneAccountBuilder::New(accountHandle, StringUtils::ParseCharSequence(label), (IPhoneAccountBuilder**)&builder);
        builder->SetAddress(address);
        builder->SetSubscriptionAddress(subscriptionAddress);
        builder->SetCapabilities(capabilities);
        builder->SetIconResId(iconResId);
        builder->SetShortDescription(StringUtils::ParseCharSequence(shortDescription));
        builder->SetSupportedUriSchemes(supportedUriSchemes);
        AutoPtr<IPhoneAccount> phoneAccount;
        builder->Build((IPhoneAccount**)&phoneAccount);
        *result = phoneAccount;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode PhoneAccountRegistrar::SubPhoneAccountXmlSerialization::UseSipForPstnCalls(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String option;
    Settings::System::GetString(contentResolver,
            ISettingsSystem::SIP_CALL_OPTIONS, &option);
    option = (option != NULL) ? option : ISettingsSystem::SIP_ADDRESS_ONLY;
    *result = option.Equals(ISettingsSystem::SIP_ALWAYS);
    return NOERROR;
}

//=============================================================================
// PhoneAccountRegistrar::SubPhoneAccountHandleXmlSerialization
//=============================================================================
const String PhoneAccountRegistrar::SubPhoneAccountHandleXmlSerialization::CLASS_PHONE_ACCOUNT_HANDLE("phone_account_handle");
const String PhoneAccountRegistrar::SubPhoneAccountHandleXmlSerialization::COMPONENT_NAME("component_name");
const String PhoneAccountRegistrar::SubPhoneAccountHandleXmlSerialization::ID("id");

ECode PhoneAccountRegistrar::SubPhoneAccountHandleXmlSerialization::WriteToXml(
    /* [in] */ IInterface* _o,
    /* [in] */ IXmlSerializer* serializer)
{
    AutoPtr<IPhoneAccountHandle> o = IPhoneAccountHandle::Probe(_o);
    if (o != NULL) {
        serializer->WriteStartTag(String(NULL), CLASS_PHONE_ACCOUNT_HANDLE);
        AutoPtr<IComponentName> componentName;
        o->GetComponentName((IComponentName**)&componentName);
        if (componentName != NULL) {
            AutoPtr<IComponentName> componentName;
            o->GetComponentName((IComponentName**)&componentName);
            String s;
            componentName->FlattenToString(&s);
            WriteTextSafely(COMPONENT_NAME, StringUtils::ParseCharSequence(s), serializer);
        }
        String id;
        o->GetId(&id);
        WriteTextSafely(ID, StringUtils::ParseCharSequence(id), serializer);
        serializer->WriteEndTag(String(NULL), CLASS_PHONE_ACCOUNT_HANDLE);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::SubPhoneAccountHandleXmlSerialization::ReadFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 version,
    /* [in] */ IContext* context,
    /* [out] */ IInterface** result)// IPhoneAccountHandle
{
    VALIDATE_NOT_NULL(result)

    String name;
    parser->GetName(&name);
    if (name.Equals(CLASS_PHONE_ACCOUNT_HANDLE)) {
        String componentNameString(NULL);
        String idString(NULL);
        Int32 outerDepth;
        parser->GetDepth(&outerDepth);
        while (XmlUtils::NextElementWithin(parser, outerDepth)) {
            parser->GetName(&name);
            if (name.Equals(COMPONENT_NAME)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                parser->GetText(&componentNameString);
            } else if (name.Equals(ID)) {
                Int32 iNotUsed;
                parser->Next(&iNotUsed);
                parser->GetText(&idString);
            }
        }
        if (componentNameString != NULL) {
            AutoPtr<IComponentNameHelper> helper;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
            AutoPtr<IComponentName> componentName;
            helper->UnflattenFromString(componentNameString, (IComponentName**)&componentName);
            AutoPtr<IPhoneAccountHandle> pah;
            CPhoneAccountHandle::New(
                    componentName,
                    idString, (IPhoneAccountHandle**)&pah);
            *result = pah;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

//=============================================================================
// PhoneAccountRegistrar
//=============================================================================
AutoPtr<IPhoneAccountHandle> PhoneAccountRegistrar::NO_ACCOUNT_SELECTED = InitNO_ACCOUNT_SELECTED();
const Int32 PhoneAccountRegistrar::EXPECTED_STATE_VERSION = 3;
const String PhoneAccountRegistrar::FILE_NAME("phone-account-registrar-state.xml");
const String PhoneAccountRegistrar::SIP_SHARED_PREFERENCES("SIP_PREFERENCES");
AutoPtr<PhoneAccountRegistrar::XmlSerialization> PhoneAccountRegistrar::sStateXml = new SubStateXmlSerialization();
AutoPtr<PhoneAccountRegistrar::XmlSerialization> PhoneAccountRegistrar::sPhoneAccountXml = new SubPhoneAccountXmlSerialization();
AutoPtr<PhoneAccountRegistrar::XmlSerialization> PhoneAccountRegistrar::sPhoneAccountHandleXml = new SubPhoneAccountHandleXmlSerialization();

PhoneAccountRegistrar::PhoneAccountRegistrar()
{
    CCopyOnWriteArrayList::New((IList**)&mListeners);
}

ECode PhoneAccountRegistrar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, FILE_NAME);
}

ECode PhoneAccountRegistrar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& fileName)
{
    // TODO: This file path is subject to change -- it is storing the phone account registry
    // state file in the path /data/system/users/0/, which is likely not correct in a
    // multi-user setting.
    /** UNCOMMENT_FOR_MOVE_TO_SYSTEM_SERVICE
    String filePath = Environment.getUserSystemDirectory(UserHandle.myUserId()).
            getAbsolutePath();
    mAtomicFile = new AtomicFile(new File(filePath, fileName));
     UNCOMMENT_FOR_MOVE_TO_SYSTEM_SERVICE */
    AutoPtr<IFile> filesDir;
    context->GetFilesDir((IFile**)&filesDir);
    AutoPtr<IFile> file;
    CFile::New(filesDir, fileName, (IFile**)&file);
    CAtomicFile::New(file, (IAtomicFile**)&mAtomicFile);
    mState = new State();
    mContext = context;
    Read();
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetDefaultOutgoingPhoneAccount(
    /* [in] */ const String& uriScheme,
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhoneAccountHandle> userSelected;
    GetUserSelectedOutgoingPhoneAccount((IPhoneAccountHandle**)&userSelected);
    if (userSelected != NULL) {
        // If there is a default PhoneAccount, ensure it supports calls to handles with the
        // specified uriScheme.
        AutoPtr<IPhoneAccount> userSelectedAccount;
        GetPhoneAccount(userSelected, (IPhoneAccount**)&userSelectedAccount);
        Boolean isSupportsUriScheme;
        userSelectedAccount->SupportsUriScheme(uriScheme, &isSupportsUriScheme);
        if (isSupportsUriScheme) {
            *result = userSelected;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    AutoPtr<IList> outgoing;
    GetCallCapablePhoneAccounts(uriScheme, (IList**)&outgoing);
    Int32 outgoingSize;
    outgoing->GetSize(&outgoingSize);
    switch (outgoingSize) {
        case 0:
            // There are no accounts, so there can be no default
            *result = NULL;
            return NOERROR;
        case 1: {
            // There is only one account, which is by definition the default
            AutoPtr<IInterface> obj;
            outgoing->Get(0, (IInterface**)&obj);
            *result = IPhoneAccountHandle::Probe(obj);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        default:
            // There are multiple accounts with no selected default
            *result = NULL;
            return NOERROR;
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetUserSelectedOutgoingPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
    AutoPtr<ITelephonyManager> telephonyManager;
    telephonyManagerHelper->GetDefault((ITelephonyManager**)&telephonyManager);
    Int32 phoneCount;
    telephonyManager->GetPhoneCount(&phoneCount);
    if (phoneCount > 1) {
        return GetUserSelectedVoicePhoneAccount(result);
    }
    if (mState->mDefaultOutgoing != NULL) {
        // Return the registered outgoing default iff it still exists (we keep a sticky
        // default to survive account deletion and re-addition)
        Int32 accountsSize;
        mState->mAccounts->GetSize(&accountsSize);
        for (Int32 i = 0; i < accountsSize; i++) {
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            AutoPtr<IInterface> obj;
            mState->mAccounts->Get(i, (IInterface**)&obj);
            IPhoneAccount::Probe(obj)->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
            Boolean isEquals;
            IObject::Probe(phoneAccountHandle)->Equals(mState->mDefaultOutgoing, &isEquals);
            if (isEquals) {
                *result = mState->mDefaultOutgoing;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
        // At this point, there was a registered default but it has been deleted; proceed
        // as though there were no default
    }
    *result = NULL;
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetUserSelectedVoicePhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISubscriptionManager> subscriptionManagerHelper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&subscriptionManagerHelper);
    Int64 voiceSubId;
    subscriptionManagerHelper->GetDefaultVoiceSubId(&voiceSubId);
    Boolean isVoicePrompt;
    assert(0 && "ISubscriptionManager");
    // subscriptionManagerHelper->IsVoicePromptEnabled(&isVoicePrompt);
    AutoPtr<IPhoneAccountHandle> prefPhoneAccount;
    Log::I("PhoneAccountRegistrar", "getUserSelVoicePhoneAccount, voice subId = %lld prompt = %d", voiceSubId,
            isVoicePrompt);
    if (!isVoicePrompt) {
        Int32 accountsSize;
        mState->mAccounts->GetSize(&accountsSize);
        for (Int32 i = 0; i < accountsSize; i++) {
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            AutoPtr<IInterface> obj;
            mState->mAccounts->Get(i, (IInterface**)&obj);
            IPhoneAccount::Probe(obj)->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
            String id;
            phoneAccountHandle->GetId(&id);
            // emergency account present return it
            if (id.Equals("E")) {
               Log::I("PhoneAccountRegistrar", "getUserSelVoicePhoneAccount, emergency account ");
               *result = phoneAccountHandle;
               REFCOUNT_ADD(*result)
               return NOERROR;
            }
            Int64 subId = StringUtils::ParseInt64(id);
            Log::I("PhoneAccountRegistrar", "getUserSelectedVoicePhoneAccount, voice subId = %lld subId = %lld mId = %s", voiceSubId, subId, id.string());
            if (subId == voiceSubId) {
                prefPhoneAccount = phoneAccountHandle;
                break;
            }
        }
    }
    *result = prefPhoneAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccountRegistrar::SetUserSelectedOutgoingPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (accountHandle == NULL) {
        // Asking to clear the default outgoing is a valid request
        mState->mDefaultOutgoing = NULL;
    } else {
        Boolean found = FALSE;
        AutoPtr<IIterator> it;
        mState->mAccounts->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPhoneAccount> m = IPhoneAccount::Probe(obj);
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            m->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
            if (Objects::Equals(accountHandle, phoneAccountHandle.Get())) {
                found = TRUE;
                break;
            }
        }
        if (!found) {
            Log::W("PhoneAccountRegistrar", "Trying to set nonexistent default outgoing %s",
                    TO_CSTR(accountHandle));
            return NOERROR;
        }
        AutoPtr<IPhoneAccount> phoneAccount;
        GetPhoneAccount(accountHandle, (IPhoneAccount**)&phoneAccount);
        Boolean hasCapabilities;
        phoneAccount->HasCapabilities(IPhoneAccount::CAPABILITY_CALL_PROVIDER, &hasCapabilities);
        if (!hasCapabilities) {
            Log::W("PhoneAccountRegistrar", "Trying to set non-call-provider default outgoing %s",
                    TO_CSTR(accountHandle));
            return NOERROR;
        }
        mState->mDefaultOutgoing = accountHandle;
    }
    SetDefaultVoicePhoneAccount(mState->mDefaultOutgoing);
    Write();
    FireDefaultOutgoingChanged();
    return NOERROR;
}

ECode PhoneAccountRegistrar::SetDefaultVoicePhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    AutoPtr<ISubscriptionManager> subscriptionManagerHelper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&subscriptionManagerHelper);
    Boolean voicePrompt;
    assert(0 && "TODO ISubscriptionManager");
    // subscriptionManagerHelper->IsVoicePromptEnabled(&voicePrompt);
    Log::D("PhoneAccountRegistrar", "set voice default, prompt = %d", voicePrompt);
    if (mState->mDefaultOutgoing == NULL) {
        AutoPtr<IList> outgoing;
        GetCallCapablePhoneAccounts((IList**)&outgoing);
        Int32 outgoingSize;
        outgoing->GetSize(&outgoingSize);
        if ((outgoing != NULL) && (outgoingSize > 1) && (voicePrompt != TRUE)) {
            assert(0 && "TODO ISubscriptionManager");
            // subscriptionManagerHelper->SetVoicePromptEnabled(TRUE);
        }
    } else {
        String id;
        mState->mDefaultOutgoing->GetId(&id);
        // Return from here, only emergency account available
        if (id.Equals("E")) {
            Log::I("PhoneAccountRegistrar", "setDefaultVoicePhoneAccount, only emergency account present ");
            return NOERROR;
        }
        Int64 subId = StringUtils::ParseInt64(id);
        Log::I("PhoneAccountRegistrar", "set voice default subId as %lld prmotp = %d", subId, voicePrompt);
        Int64 defaultVoiceSubId;
        subscriptionManagerHelper->GetDefaultVoiceSubId(&defaultVoiceSubId);
        if (defaultVoiceSubId != subId) {
            subscriptionManagerHelper->SetDefaultVoiceSubId(subId);
        }
        if (voicePrompt == TRUE) {
            assert(0 && "TODO ISubscriptionManager");
            // subscriptionManagerHelper->SetVoicePromptEnabled(FALSE);
        }
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::SetSimCallManager(
    /* [in] */ IPhoneAccountHandle* callManager)
{
    if (callManager != NULL) {
        AutoPtr<IPhoneAccount> callManagerAccount;
        GetPhoneAccount(callManager, (IPhoneAccount**)&callManagerAccount);
        Boolean hasCapabilities;
        callManagerAccount->HasCapabilities(
                IPhoneAccount::CAPABILITY_CONNECTION_MANAGER, &hasCapabilities);
        if (callManagerAccount == NULL) {
            Log::D("PhoneAccountRegistrar", "setSimCallManager: Nonexistent call manager: %s", TO_CSTR(callManager));
            return NOERROR;
        } else if (!hasCapabilities) {
            Log::D("PhoneAccountRegistrar", "setSimCallManager: Not a call manager: %s", TO_CSTR(callManagerAccount));
            return NOERROR;
        }
    } else {
        callManager = NO_ACCOUNT_SELECTED;
    }
    mState->mSimCallManager = callManager;
    Write();
    FireSimCallManagerChanged();
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetSimCallManager(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    if (mState->mSimCallManager != NULL) {
        Boolean isEquals;
        IObject::Probe(NO_ACCOUNT_SELECTED)->Equals(mState->mSimCallManager, &isEquals);
        if (isEquals) {
            *result = NULL;
            return NOERROR;
        }
        // Return the registered sim call manager iff it still exists (we keep a sticky
        // setting to survive account deletion and re-addition)
        Int32 accountsSize;
        mState->mAccounts->GetSize(&accountsSize);
        for (Int32 i = 0; i < accountsSize; i++) {
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            AutoPtr<IInterface> obj;
            mState->mAccounts->Get(i, (IInterface**)&obj);
            IPhoneAccount::Probe(obj)->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
            Boolean isEquals;
            IObject::Probe(phoneAccountHandle)->Equals(mState->mSimCallManager, &isEquals);
            if (isEquals) {
                *result = mState->mSimCallManager;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    // See if the OEM has specified a default one.
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String defaultConnectionMgr;
    res->GetString(R::string::default_connection_manager_component, &defaultConnectionMgr);
    if (!TextUtils::IsEmpty(defaultConnectionMgr)) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        AutoPtr<IComponentName> componentName;
        helper->UnflattenFromString(defaultConnectionMgr, (IComponentName**)&componentName);
        AutoPtr<IIntent> intent;
        CIntent::New(IConnectionService::SERVICE_INTERFACE, (IIntent**)&intent);
        intent->SetComponent(componentName);
        // Make sure that the component can be resolved.
        AutoPtr<IList> resolveInfos;
        pm->QueryIntentServices(intent, 0, (IList**)&resolveInfos);
        Boolean isEmpty;
        resolveInfos->IsEmpty(&isEmpty);
        if (!isEmpty) {
            // See if there is registered PhoneAccount by this component.
            AutoPtr<IList> handles;
            GetAllPhoneAccountHandles((IList**)&handles);
            AutoPtr<IIterator> it;
            handles->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IPhoneAccountHandle> handle = IPhoneAccountHandle::Probe(obj);
                AutoPtr<IComponentName> componentName;
                handle->GetComponentName((IComponentName**)&componentName);
                Boolean isEquals;
                IObject::Probe(componentName)->Equals(componentName, &isEquals);
                if (isEquals) {
                    *result = handle;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
            Log::D("PhoneAccountRegistrar", "%s does not have a PhoneAccount; not using as default", TO_CSTR(componentName));
        } else {
            Log::D("PhoneAccountRegistrar", "%s could not be resolved; not using as default", TO_CSTR(componentName));
        }
    } else {
        Log::V("PhoneAccountRegistrar", "No default connection manager specified");
    }
    *result = NULL;
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetAllPhoneAccountHandles(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> accountHandles;
    CArrayList::New((IList**)&accountHandles);
    AutoPtr<IIterator> it;
    mState->mAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccount> m = IPhoneAccount::Probe(obj);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        m->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        accountHandles->Add(phoneAccountHandle);
    }
    *result = accountHandles;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetAllPhoneAccounts(
    /* [out] */ IList** result)
{
    return CArrayList::New(ICollection::Probe(mState->mAccounts), result);
}

ECode PhoneAccountRegistrar::GetAllPhoneAccountsCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 accountsSize;
    mState->mAccounts->GetSize(&accountsSize);
    *result = accountsSize;
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetCallCapablePhoneAccounts(
    /* [out] */ IList** result)
{
    return GetPhoneAccountHandles(IPhoneAccount::CAPABILITY_CALL_PROVIDER, result);
}

ECode PhoneAccountRegistrar::GetCallCapablePhoneAccounts(
    /* [in] */ const String& uriScheme,
    /* [out] */ IList** result)
{
    return GetPhoneAccountHandles(IPhoneAccount::CAPABILITY_CALL_PROVIDER, uriScheme, result);
}

ECode PhoneAccountRegistrar::GetPhoneAccountsForPackage(
    /* [in] */ const String& packageName,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> accountHandles;
    CArrayList::New((IList**)&accountHandles);
    AutoPtr<IIterator> it;
    mState->mAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccount> m = IPhoneAccount::Probe(obj);
        AutoPtr<IComponentName> componentName;
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        m->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
        String s;
        componentName->GetPackageName(&s);
        if (packageName.Equals(s)) {
            accountHandles->Add(phoneAccountHandle);
        }
    }
    *result = accountHandles;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetConnectionManagerPhoneAccounts(
    /* [out] */ IList** result)
{
    return GetPhoneAccountHandles(IPhoneAccount::CAPABILITY_CONNECTION_MANAGER,
            String(NULL) /* supportedUriScheme */, result);
}

ECode PhoneAccountRegistrar::GetPhoneAccount(
    /* [in] */ IPhoneAccountHandle* handle,
    /* [out] */ IPhoneAccount** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    mState->mAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccount> m = IPhoneAccount::Probe(obj);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        m->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        if (Objects::Equals(handle, phoneAccountHandle.Get())) {
            *result = m;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode PhoneAccountRegistrar::RegisterPhoneAccount(
    /* [in] */ IPhoneAccount* account)
{
    // Enforce the requirement that a connection service for a phone account has the correct
    // permission.
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    account->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
    Boolean phoneAccountHasPermission;
    PhoneAccountHasPermission(phoneAccountHandle, &phoneAccountHasPermission);
    if (!phoneAccountHasPermission) {
        Log::W("PhoneAccountRegistrar", "Phone account %s does not have BIND_CONNECTION_SERVICE permission.",
                TO_CSTR(phoneAccountHandle));
        Log::E("PhoneAccountRegistrar", "PhoneAccount connection service requires BIND_CONNECTION_SERVICE permission.");
        return E_SECURITY_EXCEPTION;
    }
    AddOrReplacePhoneAccount(account);
    return NOERROR;
}

ECode PhoneAccountRegistrar::AddOrReplacePhoneAccount(
    /* [in] */ IPhoneAccount* account)
{
    mState->mAccounts->Add(account);
    // Search for duplicates and remove any that are found.
    Int32 accountsSize;
    mState->mAccounts->GetSize(&accountsSize);
    for (Int32 i = 0; i < accountsSize - 1; i++) {
        AutoPtr<IPhoneAccountHandle> accountPhoneAccountHandle;
        account->GetAccountHandle((IPhoneAccountHandle**)&accountPhoneAccountHandle);
        AutoPtr<IInterface> obj;
        mState->mAccounts->Get(i, (IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        IPhoneAccount::Probe(obj)->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        if (Objects::Equals(
                accountPhoneAccountHandle, phoneAccountHandle)) {
            // replace existing entry.
            mState->mAccounts->Remove(i);
            break;
        }
    }
    Write();
    FireAccountsChanged();
    return NOERROR;
}

ECode PhoneAccountRegistrar::UnregisterPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    Int32 accountsSize;
    mState->mAccounts->GetSize(&accountsSize);
    for (Int32 i = 0; i < accountsSize; i++) {
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        AutoPtr<IInterface> obj;
        mState->mAccounts->Get(i, (IInterface**)&obj);
        IPhoneAccount::Probe(obj)->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        if (Objects::Equals(accountHandle, phoneAccountHandle.Get())) {
            mState->mAccounts->Remove(i);
            break;
        }
    }
    Write();
    FireAccountsChanged();
    return NOERROR;
}

ECode PhoneAccountRegistrar::ClearAccounts(
    /* [in] */ const String& packageName)
{
    Boolean accountsRemoved = FALSE;
    AutoPtr<IIterator> it;
    mState->mAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccount> phoneAccount = IPhoneAccount::Probe(obj);
        AutoPtr<IComponentName> componentName;
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        phoneAccount->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
        String s;
        componentName->GetPackageName(&s);
        if (packageName.Equals(s)) {
            AutoPtr<ICharSequence> label;
            phoneAccount->GetLabel((ICharSequence**)&label);
            Log::I("PhoneAccountRegistrar", "Removing phone account %s", TO_CSTR(label));
            it->Remove();
            accountsRemoved = TRUE;
        }
    }
    if (accountsRemoved) {
        Write();
        FireAccountsChanged();
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::AddListener(
    /* [in] */ Listener* l)
{
    mListeners->Add(TO_IINTERFACE(l));
    return NOERROR;
}

ECode PhoneAccountRegistrar::RemoveListener(
    /* [in] */ Listener* l)
{
    if (l != NULL) {
        mListeners->Remove(TO_IINTERFACE(l));
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::FireAccountsChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<Listener> l = (Listener*) IObject::Probe(obj);
        l->OnAccountsChanged(this);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::FireDefaultOutgoingChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<Listener> l = (Listener*) IObject::Probe(obj);
        l->OnDefaultOutgoingChanged(this);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::FireSimCallManagerChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<Listener> l = (Listener*) IObject::Probe(obj);
        l->OnSimCallManagerChanged(this);
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::PhoneAccountHasPermission(
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (phoneAccountHandle == NULL) {
        Log::E("PhoneAccountRegistrar", "phoneAccountHandle can not be null");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    // try {
    ECode ec;
    do {
        AutoPtr<IComponentName> componentName;
        ec = phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
        if (FAILED(ec)) break;
        AutoPtr<IServiceInfo> serviceInfo;
        ec = packageManager->GetServiceInfo(componentName, 0, (IServiceInfo**)&serviceInfo);
        if (FAILED(ec)) break;
        String permission;
        serviceInfo->GetPermission(&permission);
        *result = !permission.IsNull() &&
                permission.Equals(Manifest::permission::BIND_CONNECTION_SERVICE);
        return NOERROR;
    } while(FALSE);
    // } catch (PackageManager.NameNotFoundException e) {
    if (FAILED(ec)) {
        if ((ECode) E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION == ec) {
            Log::W("PhoneAccountRegistrar", "Name not found %d", ec);
            *result = FALSE;
            return NOERROR;
        }
    }
    // }
    return NOERROR;
}

ECode PhoneAccountRegistrar::GetPhoneAccountHandles(
    /* [in] */ Int32 flags,
    /* [out] */ IList** result)
{
    return GetPhoneAccountHandles(flags, String(NULL), result);
}

ECode PhoneAccountRegistrar::GetPhoneAccountHandles(
    /* [in] */ Int32 flags,
    /* [in] */ const String& uriScheme,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> accountHandles;
    CArrayList::New((IList**)&accountHandles);
    AutoPtr<IIterator> it;
    mState->mAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccount> m = IPhoneAccount::Probe(obj);
        Boolean hasCapabilities;
        m->HasCapabilities(flags, &hasCapabilities);
        Boolean supportsUriScheme;
        m->SupportsUriScheme(uriScheme, &supportsUriScheme);
        if (hasCapabilities && (uriScheme == NULL || supportsUriScheme)) {
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            m->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
            accountHandles->Add(phoneAccountHandle);
        }
    }
    *result = accountHandles;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccountRegistrar::Write()
{
    AutoPtr<IFileOutputStream> os;
    // try {
    ECode ec;
    do {
        ec = mAtomicFile->StartWrite((IFileOutputStream**)&os);
        if (FAILED(ec)) break;
        Boolean success = FALSE;
        // try {
        do {
            AutoPtr<IXmlSerializer> serializer;
            ec = CFastXmlSerializer::New((IXmlSerializer**)&serializer);
            if (FAILED(ec)) break;
            AutoPtr<IBufferedOutputStream> bufferedOutputStream;
            ec = CBufferedOutputStream::New(IOutputStream::Probe(os), (IBufferedOutputStream**)&bufferedOutputStream);
            if (FAILED(ec)) break;
            ec = serializer->SetOutput(IOutputStream::Probe(bufferedOutputStream) , String("utf-8"));
            if (FAILED(ec)) break;
            ec = WriteToXml(mState, serializer);
            if (FAILED(ec)) break;
            ec = serializer->Flush();
            if (FAILED(ec)) break;
            success = TRUE;
        } while(FALSE);
        // } finally {
        if (success) {
            mAtomicFile->FinishWrite(os);
        } else {
            mAtomicFile->FailWrite(os);
        }
        // }
    } while(FALSE);
    // } catch (IOException e) {
    if ((ECode) E_IO_EXCEPTION == ec) {
        Log::E("PhoneAccountRegistrar", "Writing state to XML file %d", ec);
        ec = NOERROR;
    }
    // }
    return ec;
}

ECode PhoneAccountRegistrar::Read()
{
    AutoPtr<IInputStream> is;
    // try {
    ECode ec;
    do {
        AutoPtr<IFileInputStream> fis;
        ec = mAtomicFile->OpenRead((IFileInputStream**)&fis);
        if (FAILED(ec)) break;
        is = IInputStream::Probe(fis);
    } while(FALSE);
    // } catch (FileNotFoundException ex) {
    if ((ECode) E_FILE_NOT_FOUND_EXCEPTION == ec) {
        return NOERROR;
    }
    // }
    Boolean versionChanged = FALSE;
    AutoPtr<IXmlPullParser> parser;
    // try {
    do {
        Xml::NewPullParser((IXmlPullParser**)&parser);
        AutoPtr<IBufferedInputStream> bis;
        ec = CBufferedInputStream::New(is, (IBufferedInputStream**)&bis);
        if (FAILED(ec)) break;
        ec = parser->SetInput(IInputStream::Probe(bis), String(NULL));
        if (FAILED(ec)) break;
        Int32 nextTag;
        ec = parser->NextTag(&nextTag);
        if (FAILED(ec)) break;
        ec = ReadFromXml(parser, mContext, (State**)&mState);
        if (FAILED(ec)) break;
        versionChanged = mState->mVersionNumber < EXPECTED_STATE_VERSION;
    } while(FALSE);
    // } catch (IOException | XmlPullParserException e) {
    if ((ECode)E_IO_EXCEPTION == ec || (ECode)E_XML_PULL_PARSER_EXCEPTION == ec) {
        Log::E("PhoneAccountRegistrar", "Reading state from XML file %d", ec);
        mState = new State();
        ec = NOERROR;
    }
    // } finally {
    // try {
    ec = is->Close();
    // } catch (IOException e) {
    if ((ECode) E_IO_EXCEPTION == ec) {
        Log::E("PhoneAccountRegistrar", "Closing InputStream");
        ec = NOERROR;
    }
    // }
    if (FAILED(ec)) return ec;;
    // }
    // If an upgrade occurred, write out the changed data.
    if (versionChanged) {
        Write();
    }
    return NOERROR;
}

ECode PhoneAccountRegistrar::WriteToXml(
    /* [in] */ State* state,
    /* [in] */ IXmlSerializer* serializer)
{
    return sStateXml->WriteToXml(TO_IINTERFACE(state), serializer);
}

ECode PhoneAccountRegistrar::ReadFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IContext* context,
    /* [out] */ State** result)
{

    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    sStateXml->ReadFromXml(parser, 0, context, (IInterface**)&obj);
    AutoPtr<State> s = (State*) IObject::Probe(obj);
    *result = s != NULL ? s.Get() : new State();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IPhoneAccountHandle> PhoneAccountRegistrar::InitNO_ACCOUNT_SELECTED()
{
    AutoPtr<IPhoneAccountHandle> rev;
    AutoPtr<IComponentName> componentName;
    CComponentName::New(String("null"), String("null"), (IComponentName**)&componentName);
    CPhoneAccountHandle::New(componentName, String("NO_ACCOUNT_SELECTED"), (IPhoneAccountHandle**)&rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
