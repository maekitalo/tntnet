function error(msg, timeout)
{
    if (timeout < 0)
        timeout = null;

    return noty({
        text: msg,
        type: 'error',
        timeout: timeout
    });
}

function warning(msg, timeout)
{
    if (timeout < 0)
        timeout = null;

    return noty({
        text: msg,
        type: 'warning',
        timeout: timeout
    });
}

function information(msg, timeout)
{
    if (!timeout)
        timeout = 5000;
    else if (timeout < 0)
        timeout = null;

    return noty({
        text: msg,
        type: 'information',
        timeout: timeout
    });
}

function notification(msg, timeout)
{
    if (!timeout)
        timeout = 5000;
    else if (timeout < 0)
        timeout = null;

    return noty({
        text: msg,
        type: 'notification',
        timeout: timeout
    });
}

function success(msg, timeout)
{
    if (!timeout)
        timeout = 5000;
    else if (timeout < 0)
        timeout = null;

    return noty({
        text: msg,
        type: 'success',
        timeout: timeout
    });
}

function processNotifications(notifications)
{
    for (var i = 0; i < notifications.length; ++i)
    {
        var n = notifications[i];
        if (n.severity === 0)
            success(n.message, n.timeout);
        else if (n.severity === 1)
            notification(n.message, n.timeout);
        else if (n.severity === 2)
            information(n.message, n.timeout);
        else if (n.severity === 3)
            warning(n.message, n.timeout);
        else
            error(n.message, n.timeout);
    }

}

var tntnet = {

    action:
        function(url, data, successFn, failureFn)
        {
            return $.post(url + ".action", data,
                function(data, textStatus, jqXHR)
                {
                    if (data.success)
                    {
                        if (successFn)
                            successFn(data, textStatus, jqXHR);
                    }
                    else if (data.notLoggedIn)
                    {
                        warning("session timed out");
                        setTimeout(function() {
                            window.location = '/';
                        }, 3000);
                    }
                    else
                    {
                        if (failureFn)
                            failureFn(data, textStatus, jqXHR);
                    }

                    processNotifications(data.notifications);

                }, 'json');
        },
};
