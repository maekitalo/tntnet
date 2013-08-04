Howto upload files with Tntnet
==============================

This document describes, how to upload files with Tntnet.

Sometimes there is a need for uploading files to the web application. This is
done with a special html input element of type "file". The form element needs to
specify an enctype of with the parameter "multipart/form-data". This enctype
changes the way, form-data is sent to your webapplication. Luckily Tntnet
handles all difficult stuff for you and fill your aruments specified in
`<%args>` sections.

But with upload files the situation is a little different. First the file-data
might be quite large and in would be not the most efficient way to put the data
into a `std::string` like other arguments. The second problem is, that the file
might have a additional attribute: the filename. Therefore Tntnet has a special
API to handle this uploaded data.

The data comes in a special multipart structure, which is represented in Tntnet
by the class `tnt::Multipart`. This has references to all query parameters
including uploaded files.

You get a const reference to this multipart object with the method
`getMultipart()` of your request object. Within this multipart object you can
find your file with the method `find(partName)`. The parameter partName is the
same name, you gave your upload field in your html form. You get a
`tnt::Multipart::const_iterator` to the part. If the file is not found, the
iterator points to the end-iterator retrieved with
`request.getMultipart().end()`.  If found, the dereferenced iterator is a
reference to a `tnt::Part-object`, which represents your uploaded file.  You can
ask for the mime type with `tnt::Part::getMimetype()` and for the filename with
`tnt::Part::getFilename()`.

The simplest way to fetch the data is to call `tnt::Part::getBody()`. You get
the data as a `std::string`. But this is not the most efficient way, because
Tntnet needs instantiate a `std::string` and copy the data. There is a iterator
interface for this. `tnt::Part` defines a `const_iterator`. Iterators to the
start of your body is fetched with the `getBodyBegin()` and part the end with
`getBodyEnd()`. If you don't need a `std::string` this is more efficient.

This all sounds very complicated, but I hope it gets a little clearer, when you
see a example.

The form looks like this:

    <form method="post" enctype="multipart/form-data">
     <input type="file" name="myfile">
     <input type="submit">
    </form>

And the code to process the uploaded file:

    <%cpp>

    const tnt::Multipart& mp = request.getMultipart();
    tnt::Multipart::const_iterator it = mp.find("myfile");
    if (it != mp.end())
    {
      // we found a uploaded file - write it to some upload-area
      std::ofstream out("upload/" + it->getFilename());

      out << it->getBody(); // this is less efficient, because a temporary std::string
                            // is created and the data is copied into it

      // more efficient is the use of iterators:
      for (tnt::Part::const_iterator pi = it->getBodyBegin(); pi != it->getBodyEnd(); ++pi)
        out << *pi;

      // ... or using STL-algorithm and ostreambuf_iterator:
      std::copy(it->getBodyBegin(),
                it->getBodyEnd(),
                std::ostreambuf_iterator<char>(out));
    }

    </%cpp>

The application is a little online hexdumper for the web. The user can upload a
file and see the first 1024 bytes as a hexdump.

You can find another example in the tntnet package in sdk/demos/upload.
