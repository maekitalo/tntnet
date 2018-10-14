define(['jquery', 'tntnet'], function($, tntnet) {
    return {
        onCreate: function(content) {
            $('[name=savedata]', content).click(function() {
                tntnet.action('insertdata', {
                    number: $('[name=number]', content).val(),
                    name: $('[name=name]', content).val()
                });
            });
        }
    };
});
