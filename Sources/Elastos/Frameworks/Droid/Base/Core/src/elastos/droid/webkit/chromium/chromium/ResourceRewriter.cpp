
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

void ResourceRewriter::sRewriteRValues(
    /* [in] */ IContext* ctx)
{
    // Rewrite the R 'constants' for the WebView library apk.
    SparseArray<String> packageIdentifiers = ctx.getResources().getAssets()
            .getAssignedPackageIdentifiers();

    const Int32 N = packageIdentifiers.size();
    for (Int32 i = 0; i < N; i++) {
        const String name = packageIdentifiers.valueAt(i);

        // The resources are always called com.android.webview even if the manifest has had the
        // package renamed.
        if (String("com.android.webview").Equals(name)) {
            const Int32 id = packageIdentifiers.keyAt(i);

            // TODO: We should use jarjar to remove the redundant R classes here, but due
            // to a bug in jarjar it's not possible to rename classes with '$' in their name.
            // See b/15684775.
            com::android::webview::chromium::R::OnResourcesLoaded(id);
            org::chromium::ui::R::OnResourcesLoaded(id);
            org::chromium::content::R::OnResourcesLoaded(id);

            break;
        }
    }
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
