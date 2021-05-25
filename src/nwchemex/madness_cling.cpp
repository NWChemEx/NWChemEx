#include "worldrmi.h"
#include "nwchemex/madness_cling.hpp"

bool madness::workaround_RMI_get_this_thread_is_server() {
      return RMI::get_this_thread_is_server();
}
