#!/usr/bin/node

var sys = require('sys'),
	Syslog = require('./nodesyslog').Syslog;
	
	Syslog.init("node-syslog-test", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_NEWS);
	Syslog.log(Syslog.LOG_INFO,"news info log test");
	Syslog.log(Syslog.LOG_ERR,"news log error test");
	Syslog.log(Syslog.LOG_DEBUG,"Last log message as debug:"+new Date());
	

