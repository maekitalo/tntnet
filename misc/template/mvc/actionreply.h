#ifndef ACTIONREPLY_H
#define ACTIONREPLY_H

#include "noty.h"

// Represents a reply for a action.
// A action is a http request, which do not expect any output other than
// success or failure or notifications.
// The class is derived from Noty, which adds methods for noty.js notifications.
// On the server side those notifications are displayed when the action request
// is sent back.
//
class ActionReply : public Noty
{
    friend void operator <<= (cxxtools::SerializationInfo& si, const ActionReply& reply);

public:
    ActionReply()
        : _success(true),
          _notLoggedIn(false)
        { }

    void setSuccess(const cxxtools::String& m = cxxtools::String())
    {
        _success = true;
        if (!m.empty())
            success(m);
    }

    void setSuccess(const std::string& m)
    {
        _success = true;
        success(m);
    }

    void setFailed(const cxxtools::String& m)
    {
        _success = false;
        error(m);
    }

    void setFailed(const std::string& m)
    {
        _success = false;
        error(m);
    }

    void setNotLoggedIn()
    {
        _success = false;
        _notLoggedIn = true;
    }

private:
    bool _success;
    bool _notLoggedIn;
};

inline void operator <<= (cxxtools::SerializationInfo& si, const ActionReply& reply)
{
    if (reply._notLoggedIn)
    {
        si.addMember("notLoggedIn") <<= reply._notLoggedIn;
    }
    else
    {
        si.addMember("success") <<= reply._success;
        si.addMember("notifications") <<= static_cast<const Noty&>(reply);
    }
}

#endif // ACTIONREPLY_H

