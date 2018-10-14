define(['jquery', 'tntnet'], function($, tntnet) {
    return {
        onCreate: function(content) {
            $('[name=action]', content).click(function() {
                tntnet.action('myaction');
            });

            $('[name=error]', content).click(function() {
                tntnet.error('this is a error');
            });

            $('[name=warning]', content).click(function() {
                tntnet.warning('this is a warning');
            });

            $('[name=information]', content).click(function() {
                tntnet.information('this is a information');
            });

            $('[name=notification]', content).click(function() {
                tntnet.notification('this is a notification');
            });

            $('[name=success]', content).click(function() {
                tntnet.success('this is a success');
            });

        }
    }
});
