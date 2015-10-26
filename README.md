 [![Build Status](https://travis-ci.org/Darkemon/node-syslog.svg?branch=master)](https://travis-ci.org/Darkemon/node-syslog.svg?branch=master)

# Node-Syslog

v2.0.0

This is a node module (add-on) to work with syslog (system log) daemon on unix systems.

Read the [setMask wiki page](https://github.com/schamane/node-syslog/wiki/setMask) for using the `setMask` functionality.

The current version is compatible to node 4.0.x and higher.

Node-syslog does not officially support Darwin OS and MS Windows but should work fine.

## Installation

### manual

      git clone
      node-gyp configure build

## Usage

For more information about how to use module check test.js

     var Syslog = require('./node-syslog');

     Syslog.init("node-syslog-test", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0);
     Syslog.log(Syslog.LOG_INFO, "news info log test", function() {
       console.log("syslog callback");
     });
     Syslog.log(Syslog.LOG_ERR, "news log error test");
     Syslog.logSync(Syslog.LOG_DEBUG, "Last log message as debug: " + new Date());
     Syslog.close();

Check your /var/log/messages (syslog, syslog-ng), or /var/log/everything/current (metalog) file for any test entry.
