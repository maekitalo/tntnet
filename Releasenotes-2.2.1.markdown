Releasenotes tntnet 2.2.1
=========================

2.2.1 is a bug fix release. It fixes 3 bugs. Those are:

 - a racing condition may result in a pthread unlock error
 - in some circumstances request headers are not cleared correctly and hence may occur in subsequent requests again
 - changing the root dir of tntnet did not work
