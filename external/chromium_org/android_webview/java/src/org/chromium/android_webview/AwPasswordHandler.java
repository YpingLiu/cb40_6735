package org.chromium.android_webview;

import android.net.WebAddress;
import android.os.AsyncTask;
import android.util.Log;
import android.view.ViewGroup;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

/**
 * Defines save username & password feature java handler.
 * Contains show dialog process & request data from DB.
 */
@JNINamespace("android_webview")
public class AwPasswordHandler {

    private static final String LOGTAG = "AwPasswordHandler";

    private static AwPasswordHandler sInstance = null;
    private static final String SCHEME_HOST_DELIMITER = "://";
    private static boolean sEnableSave = false;
    private long mNativeAwPasswordHandlerDelegateImpl;

    private AwContents mAwContents;
    private AwBrowserContext mBrowserContext = null;
    private PasswordDatabase mDb = null;

    /**
     * To create the save password handler instance or return the existed instance.
     * @param awContents the instance of AwContents
     * @param view the view object to show the dialog if needed
     * @param browserContext the instance of AwBrowserContext
     * @return AwPasswordHandler the instance of the handler
     */
    public static AwPasswordHandler create(AwContents awContents,
                    ViewGroup view, AwBrowserContext browserContext) {
        if (sInstance == null) {
            sInstance = new AwPasswordHandler(awContents, view, browserContext);
        }
        return sInstance;
    }

    /**
     * To get the save password handler instance.
     * @return AwPasswordHandler the instance of the handler
     */
    @CalledByNative
    public static AwPasswordHandler getInstance() {
        Log.d(LOGTAG, "getInstance, sInstance = " + sInstance);
        return sInstance;
    }

    private AwPasswordHandler(AwContents awContents, ViewGroup view,
                              AwBrowserContext browserContext) {
        mAwContents = awContents;
        mBrowserContext = browserContext;
        mDb = mBrowserContext.getPasswordDatabase(view.getContext());
    }

    /**
     * Set the object of AwContents to avoid memory issue.
     * @param awContents the object of AwContents
     */
    public void setViewInfo(AwContents awContents) {
        Log.d(LOGTAG, "setViewInfo, awcontents = " + awContents);
        mAwContents = awContents;
    }

    @CalledByNative
    private void reqeustPassword(
                    final long nativeAwPasswordHandlerDelegateImpl,
                    final String[] data,
                    final String[] data16) {
        WebAddress uri = new WebAddress(data[PasswordDatabase.FIELD_SIGNON_REALM]);
        final String newScheme = uri.getScheme() + SCHEME_HOST_DELIMITER + uri.getHost();
        mNativeAwPasswordHandlerDelegateImpl = nativeAwPasswordHandlerDelegateImpl;
        Log.d(LOGTAG, "reqeustPassword, newScheme = " + newScheme);
        Log.d(LOGTAG, "reqeustPassword, data16 = "
                + data16[0] + ", " + data16[1] + ", " + data16[2] + ", " + data16[3]);
        new AsyncTask<String, Void, PasswordDatabase.RequestRetData>() {
            @Override
            protected PasswordDatabase.RequestRetData doInBackground(String... params) {
                Log.d(LOGTAG, "reqeustPassword doInBackground " + newScheme);
                return mDb.getUsernamePassword(newScheme);
            }

            @Override
            protected void onPostExecute(PasswordDatabase.RequestRetData result) {
                Log.d(LOGTAG, "reqeustPassword onPostExecute " + result);
                if (result != null) {
                    String[] retData16 = new String[4];
                    String[] retData = new String[4];

                    if (!result.signonRealm.isEmpty()) {
                        retData[PasswordDatabase.FIELD_SIGNON_REALM] = result.signonRealm;
                    } else {
                        retData[PasswordDatabase.FIELD_SIGNON_REALM] =
                                    data[PasswordDatabase.FIELD_SIGNON_REALM];
                    }
                    if (!result.originSignonRealm.isEmpty()) {
                        retData[PasswordDatabase.FIELD_ORIGIN_SIGNON_REALM] =
                                    result.originSignonRealm;
                    } else {
                        retData[PasswordDatabase.FIELD_ORIGIN_SIGNON_REALM] =
                                    data[PasswordDatabase.FIELD_ORIGIN_SIGNON_REALM];
                    }
                    if (!result.originSpec.isEmpty()) {
                        retData[PasswordDatabase.FIELD_ORIGIN_SPEC] = result.originSpec;
                    } else {
                        retData[PasswordDatabase.FIELD_ORIGIN_SPEC] =
                                    data[PasswordDatabase.FIELD_ORIGIN_SPEC];
                    }
                    if (!result.actionSpec.isEmpty()) {
                        retData[PasswordDatabase.FIELD_ACTION_SPEC] = result.actionSpec;
                    } else {
                        retData[PasswordDatabase.FIELD_ACTION_SPEC] =
                                    data[PasswordDatabase.FIELD_ACTION_SPEC];
                    }
                    Log.d(LOGTAG, "reqeustPassword onPostExecute, retData = "
                            + retData[0] + ", " + retData[1]);
                    Log.d(LOGTAG, "reqeustPassword onPostExecute, retData = "
                            + retData[2] + ", " + retData[3]);

                    if (!result.usernameName.isEmpty()) {
                        retData16[PasswordDatabase.FIELD_USERNAME_NAME] = result.usernameName;
                    } else {
                        retData16[PasswordDatabase.FIELD_USERNAME_NAME] =
                                    data16[PasswordDatabase.FIELD_USERNAME_NAME];
                    }
                    if (!result.usernameValue.isEmpty()) {
                        retData16[PasswordDatabase.FIELD_USERNAME_VALUE] = result.usernameValue;
                    } else {
                        retData16[PasswordDatabase.FIELD_USERNAME_VALUE] =
                                    data16[PasswordDatabase.FIELD_USERNAME_VALUE];
                    }
                    if (!result.passwordName.isEmpty()) {
                        retData16[PasswordDatabase.FIELD_PASSWORD_NAME] = result.passwordName;
                    } else {
                        retData16[PasswordDatabase.FIELD_PASSWORD_NAME] =
                                    data16[PasswordDatabase.FIELD_PASSWORD_NAME];
                    }
                    retData16[PasswordDatabase.FIELD_PASSWORD_VALUE] = result.passwordValue;
                    Log.d(LOGTAG, "reqeustPassword onPostExecute, retData16 = "
                            + retData16[0] + ", " + retData16[1] + ", "
                            + retData16[2] + ", " + retData16[3]);
                    nativeOnRequestDone(mNativeAwPasswordHandlerDelegateImpl, retData, retData16);
                }
            }
        } .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    /**
     * Save password dialog click handler.
     * @param data the string form data
     * @param data16 the string16 form data
     * @param remember whether to save the password to database
     */
    public void clickHandler(final String[] data, final String[] data16, final boolean remember) {
        WebAddress uri = new WebAddress(data[PasswordDatabase.FIELD_SIGNON_REALM]);
        final String newScheme = uri.getScheme() + SCHEME_HOST_DELIMITER + uri.getHost();
        new AsyncTask<Object, Void, Void>() {
            @Override
            protected Void doInBackground(Object... params) {
                Log.d(LOGTAG, "clickHandler doInBackground " + newScheme + ", " + remember);
                mDb.setUsernamePassword(newScheme, data, data16, remember);
                return null;
            }
        } .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    @CalledByNative
    private void showPasswordDialog(final String[] data, final String[] data16, boolean permitted) {
        Log.d(LOGTAG, "showPasswordDialog permitted " + permitted);
        if (!permitted) {
            clickHandler(data, data16, true);
            return;
        }
        if (mAwContents != null) {
            mAwContents.showPasswordDialog(data, data16);
        }
    }

    /**
     * Save password setting item switch.
     * @param save whether to enable the save password feature
     */
    public static void setSavePassword(boolean save) {
        Log.d(LOGTAG, "setSavePassword " + save);
        sEnableSave = save;
    }

    @CalledByNative
    private boolean getSavePassword() {
        Log.d(LOGTAG, "getSavePassword " + sEnableSave);
        return sEnableSave;
    }

    private native void nativeOnRequestDone(
        long nativeAwPasswordHandlerDelegateImpl, String[] data, String[] data16);
}
