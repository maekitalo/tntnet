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
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"

namespace tnt
{
  namespace ecppc
  {
    ecppc::ecppc(int& argc, char* argv[])
      : requestname(cxxtools::arg<std::string>(argc, argv, 'n')),
        ns(cxxtools::arg<std::string>(argc, argv, 'N')),
        ofile(cxxtools::arg<std::string>(argc, argv, 'o')),
        odir(cxxtools::arg<std::string>(argc, argv, 'O')),
        mimetype(argc, argv, 'm'),
        binary(argc, argv, 'b'),
        singleton(argc, argv, 's'),
        componentclass(argc, argv, 'C'),
        baseclass(argc, argv, 'B'),
        htmlcompress(argc, argv, "--compress-html"),
        csscompress(argc, argv, "--compress-css"),
        jscompress(argc, argv, "--compress-js"),
        compress(argc, argv, 'z'),
        externData(argc, argv, 'x'),
        verbose(argc, argv, 'v'),
        debug(argc, argv, 'd'),
        splitBar(argc, argv, 'S'),
        splitChars(argc, argv, "--split-chars"),
        generateDependencies(argc, argv, 'M'),
        generateHeader(argc, argv, 'h')
    {
      if (argc != 2 || argv[1][0] == '-')
        throw usage(argv[0]);
      inputfile = argv[1];
    }

    int ecppc::run()
    {
      // requestname aus Inputdatei
      if (requestname.empty())
      {
        std::string input = inputfile;
        std::string::size_type pos_dot = input.find_last_of(".");
        std::string::size_type pos_slash = input.find_last_of("\\/");
        if (pos_dot != std::string::npos)
        {
          if (pos_slash == std::string::npos)
            requestname = input.substr(0, pos_dot);
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

    int ecppc::runGenerator()
    {
      if (ofile.empty())
      {
        if (ns.empty())
          ofile = requestname;
        else
          ofile = ns + '/' + requestname;
      }

      // Output auf ".h" and ".cpp" pruefen
      if (ofile.size() == ofile.rfind(".h") + 2)
        ofile = ofile.substr(0, ofile.size() - 2);
      else if (ofile.size() == ofile.rfind(".cpp") + 4)
        ofile = ofile.substr(0, ofile.size() - 4);

      // create generator
      tnt::ecppc::generator generator(requestname, ns);

      // initialize
      generator.setDebug(debug);

      if (mimetype.isSet())
        generator.setMimetype(mimetype);

      if (htmlcompress)
        generator.setHtmlCompress();
      else if (csscompress)
        generator.setCssCompress();
      else if (jscompress)
        generator.setJsCompress();

      generator.setCompress(compress);
      generator.setExternData(externData);

      //
      // parse sourcefile
      //

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

      // wenn Kommandozeilenparameter -s explizit angegeben ist, übergebe diesen an den
      // Generator
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
      // generiere Code
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

    int ecppc::runDepencencies()
    {
      tnt::ecppc::dependencygenerator generator(requestname, inputfile);
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

    void ecppc::runParser(std::istream& in, tnt::ecpp::parseHandler& handler)
    {
      // create parser
      tnt::ecpp::parser parser(handler);

      parser.setSplitBar(splitBar);
      if (splitChars.isSet())
      {
        if (splitChars.getValue()[0] == '\0'
         || splitChars.getValue()[1] == '\0'
         || splitChars.getValue()[2] != '\0')
          throw std::runtime_error("--split-chars needs exactly 2 characters");

        parser.setSplitChars(splitChars.getValue()[0],
                             splitChars.getValue()[1]);
      }

      parser.parse(in);
    }

    usage::usage(const char* progname)
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
           "  -B class         additional base-class\n"
           "  -C class         alternative base-class (derived from tnt::ecppComponent)\n"
           "  --compress-html  remove some space in HTML-code\n"
           "  --compress-css   remove some space in CSS-code\n"
           "  --compress-js    remove some space in JavaScript-code\n"
           "  -z               compress constant data\n"
           "  -x               look for constants in language-specific library\n"
           "  -v               verbose\n"
           "  -d               debug\n"
           "  -S               split chunks at '{' und '}'\n"
           "  --split-chars zz select alternative split-chars\n"
           "  -M               generate dependency form Makefile\n"
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
    tnt::ecppc::ecppc app(argc, argv);
    return app.run();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
