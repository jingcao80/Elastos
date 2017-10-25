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

#include "elastos/droid/server/tv/PersistentDataStore.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Media::Tv::CTvContentRatingHelper;
using Elastos::Droid::Media::Tv::ITvContentRatingHelper;
using Elastos::Droid::Media::Tv::ITvInputManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::IBuffer;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

//=============================================================================
// PersistentDataStore::InnerSub_Runnable
//=============================================================================
PersistentDataStore::InnerSub_Runnable::InnerSub_Runnable(
    /* [in] */ PersistentDataStore* host)
    : mHost(host)
{}

ECode PersistentDataStore::InnerSub_Runnable::Run()
{
    return mHost->Save();
}

//=============================================================================
// PersistentDataStore
//=============================================================================
const String PersistentDataStore::TAG("TvInputManagerService");
const String PersistentDataStore::TAG_TV_INPUT_MANAGER_STATE("tv-input-manager-state");
const String PersistentDataStore::TAG_BLOCKED_RATINGS("blocked-ratings");
const String PersistentDataStore::TAG_RATING("rating");
const String PersistentDataStore::TAG_PARENTAL_CONTROLS("parental-controls");
const String PersistentDataStore::ATTR_STRING("string");
const String PersistentDataStore::ATTR_ENABLED("enabled");

PersistentDataStore::PersistentDataStore()
    : mBlockedRatingsChanged(FALSE)
    , mParentalControlsEnabled(FALSE)
    , mParentalControlsEnabledChanged(FALSE)
    , mLoaded(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    AutoPtr<IArrayList> arrayList;
    CArrayList::New((IArrayList**)&arrayList);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->SynchronizedList(IList::Probe(arrayList), (IList**)&mBlockedRatings);

}

ECode PersistentDataStore::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    mContext = context;
    AutoPtr<IFile> userDir = Environment::GetUserSystemDirectory(userId);
    Boolean result;
    if (userDir->Exists(&result), !result) {
        if (userDir->Mkdirs(&result), !result) {
            Logger::E(TAG, "User dir cannot be created: %s", TO_CSTR(userDir));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    AutoPtr<IFile> file;
    CFile::New(userDir, String("tv-input-manager-state.xml"), (IFile**)&file);
    CAtomicFile::New(file, (IAtomicFile**)&mAtomicFile);
    return NOERROR;
}

ECode PersistentDataStore::IsParentalControlsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    LoadIfNeeded();
    *result = mParentalControlsEnabled;
    return NOERROR;
}

ECode PersistentDataStore::SetParentalControlsEnabled(
    /* [in] */ Boolean enabled)
{
    LoadIfNeeded();
    if (mParentalControlsEnabled != enabled) {
        mParentalControlsEnabled = enabled;
        mParentalControlsEnabledChanged = TRUE;
        PostSave();
    }
    return NOERROR;
}

ECode PersistentDataStore::IsRatingBlocked(
    /* [in] */ ITvContentRating* rating,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    LoadIfNeeded();
    {
        AutoLock syncLock(mBlockedRatings);
        AutoPtr<IIterator> it;
        mBlockedRatings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> blockedRating;
            it->GetNext((IInterface**)&blockedRating);
            Boolean b;
            rating->Contains(ITvContentRating::Probe(blockedRating), &b);
            if (b) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode PersistentDataStore::GetBlockedRatings(
    /* [out, callee] */ ArrayOf<ITvContentRating*>** result)
{
    VALIDATE_NOT_NULL(result)

    LoadIfNeeded();
    Int32 size;
    mBlockedRatings->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > outArray;
    mBlockedRatings->ToArray(array, (ArrayOf<IInterface*>**)&outArray);
    size = outArray->GetLength();
    *result = ArrayOf<ITvContentRating*>::Alloc(size);
    REFCOUNT_ADD(*result)
    for (Int32 i = 0; i < size; ++i) {
        (*result)->Set(i, ITvContentRating::Probe((*outArray)[i]));
    }
    return NOERROR;
}

ECode PersistentDataStore::AddBlockedRating(
    /* [in] */ ITvContentRating* rating)
{
    LoadIfNeeded();
    Boolean isContains;
    if (rating != NULL && (mBlockedRatings->Contains(rating, &isContains), !isContains)) {
        mBlockedRatings->Add(rating);
        mBlockedRatingsChanged = TRUE;
        PostSave();
    }
    return NOERROR;
}

ECode PersistentDataStore::RemoveBlockedRating(
    /* [in] */ ITvContentRating* rating)
{
    LoadIfNeeded();
    Boolean isContains;
    if (rating != NULL && (mBlockedRatings->Contains(rating, &isContains), !isContains)) {
        mBlockedRatings->Remove(rating);
        mBlockedRatingsChanged = TRUE;
        PostSave();
    }
    return NOERROR;
}

ECode PersistentDataStore::LoadIfNeeded()
{
    if (!mLoaded) {
        Load();
        mLoaded = TRUE;
    }
    return NOERROR;
}

ECode PersistentDataStore::ClearState()
{
    mBlockedRatings->Clear();
    mParentalControlsEnabled = FALSE;
    return NOERROR;
}

ECode PersistentDataStore::Load()
{
    ClearState();
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
    if (FAILED(ec)) {
        if ((ECode)E_FILE_NOT_FOUND_EXCEPTION == ec)
            return NOERROR;
        return ec;
    }
    // }
    AutoPtr<IXmlPullParser> parser;
    // try {
    do {
        ec = Xml::NewPullParser((IXmlPullParser**)&parser);
        if (FAILED(ec)) break;
        AutoPtr<IBufferedInputStream> bis;
        ec = CBufferedInputStream::New(is, (IBufferedInputStream**)&bis);
        if (FAILED(ec)) break;
        ec = parser->SetInput(IInputStream::Probe(bis), String(NULL));
        if (FAILED(ec)) break;
        ec = LoadFromXml(parser);
        if (FAILED(ec)) break;
    } while(FALSE);
    // } catch (IOException | XmlPullParserException ex) {
    if (FAILED(ec)) {
        if ((ECode)E_IO_EXCEPTION == ec) {
            Slogger::W(TAG, "Failed to load tv input manager persistent store data. %d", ec);
            ClearState();
        }
        else
            return ec;
    }
    // } finally {
    AutoPtr<IIoUtils> helper;
    CIoUtils::AcquireSingleton((IIoUtils**)&helper);
    helper->CloseQuietly(ICloseable::Probe(is));
    // }
    return NOERROR;
}

ECode PersistentDataStore::PostSave()
{
    mHandler->RemoveCallbacks(mSaveRunnable);
    Boolean bNoUse;
    mHandler->Post(mSaveRunnable, &bNoUse);
    return NOERROR;
}

ECode PersistentDataStore::Save()
{
    AutoPtr<IFileOutputStream> os;
    ECode ec;
    // try {
    do {
        ec = mAtomicFile->StartWrite((IFileOutputStream**)&os);
        if (FAILED(ec)) break;
        Boolean success = FALSE;
        // try {
        do {
            AutoPtr<IXmlSerializer> serializer;
            ec = CFastXmlSerializer::New((IXmlSerializer**)&serializer);
            if (FAILED(ec)) break;
            AutoPtr<IBufferedOutputStream> bos;
            ec = CBufferedOutputStream::New(IOutputStream::Probe(os), (IBufferedOutputStream**)&bos);
            if (FAILED(ec)) break;
            ec = serializer->SetOutput(IOutputStream::Probe(bos), String("utf-8"));
            if (FAILED(ec)) break;
            ec = SaveToXml(serializer);
            if (FAILED(ec)) break;
            ec = serializer->Flush();
            if (FAILED(ec)) break;
            success = TRUE;
        } while(FALSE);
        // } finally {
        if (success) {
            ec = mAtomicFile->FinishWrite(os);
            if (FAILED(ec)) break;
            ec = BroadcastChangesIfNeeded();
            if (FAILED(ec)) break;
        } else {
            ec = mAtomicFile->FailWrite(os);
            if (FAILED(ec)) break;
        }
        if (FAILED(ec)) return ec;
        // }
    } while(FALSE);
    // } catch (IOException ex) {
    if (FAILED(ec)) {
        if ((ECode)E_IO_EXCEPTION == ec)
            Slogger::W(TAG, "Failed to save tv input manager persistent store data. %d", ec);
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode PersistentDataStore::BroadcastChangesIfNeeded()
{
    if (mParentalControlsEnabledChanged) {
        mParentalControlsEnabledChanged = FALSE;
        AutoPtr<IIntent> intent;
        CIntent::New(ITvInputManager::ACTION_PARENTAL_CONTROLS_ENABLED_CHANGED, (IIntent**)&intent);
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
    }
    if (mBlockedRatingsChanged) {
        mBlockedRatingsChanged = FALSE;
        AutoPtr<IIntent> intent;
        CIntent::New(ITvInputManager::ACTION_BLOCKED_RATINGS_CHANGED, (IIntent**)&intent);
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
    }
    return NOERROR;
}

ECode PersistentDataStore::LoadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    XmlUtils::BeginDocument(parser, TAG_TV_INPUT_MANAGER_STATE);
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        parser->GetName(&name);
        if (name.Equals(TAG_BLOCKED_RATINGS)) {
            LoadBlockedRatingsFromXml(parser);
        }
        else if (name.Equals(TAG_PARENTAL_CONTROLS)) {
            String enabled;
            parser->GetAttributeValue(String(NULL), ATTR_ENABLED, &enabled);
            if (TextUtils::IsEmpty(enabled)) {
                Logger::E(TAG, "Missing %s attribute on %s", ATTR_ENABLED.string(), TAG_PARENTAL_CONTROLS.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            mParentalControlsEnabled = StringUtils::ParseBoolean(enabled);
        }
    }
    return NOERROR;
}

ECode PersistentDataStore::LoadBlockedRatingsFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        String name;
        parser->GetName(&name);
        if (name.Equals(TAG_RATING)) {
            String ratingString;
            parser->GetAttributeValue(String(NULL), ATTR_STRING, &ratingString);
            if (TextUtils::IsEmpty(ratingString)) {
                Logger::E(TAG, "Missing %s attribute on %s", ATTR_STRING.string(), TAG_RATING.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            AutoPtr<ITvContentRatingHelper> helper;
            CTvContentRatingHelper::AcquireSingleton((ITvContentRatingHelper**)&helper);
            AutoPtr<ITvContentRating> tvcr;
            helper->UnflattenFromString(ratingString, (ITvContentRating**)&tvcr);
            mBlockedRatings->Add(tvcr);
        }
    }
    return NOERROR;
}

ECode PersistentDataStore::SaveToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    serializer->StartDocument(String(NULL), TRUE);
    serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
    serializer->WriteStartTag(String(NULL), TAG_TV_INPUT_MANAGER_STATE);
    serializer->WriteStartTag(String(NULL), TAG_BLOCKED_RATINGS);
    {
        AutoLock syncLock(mBlockedRatings);
        AutoPtr<IIterator> it;
        mBlockedRatings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> rating;
            it->GetNext((IInterface**)&rating);
            serializer->WriteStartTag(String(NULL), TAG_RATING);
            String str;
            ITvContentRating::Probe(rating)->FlattenToString(&str);
            serializer->WriteAttribute(String(NULL), ATTR_STRING, str);
            serializer->WriteEndTag(String(NULL), TAG_RATING);
        }
    }
    serializer->WriteEndTag(String(NULL), TAG_BLOCKED_RATINGS);
    serializer->WriteStartTag(String(NULL), TAG_PARENTAL_CONTROLS);
    serializer->WriteAttribute(String(NULL), ATTR_ENABLED, StringUtils::ToString(mParentalControlsEnabled));
    serializer->WriteEndTag(String(NULL), TAG_PARENTAL_CONTROLS);
    serializer->WriteEndTag(String(NULL), TAG_TV_INPUT_MANAGER_STATE);
    serializer->EndDocument();
    return NOERROR;
}

} // namespace Tv
} // namespace Server
} // namespace Droid
} // namespace Elastos
