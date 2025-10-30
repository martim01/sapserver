#ifndef PML_SAPSERVER_NAMEDTYPES_H
#define PML_SAPSERVER_NAMEDTYPES_H

#include <string>

#include "namedtype.h"


using IpAddress = NamedType<std::string, struct IpAddressParameter>;
using IpInterface = NamedType<std::string, struct IpInterfaceParameter>;

#endif