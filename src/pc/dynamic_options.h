#ifndef DYNAMIC_OPTIONS_H
#define DYNAMIC_OPTIONS_H

#include "types.h"
#include <stdio.h>
struct Option;

void dynos_init();
void dynos_add_binds(void (*func)(u32, u32 *));
void dynos_load_bind(const char *name, const char **values);
void dynos_save_binds(FILE *f);

u32 dynos_get_value(const char *id, const char *name);
void dynos_set_value(const char *id, const char *name, u32 value);

// Warning: This is C++ code, use this macro inside a .cpp file
#define DYNOS_DEFINE_ACTION(func) \
class DynosAction_##func { \
public: \
	inline DynosAction_##func() { \
		dynos_add_button_action(#func, func); \
	} \
private: \
	static DynosAction_##func sDynosAction_##func; \
}; \
DynosAction_##func DynosAction_##func::sDynosAction_##func;

#ifdef __cplusplus
extern "C" {
#endif
void dynos_add_button_action(const char *funcName, void (*funcPtr)(struct Option *, s32));
#ifdef __cplusplus
}
#endif

#endif // DYNAMIC_OPTIONS_H