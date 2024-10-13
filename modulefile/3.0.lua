-- openCL Module

-- Set the version and installation prefix
version = "3.0"
prefix = "/Users/zakaria/Softwares/tools/openCL"

-- Provide a description of the GCC compiler
whatis("OpenCL Library")

-- Provide help text describing the GCC compiler
help([[
This module provides the OpenCL Lib.
--
To use the OpenCL library, include the following in your C code:
  module load openCL
]])

-- Add GCC binary, library, and man directory paths to the environment
prepend_path("PATH",            prefix .. "/bin")
--prepend_path("LD_LIBRARY_PATH", prefix .. "/lib")
prepend_path("DYLD_LIBRARY_PATH", prefix .. "/lib")  -- For macOS
prepend_path("MANPATH",         prefix .. "/share")
prepend_path("CPLUS_INCLUDE_PATH", pathJoin(prefix, "include"))
prepend_path("C_INCLUDE_PATH", pathJoin(prefix, "include"))

--

-- Utils
local function fancy_message(msg, msg_color, name, name_color)
    local reset = "\27[0m"
    LmodMessage(msg_color .. msg .. " " .. name_color .. name .. reset)
  end
  
  -- Color codes
  local red = "\27[31m"
  local green = "\27[32m"
  local yellow = "\27[33m"
  local blue = "\27[34m"
  local magenta = "\27[35m"
  local cyan = "\27[36m"
  
  -- Print a final message indicating Deal.II has been loaded
  if mode() == "load"  then
    fancy_message("OpenCL version " .. version .. " has been loaded!", yellow, "", red)
  end

