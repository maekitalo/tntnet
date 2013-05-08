function getRequest()
{
  try { return new XMLHttpRequest();                   } catch (e) { }
  try { return new ActiveXObject("Msxml2.XMLHttp");    } catch (e) { }
  try { return new ActiveXObject("Microsoft.XMLHTTP"); } catch (e) { }
  return null;
}

function ajaxGet(url, fn, failFn)
{
  request = getRequest();
  request.open("GET", url);
  request.onreadystatechange = function () {
      if (request.readyState == 4)
      {
        if (request.status == 200)
          fn(request);
        else if (failFn != null)
          failFn(request);
      }
    }
  request.send(null);
}
