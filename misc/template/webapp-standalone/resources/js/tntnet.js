define(['jquery', 'Noty'], function($, Noty)
{

    var noty = function(options)
    {
        return new Noty(options).show();
    };

    return {
        error: function(msg, timeout)
        {
            if (timeout < 0)
                timeout = null;

            return noty({
                text: msg,
                type: 'error',
                timeout: timeout
            });
        },

        warning: function(msg, timeout)
        {
            if (timeout < 0)
                timeout = null;

            return noty({
                text: msg,
                type: 'warning',
                timeout: timeout
            });
        },

        information: function(msg, timeout)
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
        },

        notification: function(msg, timeout)
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
        },

        success: function(msg, timeout)
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
        },

        action: function(url, data, successFn, failureFn)
        {
            var my = this;
            return $.post(url + ".action", data,
                function(data, textStatus, jqXHR)
                {
                    if (data.success)
                    {
                        if (successFn)
                            successFn(data, textStatus, jqXHR);
                    }
                    else
                    {
                        if (failureFn)
                            failureFn(data, textStatus, jqXHR);
                    }

                    for (var i = 0; i < data.notifications.length; ++i)
                    {
                        var n = data.notifications[i];
                        if (n.severity === 0)
                            my.success(n.message, n.timeout);
                        else if (n.severity === 1)
                            my.notification(n.message, n.timeout);
                        else if (n.severity === 2)
                            my.information(n.message, n.timeout);
                        else if (n.severity === 3)
                            my.warning(n.message, n.timeout);
                        else
                            my.error(n.message, n.timeout);
                    }

                }, 'json');
        }
    };

});
