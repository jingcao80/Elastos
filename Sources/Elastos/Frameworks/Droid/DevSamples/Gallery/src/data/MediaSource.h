
#ifndef __MEDIASOURCE_H__
#define __MEDIASOURCE_H__

#include <elautoptr.h>
#include <cmdef.h>
#include "elastos/droid/app/Activity.h"
#include "Path.h"
#include "MediaObject.h"
#include "MediaSet.h"

namespace Gallery {

class MediaSource
    : public ElRefBase
    , public IInterface
{
public:
    class PathId
        : public ElRefBase
        , public IInterface
    {
    public:
        PathId(
            /* [in] */ Path* path,
            /* [in] */ Int32 id);

        CAR_INTERFACE_DECL()

    public:
        AutoPtr<Path> path;
        Int32 id;
    };

public:
    MediaSource(
        /* [in] */ const String& prefix);

    CAR_INTERFACE_DECL()

    CARAPI_(String) GetPrefix();

    virtual CARAPI_(AutoPtr<Path>) FindPathByUri(
        /* [in] */ IUri* uri,
        /* [in] */ const String& type);

    virtual CARAPI_(AutoPtr<MediaObject>) CreateMediaObject(
        /* [in] */ Path* path) = 0;

    virtual CARAPI_(void) Resume();

    virtual CARAPI_(void) Pause();

    virtual CARAPI_(AutoPtr<Path>) GetDefaultSetOf(
        /* [in] */ Path* item);

    virtual CARAPI_(Int64) GetTotalUsedCacheSize();

    virtual CARAPI_(Int64) GetTotalTargetCacheSize();

    virtual CARAPI_(void) MapMediaItems(
        /* [in] */ ArrayOf<PathId*>* list,
        /* [in] */ MediaSet::ItemConsumer* consumer);

private:
    static const String TAG;
    String mPrefix;

};

} // namespace Gallery

#endif // __MEDIASOURCE_H__