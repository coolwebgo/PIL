#include "HostEntry.h"
#include "base/Debug/Exception.h"
#include "base/Debug/Assert.h"
#include <algorithm>


namespace pi{


HostEntry::HostEntry()
{
}


HostEntry::HostEntry(struct hostent* entry)
{
    pi_check_ptr (entry);
    if(!entry){
        std::cerr<<"No entry found.\n";
    }

    _name = entry->h_name;
    char** alias = entry->h_aliases;
    if (alias)
    {
        while (*alias)
        {
            _aliases.push_back(std::string(*alias));
            ++alias;
        }
    }
    char** address = entry->h_addr_list;
    if (address)
    {
        while (*address)
        {
            _addresses.push_back(IPAddress(*address, entry->h_length));
            ++address;
        }
    }
}


#if defined(PIL_HAVE_IPv6) || defined(PIL_HAVE_ADDRINFO)


HostEntry::HostEntry(struct addrinfo* ainfo)
{
    pi_check_ptr (ainfo);

    for (struct addrinfo* ai = ainfo; ai; ai = ai->ai_next)
    {
        if (ai->ai_canonname)
        {
            _name.assign(ai->ai_canonname);
        }
        if (ai->ai_addrlen && ai->ai_addr)
        {
            switch (ai->ai_addr->sa_family)
            {
            case AF_INET:
                _addresses.push_back(IPAddress(&reinterpret_cast<struct sockaddr_in*>(ai->ai_addr)->sin_addr, sizeof(in_addr)));
                break;
#if defined(PIL_HAVE_IPv6)
            case AF_INET6:
                _addresses.push_back(IPAddress(&reinterpret_cast<struct sockaddr_in6*>(ai->ai_addr)->sin6_addr, sizeof(in6_addr), reinterpret_cast<struct sockaddr_in6*>(ai->ai_addr)->sin6_scope_id));
                break;
#endif
            }
        }
    }
}


#endif // PIL_HAVE_IPv6


#if defined(PIL_VXWORKS)


HostEntry::HostEntry(const std::string& name, const IPAddress& addr):
    _name(name)
{
    _addresses.push_back(addr);
}


#endif // PIL_VXWORKS


HostEntry::HostEntry(const HostEntry& entry):
    _name(entry._name),
    _aliases(entry._aliases),
    _addresses(entry._addresses)
{
}


HostEntry& HostEntry::operator = (const HostEntry& entry)
{
    if (&entry != this)
    {
        _name      = entry._name;
        _aliases   = entry._aliases;
        _addresses = entry._addresses;
    }
    return *this;
}


void HostEntry::swap(HostEntry& hostEntry)
{
    std::swap(_name, hostEntry._name);
    std::swap(_aliases, hostEntry._aliases);
    std::swap(_addresses, hostEntry._addresses);
}


HostEntry::~HostEntry()
{
}


} // namespace pi
