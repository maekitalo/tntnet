$(function() {

  function calc(op)
  {
    $.getJSON('/docalc', {
            arg1 : $('#arg1').val(),
            arg2 : $('#arg2').val(),
            op   :   op
          },
          function(result)
          {
            if (result.resultOk)
            {
              $('#outarg1').html(result.arg1);
              $('#outarg2').html(result.arg2);
              $('#outop').html(result.op);
              $('#outresult').html(result.result);
              $('#result').show();
            }
            else
            {
              $('#result').hide();
            }
          });
  }

  $('#plus').click(function() { calc('+') });
  $('#minus').click(function() { calc('-') });
  $('#mul').click(function() { calc('*') });
  $('#div').click(function() { calc('/') });
});
