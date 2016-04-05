package com.android.settings.applications;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

// View Holder used when displaying views
public class AppViewHolder {
    public ApplicationsState.AppEntry entry;
    public View rootView;
    public TextView appName;
    public ImageView appIcon;
    public TextView appSize;
    public TextView disabled;
    public CheckBox checkBox;

    static public AppViewHolder CreateOrRecycle(LayoutInflater inflater, View convertView) {
        if (convertView == NULL) {
            convertView = inflater->Inflate(R.layout.manage_applications_item, NULL);

            // Creates a ViewHolder and store references to the two children views
            // we want to bind data to.
            AppViewHolder holder = new AppViewHolder();
            holder.rootView = convertView;
            holder.appName = (TextView) convertView->FindViewById(R.id.app_name);
            holder.appIcon = (ImageView) convertView->FindViewById(R.id.app_icon);
            holder.appSize = (TextView) convertView->FindViewById(R.id.app_size);
            holder.disabled = (TextView) convertView->FindViewById(R.id.app_disabled);
            holder.checkBox = (CheckBox) convertView->FindViewById(R.id.app_on_sdcard);
            convertView->SetTag(holder);
            return holder;
        } else {
            // Get the ViewHolder back to get fast access to the TextView
            // and the ImageView.
            return (AppViewHolder)convertView->GetTag();
        }
    }

    void UpdateSizeText(CharSequence invalidSizeStr, Int32 whichSize) {
        if (ManageApplications.DEBUG) Logger::I(ManageApplications.TAG, "updateSizeText of " + entry.label + " " + entry
                + ": " + entry.sizeStr);
        if (entry.sizeStr != NULL) {
            switch (whichSize) {
                case ManageApplications.SIZE_INTERNAL:
                    appSize->SetText(entry.internalSizeStr);
                    break;
                case ManageApplications.SIZE_EXTERNAL:
                    appSize->SetText(entry.externalSizeStr);
                    break;
                default:
                    appSize->SetText(entry.sizeStr);
                    break;
            }
        } else if (entry.size == ApplicationsState.SIZE_INVALID) {
            appSize->SetText(invalidSizeStr);
        }
    }
}