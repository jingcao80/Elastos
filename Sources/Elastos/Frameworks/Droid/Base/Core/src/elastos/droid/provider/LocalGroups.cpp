
#include "elastos/droid/provider/LocalGroups.h"
#include "elastos/droid/provider/CLocalGroupsGroup.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/content/CContentUris.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::IO::ICloseable;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<IUri> InitAUTHORITY_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + ILocalGroups::AUTHORITY, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> LocalGroups::AUTHORITY_URI = InitAUTHORITY_URI();

static AutoPtr<IUri> InitCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(LocalGroups::AUTHORITY_URI, String("local-groups"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> LocalGroups::CONTENT_URI = InitCONTENT_URI();

ECode LocalGroups::Group::RestoreGroup(
    /* [in] */ ICursor* cursor,
    /* [out] */ ILocalGroupsGroup** group)
{
    VALIDATE_NOT_NULL(group);

    *group = NULL;
    Int32 count = -1;
    if (!cursor && cursor->GetCount(&count), count == 0) {
        return NOERROR;
    }

    AutoPtr<ILocalGroupsGroup> localGroupsGroup;
    FAIL_RETURN(CLocalGroupsGroup::New((ILocalGroupsGroup**)&localGroupsGroup));

    Int32 columnIndex; Int64 id; String title;
    cursor->GetColumnIndex(ILocalGroupsGroupColumns::_ID, &columnIndex);
    cursor->GetInt64(columnIndex, &id);
    localGroupsGroup->SetId(id);
    cursor->GetColumnIndex(ILocalGroupsGroupColumns::TITLE, &columnIndex);
    cursor->GetString(columnIndex, &title);
    localGroupsGroup->SetTitle(title);
    cursor->GetColumnIndex(ILocalGroupsGroupColumns::COUNT, &columnIndex);
    cursor->GetInt32(columnIndex, &count);
    localGroupsGroup->SetCount(count);

    *group = localGroupsGroup;
    REFCOUNT_ADD(*group);
    return NOERROR;
}

ECode LocalGroups::Group::RestoreGroupById(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 groupId,
    /* [out] */ ILocalGroupsGroup** group)
{
    VALIDATE_NOT_NULL(group);

    *group = NULL;
    AutoPtr<IUri> uri;
    // TODO:
    // ContentUris::WithAppendedId(LocalGroups::CONTENT_URI, groupId, (IUri**)&uri);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    contentUris->WithAppendedId(LocalGroups::CONTENT_URI, groupId, (IUri**)&uri);

    AutoPtr<ICursor> c;
    // try {
    Boolean succeeded = FALSE;
    ECode ec = cr->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&c);
    if (SUCCEEDED(ec) && c && (c->MoveToNext(&succeeded), succeeded)) {
        ec = RestoreGroup(c, group);
    }
    // } finally {
    if (c) ICloseable::Probe(c)->Close();
    // }

    return ec;
}

} //Provider
} //Droid
} //Elastos
