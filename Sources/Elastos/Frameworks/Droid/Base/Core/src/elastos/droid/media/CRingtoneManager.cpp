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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/media/CRingtoneManager.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/provider/CMediaStoreAudioMedia.h"
#include "elastos/droid/internal/database/CSortCursor.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/media/CRingtone.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IProfileGroup;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::App::ProfileGroupMode;
using Elastos::Droid::App::ProfileGroupMode_DEFAULT;
using Elastos::Droid::App::ProfileGroupMode_SUPPRESS;
using Elastos::Droid::App::ProfileGroupMode_OVERRIDE;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Internal::Database::CSortCursor;
using Elastos::Droid::Internal::Database::ISortCursor;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylistsMembers;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

const String CRingtoneManager::TAG("RingtoneManager");

static AutoPtr<ArrayOf<String> > InitINTERNAL_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    array->Set(0, IMediaStoreAudioPlaylistsMembers::ID);
    array->Set(1, IMediaStoreMediaColumns::TITLE);

    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    String internalContentUri;
    AutoPtr<IUri> uri;
    am->GetINTERNAL_CONTENT_URI((IUri**)&uri);
    IObject::Probe(uri)->ToString(&internalContentUri);

    String info;
    info.AppendFormat("\"%s\"", internalContentUri.string());
    array->Set(2, info);
    array->Set(3, IMediaStoreAudioAudioColumns::TITLE_KEY);
    return array;
}

static AutoPtr<ArrayOf<String> > InitMEDIA_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    array->Set(0, IMediaStoreAudioPlaylistsMembers::ID);
    array->Set(1, IMediaStoreMediaColumns::TITLE);

    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    String externalContentUri;
    AutoPtr<IUri> uri;
    am->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    IObject::Probe(uri)->ToString(&externalContentUri);

    String info;
    info.AppendFormat("\"%s\"", externalContentUri.string());
    array->Set(2, info);
    array->Set(3, IMediaStoreAudioAudioColumns::TITLE_KEY);
    return array;
}

AutoPtr< ArrayOf<String> > CRingtoneManager::INTERNAL_COLUMNS = InitINTERNAL_COLUMNS();
AutoPtr< ArrayOf<String> > CRingtoneManager::MEDIA_COLUMNS = InitMEDIA_COLUMNS();

CAR_OBJECT_IMPL(CRingtoneManager)

CAR_INTERFACE_IMPL(CRingtoneManager, Object, IRingtoneManager)

CRingtoneManager::CRingtoneManager()
    : mType(TYPE_RINGTONE)
    , mStopPreviousRingtone(TRUE)
{
}

CRingtoneManager::~CRingtoneManager()
{}

ECode CRingtoneManager::constructor(
    /* [in] */ IActivity* activity)
{
    mActivity = activity;
    mContext = IContext::Probe(activity);
    SetType(mType);
    return NOERROR;
}

ECode CRingtoneManager::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    SetType(mType);
    return NOERROR;
}

ECode CRingtoneManager::SetType(
    /* [in] */ Int32 type)
{
    if (mCursor != NULL) {
        Logger::E(TAG, "Setting filter columns should be done before querying for ringtones.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mType = type;
    SetFilterColumnsList(type);

    return NOERROR;
}

ECode CRingtoneManager::InferStreamType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);

    switch (mType) {
        case TYPE_ALARM:
        {
            *result = IAudioManager::STREAM_ALARM;
            return NOERROR;
        }
        case TYPE_NOTIFICATION:
        {
            *result = IAudioManager::STREAM_NOTIFICATION;
            return NOERROR;
        }
        default:
        {
            *result = IAudioManager::STREAM_RING;
            return NOERROR;
        }
    }
}

ECode CRingtoneManager::SetStopPreviousRingtone(
    /* [in] */ Boolean stopPreviousRingtone)
{
    mStopPreviousRingtone = stopPreviousRingtone;
    return NOERROR;
}

ECode CRingtoneManager::GetStopPreviousRingtone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStopPreviousRingtone;
    return NOERROR;
}

ECode CRingtoneManager::StopPreviousRingtone()
{
    if (mPreviousRingtone != NULL) {
        mPreviousRingtone->Stop();
    }
    return NOERROR;
}

ECode CRingtoneManager::GetIncludeDrm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

ECode CRingtoneManager::SetIncludeDrm(
    /* [in] */ Boolean includeDrm)
{
    if (includeDrm) {
        Slogger::W(TAG, "setIncludeDrm no longer supported");
    }
    return NOERROR;
}

ECode CRingtoneManager::GetCursor(
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean tempState;
    if (mCursor != NULL && (mCursor->Requery(&tempState), tempState)) {
        *result = mCursor;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<ICursor> internalCursor = GetInternalRingtones();
    AutoPtr<ICursor> mediaCursor = GetMediaRingtones();
    AutoPtr<ArrayOf<ICursor*> > cursors = ArrayOf<ICursor*>::Alloc(2);
    cursors->Set(0, internalCursor.Get());
    cursors->Set(1, mediaCursor.Get());
    AutoPtr<ISortCursor> sc;
    CSortCursor::New(cursors.Get(), IMediaStoreAudioMedia::DEFAULT_SORT_ORDER, (ISortCursor**)&sc);
    mCursor = ICursor::Probe(sc);
    *result = mCursor.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtoneManager::GetRingtone(
    /* [in] */ Int32 position,
    /* [out] */ IRingtone** result)
{
    VALIDATE_NOT_NULL(result);

    if (mStopPreviousRingtone && mPreviousRingtone != NULL) {
        mPreviousRingtone->Stop();
    }

    AutoPtr<IUri> uri;
    GetRingtoneUri(position, (IUri**)&uri);
    Int32 tempValue;
    InferStreamType(&tempValue);
    GetRingtone(mContext, uri, (IRingtone**)&mPreviousRingtone);
    *result = mPreviousRingtone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtoneManager::GetRingtoneUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    // use cursor directly instead of requerying it, which could easily
    // cause position to shuffle.
    Boolean tempState;
    if (mCursor == NULL || !(mCursor->MoveToPosition(position, &tempState), tempState)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IUri> temp = GetUriFromCursor(mCursor);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtoneManager::GetRingtonePosition(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (ringtoneUri == NULL) {
        *result = -1;
        return NOERROR;
    }

    AutoPtr<ICursor> cursor;
    GetCursor((ICursor**)&cursor);
    Int32 cursorCount;
    cursor->GetCount(&cursorCount);

    Boolean tempState;
    cursor->MoveToFirst(&tempState);
    if (!tempState) {
        *result = -1;
        return NOERROR;
    }

    // Only create Uri objects when the actual URI changes
    AutoPtr<IUri> currentUri;
    String previousUriString;
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    for (Int32 i = 0; i < cursorCount; i++) {
        String uriString;
        cursor->GetString(URI_COLUMN_INDEX, &uriString);
        if (currentUri == NULL || !uriString.Equals(previousUriString)) {
            AutoPtr<IUriHelper> uriHelper;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
            currentUri = NULL;
            uriHelper->Parse(uriString, (IUri**)&currentUri);
        }


        Int64 tempValue;
        cursor->GetInt64(ID_COLUMN_INDEX, &tempValue);
        AutoPtr<IUri> uri;
        contentUris->WithAppendedId(currentUri, tempValue, (IUri**)&uri);
        IObject::Probe(ringtoneUri)->Equals(uri, &tempState);
        if (tempState) {
            *result = i;
            return NOERROR;
        }

        cursor->Move(1, &tempState);

        previousUriString = uriString;
    }

    *result = -1;
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetValidRingtoneUri(
    /* [in] */ IContext* context,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<CRingtoneManager> rm;
    CRingtoneManager::NewByFriend(context, (CRingtoneManager**)&rm);

    AutoPtr<IUri> uri = GetValidRingtoneUriFromCursorAndClose(context, rm->GetInternalRingtones());

    if (uri == NULL) {
        uri = GetValidRingtoneUriFromCursorAndClose(context, rm->GetMediaRingtones());
    }

    *result = uri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetRingtone(
    /* [in] */ IContext* context,
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ IRingtone** result)
{
    VALIDATE_NOT_NULL(result);

    // Don't set the stream type
    AutoPtr<IRingtone> temp = GetRingtone(context, ringtoneUri, -1);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String setting = GetSettingForType(type);
    if (setting == NULL) {
        Logger::E(TAG, "Invalid type: %d", type);
        return NOERROR;
    }
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String uriString;
    Settings::System::GetString(contentResolver, setting, &uriString);

    AutoPtr<IUri> uri;
    if (uriString != NULL) {
        AutoPtr<IUriHelper> uriHelper;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
        uriHelper->Parse(uriString, (IUri**)&uri);
    }

    if ((uriString == NULL) || (type & TYPE_RINGTONE) == 0) {
        if (uriString != NULL) {
            *result = uri;
            REFCOUNT_ADD(*result);
        }

        return NOERROR;
    }

    AutoPtr<IUri> ringToneUri;
    GetStaticDefaultRingtoneUri(context, (IUri**)&ringToneUri);
    AutoPtr<ICursor> cursor;
    ECode ec = contentResolver->Query(uri, NULL, String(""), NULL, String(""), (ICursor**)&cursor);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to query %s", TO_CSTR(uri));
    }
    else {
        Int32 count = 0;
        if ((cursor != NULL) && (cursor->GetCount(&count), count > 0)) {
            ringToneUri = uri;
        }
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    *result = ringToneUri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::SetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ IUri* ringtoneUri)
{
    String setting = GetSettingForType(type);
    if (setting == NULL) {
        return NOERROR;
    }
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String tempText;
    if (ringtoneUri != NULL) {
        IObject::Probe(ringtoneUri)->ToString(&tempText);
    }

    Boolean restlt;
    return Settings::System::PutString(contentResolver, setting,
            tempText, &restlt);
}

/*static*/
ECode CRingtoneManager::IsDefault(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 tempValue;
    GetDefaultType(ringtoneUri, &tempValue);
    *result =  tempValue != -1;
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetDefaultType(
    /* [in] */ IUri* defaultRingtoneUri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<IUri> uri;
    if (defaultRingtoneUri == NULL) {
        *result = -1;
    }
    else if (Object::Equals(defaultRingtoneUri, Settings::System::DEFAULT_RINGTONE_URI)
        || Object::Equals(defaultRingtoneUri, Settings::System::DEFAULT_RINGTONE_URI_2)
        || Object::Equals(defaultRingtoneUri, Settings::System::DEFAULT_RINGTONE_URI_3)
    ) {
        *result = TYPE_RINGTONE;
    }
    else if (Object::Equals(defaultRingtoneUri, Settings::System::DEFAULT_NOTIFICATION_URI)){
        *result = TYPE_NOTIFICATION;
    }
    else if (Object::Equals(defaultRingtoneUri, Settings::System::DEFAULT_ALARM_ALERT_URI)){
        *result = TYPE_ALARM;
    }
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetDefaultUri(
    /* [in] */ Int32 type,
    /* [out] */ IUri ** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUri> uri;
    if ((type & TYPE_RINGTONE) != 0) {
        *result = Settings::System::DEFAULT_RINGTONE_URI;
        REFCOUNT_ADD(*result);
    }
    else if ((type & TYPE_NOTIFICATION) != 0) {
        *result = Settings::System::DEFAULT_NOTIFICATION_URI;
        REFCOUNT_ADD(*result);
    }
    else if ((type & TYPE_ALARM) != 0) {
        *result = Settings::System::DEFAULT_ALARM_ALERT_URI;
        REFCOUNT_ADD(*result);
    }
    else {
        *result = NULL;
    }
    return NOERROR;
}

ECode CRingtoneManager::GetStaticDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String uriString;
    Settings::System::GetString(contentResolver, ISettingsSystem::DEFAULT_RINGTONE, &uriString);

    if (!uriString.IsNull()) {
        return Uri::Parse(uriString, result);
    }
    return NOERROR;
}

ECode CRingtoneManager::GetDefaultRingtoneSubIdByUri(
    /* [in] */ IUri* defaultRingtoneUri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (defaultRingtoneUri == NULL) {
        *result = -1;
        return NOERROR;
    }
    /**
     * URI is encoded as below:
     * DEFAULT_RINGTONE_URI: content://settings/system/ringtone
     * DEFAULT_RINGTONE_URI_2: content://settings/system/ringtone_2
     * DEFAULT_RINGTONE_URI_3: content://settings/system/ringtone_3
     */
    String defaultUriStr = Object::ToString(defaultRingtoneUri);
    String defaultRingtoneUriStr = Object::ToString(Settings::System::DEFAULT_RINGTONE_URI);
    if (defaultUriStr.Equals(defaultRingtoneUriStr)) {
        *result = 0;
        return NOERROR;
    }

    Int32 parsedSubId = -1;
    if (defaultUriStr.StartWith(defaultRingtoneUriStr)) {
        parsedSubId = StringUtils::ParseInt32(defaultUriStr.Substring(defaultUriStr.LastIndexOf('_') + 1));
        if ((parsedSubId > 0 &&  parsedSubId <= ISettingsSystem::MAX_NUM_RINGTONES)) {
            *result = parsedSubId - 1;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode CRingtoneManager::GetDefaultRingtoneUriBySubId(
    /* [in] */ Int32 subId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    if (!(subId >= 0 &&  subId < ISettingsSystem::MAX_NUM_RINGTONES)) {
        *result = NULL;
        return NOERROR;
    }
    if (subId == 0) {
        *result = Settings::System::DEFAULT_RINGTONE_URI;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        String defaultRingtoneUriStr;
        IObject::Probe(Settings::System::DEFAULT_RINGTONE_URI)->ToString(&defaultRingtoneUriStr);
        String uriString = defaultRingtoneUriStr + String("_") + StringUtils::ToString(subId + 1);
        return Uri::Parse(uriString, result);
    }
}

ECode CRingtoneManager::GetActualRingtoneUriBySubId(
    /* [in] */ IContext* context,
    /* [in] */ Int32 subId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    if (!(subId >= 0 &&  subId < ISettingsSystem::MAX_NUM_RINGTONES)) {
        *result = NULL;
        return NOERROR;
    }
    String setting;
    if (subId == 0) {
        setting = ISettingsSystem::RINGTONE;
    }
    else {
        setting = ISettingsSystem::RINGTONE + String("_") + StringUtils::ToString(subId + 1);
    }

    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String uriString;
    Settings::System::GetString(contentResolver, setting, &uriString);
    if (uriString.IsEmpty()) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IUri> ringToneUri;
    GetStaticDefaultRingtoneUri(context, (IUri**)&ringToneUri);
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<IUri> uri;
        Uri::Parse(uriString, (IUri**)&uri);
        contentResolver->Query(uri, NULL, String(""), NULL, String(""), (ICursor**)&cursor);
        Int32 count = 0;
        if ((cursor != NULL) && (cursor->GetCount(&count), count > 0)) {
            ringToneUri = NULL;
            Uri::Parse(uriString, (IUri**)&ringToneUri);
        }
    // } catch (SQLiteException ex) {
        // Log.e(TAG, "ex " + ex);
    // } finally {
        if (cursor != NULL)
            ICloseable::Probe(cursor)->Close();
    // }
    *result = ringToneUri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtoneManager::SetActualRingtoneUriBySubId(
    /* [in] */ IContext* context,
    /* [in] */ Int32 subId,
    /* [in] */ IUri* ringtoneUri)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(ringtoneUri);
    if (!(subId >= 0 &&  subId < ISettingsSystem::MAX_NUM_RINGTONES)) {
        return NOERROR;
    }
    String setting;
    if (subId == 0) {
        setting = ISettingsSystem::RINGTONE;
    }
    else {
        setting = ISettingsSystem::RINGTONE + String("_") + StringUtils::ToString(subId + 1);
    }

    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String ringtoneUriStr("");
    if (ringtoneUri != NULL) {
        IObject::Probe(ringtoneUri)->ToString(&ringtoneUriStr);
    }
    Boolean resTmp = FALSE;
    Settings::System::PutString(contentResolver, setting, ringtoneUriStr, &resTmp);
    return NOERROR;
}

/*static*/
AutoPtr<IUri> CRingtoneManager::GetUriFromCursor(
    /* [in] */ ICursor* cursor)
{
    String tempText;
    cursor->GetString(URI_COLUMN_INDEX, &tempText);
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(tempText, (IUri**)&uri);

    Int64 tempValue;
    cursor->GetInt64(ID_COLUMN_INDEX, &tempValue);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> tempUri;
    contentUris->WithAppendedId(uri, tempValue, (IUri**)&tempUri);
    return tempUri;
}

/*static*/
AutoPtr<IUri> CRingtoneManager::GetValidRingtoneUriFromCursorAndClose(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IUri> uri;
    if (cursor != NULL) {
        Boolean tempState;
        cursor->MoveToFirst(&tempState);
        if (tempState) {
            uri = GetUriFromCursor(cursor);
        }
        ICloseable::Probe(cursor)->Close();
    }

    return uri;
}

AutoPtr<ICursor> CRingtoneManager::GetInternalRingtones()
{
    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    AutoPtr<IUri> uri;
    am->GetINTERNAL_CONTENT_URI((IUri**)&uri);
    return Query(uri, INTERNAL_COLUMNS,
        ConstructBooleanTrueWhereClause(mFilterColumns),
        NULL, IMediaStoreAudioMedia::DEFAULT_SORT_ORDER);
}

AutoPtr<ICursor> CRingtoneManager::GetMediaRingtones()
{
     // Get the external media cursor. First check to see if it is mounted.
    String status = Environment::GetExternalStorageState();
    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    AutoPtr<IUri> uri;
    am->GetEXTERNAL_CONTENT_URI((IUri**)&uri);

    return (status.Equals(IEnvironment::MEDIA_MOUNTED) || status.Equals(IEnvironment::MEDIA_MOUNTED_READ_ONLY))
        ? Query(uri, MEDIA_COLUMNS,
            ConstructBooleanTrueWhereClause(mFilterColumns), NULL,
            IMediaStoreAudioMedia::DEFAULT_SORT_ORDER)
        : NULL;
}

void CRingtoneManager::SetFilterColumnsList(
    /* [in] */ Int32 type)
{
    List<String>& columns = mFilterColumns;
    columns.Clear();

    if ((type & TYPE_RINGTONE) != 0) {
        columns.PushBack(IMediaStoreAudioAudioColumns::IS_RINGTONE);
    }

    if ((type & TYPE_NOTIFICATION) != 0) {
        columns.PushBack(IMediaStoreAudioAudioColumns::IS_NOTIFICATION);
    }

    if ((type & TYPE_ALARM) != 0) {
        columns.PushBack(IMediaStoreAudioAudioColumns::IS_ALARM);
    }
}

String CRingtoneManager::ConstructBooleanTrueWhereClause(
    /* [in] */ List<String>& columns)
{
    StringBuilder sb;
    sb += "(";

    List<String>::ReverseIterator rit;
    for (rit = columns.RBegin(); rit != columns.REnd(); ++rit) {
        sb += *rit;
        sb += "=1 or ";
    }

    if (!columns.IsEmpty()) {
        // Remove last ' or '
        Int32 size;
        sb.GetLength(&size);
        sb.SetLength(size - 4);
    }

    sb += ")";
    return sb.ToString();
}

AutoPtr<ICursor> CRingtoneManager::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ICursor> cursor;
    if (mActivity != NULL) {
        mActivity->ManagedQuery(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&cursor);
    }
    else {
        AutoPtr<IContentResolver> contentResolver;
        mContext->GetContentResolver((IContentResolver**)&contentResolver);
        contentResolver->Query(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&cursor);
    }
    return cursor;
}

/*static*/
AutoPtr<IRingtone> CRingtoneManager::GetRingtone(
    /* [in] */ IContext* context,
    /* [in] */ IUri* ringtoneUri,
    /* [in] */ Int32 streamType)
{
    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&interfaceTmp);
    AutoPtr<IProfileManager> pm = IProfileManager::Probe(interfaceTmp);
    String packageName;
    context->GetPackageName(&packageName);
    AutoPtr<IProfileGroup> profileGroup;
    pm->GetActiveProfileGroup(packageName, (IProfileGroup**)&profileGroup);

//    try {
    AutoPtr<IRingtone> r;
    ECode ec = CRingtone::New(context, TRUE, (IRingtone**)&r);
    if (FAILED(ec)) {
        goto exception;
    }
    if (streamType >= 0) {
        ec = r->SetStreamType(streamType);
        if (FAILED(ec)) {
            goto exception;
        }
    }
    if (profileGroup != NULL) {
        ProfileGroupMode ringerMode = ProfileGroupMode_DEFAULT;
        profileGroup->GetRingerMode(&ringerMode);
        switch (ringerMode) {
            case ProfileGroupMode_OVERRIDE :
                {
                    AutoPtr<IUri> ringer;
                    profileGroup->GetRingerOverride((IUri**)&ringer);
                    r->SetUri(ringer);
                }
                return r;
            case ProfileGroupMode_SUPPRESS :
                r = NULL;
                return r;
        }
    }
    ec = r->SetUri(ringtoneUri);
    FAIL_GOTO(ec, exception)

    return r;
//    } catch (Exception ex) {
exception:
        String tempText;
        IObject::Probe(ringtoneUri)->ToString(&tempText);
        Logger::E(TAG, "Failed to open ringtone %s : 0x%08x", tempText.string(), ec);
//    }
    return NULL;
}

/*static*/
String CRingtoneManager::GetSettingForType(
    /* [in] */ Int32 type)
{
    if ((type & TYPE_RINGTONE) != 0) {
        return ISettingsSystem::RINGTONE;
    }
    else if ((type & TYPE_NOTIFICATION) != 0) {
        return ISettingsSystem::NOTIFICATION_SOUND;
    }
    else if ((type & TYPE_ALARM) != 0) {
        return ISettingsSystem::ALARM_ALERT;
    }
    else {
        return String(NULL);
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
