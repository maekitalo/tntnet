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
  static void removePrefix(const std::string& prefix, std::string& fname)
  {
    if (fname.compare(0, prefix.size(), prefix) == 0)
      fname.erase(0, prefix.size());
  }

  namespace ecppc
  {
    Ecppc::Ecppc(int& argc, char* argv[])
      : _componentname(cxxtools::Arg<std::string>(argc, argv, 'n')),
        _inputfile(0),
        _ofile(cxxtools::Arg<std::string>(argc, argv, 'o')),
        _odir(cxxtools::Arg<std::string>(argc, argv, 'O')),
        _mimetype(cxxtools::Arg<std::string>(argc, argv, 'm')),
        _mimedb(cxxtools::Arg<std::string>(argc, argv, "--mimetypes", "/etc/mime.types")),
        _logCategory(cxxtools::Arg<std::string>(argc, argv, 'l'))
    {
      while (true)
      {
        cxxtools::Arg<std::string> include(argc, argv, 'I');
        if (!include.isSet())
          break;
        log_debug("include: " << include.getValue());
        _includes.push_back(include);
      }

      // boolean arguments have to be read after the includes
      // because else parts of the include directory string
      // might be interpreted as flags and taken out, often
      // resulting in an missing include error message.
      cxxtools::Arg<bool> version(argc, argv, 'V');
      cxxtools::Arg<bool> versionLong(argc, argv, "--version");
      if (version || versionLong)
      {
          std::cout << PACKAGE_STRING << std::endl;
          exit(0);
      }

      cxxtools::Arg<bool> help(argc, argv, 'h');
      cxxtools::Arg<bool> helpLong(argc, argv, "--help");

      if(help || helpLong)
      {
          std::cout << PACKAGE_STRING "\n\n"
              "ecppc-compiler\n\n"
              "usage: " << argv[0] << " [options] ecpp-source\n\n"
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
              "  -C, --cmake       generate dependency for cmake\n"
              "  -p                keep path when generating component name from filename\n"
              "  -l log-category   set log category (default: component.compname)\n"
              "  -L                disable generation of #line-directives\n"
              "\n"
              "  -h, --help        display this information\n"
              "  -V, --version     display program version\n" << std::endl;

          exit(0);
      }

      _binary = cxxtools::Arg<bool>(argc, argv, 'b');
      _multibinary = cxxtools::Arg<bool>(argc, argv, 'b');
      _keepPath = cxxtools::Arg<bool>(argc, argv, 'p');
      _compress = cxxtools::Arg<bool>(argc, argv, 'z');
      _verbose = cxxtools::Arg<bool>(argc, argv, 'v');
      _generateDependencies = cxxtools::Arg<bool>(argc, argv, 'M');
      _generateCMakeDependencies = cxxtools::Arg<bool>(argc, argv, 'C') || cxxtools::Arg<bool>(argc, argv, "--cmake");
      _disableLinenumbers = cxxtools::Arg<bool>(argc, argv, 'L');

      if (_multibinary)
      {
        cxxtools::Arg<const char*> ifiles(argc, argv, 'i');
        cxxtools::Arg<std::string> removeFromPath(argc, argv, 'r');

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

            if (removeFromPath.isSet())
              removePrefix(removeFromPath, key);

            _inputFiles.insert(inputfiles_type::value_type(key, ifile));
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

          if (removeFromPath.isSet())
            removePrefix(removeFromPath, key);


          _inputFiles.insert(inputfiles_type::value_type(key, ifile));
        }
      }
      else
      {
        if (argc < 2 || argv[1][0] == '-')
          throw std::runtime_error(
            "error: exactly one input file has to be specified\n"
            "usage: " + std::string(argv[0]) + " [options] ecpp-source\n"
            "more info with -h / --help"
          );

        _inputfile = argv[1];
      }
    }

    int Ecppc::run()
    {
      // componentname from inputfilename
      if (_componentname.empty())
      {
        if (_multibinary)
        {
          std::cerr << "warning: no componentname passed (with -n) - using \"images\"" << std::endl;
          _componentname = "images";
        }
        else
        {
          _componentname = _inputfile;

          std::string::size_type pos_slash = _componentname.find_last_of("\\/");
          std::string::size_type pos_dot = _componentname.find_last_of(".");

          // read file name extension
          if (pos_dot != std::string::npos && (pos_slash == std::string::npos || pos_slash < pos_dot))
            _extname = _componentname.substr(pos_dot + 1);
          log_debug("componentname=" << _componentname << " extname=" << _extname);

          // remove path unless disabled
          if (!_keepPath &&  pos_slash != std::string::npos)
              _componentname.erase(0, pos_slash + 1);
          log_debug("componentname(1)=" << _componentname);

          // remove extension .ecpp
          bool ecpp = _componentname.size() >= 4 && _componentname.compare(_componentname.size() - 5, 5, ".ecpp") == 0;
          if (ecpp)
            _componentname.erase(_componentname.size() - 5);
          log_debug("componentname(2)=" << _componentname << " ecpp=" << ecpp);

          if (_ofile.empty() && !_generateDependencies && !_generateCMakeDependencies)
          {
            _ofile = _inputfile;
            if (ecpp)
              _ofile.erase(_ofile.size() - 5);
            _ofile += ".cpp";
          }
          log_debug("ofile=" << _ofile);

          if (_componentname.empty())
          {
            std::cerr << "cannot derive component name from filename. Use -n" << std::endl;
            return -1;
          }
        }
      }

      if (_ofile.empty() && !_generateDependencies && !_generateCMakeDependencies)
        _ofile = _componentname;

      if (_generateDependencies || _generateCMakeDependencies)
        return runDependencies(_generateCMakeDependencies);
      else
        return runGenerator();
    }

    int Ecppc::runGenerator()
    {
      // strip cpp-extension from outputfilename
      if (_ofile.size() == _ofile.rfind(".cpp") + 4)
        _ofile = _ofile.substr(0, _ofile.size() - 4);

      // create generator
      tnt::ecppc::Generator generator(_componentname);

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

        for (inputfiles_type::const_iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it)
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

    int Ecppc::runDependencies(bool cmake)
    {
      log_trace("runDependencies");

      tnt::ecppc::Dependencygenerator generator(_inputfile);

      std::ifstream in(_inputfile);
      if (!in)
        throw std::runtime_error(std::string("can't read ") + _inputfile);

      if (!_binary)
        runParser(in, generator, false);

      if (_ofile.empty())
      {
        if (cmake)
            generator.getCMakeDependencies(std::cout);
        else
            generator.getDependencies(std::cout);
      }
      else
      {
        std::ofstream out(_ofile.c_str());
        if (cmake)
            generator.getCMakeDependencies(out);
        else
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
  }
}

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
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
