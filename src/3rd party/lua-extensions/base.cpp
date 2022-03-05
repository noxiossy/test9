#include "lua.hpp"

extern "C"{
    #include "lfs.h"
    #include "lmarshal.h"
}

//#include "Libs.h"
#include "script_additional_libs.h"

extern "C" __declspec(dllexport) int luaopen_lua_extensions(lua_State *L){
    //luaopen_debug(L);

    open_additional_libs(L);

    luaopen_lfs(L);
    //open_string(L);
    //open_math(L);
    //open_table(L);
    luaopen_marshal(L);
    //open_kb(L);
    //open_log(L); 

    return 0;
}
