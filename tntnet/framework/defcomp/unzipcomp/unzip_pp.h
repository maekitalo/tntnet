/* unzip_pp.h
   Copyright (C) 2003 Tommi MÃ¤kitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef UNZIP_PP_H
#define UNZIP_PP_H

#include "unzip.h"
#include <stdexcept>
#include <iostream>

class unzipError : public std::runtime_error
{
    int err;

  public:
    unzipError(int e, const std::string& msg = "unzipError")
      : std::runtime_error(msg),
        err(e)
        { }

    int getErr() const  { return err; }
};

class unzipFile
{
    unzFile file;

    int checkError(int err, const char* function) const;

  public:
    unzipFile(const std::string& path)
    {
      if (!(file = unzOpen(path.c_str())))
        throw std::runtime_error("error opening " + path);
    }

    ~unzipFile()
    {
      if (file)
        unzClose(file);
    }

    void goToFirstFile()
    { checkError(unzGoToFirstFile(file), "unzGoToFirstFile"); }

    void goToNextFile()
    { checkError(unzGoToNextFile(file), "unzGoToNextFile"); }

    void locateFile(const std::string& fileName, bool caseSensitivity)
    { checkError(unzLocateFile(file, fileName.c_str(), caseSensitivity ? 1 : 0), "unzLocateFile"); }

    void openCurrentFile()
    { checkError(unzOpenCurrentFile(file), "unzOpenCurrentFile"); }

    void openCurrentFile(const std::string& pw)
    { checkError(unzOpenCurrentFilePassword(file, pw.c_str()), "unzOpenCurrentFilePassword"); }

    void closeCurrentFile()
    { checkError(unzCloseCurrentFile(file), "unzCloseCurrentFile"); }

    int readCurrentFile(voidp buf, unsigned len)
    { return checkError(unzReadCurrentFile(file, buf, len), "unzReadCurrentFile"); }

    unzFile getHandle()        { return file; }
};

class unzipFileStreamBuf : public std::streambuf
{
    size_t bufsize;
    char_type buffer[512];
    unzipFile& file;

  public:
    unzipFileStreamBuf(unzipFile& f, const std::string& fileName, bool caseSensitivity)
      : file(f)
    {
      f.locateFile(fileName, caseSensitivity);
      f.openCurrentFile();
    }

    unzipFileStreamBuf(unzipFile& f, const std::string& fileName, bool caseSensitivity,
      const std::string& password)
      : file(f)
    {
      f.locateFile(fileName, caseSensitivity);
      f.openCurrentFile(password);
    }

    ~unzipFileStreamBuf()
    { file.closeCurrentFile(); }

    /// überladen aus std::streambuf
    int_type overflow(int_type c);
    /// überladen aus std::streambuf
    int_type underflow();
    /// überladen aus std::streambuf
    int sync();


};

class unzipFileStream : public std::istream
{
    unzipFileStreamBuf streambuf;

  public:
    unzipFileStream(unzipFile& f, const std::string& fileName, bool caseSensitivity)
      : std::istream(&streambuf),
        streambuf(f, fileName, caseSensitivity)
    { }
};

#endif // UNZIP_PP_H

