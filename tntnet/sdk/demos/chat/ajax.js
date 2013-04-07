function getRequest()
{
  try { return new XMLHttpRequest();                   } catch (e) { }
  try { return new ActiveXObject("Msxml2.XMLHttp");    } catch (e) { }
  try { return new ActiveXObject("Microsoft.XMLHTTP"); } catch (e) { }
  return null;
}

function ajaxGet(url, parameter, fn, failFn)
{
  request = getRequest();
  request.open("POST", url, true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.onreadystatechange = function () {
      if (request.readyState == 4)
      {
        if (request.status == 200)
        {
          if (fn != null)
            fn(request);
        }
        else if (failFn != null)
          failFn(request);
      }
    }
  request.send(parameter);
}
