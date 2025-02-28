
#include "platform.h"

#if _ON_WINDOWS_
#include "win_platform.cpp"
#else
#error The application does not support your Platform yet!
#endif
