$(function() { 

  var timeout = 0;
  var cb = function() {
    $('#messages').load(
        '/messages',
        { timeout: timeout },
        cb);

    timeout = 30;
  }

  cb();

  var message = $('#message');

  message.focus();
  $('#send').click(
      function() {
        $.post('/put', {
              msg: message.val()
          }
        );

        message.val('').focus();
      }
    );
});
