////////////////////////////////////////////////////////////////////////
// tntconfigtest.cpp
//

#include <tnt/tntconfig.h>

int main(int argc, char* argv[])
{
  using namespace std;
  try
  {
    const char* fname = argc > 1 ? argv[1] : "tntnet.conf";
    tntconfig config(fname);

    const tntconfig::config_values_type& values = config.getConfigValues();
    tntconfig::config_values_type::const_iterator vi;
    for (vi = values.begin(); vi != values.end(); ++vi)
    {
      const tntconfig::config_entry_type& v = *vi;
      cout << "key=" << v.key << endl;
      for (tntconfig::config_value_type::const_iterator i
        = v.values.begin(); i != v.values.end(); ++i)
        cout << " value=" << *i << endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
