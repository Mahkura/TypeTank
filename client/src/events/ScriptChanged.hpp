#pragma once
#include <memory>
#include <string>

namespace tt
{

struct ScriptChanged
{
    //! non-null
    std::shared_ptr<std::string> script_content;
};
    
} // namespace tt
