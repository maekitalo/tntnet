#include <tnt/query_params.h>
#include <tnt/httprequest.h>
#include <cxxtools/regex.h>

namespace tnt
{

namespace qhelper
{

tnt::Part QArg<tnt::Part>::get(const HttpRequest& r, const QueryParams& q, const std::string& name)
{
    const auto& mp = r.getMultipart();

    for (const auto& m: mp)
    {
        if (m.getName() == name)
            return m;
    }

    return tnt::Part();
}

std::vector<tnt::Part> QArg<tnt::Part>::getvector(const HttpRequest& r, const QueryParams& q, const std::string& name)
{
    std::vector<tnt::Part> result;

    const auto& mp = r.getMultipart();

    const std::string vname = name + "[]";

    static const cxxtools::Regex re('^' + name + "[[][0-9]*[]]$");

    for (const auto& m: mp)
    {
        if (m.getName() == name
            || re.match(m.getName()))
            result.emplace_back(m);
    }

    return result;
}

}
}
