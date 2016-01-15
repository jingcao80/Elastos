
#include "Elastos.Droid.Database.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsGroupMembership.h"
#include "elastos/droid/provider/CContactsGroups.h"
#include "elastos/droid/provider/CContactsPeople.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::Uri;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IByte;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsPeople)

CAR_INTERFACE_IMPL_6(CContactsPeople, Singleton
    , IContactsPeople
    , IBaseColumns
    , ISyncConstValue
    , IContactsPeopleColumns
    , IContactsPhonesColumns
    , IContactsPresenceColumns)

static AutoPtr<ArrayOf<String> > initGROUPSPROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IBaseColumns::ID;
    return args;
}

AutoPtr<ArrayOf<String> > CContactsPeople::GROUPS_PROJECTION = initGROUPSPROJECTION();

ECode CContactsPeople::constructor()
{
    return NOERROR;
}

ECode CContactsPeople::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/people"), uri);
}

ECode CContactsPeople::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/people/filter"), uri);
}

ECode CContactsPeople::GetDELETED_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/deleted_people"), uri);
}

ECode CContactsPeople::GetWITH_EMAIL_OR_IM_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/people/with_email_or_im_filter"), uri);
}

ECode CContactsPeople::GetDEFAULTSORTORDER(
    /* [out] */ String* order)
{
    VALIDATE_NOT_NULL(order);

    *order = IContactsPeopleColumns::NAME + String(" ASC");
    return NOERROR;
}

ECode CContactsPeople::MarkAsContacted(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 personId)
{
    AutoPtr<IUri> _uri, uri, contentUri;
    GetCONTENT_URI((IUri**)&contentUri);
    AutoPtr<IContentUris> helper;
    CContentUris::AcquireSingleton((IContentUris**)&helper);
    helper->WithAppendedId(contentUri, personId, (IUri**)&_uri);
    Uri::WithAppendedPath(_uri, String("update_contact_time"), (IUri**)&uri);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    // There is a trigger in place that will update TIMES_CONTACTED when
    // LAST_TIME_CONTACTED is modified.
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    values->Put(IContactsPeopleColumns::LAST_TIME_CONTACTED, now);
    Int32 result;
    return resolver->Update(uri, values, String(NULL), NULL, &result);
}

ECode CContactsPeople::TryGetMyContactsGroupId(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    AutoPtr<IContactsGroups> helper;
    CContactsGroups::AcquireSingleton((IContactsGroups**)&helper);
    AutoPtr<IUri> uri;
    helper->GetCONTENT_URI((IUri**)&uri);
    StringBuilder builder;
    builder += IContactsGroupsColumns::SYSTEM_ID;
    builder += "='";
    builder += IContactsGroups::GROUP_MY_CONTACTS;
    builder += "'";
    String selection = builder.ToString();
    AutoPtr<ICursor> groupsCursor;
    resolver->Query(uri, GROUPS_PROJECTION, selection, NULL, String(NULL), (ICursor**)&groupsCursor);
    if (groupsCursor != NULL) {
        //try {
        Boolean result;
        ECode ec = groupsCursor->MoveToFirst(&result);
        if (SUCCEEDED(ec) && result) {
            groupsCursor->GetInt64(0, id);
            ICloseable::Probe(groupsCursor)->Close();
            return NOERROR;
        }
        //} finally {
        ICloseable::Probe(groupsCursor)->Close();
        //}
    }
    *id = 0;
    return NOERROR;
}

ECode CContactsPeople::AddToMyContactsGroup(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 personId,
    /* [out] */ IUri** uri)
{
    Int64 groupId;
    TryGetMyContactsGroupId(resolver, &groupId);
    if (groupId == 0) {
        //throw new IllegalStateException("Failed to find the My Contacts group");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return AddToGroup(resolver, personId, groupId, uri);
}

ECode CContactsPeople::AddToGroup(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 personId,
    /* [in] */ const String& groupName,
    /* [out] */ IUri** uri)
{
    Int64 groupId = 0;
    AutoPtr<IContactsGroups> helper;
    CContactsGroups::AcquireSingleton((IContactsGroups**)&helper);
    AutoPtr<IUri> _uri;
    helper->GetCONTENT_URI((IUri**)&_uri);
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = groupName;
    AutoPtr<ICursor> groupsCursor;
    resolver->Query(_uri, GROUPS_PROJECTION,
        IContactsGroupsColumns::NAME + String("=?"), args, String(NULL), (ICursor**)&groupsCursor);
    if (groupsCursor != NULL) {
        //try {
        Boolean result;
        if (groupsCursor->MoveToFirst(&result), result) {
            groupsCursor->GetInt64(0, &groupId);
        }
        //} finally {
        ICloseable::Probe(groupsCursor)->Close();
        //}
    }

    if (groupId == 0) {
        //throw new IllegalStateException("Failed to find the My Contacts group");
        *uri = NULL;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return AddToGroup(resolver, personId, groupId, uri);
}

ECode CContactsPeople::AddToGroup(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 personId,
    /* [in] */ Int64 groupId,
    /* [out] */ IUri** uri)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IContactsGroupMembership::PERSON_ID, personId);
    values->Put(IContactsGroupMembership::GROUP_ID, groupId);

    AutoPtr<IUri> _uri;
    AutoPtr<IContactsGroupMembership> helper;
    CContactsGroupMembership::AcquireSingleton((IContactsGroupMembership**)&helper);
    helper->GetCONTENT_URI((IUri**)&_uri);
    return resolver->Insert(_uri, values, uri);
}

ECode CContactsPeople::CreatePersonInMyContactsGroup(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** uri)
{
    AutoPtr<IUri> _uri;
    GetCONTENT_URI((IUri**)&_uri);
    AutoPtr<IUri> contactUri;
    resolver->Insert(_uri, values, (IUri**)&contactUri);
    if (contactUri == NULL) {
        //Log.e(TAG, "Failed to create the contact");
        *uri = NULL;
        return NOERROR;
    }

    AutoPtr<IContentUris> helper;
    CContentUris::AcquireSingleton((IContentUris**)&helper);
    Int64 id;
    helper->ParseId(contactUri, &id);
    AutoPtr<IUri> groupUri;
    AddToMyContactsGroup(resolver, id, (IUri**)&groupUri);
    if (groupUri == NULL) {
        Int32 result;
        resolver->Delete(contactUri, String(NULL), NULL, &result);
        *uri = NULL;
        return NOERROR;
    }
    *uri =  contactUri;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsPeople::QueryGroups(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 person,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    AutoPtr<IContactsGroupMembership> helper;
    CContactsGroupMembership::AcquireSingleton((IContactsGroupMembership**)&helper);
    AutoPtr<IUri> uri;
    helper->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = StringUtils::ToString(person);
    return resolver->Query(uri, NULL, String("person=?"),
            args, IContactsGroups::DEFAULT_SORT_ORDER, cursor);
}

ECode CContactsPeople::SetPhotoData(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* person,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IUri> photoUri;
    Uri::WithAppendedPath(person, IContactsPhotos::CONTENT_DIRECTORY, (IUri**)&photoUri);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IContactsPhotosColumns::DATA, data);
    Int32 result;
    return cr->Update(photoUri, values, String(NULL), NULL, &result);
}

ECode CContactsPeople::OpenContactPhotoInputStream(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* person,
    /* [out] */ IInputStream** stream)
{
    AutoPtr<IUri> photoUri;
    Uri::WithAppendedPath(person, IContactsPhotos::CONTENT_DIRECTORY, (IUri**)&photoUri);

    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IContactsPhotosColumns::DATA;
    AutoPtr<ICursor> cursor;
    cr->Query(photoUri, args, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    Boolean result;
    if (cursor == NULL || (cursor->MoveToNext(&result), !result)) {
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        *stream = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > data;
    cursor->GetBlob(0, (ArrayOf<Byte>**)&data);
    if (data == NULL) {
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        *stream = NULL;
        return NOERROR;
    }
    AutoPtr<IByteArrayInputStream> _stream;
    CByteArrayInputStream::New(data, (IByteArrayInputStream**)&_stream);
    *stream = IInputStream::Probe(_stream);
    REFCOUNT_ADD(*stream);
    if (cursor != NULL) ICloseable::Probe(cursor)->Close();
    return NOERROR;
}

ECode CContactsPeople::LoadContactPhoto(
    /* [in] */ IContext* context,
    /* [in] */ IUri* person,
    /* [in] */ Int32 placeholderImageResource,
    /* [in] */ IBitmapFactoryOptions* options,
    /* [out] */ IBitmap** bitmap)
{
    if (person == NULL) {
        AutoPtr<IBitmap> bm = LoadPlaceholderPhoto(placeholderImageResource, context, options);
        *bitmap = bm;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IInputStream> stream;
    OpenContactPhotoInputStream(resolver, person, (IInputStream**)&stream);
    AutoPtr<IBitmap> bm;
    if (stream != NULL) {
        AutoPtr<IBitmapFactory> helper;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&helper);
        helper->DecodeStream(stream, NULL, options, (IBitmap**)&bm);
    }
    if (bm == NULL) {
        bm = LoadPlaceholderPhoto(placeholderImageResource, context, options);
    }
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

AutoPtr<IBitmap> CContactsPeople::LoadPlaceholderPhoto(
    /* [in] */ Int32 placeholderImageResource,
    /* [in] */ IContext* context,
    /* [in] */ IBitmapFactoryOptions* options)
{
    if (placeholderImageResource == 0) {
        return NULL;
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IBitmapFactory> helper;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&helper);
    AutoPtr<IBitmap> bm;
    helper->DecodeResource(res, placeholderImageResource, options, (IBitmap**)&bm);
    return bm;
}

} // Provider
} // Droid
} // Elastos
