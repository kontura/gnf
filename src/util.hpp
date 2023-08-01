#ifndef GNF_UTIL_HEADER
#define GNF_UTIL_HEADER

#include <libdnf5/base/base.hpp>
#include <libdnf5/rpm/package_query.hpp>


//this is just until ifilter_provides is fixed and performant
bool reldeplist_contains(const libdnf5::rpm::ReldepList list, libdnf5::rpm::Reldep in_reldep) {
    for (const auto reldep: list) {
        if (reldep == in_reldep) {

            return true;
        }
    }

    return false;
}



#endif // GNF_UTIL_HEADER
