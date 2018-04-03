/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright 2001-2004 The Apache Software Foundation.
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


package org.apache.commons.logging.impl;

import java.io.Serializable;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.logging.Log;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.FileHandler;
import java.util.logging.Formatter;
import java.util.logging.LogRecord;

/**
 * <p>Implementation of the <code>org.apache.commons.logging.Log</code>
 * interface that wraps the standard JDK logging mechanisms that were
 * introduced in the Merlin release (JDK 1.4).</p>
 *
 * @author <a href="mailto:sanders@apache.org">Scott Sanders</a>
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @author <a href="mailto:donaldp@apache.org">Peter Donald</a>
 * @version $Revision: 370652 $ $Date: 2006-01-19 22:23:48 +0000 (Thu, 19 Jan 2006) $
 *
 * @deprecated Please use {@link java.net.URL#openConnection} instead.
 *     Please visit <a href="http://android-developers.blogspot.com/2011/09/androids-http-clients.html">this webpage</a>
 *     for further details.
 */

@Deprecated
public class Jdk14Logger implements Log, Serializable {

    /**
     * This member variable simply ensures that any attempt to initialise
     * this class in a pre-1.4 JVM will result in an ExceptionInInitializerError.
     * It must not be private, as an optimising compiler could detect that it
     * is not used and optimise it away.
     */
    protected static final Level dummyLevel = Level.FINE;

    protected static int sLogLevel = 0;

    // ----------------------------------------------------------- Constructors

    ///M: Support log to file.

    /**
      * Class for log foramt of file name
      */
    private class HttpSimpleFormatter extends Formatter {

        HttpSimpleFormatter() {
            super();
        }

        /**
         * Converts a {@link LogRecord} object into a human readable string
         * representation.
         *
         * @param r
         *            the log record to be formatted into a string.
         * @return the formatted string.
         */
        @Override
        public String format(LogRecord r) {
            StringBuilder sb = new StringBuilder();
            SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd:HH-mm-ss");
            Date date = new Date(r.getMillis());
            sb.append(dateFormat.format(date)).append(" ");
            sb.append(String.valueOf(r.getMillis())).append(" ");
            sb.append("http").append(" ");
            sb.append(r.getThreadID()).append(" ");
            sb.append(formatMessage(r)).append(System.lineSeparator());
            return sb.toString();
        }


    }

    /**
     * Construct a named instance of this Logger.
     *
     * @param name Name of the logger to be constructed
     */
    public Jdk14Logger(String name) {

        this.name = name;
        logger = getLogger();

        ///M: Support http log controlled by Android SystemProperties @{
        try {
            Class<?> classType = Class.forName("android.os.SystemProperties");
            Method getIntMethod  = classType.getDeclaredMethod("getInt", String.class, int.class);
            Integer v = (Integer) getIntMethod.invoke(classType, "net.httplog.level", 0);
            sLogLevel = v.intValue();

            if (sLogLevel == 2 && name.indexOf("org.apache.http.wire") != -1) {
                configLogFile();
            }
        } catch (ClassNotFoundException c) {
            System.out.println("error:" + c.getMessage());
        } catch (NoSuchMethodException ie) {
            System.out.println("error:" + ie.getMessage());
        } catch (InvocationTargetException iee) {
            System.out.println("error:" + iee.getMessage());
        } catch (IllegalAccessException iae) {
            System.out.println("error:" + iae.getMessage());
        }
        ///@}
    }

    ///M: HTTP OTA message logging
    private void configLogFile() {
        try {
            FileHandler fh;
            Date date = new Date() ;
            SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
            Thread t = Thread.currentThread();

            File folder = new File("/data/http");
            if (!folder.isDirectory()) {
                folder.mkdir();
            }

            //String filePath = "http_" + dateFormat.format(date) + ".log";
            String filePath = "%thttp.log";
            String tempPath = System.getProperty("java.io.tmpdir");
            System.out.println("http log file path:" + tempPath + ":" + filePath);

            fh = new FileHandler(filePath, true);
            fh.setFormatter(new HttpSimpleFormatter());
            logger.addHandler(fh);
        } catch (SecurityException e) {
            System.out.println("error:" + e.getMessage());
        } catch (IOException ioe) {
            System.out.println("error:" + ioe.getMessage());
        }
    }


    // ----------------------------------------------------- Instance Variables


    /**
     * The underlying Logger implementation we are using.
     */
    protected transient Logger logger = null;


    /**
     * The name of the logger we are wrapping.
     */
    protected String name = null;


    // --------------------------------------------------------- Public Methods

    private void log( Level level, String msg, Throwable ex ) {

        Logger logger = getLogger();
        if (logger.isLoggable(level)) {
            // Hack (?) to get the stack trace.
            Throwable dummyException=new Throwable();
            StackTraceElement locations[]=dummyException.getStackTrace();
            // Caller will be the third element
            String cname="unknown";
            String method="unknown";
            if( locations!=null && locations.length >2 ) {
                StackTraceElement caller=locations[2];
                cname=caller.getClassName();
                method=caller.getMethodName();
            }
            if( ex==null ) {
                ///M: Dyamic http logging @{
                if (sLogLevel == 1) {
                    if (name.indexOf("org.apache.http.headers") != -1) {
                        System.out.println(name + ":" + method + ":" + msg);
                    }
                } else if (sLogLevel == 2) {
                    if (name.indexOf("org.apache.http.wire") != -1) {
                        System.out.println(name + ":" + method + ":" + msg);
                    }
                } else if (sLogLevel == 3) {
                    System.out.println(name + ":" + method + ":" + msg);
                }
                //@}
                logger.logp( level, cname, method, msg );
            } else {
                logger.logp( level, cname, method, msg, ex );
            }
        }

    }

    /**
     * Logs a message with <code>java.util.logging.Level.FINE</code>.
     *
     * @param message to log
     * @see org.apache.commons.logging.Log#debug(Object)
     */
    public void debug(Object message) {
        log(Level.FINE, String.valueOf(message), null);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.FINE</code>.
     *
     * @param message to log
     * @param exception log this cause
     * @see org.apache.commons.logging.Log#debug(Object, Throwable)
     */
    public void debug(Object message, Throwable exception) {
        log(Level.FINE, String.valueOf(message), exception);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.SEVERE</code>.
     *
     * @param message to log
     * @see org.apache.commons.logging.Log#error(Object)
     */
    public void error(Object message) {
        log(Level.SEVERE, String.valueOf(message), null);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.SEVERE</code>.
     *
     * @param message to log
     * @param exception log this cause
     * @see org.apache.commons.logging.Log#error(Object, Throwable)
     */
    public void error(Object message, Throwable exception) {
        log(Level.SEVERE, String.valueOf(message), exception);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.SEVERE</code>.
     *
     * @param message to log
     * @see org.apache.commons.logging.Log#fatal(Object)
     */
    public void fatal(Object message) {
        log(Level.SEVERE, String.valueOf(message), null);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.SEVERE</code>.
     *
     * @param message to log
     * @param exception log this cause
     * @see org.apache.commons.logging.Log#fatal(Object, Throwable)
     */
    public void fatal(Object message, Throwable exception) {
        log(Level.SEVERE, String.valueOf(message), exception);
    }


    /**
     * Return the native Logger instance we are using.
     */
    public Logger getLogger() {
        if (logger == null) {
            logger = Logger.getLogger(name);
        }

        ///M: Dymaic debug logging @{
        if (sLogLevel > 0) {
            logger.setLevel(Level.ALL);
        }
        //@}

        return (logger);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.INFO</code>.
     *
     * @param message to log
     * @see org.apache.commons.logging.Log#info(Object)
     */
    public void info(Object message) {
        log(Level.INFO, String.valueOf(message), null);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.INFO</code>.
     *
     * @param message to log
     * @param exception log this cause
     * @see org.apache.commons.logging.Log#info(Object, Throwable)
     */
    public void info(Object message, Throwable exception) {
        log(Level.INFO, String.valueOf(message), exception);
    }


    /**
     * Is debug logging currently enabled?
     */
    public boolean isDebugEnabled() {
        return (getLogger().isLoggable(Level.FINE));
    }


    /**
     * Is error logging currently enabled?
     */
    public boolean isErrorEnabled() {
        return (getLogger().isLoggable(Level.SEVERE));
    }


    /**
     * Is fatal logging currently enabled?
     */
    public boolean isFatalEnabled() {
        return (getLogger().isLoggable(Level.SEVERE));
    }


    /**
     * Is info logging currently enabled?
     */
    public boolean isInfoEnabled() {
        return (getLogger().isLoggable(Level.INFO));
    }


    /**
     * Is trace logging currently enabled?
     */
    public boolean isTraceEnabled() {
        return (getLogger().isLoggable(Level.FINEST));
    }


    /**
     * Is warn logging currently enabled?
     */
    public boolean isWarnEnabled() {
        return (getLogger().isLoggable(Level.WARNING));
    }


    /**
     * Logs a message with <code>java.util.logging.Level.FINEST</code>.
     *
     * @param message to log
     * @see org.apache.commons.logging.Log#trace(Object)
     */
    public void trace(Object message) {
        log(Level.FINEST, String.valueOf(message), null);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.FINEST</code>.
     *
     * @param message to log
     * @param exception log this cause
     * @see org.apache.commons.logging.Log#trace(Object, Throwable)
     */
    public void trace(Object message, Throwable exception) {
        log(Level.FINEST, String.valueOf(message), exception);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.WARNING</code>.
     *
     * @param message to log
     * @see org.apache.commons.logging.Log#warn(Object)
     */
    public void warn(Object message) {
        log(Level.WARNING, String.valueOf(message), null);
    }


    /**
     * Logs a message with <code>java.util.logging.Level.WARNING</code>.
     *
     * @param message to log
     * @param exception log this cause
     * @see org.apache.commons.logging.Log#warn(Object, Throwable)
     */
    public void warn(Object message, Throwable exception) {
        log(Level.WARNING, String.valueOf(message), exception);
    }


}
