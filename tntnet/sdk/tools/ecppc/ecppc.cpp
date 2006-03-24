/* ecppc.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#include "tnt/ecppc/ecppc.h"
#include "tnt/ecppc/generator.h"
#include "tnt/ecppc/dependencygenerator.h"
#include <tnt/mimedb.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include <cxxtools/loginit.h>

namespace tnt
{
  namespace ecppc
  {
    Ecppc::Ecppc(int& argc, char* argv[])
      : requestname(cxxtools::Arg<std::string>(argc, argv, 'n')),
        ns(cxxtools::Arg<std::string>(argc, argv, 'N')),
        ofile(cxxtools::Arg<std::string>(argc, argv, 'o')),
        odir(cxxtools::Arg<std::string>(argc, argv, 'O')),
        mimetype(argc, argv, 'm'),
        mimedb(argc, argv, "--mimetypes", "/etc/mime.types"),
        binary(argc, argv, 'b'),
        multibinary(argc, argv, 'b'),
        singleton(argc, argv, 's'),
        componentclass(argc, argv, 'C'),
        baseclass(argc, argv, 'B'),
        compress(argc, argv, 'z'),
        verbose(argc, argv, 'v'),
        debug(argc, argv, 'd'),
        trace(argc, argv, 't'),
        generateDependencies(argc, argv, 'M'),
        generateHeader(argc, argv, 'h')
    {
      if (argc < 2 || argv[1][0] == '-')
        throw Usage(argv[0]);
      inputfile = argv[1];
      std::copy(argv + 1, argv + argc, std::inserter(inputfiles, inputfiles.end()));

      if (debug)
        log_init_debug();
      else
        log_init();
    }

    int Ecppc::run()
    {
      // requestname aus Inputdatei
      if (requestname.empty())
      {
        if (multibinary)
        {
          std::cerr << "warning: no requestname passed (with -n) - using \"images\"" << std::endl;
          requestname = "images";
        }
        else
        {
          std::string input = inputfile;
          std::string::size_type pos_dot = input.find_last_of(".");
          std::string::size_type pos_slash = input.find_last_of("\\/");
          if (pos_dot != std::string::npos)
          {
            if (pos_slash == std::string::npos)
            {
              requestname = input.substr(0, pos_dot);
            }
            else if (pos_slash < pos_dot)
            {
              requestname = input.substr(pos_slash + 1, pos_dot - pos_slash - 1);

              if (ns.empty())
              {
                std::string::size_type pos_slash2 = input.find_last_of("\\/", pos_slash - 1);
                if (pos_slash2 == std::string::npos)
                  ns = input.substr(0, pos_slash);
                else
                  ns = input.substr(pos_slash2 + 1, pos_slash - pos_slash2 - 1);
              }
            }

            extname = input.substr(pos_dot + 1);
          }
          else
          {
            requestname = input;
          }

          if (requestname.empty())
          {
            std::cerr << "cannot derive classname from filename. Use -n" << std::endl;
            return -1;
          }
        }
      }
      else if (ns.empty())
      {
        std::string::size_type p = requestname.find("::");
        if (p != std::string::npos)
        {
          ns = requestname.substr(0, p);
          requestname.erase(0, p + 2);
        }
      }

      if (generateDependencies)
        return runDepencencies();
      else
        return runGenerator();
    }

    int Ecppc::runGenerator()
    {
      if (ofile.empty())
      {
        if (ns.empty())
          ofile = requestname;
        else
          ofile = ns + '/' + requestname;
      }

      // strip cpp-extension from outputfilename
      if (ofile.size() == ofile.rfind(".cpp") + 4)
        ofile = ofile.substr(0, ofile.size() - 4);

      // create generator
      tnt::ecppc::Generator generator(requestname, ns);

      // initialize
      generator.setDebug(trace);

      if (mimetype.isSet())
        generator.setMimetype(mimetype);
      else if (!extname.empty() && !multibinary)
      {
        tnt::MimeDb db(mimedb);
        tnt::MimeDb::const_iterator it = db.find(extname);
        if (it != db.end())
          generator.setMimetype(it->second);
      }

      generator.setCompress(compress);

      if (singleton.isSet())
        generator.setSingleton(singleton);
      if (componentclass.isSet())
        generator.setComponentclass(componentclass);
      if (baseclass.isSet())
        generator.setBaseclass(baseclass);

      std::string obase = odir;
      if (!obase.empty())
        obase += '/';
      obase += ofile;
      
      //
      // parse sourcefile
      //

      if (multibinary)
      {
        tnt::MimeDb mimeDb;
        if (!mimetype.isSet())
          mimeDb.read(mimedb);

        for (inputfiles_type::const_iterator it = inputfiles.begin();
             it != inputfiles.end(); ++it)
        {
          std::string inputfile = *it;

          struct stat st;
          if (stat(inputfile.c_str(), &st) != 0)
            throw std::runtime_error("can't stat " + inputfile);

          std::ifstream in(inputfile.c_str());
          if (!in)
            throw std::runtime_error("can't read " + inputfile);

          std::ostringstream content;
          content << in.rdbuf();

          std::string mime;
          if (mimetype.isSet())
            mime = mimetype;
          else
            mime = mimeDb.getMimetype(inputfile);

          // strip path
          std::string::size_type p;
          if ((p = inputfile.find_last_of('/')) != std::string::npos)
            inputfile.erase(0, p + 1);

          generator.addImage(inputfile, content.str(), mime, st.st_ctime);
        }
      }
      else
      {
        std::ifstream in(inputfile);
        if (binary)
        {
          std::ostringstream html;
          html << in.rdbuf();
          generator.onHtml(html.str());
          generator.setRawMode();

          struct stat st;
          if (stat(inputfile, &st) == 0)
            generator.setLastModifiedTime(st.st_ctime);
        }
        else
          runParser(in, generator);
      }

      //
      // generate Code
      //
      if (generateHeader)
      {
        if (verbose)
          std::cout << "generate " << obase << ".h" << std::endl;
        std::ofstream hout((obase + ".h").c_str());
        generator.getHeader(hout, ofile + ".h");
        hout.close();

        if (verbose)
          std::cout << "generate " << obase << ".cpp" << std::endl;
        std::ofstream sout((obase + ".cpp").c_str());
        generator.getCpp(sout, ofile + ".cpp");
        sout.close();
      }
      else
      {
        if (verbose)
          std::cout << "generate " << obase << ".cpp" << std::endl;
        std::ofstream sout((obase + ".cpp").c_str());
        generator.getCppWoHeader(sout, ofile + ".cpp");
        sout.close();
      }

      return 0;
    }

    int Ecppc::runDepencencies()
    {
      tnt::ecppc::Dependencygenerator generator(requestname, inputfile);
      std::ifstream in(inputfile);
      if (!binary)
        runParser(in, generator);

      if (ofile.empty())
        generator.getDependencies(std::cout, generateHeader);
      else
      {
        std::ofstream out(ofile.c_str());
        generator.getDependencies(out, generateHeader);
      }

      return 0;
    }

    void Ecppc::runParser(std::istream& in, tnt::ecpp::ParseHandler& handler)
    {
      // create parser
      tnt::ecpp::Parser parser(handler, inputfile);

      // call parser
      parser.parse(in);
    }

    Usage::Usage(const char* progname)
    {
      std::ostringstream o;
      o << PACKAGE_STRING "\n\n"
           "ecppc-compiler\n\n"
           "usage: " << progname << " [options] ecpp-source\n\n"
           "  -o filename      outputfile\n"
           "  -n name          classname\n"
           "  -m type          Mimetype\n"
           "  -s               generate singleton\n"
           "  -s-              generate no singleton\n"
           "  -b               binary\n"
           "  -bb              generate multibinary component\n"
           "  -B class         additional base-class\n"
           "  -C class         alternative base-class (derived from tnt::ecppComponent)\n"
           "  -z               compress constant data\n"
           "  -v               verbose\n"
           "  -t               generate traces\n"
           "  -M               generate dependency for Makefile\n"
           "  -h               generate separate header-file\n";
      msg = o.str();
    }
  }
}

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    tnt::ecppc::Ecppc app(argc, argv);
    return app.run();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
