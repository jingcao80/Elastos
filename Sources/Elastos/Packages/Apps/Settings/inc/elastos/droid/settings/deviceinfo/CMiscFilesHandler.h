#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CMISCFILESHANDLER_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CMISCFILESHANDLER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Settings_Deviceinfo_CMiscFilesHandler.h"
#include "elastos/droid/app/ListActivity.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ListActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IMultiChoiceModeListener;
using Elastos::IO::IFile;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * This class handles the selection and removal of Misc files.
 */
CarClass(CMiscFilesHandler)
    , public ListActivity
{
public:
    class MemoryMearurementAdapter
        : public BaseAdapter
    {
    private:
        class GetViewOnCheckedChangeListener
            : public Object
            , public ICompoundButtonOnCheckedChangeListener
        {
        public:
            TO_STRING_IMPL("CMiscFilesHandler::MemoryMearurementAdapter::GetViewOnCheckedChangeListener")

            CAR_INTERFACE_DECL()

            GetViewOnCheckedChangeListener(
                /* [in] */ MemoryMearurementAdapter* host,
                /* [in] */ IListView* listView,
                /* [in] */ Int32 listPosition);

            //@Override
            CARAPI OnCheckedChanged(
                /* [in] */ ICompoundButton* buttonView,
                /* [in] */ Boolean isChecked);

        private:
            MemoryMearurementAdapter* mHost;
            AutoPtr<IListView> mListView;
            Int32 mListPosition;
        };

        class GetOnLongClickListener
            : public Object
            , public IViewOnLongClickListener
        {
        public:
            TO_STRING_IMPL("CMiscFilesHandler::MemoryMearurementAdapter::GetOnLongClickListener")

            CAR_INTERFACE_DECL()

            GetOnLongClickListener(
                /* [in] */ MemoryMearurementAdapter* host,
                /* [in] */ IListView* listView,
                /* [in] */ Int32 listPosition,
                /* [in] */ Boolean viewIsChecked);

            //@Override
            CARAPI OnLongClick(
                /* [in] */ IView* v,
                /* [out] */ Boolean* result);

        private:
            MemoryMearurementAdapter* mHost;
            AutoPtr<IListView> mListView;
            Int32 mListPosition;
            Boolean mViewIsChecked;
        };

        class GetOnClickListener
            : public Object
            , public IViewOnClickListener
        {
        public:
            TO_STRING_IMPL("CMiscFilesHandler::MemoryMearurementAdapter::GetOnClickListener")

            CAR_INTERFACE_DECL()

            GetOnClickListener(
                /* [in] */ MemoryMearurementAdapter* host,
                /* [in] */ IListView* listView,
                /* [in] */ Int32 listPosition,
                /* [in] */ Boolean viewIsChecked);

            //@Override
            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            MemoryMearurementAdapter* mHost;
            AutoPtr<IListView> mListView;
            Int32 mListPosition;
            Boolean mViewIsChecked;
        };

    public:
        TO_STRING_IMPL("CMiscFilesHandler::MemoryMearurementAdapter")

        MemoryMearurementAdapter(
            /* [in] */ IActivity* activity,
            /* [in] */ CMiscFilesHandler* host);

        //@Override
        CARAPI GetCount(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** result); //StorageMeasurement.FileInfo

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        CARAPI RemoveAll(
            /* [in] */ IList* objs); //List<Object>

        CARAPI_(Int64) GetDataSize();

        //@Override
        CARAPI NotifyDataSetChanged();

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        AutoPtr<IList> mData; //ArrayList<StorageMeasurement.FileInfo>
        Int64 mDataSize;
        AutoPtr<IContext> mContext;
        CMiscFilesHandler* mHost;
    };

private:
    class ModeCallback
        : public Object
        , public IMultiChoiceModeListener
    {
    public:
        TO_STRING_IMPL("CMiscFilesHandler::ModeCallback")

        CAR_INTERFACE_DECL()

        ModeCallback(
            /* [in] */ IContext* context,
            /* [in] */ CMiscFilesHandler* host);

        CARAPI OnCreateActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

        CARAPI OnPrepareActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

        CARAPI OnActionItemClicked(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        CARAPI OnDestroyActionMode(
            /* [in] */ IActionMode* mode);

        CARAPI OnItemCheckedStateChanged(
            /* [in] */ IActionMode* mode,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [in] */ Boolean checked);

    private:
        // Deletes all files and subdirectories under given dir.
        // Returns TRUE if all deletions were successful.
        // If a deletion fails, the method stops attempting to delete and returns FALSE.
        CARAPI_(Boolean) DeleteDir(
            /* [in] */ IFile* dir);

    private:
        Int32 mDataCount;
        AutoPtr<IContext> mContext;
        CMiscFilesHandler* mHost;
    };

public:
    TO_STRING_IMPL("CMiscFilesHandler")

    CMiscFilesHandler();

    virtual ~CMiscFilesHandler();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    static const String TAG;
    String mNumSelectedFormat;
    String mNumBytesSelectedFormat;
    AutoPtr<MemoryMearurementAdapter> mAdapter;
    AutoPtr<ILayoutInflater> mInflater;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CMISCFILESHANDLER_H__