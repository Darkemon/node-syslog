var SyslogWrapper = require('./build/Release/syslog').Syslog;

/*
 * Export Syslog as module.
 */
module.exports = {
  init    : SyslogWrapper.init,
  log     : SyslogWrapper.log,
  logSync : SyslogWrapper.logSync,
  setMask : SyslogWrapper.setMask,
  close   : SyslogWrapper.close,
  version : '2.0.0',

  /*
   * Facilities.
   */
  LOG_KERN    : (0<<3),
  LOG_USER    : (1<<3),
  LOG_MAIL    : (2<<3),
  LOG_DAEMON  : (3<<3),
  LOG_AUTH    : (4<<3),
  LOG_SYSLOG  : (5<<3),
  LOG_LPR     : (6<<3),
  LOG_NEWS    : (7<<3),
  LOG_UUCP    : (8<<3),
  LOG_LOCAL0  : (16<<3),
  LOG_LOCAL1  : (17<<3),
  LOG_LOCAL2  : (18<<3),
  LOG_LOCAL3  : (19<<3),
  LOG_LOCAL4  : (20<<3),
  LOG_LOCAL5  : (21<<3),
  LOG_LOCAL6  : (22<<3),
  LOG_LOCAL7  : (23<<3),

  /*
   * Option flag for openlog.
   */
  LOG_PID     : 0x01,
  LOG_CONS    : 0x02,
  LOG_ODELAY  : 0x04,
  LOG_NDELAY  : 0x08,
  LOG_NOWAIT  : 0x10,

  /*
   * Priorities.
   */
  LOG_EMERG   : 0,
  LOG_ALERT   : 1,
  LOG_CRIT    : 2,
  LOG_ERR     : 3,
  LOG_WARNING : 4,
  LOG_NOTICE  : 5,
  LOG_INFO    : 6,
  LOG_DEBUG   : 7
};
