#ifndef __ALM__EXCEPTIONS__
#define __ALM__EXCEPTIONS__

#include <exception>

namespace alm
{

struct create_socket_exception   : std::exception {};
struct bind_socket_exception     : std::exception {};
struct listen_socket_exception   : std::exception {};
struct accept_socket_exception   : std::exception {};
struct poll_socket_exception   : std::exception {};
struct socket_closed_exception   : std::exception {};
struct socket_error_exception    : std::exception {};
struct read_header_exception  : std::exception {};
struct message_allocation_exception  : std::exception {};

struct address_family_exception     : std::exception {};
struct ip_address_exception         : std::exception {};
struct connect_failed_exception     : std::exception {};

struct file_not_found_exception     : std::exception {};

struct out_of_bounds_exception : std::exception { };

}

#endif
