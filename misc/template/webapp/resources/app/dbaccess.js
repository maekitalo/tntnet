define(['jquery'], function($) {
    return {
        onCreate: function(content) {
            $('[name=getdata]', content).click(function() {
                $.getJSON('dbaccess.json', {
                        number: $('#dbnumber', content).val()
                    },
                    function(result) {
                        $('#dbname').html(result.name);
                    });
            });
        },

        onShow: function(content) {
            this.updateSelection();
        },

        updateSelection: function() {
            $.getJSON("getnumbers.json", function(names) {
                var sel = $('#dbnumber');
                sel.empty();
                $('<option>').appendTo(sel);
                $.each(names, function(idx, number) {
                    $('<option>', { value: number, text: number }).appendTo(sel);
                });
            });
        }
    };
});
