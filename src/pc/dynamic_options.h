#ifndef DYNAMIC_OPTIONS_H
#define DYNAMIC_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
int  dynos_get_value(const char *name);
void dynos_set_value(const char *name, int value);
void dynos_add_action(const char *funcName, bool (*funcPtr)(const char *), bool overwrite);

#ifdef __cplusplus
}
#endif

// Warning: This is C++ code, use this macro inside a .cpp file
// The action signature is "bool (*) (const char *)"
// The input is the button name (not label)
// The output is the result of the action
#define DYNOS_DEFINE_ACTION(func) \
extern "C" { extern bool func(const char *); } \
class DynosAction_##func { \
public: \
	inline DynosAction_##func() { \
		dynos_add_action(#func, func, false); \
	} \
private: \
	static DynosAction_##func sDynosAction_##func; \
}; \
DynosAction_##func DynosAction_##func::sDynosAction_##func;

#endif // DYNAMIC_OPTIONS_H