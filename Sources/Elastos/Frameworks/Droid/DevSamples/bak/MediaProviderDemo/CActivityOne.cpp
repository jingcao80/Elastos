
#include "CActivityOne.h"
#include "R.h"
#include <stdio.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::CMediaStore;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::CMediaStoreImagesMedia;
using Elastos::Droid::Provider::IMediaStoreImagesMedia;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace MediaProviderDemo {


CAR_INTERFACE_IMPL(CActivityOne::ViewClickListener, IViewOnClickListener)

ECode CActivityOne::ViewClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    if (id == R::id::BroadcastButton) {
        LOGGERD("CActivityOne", "Ready to SendBroadcast message!")
        String text = String("android.intent.action.MEDIA_MOUNTED");
        String tempText;
        AutoPtr<IFile> file;
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        env->GetExternalStorageDirectory((IFile**)&file);
        file->ToString(&tempText);
        tempText = String("file://") + tempText;
        LOGGERD("CActivityOne", "Environment::GetExternalStorageDirectory = %s",tempText.string())
        AutoPtr<IUri> uri;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        FAIL_RETURN(helper->Parse(tempText, (IUri**)&uri));
        AutoPtr<IIntent> pIntent;
        CIntent::New(text, uri, (IIntent**)&pIntent);
        mHost->SendBroadcast(pIntent);
    }
    else if (id == R::id::GetPathButton) {
        LOGGERD("CActivityOne", "Ready to Get Path!")
        AutoPtr<IContentResolver> cr;
        mHost->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(3);
        projections->Set(0, IMediaStoreAudioAudioColumns::ID);
        projections->Set(1, IMediaStoreAudioAudioColumns::DATA);
        projections->Set(2, IMediaStoreAudioAudioColumns::DISPLAY_NAME);
        AutoPtr<ICursor> c;
        AutoPtr<IUri> uri;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->Parse(String("content://media/external/audio/media"), (IUri**)&uri);

        cr->Query(uri, projections, String(NULL), NULL, String(NULL), (ICursor**)&c);

        if (c != NULL) {
            Boolean b;
            Int32 count, id;
            c->GetCount(&count);
            String path, displayName;

            LOGGERD("CActivityOne", "Query Count = %d", count)
            while(c->MoveToNext(&b),b) {
                c->GetInt32(0, &id);
                c->GetString(1, &path);
                c->GetString(2, &displayName);
                LOGGERD("CActivityOne::ViewClickListener::OnClick", "Media id = %d, data = %s, displayName = %s",
                    id, path.string(), displayName.string())
            }

            c->Close();
        }
    }
    else if (id == R::id::QueryImageButton) {
        LOGGERD("CActivityOne", "Ready to Query Image!")
        AutoPtr<IContentResolver> cr;
        mHost->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ICursor> c;
        AutoPtr<IUri> uri;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->Parse(String("content://media/external/images/media"), (IUri**)&uri);

        cr->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&c);
        if (c != NULL) {
            Boolean b;
            Int32 count, id, columnCount;
            String mimeType, path, title;
            c->GetCount(&count);
            c->GetColumnCount(&columnCount);
            LOGGERD("CActivityOne", "Query Image Count = %d, columnCount = %d", count, columnCount)

            AutoPtr<ArrayOf<String> > columnNames;
            c->GetColumnNames((ArrayOf<String>**)&columnNames);
            LOGGERD("CActivityOne", "=== Total columns %d", columnNames->GetLength());
            for (Int32 i = 0; i < columnNames->GetLength(); ++i) {
                LOGGERD("CActivityOne", " >> column %d : %s", i , (*columnNames)[i].string());
            }

            while(c->MoveToNext(&b), b) {
                c->GetInt32(0, &id);
                c->GetString(1, &path);
                c->GetString(4, &mimeType);
                c->GetString(5, &title);
                LOGGERD("CActivityOne::ViewClickListener::OnClick", "Media id = %d, mimeType = %s, path = %s, tile = %s",
                    id, mimeType.string(), path.string(), title.string())
            }

            c->Close();
        }
    }

    else if (id == R::id::QueryMediaImageButton) {
        AutoPtr<IMediaStore> mediaStore;
        CMediaStore::AcquireSingleton((IMediaStore**)&mediaStore);
        String version;
        mediaStore->GetVersion((IContext*)mHost->Probe(EIID_IContext), &version);
        LOGGERD("CActivityOne", "Ready to Query Media Image!, mediaStore version: %s", version.string())

        AutoPtr<IMediaStoreImagesMedia> imageMedia;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&imageMedia);
        AutoPtr<IContentResolver> cr;
        mHost->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IUri> uri;
        imageMedia->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        String uriStr;
        uri->ToString(&uriStr);
        LOGGERD("CActivityOne", "Query Media Image uri = %s", uriStr.string());

        AutoPtr<ICursor> c;
        imageMedia->Query(cr, uri, NULL, (ICursor**)&c);
        if (c != NULL) {
            Boolean b;
            Int32 count, id, columnCount;
            String mimeType, path, title;
            c->GetCount(&count);
            c->GetColumnCount(&columnCount);
            LOGGERD("CActivityOne", "Query Media Image Count = %d, columnCount = %d", count, columnCount)

            AutoPtr<ArrayOf<String> > columnNames;
            c->GetColumnNames((ArrayOf<String>**)&columnNames);
            LOGGERD("CActivityOne", "=== Total columns %d", columnNames->GetLength());
            for (Int32 i = 0; i < columnNames->GetLength(); ++i) {
                LOGGERD("CActivityOne", " >> column %d : %s", i , (*columnNames)[i].string());
            }

            while(c->MoveToNext(&b), b) {
                c->GetInt32(0, &id);
                c->GetString(1, &path);
                c->GetString(4, &mimeType);
                c->GetString(5, &title);
                LOGGERD("CActivityOne::ViewClickListener::OnClick", "Media id = %d, mimeType = %s, path = %s, tile = %s",
                    id, mimeType.string(), path.string(), title.string())
            }

            c->Close();
        }
    }

    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<ViewClickListener> listener = new ViewClickListener(this);

    AutoPtr<IView> view = FindViewById(R::id::BroadcastButton);
    assert(view != NULL);
    view->SetOnClickListener(listener.Get());

    view = FindViewById(R::id::GetPathButton);
    assert(view != NULL);
    view->SetOnClickListener(listener.Get());

    view = FindViewById(R::id::QueryImageButton);
    assert(view != NULL);
    view->SetOnClickListener(listener.Get());

    view = FindViewById(R::id::QueryMediaImageButton);
    assert(view != NULL);
    view->SetOnClickListener(listener.Get());

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnRestart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

} // namespace MediaProviderDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
