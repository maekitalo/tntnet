////////////////////////////////////////////////////////////////////////
// unzip_pp.h
//

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

    int checkError(int err)
    {
      if (err < 0)
        throw unzipError(err);
      return err;
    }

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
    { checkError(unzGoToFirstFile(file)); }

    void goToNextFile()
    { checkError(unzGoToNextFile(file)); }

    void locateFile(const std::string& fileName, bool caseSensitivity)
    { checkError(unzLocateFile(file, fileName.c_str(), caseSensitivity ? 1 : 0)); }

    void openCurrentFile()
    { checkError(unzOpenCurrentFile(file)); }

    void openCurrentFile(const std::string& pw)
    { checkError(unzOpenCurrentFilePassword(file, pw.c_str())); }

    void closeCurrentFile()
    { checkError(unzCloseCurrentFile(file)); }

    int readCurrentFile(voidp buf, unsigned len)
    { return checkError(unzReadCurrentFile(file, buf, len)); }

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

