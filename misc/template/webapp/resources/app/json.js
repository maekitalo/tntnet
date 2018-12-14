define(['jquery'], function($) {
    return {
        onCreate: function(content) {
            $.getJSON('example.json', function(result) {
                $('#loadtime').html(result.dt);
            });

            $('[name=getdata]', content).click(function() {
                $.getJSON('example.json', function(result) {
                    $('#clicktime').html(result.dt);
                });
            });
        },
        onShow: function(content) {
            $.getJSON('example.json', function(result) {
                $('#updatetime').html(result.dt);
            });
        }
    }
});
