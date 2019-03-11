#include<SDE/Types.hpp>

namespace NWChemEx {
  template<class... Args>
  SDE::type::result_map execute(std::string& driver, std::string& method, Args... args); 
  
}
