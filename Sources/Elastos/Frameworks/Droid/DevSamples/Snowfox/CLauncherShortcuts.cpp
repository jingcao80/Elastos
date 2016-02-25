
#include "CLauncherShortcuts.h"
#include "jansson.h"
#include <elastos/coredef.h>

using Elastos::IO::CFile;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

class FetchWebApps{//:public AsyncTask<Void, Void, Void> {

protected:
    ECode DoInBackground(Void*unused, ...)
    {
        //mWebappsList = NULL;

        AutoPtr<IContext> context;
        //GetApplicationContext((IContext**)&context);

        AutoPtr<IFile> dir;
        //context->GetFilesDir((IFile**)&dir)

        AutoPtr<IFile> home;
        ASSERT_SUCCEEDED(CFile::New((IFile*)dir, String("mozilla"), (IFile**)&home));

        Boolean exist;
        home->Exists(&exist);
        if (!exist)
        {
            AutoPtr<IFile> exDir;
            //context->GetExternalFilesDir(NULL, (IFile**)&exDir);
            String path;
            exDir->GetPath(&path);
            ASSERT_SUCCEEDED(CFile::New(path, String("mozilla") ,(IFile**)&home));
        }

        home->Exists(&exist);
        if (!exist)
            return NOERROR;

        AutoPtr<IFile> profile;
        ArrayOf<String>* files;
        home->List(&files);
        for (Int32 i=0;i< files->GetLength();i++) {
            if ((*files)[i].EndWith(".default")) {
                ASSERT_SUCCEEDED(CFile::New(home, (*files)[i], (IFile**)&profile));
                break;
            }
        }

        ArrayOf<String>::Free(files);

        if (profile == NULL)
            return NOERROR;

        // Save the folder path to be used during click event
        //CFile::New(profile, String("webapps"), (IFile**)&mWebappsFolder);
        //mWebappsFolder->Exists(&exist);
        if (!exist)
            return NOERROR;

        AutoPtr<IFile> webapps;
        CFile::New(mWebappsFolder, String("webapps.json"), (IFile**)&webapps);
        webapps->Exists(&exist);
        if (!exist)
            return NOERROR;

        // Parse the contents into a string
        String webappsJson;
        ECode ec;
        AutoPtr<IFileReader> freader;
        ASSERT_SUCCEEDED(CFileReader::New(webapps, (IFileReader**)&freader));
        AutoPtr<IBufferedReader> in;
        ASSERT_SUCCEEDED(CBufferedReader::New((IFileReader*)freader, (IBufferedReader**)&in));
        String line;
        ec = in->ReadLine(&line);
        if (FAILED(ec) && ec != (ECode)E_IO_EXCEPTION) { goto go_on; }
        while (line != NULL) {
            webappsJson += line;
            ec = in->ReadLine(&line);
            if (FAILED(ec) && ec != (ECode)E_IO_EXCEPTION) { goto go_on; }
        }
go_on:
        if (webappsJson.GetLength() == 0)
            return NOERROR;

        mWebappsList = new List< AutoPtr<StringMap>();

        json_t *webApps;
        json_error_t error;
        json_t * uriObject;
        webApps = json_loads(webappsJson.string(), 0, &error);
        if (webApps == NULL) { return NOERROR; }

        AutoPtr<StringMap> map;

        const char *key;
        json_t *value;
        String mWebappsFolderPath;
        mWebappsFolder->GetPath(&mWebappsFolderPath);

        json_object_foreach(webApps, key, value) {
            map = new StringMap();
            map[String("appKey")] = String (key);
            map[String("favicon")] =  mWebappsFolderPath + "/" + key + "/icon.png";
            map[String("title")] = String(json_string_value(json_object_get(value, "title")));
            map[String("uri")] = String(json_string_value(json_object_get(value, "appURI")));

            mWebappsList->Add(map);
        }

        json_decref(webApps);
        return NOERROR;
    }

    ECode OnPostExecute(Void* unused)
    {

        class OnPostExecuteOnClickListener
            : public ElRefBase
            , public IDialogInterfaceOnClickListener
        {
        public:
            CARAPI_(PInterface) Probe(
                /* [in] */ REIID riid)
            {
                if (riid == EIID_IInterface) {
                    return (PInterface)(IDialogInterfaceOnClickListener*)this;
                }
                else if (riid == EIID_IDialogInterfaceOnClickListener) {
                    return (IDialogInterfaceOnClickListener*)this;
                }

                return NULL;
            }

            CARAPI_(UInt32) AddRef()
            {
                return ElRefBase::AddRef();
            }

            CARAPI_(UInt32) Release()
            {
                return ElRefBase::Release();
            }

            CARAPI GetInterfaceID(
                /* [in] */ IInterface* pObject,
                /* [in] */ InterfaceID* pIID)
            {
                return E_NOT_IMPLEMENTED;
            }

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which)
            {
                FAIL_RETURN(dialog->Dismiss());
                FAIL_RETURN(OnListItemClick(which));
                FAIL_RETURN(Finish());
            }
        };



        class OnPostExecuteOnCancelListener
            : public ElRefBase
            , public IDialogInterfaceOnCancelListener
        {
        public:
            CARAPI_(PInterface) Probe(
                /* [in] */ REIID riid)
            {
                if (riid == EIID_IInterface) {
                    return (PInterface)(IDialogInterfaceOnClickListener*)this;
                }
                else if (riid == EIID_IDialogInterfaceOnClickListener) {
                    return (IDialogInterfaceOnClickListener*)this;
                }

                return NULL;
            }

            CARAPI_(UInt32) AddRef()
            {
                return ElRefBase::AddRef();
            }

            CARAPI_(UInt32) Release()
            {
                return ElRefBase::Release();
            }

            CARAPI GetInterfaceID(
                /* [in] */ IInterface* pObject,
                /* [in] */ InterfaceID* pIID)
            {
                return E_NOT_IMPLEMENTED;
            }

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog)
            {
                FAIL_RETURN(dialog->Dismiss());
                FAIL_RETURN(Finish());
            }
        };

        ECode ec;
        if (mWebappsList != NULL) {
            AutoPtr<IAlertDialogBuilder> builder;
            if (Build::VERSION::SDK_INT >= 11) {
                ASSERT_SUCCEEDED(CAlertDialogBuilder::New(this, AlertDialog_THEME_HOLO_LIGHT, (IAlertDialogBuilder**)&builder));
            }
            else {
                ASSERT_SUCCEEDED(CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder));
            }

            builder->SetTitle(2131034125);

            ArrayOf_<String, 2> from;
            from[0] = "favicon";
            from[0] = "title";
            ArrayOf_<Int32, 2> to;
            to[0] = 2131165188;
            to[0] = 2131165189;
            AutoPtr<ISimpleAdapter> adapter;
            CSimpleAdapter::New(this, mWebappsList, 2130903042, from, to, (ISimpleAdapter**)&adapter);
            AutoPtr<IListAdapter> listAdapter;
            listAdapter = IListAdapter::Probe(adapter);
            builder->SetAdapter((IListAdapter*)listAdapter, new OnPostExecuteOnClickListener());

            builder->SetOnCancelListener(new OnPostExecuteOnCancelListener());
            AutoPtr<IAlertDialog> alterDialog;
            builder->Create((IAlertDialog**)&alterDialog);
            alterDialog->Show();
        }
        else {
            AutoPtr<IToast> toast;
            CToast::MakeText(this, 2131034126, Toast_LENGTH_LONG, (IToast**)&toast);
            toast->Show()
            Finish();
        }
    }
};


ECode CLauncherShortcuts::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    CActivity::OnCreate(savedInstanceState);
    RequestWindowFeature(Window_FEATURE_NO_TITLE);
    SetContentView(2130903041);

    AutoPtr<IIntent> pIIntent = GetIntent();
    assert(pIIntent);
    String action;
    pIIntent->GetAction(&action);

    if (action.Equals(Intent_ACTION_CREATE_SHORTCUT)) {
        // Doing it as a background task, as it involves file access
        //new FetchWebApps().execute();
    }

    return NOERROR;
}

ECode CLauncherShortcuts::OnListItemClick(
    /* [in] */ Int32 id)
{
    AutoPtr<StringMap> map = (*mWebappsList)[id];

    String uri = (*map)[String("uri")];
    String title = (*map)[String("title")];
    String appKey = (*map)[String("appKey")];
    String favicon = (*map)[String("favicon")];

    AutoPtr<IFile> manifestFile;
    CFile::New((IFile*)mWebappsFolder, appKey + "/manifest.json", (IFile**)&manifestFile);

    // Parse the contents into a string
    String manifestJson;
    ECode ec;
    AutoPtr<IFileReader> reader;
    AutoPtr<IBufferedReader> in;
    ASSERT_SUCCEEDED(CFileReader::New((IFile*)manifestFile, (IFileReader**)&reader));
    ASSERT_SUCCEEDED(CBufferedReader::New((IBufferedReader*)reader, (IBufferedReader**)&in));
    String line;
    ec = in->ReadLine(&line);
    if (FAILED(ec) && ec != (ECode)E_IO_EXCEPTION) { goto go_on; }
    while (line != NULL) {
        manifestJson += line;
        ec = in->ReadLine(&line);
        if (FAILED(ec) && ec != (ECode)E_IO_EXCEPTION) { goto go_on; }
    }

go_on:
    json_t *manifest;
    json_error_t error;
    json_t * uriObject;
    manifest = json_loads(manifestJson.string(), 0, &error);
    uri += String(json_string_value(json_object_get(manifest, "launch_path")));
    json_decref(manifest);

    AutoPtr<IIntent> shortcutintent;
    CIntent::New(String("org.mozilla.gecko.WEBAPP"), (IIntent**)&shortcutintent);
    //shortcutintent->SetClass(this, App.class);
    shortcutintent->PutExtra(String("args"), String("--webapp=") + uri);

    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
    intent->PutExtra(Intent_EXTRA_SHORTCUT_NAME, title);
    intent->PutExtra(Intent_EXTRA_SHORTCUT_INTENT, shortcutintent);

    AutoPtr<IDisplayMetrics> dm;
    ASSERT_SUCCEEDED(CDisplayMetrics::New((IDisplayMetrics**)&dm));
    AutoPtr<ILocalWindowManager> wmgr;
    GetWindowManager((ILocalWindowManager**)&wmgr);
    AutoPtr<IDisplay> display;
    wmgr->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<IDisplayMetrics> metrics;
    display->GetMetrics((IDisplayMetrics**)&dm);
    int size;
    switch (dm->mDensityDpi) {
        case DisplayMetrics_DENSITY_MEDIUM:
            size = 48;
            break;
        case DisplayMetrics_DENSITY_HIGH:
            size = 72;
            break;
        default:
            size = 72;
    }

    AutoPtr<IBitmap> bitmap;
    CBitmapFactory::DecodeFile(favicon, (IBitmap**)&bitmap);
    if (bitmap != NULL) {
        AutoPtr<IBitmap> scaledBitmap;
        CBitmap::CreateScaledBitmap(bitmap, size, size, TRUE, (IBitmap**)&scaledBitmap);
        intent->PutExtra(Intent_EXTRA_SHORTCUT_ICON, scaledBitmap);
    }

    // Now, return the result to the launcher
    SetResult(RESULT_OK, intent);
    Finish();

}

CLauncherShortcuts::~CLauncherShortcuts()
{}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
