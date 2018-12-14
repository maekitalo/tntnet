// interface class for JavaScript noty

#ifndef NOTY_H
#define NOTY_H

#include <cxxtools/serializationinfo.h>
#include <cxxtools/string.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/timespan.h>

// Represents a single notification, which can be displayed using noty.js
struct Notification
{
    enum Severity {
        ERROR = 4,
        WARNING = 3,
        INFORMATION = 2,
        NOTIFICATION = 1,
        SUCCESS = 0
    };

    Notification() { }

    Notification(Severity s, const cxxtools::String& m, cxxtools::Timespan t)
        : severity(s),
          message(m),
          timeout(t)
          { }

    Notification(Severity s, const std::string& m, cxxtools::Timespan t)
        : severity(s),
          message(cxxtools::Utf8Codec::decode(m)),
          timeout(t)
          { }

    Severity severity;
    cxxtools::String message;
    cxxtools::Timespan timeout;
};

// Represents notifications, which can be displayed using noty.js
class Noty
{
    friend void operator <<= (cxxtools::SerializationInfo& si, const Noty& reply);

public:
    Noty() { }

    void error(const cxxtools::String& msg, cxxtools::Milliseconds timeout = -1)
        { _notifications.push_back(Notification(Notification::ERROR, msg, timeout)); }

    void error(const std::string& msg, cxxtools::Milliseconds timeout = -1)
        { _notifications.push_back(Notification(Notification::ERROR, msg, timeout)); }

    void warning(const cxxtools::String& msg, cxxtools::Milliseconds timeout = -1)
        { _notifications.push_back(Notification(Notification::WARNING, msg, timeout)); }

    void warning(const std::string& msg, cxxtools::Milliseconds timeout = -1)
        { _notifications.push_back(Notification(Notification::WARNING, msg, timeout)); }

    void information(const cxxtools::String& msg, cxxtools::Milliseconds timeout = 5000)
        { _notifications.push_back(Notification(Notification::INFORMATION, msg, timeout)); }

    void information(const std::string& msg, cxxtools::Milliseconds timeout = 5000)
        { _notifications.push_back(Notification(Notification::INFORMATION, msg, timeout)); }

    void notification(const cxxtools::String& msg, cxxtools::Milliseconds timeout = 5000)
        { _notifications.push_back(Notification(Notification::NOTIFICATION, msg, timeout)); }

    void notification(const std::string& msg, cxxtools::Milliseconds timeout = 5000)
        { _notifications.push_back(Notification(Notification::NOTIFICATION, msg, timeout)); }

    void success(const cxxtools::String& msg, cxxtools::Milliseconds timeout = 5000)
        { _notifications.push_back(Notification(Notification::SUCCESS, msg, timeout)); }

    void success(const std::string& msg, cxxtools::Milliseconds timeout = 5000)
        { _notifications.push_back(Notification(Notification::SUCCESS, msg, timeout)); }

private:
    std::vector<Notification> _notifications;
};

inline void operator <<= (cxxtools::SerializationInfo& si, const Notification& notification)
{
    si.addMember("severity") <<= notification.severity;
    si.addMember("message") <<= notification.message;
    si.addMember("timeout") <<= static_cast<double>(cxxtools::Milliseconds(notification.timeout));
}

inline void operator <<= (cxxtools::SerializationInfo& si, const Noty& noty)
{
    si <<= noty._notifications;
}

#endif // NOTY_H

