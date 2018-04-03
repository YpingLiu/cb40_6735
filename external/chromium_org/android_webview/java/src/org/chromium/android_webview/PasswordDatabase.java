package org.chromium.android_webview;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.util.Log;

/**
 * Defines save username & password feature database.
 */
public class PasswordDatabase {

    private static final String LOGTAG = "PasswordDatabase";

    private static final int DATABASE_VERSION = 1;

    private SQLiteDatabase mDatabase = null;

    private static final String ID_COL = "_id";

    private static final String[] ID_PROJECTION = new String[] {
        ID_COL
    };

    // column id strings for "password" table
    private static final String PASSWORD_TABLE_NAME         = "passwordtb";
    private static final String PASSWORD_HOST_COL           = "host";
    private static final String PASSWORD_REMEMBER_COL       = "remember";
    private static final String PASSWORD_REALM_COL          = "signon_realm";
    private static final String PASSWORD_ORIGIN_REALM_COL   = "origin_signon_realm";
    private static final String PASSWORD_ORIGIN_SPEC_COL    = "origin_spec";
    private static final String PASSWORD_ACTION_SPEC_COL    = "action_spec";
    private static final String PASSWORD_USERNAME_NAME_COL  = "username_name";
    private static final String PASSWORD_USERNAME_VALUE_COL = "username_value";
    private static final String PASSWORD_PASSWORD_NAME_COL  = "password_name";
    private static final String PASSWORD_PASSWORD_VALUE_COL = "password_value";

    // the offset of the fields contained in data16 String[]
    public static final int FIELD_USERNAME_NAME  = 0;
    public static final int FIELD_USERNAME_VALUE = 1;
    public static final int FIELD_PASSWORD_NAME  = 2;
    public static final int FIELD_PASSWORD_VALUE = 3;

    // the offset of the fields contained in data String[]
    public static final int FIELD_SIGNON_REALM        = 0;
    public static final int FIELD_ORIGIN_SIGNON_REALM = 1;
    public static final int FIELD_ORIGIN_SPEC         = 2;
    public static final int FIELD_ACTION_SPEC         = 3;

    /**
     * Initially false until the background thread completes.
     */
    private boolean mInitialized = false;

    /**
     * Defines DB request data structure.
     */
    public static class RequestRetData {
        public boolean found;
        public boolean remember;
        public String usernameName;
        public String usernameValue;
        public String passwordName;
        public String passwordValue;
        public String signonRealm;
        public String originSignonRealm;
        public String originSpec;
        public String actionSpec;

        /**
         * Constructor.
         */
        public RequestRetData() {
            found = false;
        }
    }

    /**
     * Create an instance of PasswordDatabase for the named file, and kick-off background
     * initialization of that database.
     *
     * @param context the Context to use for opening the database.
     * @param databaseFile Name of the file to be initialized.
     */
    public PasswordDatabase(final Context context, final String databaseFile) {
        Log.d(LOGTAG, "constructor " + databaseFile);
        new Thread() {
            @Override
            public void run() {
                initOnBackgroundThread(context, databaseFile);
            }
        } .start();
    }

    /**
     * Initializes the databases and notifies any callers waiting on waitForInit.
     *
     * @param context the Context to use for opening the database.
     * @param databaseFile Name of the file to be initialized.
     */
    private synchronized void initOnBackgroundThread(Context context, String databaseFile) {
        if (mInitialized) {
            return;
        }

        initDatabase(context, databaseFile);

        // Thread done, notify.
        mInitialized = true;
        notifyAll();
    }

    /**
     * Opens the database, and upgrades it if necessary.
     *
     * @param context the Context to use for opening the database.
     * @param databaseFile Name of the file to be initialized.
     */
    private void initDatabase(Context context, String databaseFile) {
        try {
            mDatabase = context.openOrCreateDatabase(databaseFile, 0, null);
        } catch (SQLiteException e) {
            // try again by deleting the old db and create a new one
            if (context.deleteDatabase(databaseFile)) {
                mDatabase = context.openOrCreateDatabase(databaseFile, 0, null);
            }
        }

        if (mDatabase == null) {
            // Not much we can do to recover at this point
            Log.e(LOGTAG, "Unable to open or create " + databaseFile);
            return;
        }

        if (mDatabase.getVersion() != DATABASE_VERSION) {
            mDatabase.beginTransactionNonExclusive();
            try {
                createTable();
                mDatabase.setTransactionSuccessful();
            } finally {
                mDatabase.endTransaction();
            }
        }
    }

    private void createTable() {
        mDatabase.execSQL("CREATE TABLE " + PASSWORD_TABLE_NAME
                + " (" + ID_COL + " INTEGER PRIMARY KEY, "
                + PASSWORD_HOST_COL + " TEXT, "
                + PASSWORD_REMEMBER_COL + " INTEGER, "
                + PASSWORD_REALM_COL + " TEXT, "
                + PASSWORD_ORIGIN_REALM_COL + " TEXT, "
                + PASSWORD_ORIGIN_SPEC_COL + " TEXT, "
                + PASSWORD_ACTION_SPEC_COL + " TEXT, "
                + PASSWORD_USERNAME_NAME_COL + " TEXT, "
                + PASSWORD_USERNAME_VALUE_COL + " TEXT, "
                + PASSWORD_PASSWORD_NAME_COL + " TEXT, "
                + PASSWORD_PASSWORD_VALUE_COL + " TEXT," + " UNIQUE ("
                + PASSWORD_HOST_COL + ") ON CONFLICT REPLACE);");

        mDatabase.setVersion(DATABASE_VERSION);
    }

    /**
     * Waits for the background initialization thread to complete and check the database creation
     * status.
     *
     * @return true if the database was initialized, false otherwise.
     */
    private boolean waitForInit() {
        synchronized (this) {
            while (!mInitialized) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    Log.e(LOGTAG, "Caught exception while checking initialization", e);
                }
            }
        }
        return mDatabase != null;
    }

    /**
     * Set password. Tuple (PASSWORD_HOST_COL) is unique.
     *
     * @param schemePlusHost the unique key.
     * @param data signon_realm, origin_signon_realm, origin, action.
     * @param data16 username_name, username_value, password_name,
     *            password_value.
     * @param remember whether save the password of the schemePlusHost.
     */
    public void setUsernamePassword(String schemePlusHost, String[] data,
                                    String[] data16, boolean remember) {
        if (data == null || data16 == null || !waitForInit()) {
            return;
        }

        final ContentValues c = new ContentValues();
        Log.d(LOGTAG, "setUsernamePassword " + schemePlusHost + ", " + remember);
        c.put(PASSWORD_HOST_COL, schemePlusHost);
        c.put(PASSWORD_REMEMBER_COL, (remember ? 1 : 0));

        c.put(PASSWORD_REALM_COL, (remember ? data[FIELD_SIGNON_REALM] : ""));
        c.put(PASSWORD_ORIGIN_REALM_COL, (remember ? data[FIELD_ORIGIN_SIGNON_REALM] : ""));
        c.put(PASSWORD_ORIGIN_SPEC_COL, (remember ? data[FIELD_ORIGIN_SPEC] : ""));
        c.put(PASSWORD_ACTION_SPEC_COL, (remember ? data[FIELD_ACTION_SPEC] : ""));
        if (remember) {
            Log.d(LOGTAG, "setUsernamePassword1 " + data[FIELD_SIGNON_REALM] +
                          ", " + data[FIELD_ORIGIN_SIGNON_REALM]);
            Log.d(LOGTAG, "setUsernamePassword2 " + data[FIELD_ORIGIN_SPEC] +
                          ", " + data[FIELD_ACTION_SPEC]);
        }

        c.put(PASSWORD_USERNAME_NAME_COL, (remember ? data16[FIELD_USERNAME_NAME] : ""));
        c.put(PASSWORD_USERNAME_VALUE_COL, (remember ? data16[FIELD_USERNAME_VALUE] : ""));
        c.put(PASSWORD_PASSWORD_NAME_COL, (remember ? data16[FIELD_PASSWORD_NAME] : ""));
        c.put(PASSWORD_PASSWORD_VALUE_COL, (remember ? data16[FIELD_PASSWORD_VALUE] : ""));
        if (remember) {
            Log.d(LOGTAG, "setUsernamePassword3 " + data16[FIELD_USERNAME_NAME] +
                          ", " + data16[FIELD_USERNAME_VALUE]);
            Log.d(LOGTAG, "setUsernamePassword4 " + data16[FIELD_PASSWORD_NAME] +
                          ", " + data16[FIELD_PASSWORD_VALUE]);
        }
        mDatabase.insert(PASSWORD_TABLE_NAME, PASSWORD_REALM_COL, c);
    }

    /**
     * Retrieve the username and password for a given host.
     *
     * @param schemePlusHost The scheme and host which passwords applies to.
     * @return RequestRetData Contains all the data of schemePlusHost.
     */
    public RequestRetData getUsernamePassword(String schemePlusHost) {
        if (schemePlusHost == null || !waitForInit()) {
            return null;
        }

        final String[] columns = new String[] {
            PASSWORD_REALM_COL,
            PASSWORD_REMEMBER_COL,
            PASSWORD_ORIGIN_REALM_COL,
            PASSWORD_ORIGIN_SPEC_COL,
            PASSWORD_ACTION_SPEC_COL,
            PASSWORD_USERNAME_NAME_COL,
            PASSWORD_USERNAME_VALUE_COL,
            PASSWORD_PASSWORD_NAME_COL,
            PASSWORD_PASSWORD_VALUE_COL
        };
        final String selection = "(" + PASSWORD_HOST_COL + " == ?)";

        RequestRetData ret = null;
        Cursor cursor = null;
        try {
            cursor = mDatabase.query(PASSWORD_TABLE_NAME, columns, selection,
                    new String[] { schemePlusHost }, null, null, null);
            if (cursor.moveToFirst()) {
                ret = new RequestRetData();
                ret.found = true;
                ret.remember =
                    (cursor.getInt(cursor.getColumnIndex(PASSWORD_REALM_COL)) == 1) ? true : false;
                ret.signonRealm = cursor.getString(cursor.getColumnIndex(PASSWORD_REALM_COL));
                ret.originSignonRealm =
                    cursor.getString(cursor.getColumnIndex(PASSWORD_ORIGIN_REALM_COL));
                ret.originSpec = cursor.getString(cursor.getColumnIndex(PASSWORD_ORIGIN_SPEC_COL));
                ret.actionSpec = cursor.getString(cursor.getColumnIndex(PASSWORD_ACTION_SPEC_COL));

                ret.usernameName =
                    cursor.getString(cursor.getColumnIndex(PASSWORD_USERNAME_NAME_COL));
                ret.usernameValue =
                    cursor.getString(cursor.getColumnIndex(PASSWORD_USERNAME_VALUE_COL));
                ret.passwordName =
                    cursor.getString(cursor.getColumnIndex(PASSWORD_PASSWORD_NAME_COL));
                ret.passwordValue =
                    cursor.getString(cursor.getColumnIndex(PASSWORD_PASSWORD_VALUE_COL));
            }
        } catch (IllegalStateException e) {
            Log.e(LOGTAG, "getUsernamePassword ", e);
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return ret;
    }

    /**
     * Check whether the host is set to remember.
     *
     * @param schemePlusHost The scheme and host which passwords applies to.
     * @return true if set as remember, false otherwise.
     */
    public boolean isRemembered(String schemePlusHost) {
        if (schemePlusHost == null || !waitForInit()) {
            return false;
        }

        final String[] columns = new String[] {
            PASSWORD_REMEMBER_COL
        };
        final String selection = "(" + PASSWORD_HOST_COL + " == ?)";

        Cursor cursor = null;
        try {
            cursor = mDatabase.query(PASSWORD_TABLE_NAME, columns, selection,
                    new String[] { schemePlusHost }, null, null, null);
            if (cursor.moveToFirst()) {
                return (cursor.getInt(
                            cursor.getColumnIndex(PASSWORD_REALM_COL)) == 1) ? true : false;
            }
        } catch (IllegalStateException e) {
            Log.e(LOGTAG, "isRemembered ", e);
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return false;
    }

    /**
     * Determines if there are any passwords saved.
     *
     * @return true if there are passwords saved
     */
    public boolean hasUsernamePassword() {
        if (!waitForInit()) {
            return false;
        }

        Cursor cursor = null;
        boolean ret = false;
        try {
            cursor = mDatabase.query(PASSWORD_TABLE_NAME, ID_PROJECTION, null, null, null, null,
                    null);
            ret = cursor.moveToFirst();
        } catch (IllegalStateException e) {
            Log.e(LOGTAG, "hasUsernamePassword", e);
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return ret;
    }

    /**
     * Clears the password database.
     */
    public void clearUsernamePassword() {
        if (!waitForInit()) {
            return;
        }
        mDatabase.delete(PASSWORD_TABLE_NAME, null, null);
    }
}

