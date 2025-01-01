#include <syslog.h>

int main()
{
    openlog("syslog_test", LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "This is a test message");
    syslog(LOG_INFO, "This is another test message");
    syslog(LOG_ERR, "This is an error message");
    closelog();
    return 0;
}