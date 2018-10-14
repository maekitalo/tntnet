define(['jquery', 'tntnet'], function($, tntnet) {

    var content = $('#content');

    $('#mainmenu a').click(function(ev) {

        ev.preventDefault();

        var link = $(this);
        var href = link.attr('href');
        var screenDiv = $('#' + href, content);

        if (screenDiv.length > 0 && screenDiv.is(':visible'))
            return;

        content.children().hide();
        $('#mainmenu a').removeClass('active');

        var mod = 'app/' + href;
        requirejs([mod], function(screen) {
            if (screenDiv.length == 0) {
                screenDiv = $('<div id="' + href + '" hidden/>');
                screenDiv.appendTo(content)
                         .load('html/' + href + '.html', function() {
                             if (typeof screen.onCreate === "function")
                                 screen.onCreate(screenDiv);
                         });
            }

            if (typeof screen.onShow === "function")
                screen.onShow(screenDiv);
            screenDiv.show();
            link.addClass('active');
        });
    });

    $(document).ajaxError(function(Event, xhr, ajaxSettings, thrownError) {
        console.error("error requesting page", ajaxSettings.url, thrownError, xhr);
        tntnet.error(thrownError ? thrownError : xhr.statusText);
    });
});
