#pragma once

#pragma push_macro("malloc")
#pragma push_macro("free")
#pragma push_macro("realloc")

#undef malloc
#undef free
#undef realloc

#pragma warning(push)
#pragma warning(disable: 4996)

#include <rapidjson/document.h>

#pragma warning(pop)

#pragma pop_macro("realloc")
#pragma pop_macro("free")
#pragma pop_macro("malloc")