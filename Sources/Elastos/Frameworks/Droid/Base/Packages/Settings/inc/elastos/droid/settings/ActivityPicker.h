/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings;

using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Intent::IShortcutIconResource;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IPaintDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

/**
 * Displays a list of all activities matching the incoming
 * {@link Intent#EXTRA_INTENT} query, along with any injected items.
 */
public class ActivityPicker extends AlertActivity implements
        DialogInterface.OnClickListener, DialogInterface.OnCancelListener {
    
    /**
     * Adapter of items that are displayed in this dialog.
     */
    private PickAdapter mAdapter;
    
    /**
     * Base {@link Intent} used when building list.
     */
    private Intent mBaseIntent;
    
    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        
        final Intent intent = GetIntent();
        
        // Read base intent from extras, otherwise assume default
        Parcelable parcel = intent->GetParcelableExtra(Intent.EXTRA_INTENT);
        if (parcel instanceof Intent) {
            mBaseIntent = (Intent) parcel;
        } else {
            mBaseIntent = new Intent(IIntent::ACTION_MAIN, NULL);
            mBaseIntent->AddCategory(Intent.CATEGORY_DEFAULT);
        }

        // Create dialog parameters
        AlertController.AlertParams params = mAlertParams;
        params.mOnClickListener = this;
        params.mOnCancelListener = this;
        
        // Use custom title if provided, otherwise default window title
        if (intent->HasExtra(Intent.EXTRA_TITLE)) {
            params.mTitle = intent->GetStringExtra(Intent.EXTRA_TITLE);
        } else {
            params.mTitle = GetTitle();
        }
        
        // Build list adapter of pickable items
        List<PickAdapter.Item> items = GetItems();
        mAdapter = new PickAdapter(this, items);
        params.mAdapter = mAdapter;

        SetupAlert();
    }
    
    /**
     * Handle clicking of dialog item by passing back
     * {@link #GetIntentForPosition(Int32)} in {@link #SetResult(Int32, Intent)}.
     */
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        Intent intent = GetIntentForPosition(which);
        SetResult(Activity.RESULT_OK, intent);
        Finish();
    }
    
    /**
     * Handle canceled dialog by passing back {@link Activity#RESULT_CANCELED}.
     */
    CARAPI OnCancel(DialogInterface dialog) {
        SetResult(Activity.RESULT_CANCELED);
        Finish();
    }

    /**
     * Build the specific {@link Intent} for a given list position. Convenience
     * method that calls through to {@link PickAdapter.Item#GetIntent(Intent)}.
     */
    protected Intent GetIntentForPosition(Int32 position) {
        PickAdapter.Item item = (PickAdapter.Item) mAdapter->GetItem(position);
        return item->GetIntent(mBaseIntent);
    }

    /**
     * Build and return list of items to be shown in dialog. Default
     * implementation mixes activities matching {@link #mBaseIntent} from
     * {@link #PutIntentItems(Intent, List)} with any injected items from
     * {@link Intent#EXTRA_SHORTCUT_NAME}. Override this method in subclasses to
     * change the items shown.
     */
    protected List<PickAdapter.Item> GetItems() {
        PackageManager packageManager = GetPackageManager();
        List<PickAdapter.Item> items = new ArrayList<PickAdapter.Item>();
        
        // Add any injected pick items
        final Intent intent = GetIntent();
        ArrayList<String> labels =
            intent->GetStringArrayListExtra(Intent.EXTRA_SHORTCUT_NAME);
        ArrayList<ShortcutIconResource> icons =
            intent->GetParcelableArrayListExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE);
        
        if (labels != NULL && icons != NULL && labels->Size() == icons->Size()) {
            for (Int32 i = 0; i < labels->Size(); i++) {
                String label = labels->Get(i);
                Drawable icon = NULL;
                
                try {
                    // Try loading icon from requested package
                    ShortcutIconResource iconResource = icons->Get(i);
                    Resources res = packageManager->GetResourcesForApplication(
                            iconResource.packageName);
                    icon = res->GetDrawable(res->GetIdentifier(
                            iconResource.resourceName, NULL, NULL));
                } catch (NameNotFoundException e) {
                    // Ignore
                }
                
                items->Add(new PickAdapter->Item(this, label, icon));
            }
        }

        // Add any intent items if base was given
        if (mBaseIntent != NULL) {
            PutIntentItems(mBaseIntent, items);
        }
        
        return items;
    }

    /**
     * Fill the given list with any activities matching the base {@link Intent}. 
     */
    protected void PutIntentItems(Intent baseIntent, List<PickAdapter.Item> items) {
        PackageManager packageManager = GetPackageManager();
        List<ResolveInfo> list = packageManager->QueryIntentActivities(baseIntent,
                0 /* no flags */);
        Collections->Sort(list, new ResolveInfo->DisplayNameComparator(packageManager));
        
        final Int32 listSize = list->Size();
        for (Int32 i = 0; i < listSize; i++) {
            ResolveInfo resolveInfo = list->Get(i);
            items->Add(new PickAdapter->Item(this, packageManager, resolveInfo));
        }
    }
    
    /**
     * Adapter which shows the set of activities that can be performed for a
     * given {@link Intent}.
     */
    protected static class PickAdapter extends BaseAdapter {
        
        /**
         * Item that appears in a {@link PickAdapter} list.
         */
        public static class Item implements AppWidgetLoader.LabelledItem {
            protected static IconResizer sResizer;
            
            protected IconResizer GetResizer(Context context) {
                if (sResizer == NULL) {
                    final Resources resources = context->GetResources();
                    Int32 size = (Int32) resources->GetDimension(android.R.dimen.app_icon_size);
                    sResizer = new IconResizer(size, size, resources->GetDisplayMetrics());
                }
                return sResizer;
            }
            
            CharSequence label;
            Drawable icon;
            String packageName;
            String className;
            Bundle extras;
            
            /**
             * Create a list item from given label and icon.
             */
            Item(Context context, CharSequence label, Drawable icon) {
                this.label = label;
                this.icon = GetResizer(context).CreateIconThumbnail(icon);
            }

            /**
             * Create a list item and fill it with details from the given
             * {@link ResolveInfo} object.
             */
            Item(Context context, PackageManager pm, ResolveInfo resolveInfo) {
                label = resolveInfo->LoadLabel(pm);
                if (label == NULL && resolveInfo.activityInfo != NULL) {
                    label = resolveInfo.activityInfo.name;
                }

                icon = GetResizer(context).CreateIconThumbnail(resolveInfo->LoadIcon(pm));
                packageName = resolveInfo.activityInfo.applicationInfo.packageName;
                className = resolveInfo.activityInfo.name;
            }

            /**
             * Build the {@link Intent} described by this item. If this item
             * can't create a valid {@link android.content.ComponentName}, it will return
             * {@link IIntent::ACTION_CREATE_SHORTCUT} filled with the item label.
             */
            Intent GetIntent(Intent baseIntent) {
                Intent intent = new Intent(baseIntent);
                if (packageName != NULL && className != NULL) {
                    // Valid package and class, so fill details as normal intent
                    intent->SetClassName(packageName, className);
                    if (extras != NULL) {
                        intent->PutExtras(extras);
                    }
                } else {
                    // No valid package or class, so treat as shortcut with label
                    intent->SetAction(IIntent::ACTION_CREATE_SHORTCUT);
                    intent->PutExtra(Intent.EXTRA_SHORTCUT_NAME, label);
                }
                return intent;
            }

            public CharSequence GetLabel() {
                return label;
            }
        }
        
        private final LayoutInflater mInflater;
        private final List<Item> mItems;
        
        /**
         * Create an adapter for the given items.
         */
        public PickAdapter(Context context, List<Item> items) {
            mInflater = (LayoutInflater) context->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mItems = items;
        }

        /**
         * {@inheritDoc}
         */
        public Int32 GetCount() {
            return mItems->Size();
        }

        /**
         * {@inheritDoc}
         */
        public Object GetItem(Int32 position) {
            return mItems->Get(position);
        }

        /**
         * {@inheritDoc}
         */
        public Int64 GetItemId(Int32 position) {
            return position;
        }

        /**
         * {@inheritDoc}
         */
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            if (convertView == NULL) {
                convertView = mInflater->Inflate(R.layout.pick_item, parent, FALSE);
            }
            
            Item item = (Item) GetItem(position);
            TextView textView = (TextView) convertView;
            textView->SetText(item.label);
            textView->SetCompoundDrawablesWithIntrinsicBounds(item.icon, NULL, NULL, NULL);
            
            return convertView;
        }
    }
        
    /**
     * Utility class to resize icons to match default icon size. Code is mostly
     * borrowed from Launcher.
     */
    private static class IconResizer {
        private final Int32 mIconWidth;
        private final Int32 mIconHeight;

        private final DisplayMetrics mMetrics;
        private final Rect mOldBounds = new Rect();
        private final Canvas mCanvas = new Canvas();
        
        public IconResizer(Int32 width, Int32 height, DisplayMetrics metrics) {
            mCanvas->SetDrawFilter(new PaintFlagsDrawFilter(Paint.DITHER_FLAG,
                    Paint.FILTER_BITMAP_FLAG));

            mMetrics = metrics;
            mIconWidth = width;
            mIconHeight = height; 
        }

        /**
         * Returns a Drawable representing the thumbnail of the specified Drawable.
         * The size of the thumbnail is defined by the dimension
         * android.R.dimen.launcher_application_icon_size.
         *
         * This method is not thread-safe and should be invoked on the UI thread only.
         *
         * @param icon The icon to get a thumbnail of.
         *
         * @return A thumbnail for the specified icon or the icon itself if the
         *         thumbnail could not be created. 
         */
        public Drawable CreateIconThumbnail(Drawable icon) {
            Int32 width = mIconWidth;
            Int32 height = mIconHeight;

            if (icon == NULL) {
                return new EmptyDrawable(width, height);
            }
            
            try {
                if (icon instanceof PaintDrawable) {
                    PaintDrawable painter = (PaintDrawable) icon;
                    painter->SetIntrinsicWidth(width);
                    painter->SetIntrinsicHeight(height);
                } else if (icon instanceof BitmapDrawable) {
                    // Ensure the bitmap has a density.
                    BitmapDrawable bitmapDrawable = (BitmapDrawable) icon;
                    Bitmap bitmap = bitmapDrawable->GetBitmap();
                    if (bitmap->GetDensity() == Bitmap.DENSITY_NONE) {
                        bitmapDrawable->SetTargetDensity(mMetrics);
                    }
                }
                Int32 iconWidth = icon->GetIntrinsicWidth();
                Int32 iconHeight = icon->GetIntrinsicHeight();
    
                if (iconWidth > 0 && iconHeight > 0) {
                    if (width < iconWidth || height < iconHeight) {
                        final Float ratio = (Float) iconWidth / iconHeight;
    
                        if (iconWidth > iconHeight) {
                            height = (Int32) (width / ratio);
                        } else if (iconHeight > iconWidth) {
                            width = (Int32) (height * ratio);
                        }
    
                        final Bitmap.Config c = icon->GetOpacity() != PixelFormat.OPAQUE ?
                                    Bitmap.Config.ARGB_8888 : Bitmap.Config.RGB_565;
                        final Bitmap thumb = Bitmap->CreateBitmap(mIconWidth, mIconHeight, c);
                        final Canvas canvas = mCanvas;
                        canvas->SetBitmap(thumb);
                        // Copy the old bounds to restore them later
                        // If we were to do oldBounds = icon->GetBounds(),
                        // the call to SetBounds() that follows would
                        // change the same instance and we would lose the
                        // old bounds
                        mOldBounds->Set(icon->GetBounds());
                        final Int32 x = (mIconWidth - width) / 2;
                        final Int32 y = (mIconHeight - height) / 2;
                        icon->SetBounds(x, y, x + width, y + height);
                        icon->Draw(canvas);
                        icon->SetBounds(mOldBounds);
                        //noinspection deprecation
                        icon = new BitmapDrawable(thumb);
                        ((BitmapDrawable) icon).SetTargetDensity(mMetrics);
                        canvas->SetBitmap(NULL);
                    } else if (iconWidth < width && iconHeight < height) {
                        final Bitmap.Config c = Bitmap.Config.ARGB_8888;
                        final Bitmap thumb = Bitmap->CreateBitmap(mIconWidth, mIconHeight, c);
                        final Canvas canvas = mCanvas;
                        canvas->SetBitmap(thumb);
                        mOldBounds->Set(icon->GetBounds());
                        final Int32 x = (width - iconWidth) / 2;
                        final Int32 y = (height - iconHeight) / 2;
                        icon->SetBounds(x, y, x + iconWidth, y + iconHeight);
                        icon->Draw(canvas);
                        icon->SetBounds(mOldBounds);
                        //noinspection deprecation
                        icon = new BitmapDrawable(thumb);
                        ((BitmapDrawable) icon).SetTargetDensity(mMetrics);
                        canvas->SetBitmap(NULL);
                    }
                }
    
            } catch (Throwable t) {
                icon = new EmptyDrawable(width, height);
            }

            return icon;
        }
    }

    private static class EmptyDrawable extends Drawable {
        private final Int32 mWidth;
        private final Int32 mHeight;

        EmptyDrawable(Int32 width, Int32 height) {
            mWidth = width;
            mHeight = height;
        }

        //@Override
        public Int32 GetIntrinsicWidth() {
            return mWidth;
        }

        //@Override
        public Int32 GetIntrinsicHeight() {
            return mHeight;
        }

        //@Override
        public Int32 GetMinimumWidth() {
            return mWidth;
        }

        //@Override
        public Int32 GetMinimumHeight() {
            return mHeight;
        }

        //@Override
        CARAPI Draw(Canvas canvas) {
        }

        //@Override
        CARAPI SetAlpha(Int32 alpha) {
        }

        //@Override
        CARAPI SetColorFilter(ColorFilter cf) {
        }

        //@Override
        public Int32 GetOpacity() {
            return PixelFormat.TRANSLUCENT;
        }
    }
}
