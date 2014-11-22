/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "tnt/ecppc/ecppc.h"
#include "tnt/ecppc/generator.h"
#include "tnt/ecppc/dependencygenerator.h"
#include <tnt/mimedb.h>
#include <fstream>
#include <sstream>
#include <functional>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include <cxxtools/log.h>

log_define("tntnet.ecppc")

namespace tnt
{
  namespace ecppc
  {
    Ecppc::Ecppc(int& argc, char* argv[])
      : _requestname(cxxtools::Arg<std::string>(argc, argv, 'n')),
        _inputfile(0),
        _ofile(cxxtools::Arg<std::string>(argc, argv, 'o')),
        _odir(cxxtools::Arg<std::string>(argc, argv, 'O')),
        _mimetype(cxxtools::Arg<std::string>(argc, argv, 'm')),
        _mimedb(cxxtools::Arg<std::string>(argc, argv, "--mimetypes", "/etc/mime.types")),
        _logCategory(cxxtools::Arg<std::string>(argc, argv, 'l')),
        _binary(cxxtools::Arg<bool>(argc, argv, 'b')),
        _multibinary(cxxtools::Arg<bool>(argc, argv, 'b')),
        _keepPath(cxxtools::Arg<bool>(argc, argv, 'p')),
        _compress(cxxtools::Arg<bool>(argc, argv, 'z')),
        _verbose(cxxtools::Arg<bool>(argc, argv, 'v')),
        _generateDependencies(cxxtools::Arg<bool>(argc, argv, 'M')),
        _disableLinenumbers(cxxtools::Arg<bool>(argc, argv, 'L')),
        _help(cxxtools::Arg<bool>(argc, argv, 'h')),
        _helpLong(cxxtools::Arg<bool>(argc, argv, "--help"))
    {
      // TODO: Rewrite usage string to simply be put to stdout
      // instead of stderr as when throwing and catching in main
      if(_help || _helpLong)
        throw Usage(argv[0]);

      while (true)
      {
        cxxtools::Arg<std::string> include(argc, argv, 'I');
        if (!include.isSet())
          break;
        log_debug("include: " << include.getValue());
        _includes.push_back(include);
      }

      if (_multibinary)
      {
        cxxtools::Arg<const char*> ifiles(argc, argv, 'i');
        if (ifiles.isSet())
        {
          std::ifstream in(ifiles.getValue());
          std::string ifile;
          while (std::getline(in, ifile))
          {
            std::string key = ifile;
            if (!_keepPath)
            {
              // strip path
              std::string::size_type p;
              if ((p = key.find_last_of('/')) != std::string::npos)
                key.erase(0, p + 1);
            }

            _inputfiles.insert(inputfiles_type::value_type(key, ifile));
          }
        }

        for (int a = 1; a < argc; ++a)
        {
          std::string ifile = argv[a];
          std::string key = ifile;
          if (!_keepPath)
          {
            // strip path
            std::string::size_type p;
            if ((p = key.find_last_of('/')) != std::string::npos)
              key.erase(0, p + 1);
          }

          _inputfiles.insert(inputfiles_type::value_type(key, ifile));
        }
      }
      else
      {
        if (argc < 2 || argv[1][0] == '-')
          throw Usage(argv[0]);
        _inputfile = argv[1];
      }
    }

    int Ecppc::run()
    {
      // requestname from inputfilename
      if (_requestname.empty())
      {
        if (_multibinary)
        {
          std::cerr << "warning: no requestname passed (with -n) - using \"images\"" << std::endl;
          _requestname = "images";
        }
        else
        {
          std::string input = _inputfile;
          std::string::size_type pos_dot = input.find_last_of(".");
          if (pos_dot != std::string::npos)
          {
            std::string::size_type pos_slash;
            if (_keepPath || (pos_slash = input.find_last_of("\\/")) == std::string::npos)
              _requestname = input.substr(0, pos_dot);
            else if (pos_slash < pos_dot)
              _requestname = input.substr(pos_slash + 1, pos_dot - pos_slash - 1);

            if (_ofile.empty() && !_generateDependencies)
              _ofile = input.substr(0, pos_dot);

            _extname = input.substr(pos_dot + 1);
          }
          else
          {
            _requestname = input;
          }

          if (_requestname.empty())
          {
            std::cerr << "cannot derive component name from filename. Use -n" << std::endl;
            return -1;
          }
        }
      }

      if (_ofile.empty() && !_generateDependencies)
        _ofile = _requestname;

      if (_generateDependencies)
        return runDependencies();
      else
        return runGenerator();
    }

    int Ecppc::runGenerator()
    {
      // strip cpp-extension from outputfilename
      if (_ofile.size() == _ofile.rfind(".cpp") + 4)
        _ofile = _ofile.substr(0, _ofile.size() - 4);

      // create generator
      tnt::ecppc::Generator generator(_requestname);

      // initialize
      generator.enableLinenumbers(!_disableLinenumbers);
      Bodypart::enableLinenumbers(!_disableLinenumbers);

      if (!_mimetype.empty())
        generator.setMimetype(_mimetype);
      else if (!_extname.empty() && !_multibinary)
      {
        tnt::MimeDb db(_mimedb);
        std::string mimeType = db.getMimetype(_extname);
        if (mimeType.empty())
        {
          if (_extname != "ecpp")
            std::cerr << "warning: unknown mimetype" << std::endl;
        }
        else
          generator.setMimetype(mimeType);
      }

      generator.setCompress(_compress);
      generator.setLogCategory(_logCategory);

      std::string obase = _odir;
      if (!obase.empty())
        obase += '/';
      obase += _ofile;

      //
      // parse sourcefile
      //

      if (_multibinary)
      {
        tnt::MimeDb mimeDb;
        if (_mimetype.empty())
          mimeDb.read(_mimedb);

        for (inputfiles_type::const_iterator it = _inputfiles.begin(); it != _inputfiles.end(); ++it)
        {
          std::string key = it->first;
          std::string ifile = it->second;

          struct stat st;
          log_debug("check for input file " << ifile);
          if (stat(ifile.c_str(), &st) != 0)
          {
            // search for input file in includes list
            for (includes_type::const_iterator incl = _includes.begin(); incl != _includes.end(); ++incl)
            {
              std::string inputfile = *incl + '/' + it->second;
              log_debug("check for input file " << inputfile);
              if (stat(inputfile.c_str(), &st) == 0)
              {
                ifile = inputfile;
                break;
              }
            }
          }

          log_debug("read input file " << ifile);
          std::ifstream in(ifile.c_str());
          if (!in)
            throw std::runtime_error("can't read " + ifile);

          std::ostringstream content;
          content << in.rdbuf();

          std::string mime;
          if (!_mimetype.empty())
            mime = _mimetype;
          else
          {
            mime = mimeDb.getMimetype(ifile);
            if (mime.empty())
            {
              mime = "application/x-data";
              std::cerr << "warning: no mimetype found for \"" << ifile << "\" using " << mime << std::endl;
            }
          }

          generator.addImage(key, content.str(), mime, st.st_ctime);
        }
      }
      else
      {
        std::ifstream in(_inputfile);
        if (!in)
          throw std::runtime_error(std::string("can't read ") + _inputfile);

        if (_binary)
        {
          std::ostringstream html;
          html << in.rdbuf();
          generator.onHtml(html.str());
          generator.setRawMode();

          struct stat st;
          if (stat(_inputfile, &st) == 0)
            generator.setLastModifiedTime(st.st_ctime);
        }
        else
        {
          bool success = runParser(in, generator, true);
          if (!success)
            return 1;
        }
      }

      //
      // generate code
      //
      if (_verbose)
        std::cout << "generate " << obase << ".cpp" << std::endl;
      std::ofstream sout((obase + ".cpp").c_str());
      generator.getCpp(sout, _ofile + ".cpp");
      sout.close();

      if (!sout)
        throw std::runtime_error("error writing file \"" + _ofile + ".cpp\"");

      return 0;
    }

    int Ecppc::runDependencies()
    {
      log_trace("runDependencies");

      tnt::ecppc::Dependencygenerator generator(_requestname, _inputfile);

      std::ifstream in(_inputfile);
      if (!in)
        throw std::runtime_error(std::string("can't read ") + _inputfile);

      if (!_binary)
        runParser(in, generator, false);

      if (_ofile.empty())
        generator.getDependencies(std::cout);
      else
      {
        std::ofstream out(_ofile.c_str());
        generator.getDependencies(out);
      }

      return 0;
    }

    bool Ecppc::runParser(std::istream& in, tnt::ecpp::ParseHandler& handler, bool continueOnError)
    {
      // create parser
      tnt::ecpp::Parser parser(handler, _inputfile);

      // initialize parser
      for (includes_type::const_iterator it = _includes.begin(); it != _includes.end(); ++it)
        parser.addInclude(*it);

      // call parser
      bool success = true;
      while (in)
      {
        try
        {
          parser.parse(in);
        }
        catch (const std::exception& e)
        {
          if (!continueOnError)
            throw;

          success = false;
          std::cerr << e.what() << std::endl;
        }
      }

      return success;
    }

    Usage::Usage(const char* progname)
    {
      std::ostringstream o;
      o << PACKAGE_STRING "\n\n"
           "ecppc-compiler\n\n"
           "usage: " << progname << " [options] ecpp-source\n\n"
           "  -o filename       outputfile\n"
           "  -n name           componentname\n"
           "  -I dir            include-directory\n"
           "  -m type           mimetype\n"
           "  --mimetypes file  read mimetypes from file (default /etc/mime.types)\n"
           "  -b                binary\n"
           "  -bb               generate multibinary component\n"
           "  -i filename       read filenames for multibinary component from specified file\n"
           "  -z                compress constant data\n"
           "  -v                verbose\n"
           "  -M                generate dependency for Makefile\n"
           "  -p                keep path when generating component name from filename\n"
           "  -l log-category   set log category (default: component.compname)\n"
           "  -L                disable generation of #line-directives\n"
           "\n"
           "  -h, --help        display this information\n"
           "  -V, --version     display program version\n";
      _msg = o.str();
    }
  }
}

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    cxxtools::Arg<bool> version(argc, argv, 'V');
    cxxtools::Arg<bool> versionLong(argc, argv, "--version");
    if (version || versionLong)
    {
      std::cout << PACKAGE_STRING << std::endl;
      return 0;
    }

    log_init();
    tnt::ecppc::Ecppc app(argc, argv);
    return app.run();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
