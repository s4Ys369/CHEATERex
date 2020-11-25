#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include "controller/controller_keyboard.h"
#include "controller/controller_sdl.h"
#include "game/object_list_processor.h"
#include "game/spawn_object.h"
#include "game/sound_init.h"
#include "engine/level_script.h"
#include "fs/fs.h"
#include "levels.h"
#include "configfile.h"
#include "gfx_dimensions.h"

#ifdef size_t
#undef size_t
#endif
#ifdef IS_64_BIT
#define size_t unsigned long long
#else
#define size_t unsigned int
#endif

//
// C String to SM64 String conversion
//

static const unsigned char sSm64CharTable[][2] = {
    { '0', 0 },   { '1', 1 },   { '2', 2 },   { '3', 3 },   { '4', 4 },   { '5', 5 },   { '6', 6 },
    { '7', 7 },   { '8', 8 },   { '9', 9 },   { 'A', 10 },  { 'B', 11 },  { 'C', 12 },  { 'D', 13 },
    { 'E', 14 },  { 'F', 15 },  { 'G', 16 },  { 'H', 17 },  { 'I', 18 },  { 'J', 19 },  { 'K', 20 },
    { 'L', 21 },  { 'M', 22 },  { 'N', 23 },  { 'O', 24 },  { 'P', 25 },  { 'Q', 26 },  { 'R', 27 },
    { 'S', 28 },  { 'T', 29 },  { 'U', 30 },  { 'V', 31 },  { 'W', 32 },  { 'X', 33 },  { 'Y', 34 },
    { 'Z', 35 },  { 'a', 36 },  { 'b', 37 },  { 'c', 38 },  { 'd', 39 },  { 'e', 40 },  { 'f', 41 },
    { 'g', 42 },  { 'h', 43 },  { 'i', 44 },  { 'j', 45 },  { 'k', 46 },  { 'l', 47 },  { 'm', 48 },
    { 'n', 49 },  { 'o', 50 },  { 'p', 51 },  { 'q', 52 },  { 'r', 53 },  { 's', 54 },  { 't', 55 },
    { 'u', 56 },  { 'v', 57 },  { 'w', 58 },  { 'x', 59 },  { 'y', 60 },  { 'z', 61 },  { '\'', 62 },
    { '.', 63 },  { ',', 111 }, { ' ', 158 }, { '-', 159 }, { '(', 225 }, { ')', 227 }, { '&', 229 },
    { ':', 230 }, { '!', 242 }, { '%', 243 }, { '?', 244 }, { '"', 246 }, { '~', 247 }, { '_', 253 }
};
static const unsigned int sSm64CharCount = sizeof(sSm64CharTable) / sizeof(sSm64CharTable[0]);

static unsigned char sm64_char(char c) {
    for (unsigned int i = 0; i != sSm64CharCount; ++i) {
        if (sSm64CharTable[i][0] == (unsigned char) c) {
            return sSm64CharTable[i][1];
        }
    }
    return 0xFF;
}

static const unsigned char *sm64_string_convert(const char *str, unsigned char *str64, bool caps) {
    unsigned int i = 0;
    for (; *str != 0; str++) {
        unsigned char c = sm64_char(*str);
        if (c != 0xFF) {
            if (caps && c >= 36 && c <= 61) {
                str64[i++] = c - 26;
            } else {
                str64[i++] = c;
            }
        }
    }
    str64[i] = 0xFF;
    return (const unsigned char *) str64;
}

static const char *sm64_string_invert(const unsigned char *str64, char *str) {
    unsigned int i = 0;
    for (; *str64 != 0xFF; str64++) {
        unsigned char c = *str64;
        if (c <= 9) {
            str[i++] = (c - 0) + '0';
        } else if (c <= 35) {
            str[i++] = (c - 10) + 'A';
        } else if (c <= 61) {
            str[i++] = (c - 36) + 'a';
        } else {
            str[i++] = '_';
        }
    }
    str[i] = 0;
    return (const char *) str;
}

static unsigned int strlen64(const unsigned char *str64) {
    unsigned int length = 0;
    while (*(str64++) != 0xFF) length++;
    return length;
}

static const char *string_to_dynamic(const char *str) {
    char *dstr = calloc(strlen(str) + 1, sizeof(char));
    memcpy(dstr, str, strlen(str) + 1);
    return dstr;
}

static const char *string_to_configname(const char *str) {
    if (str == NULL || strcmp(str, "NULL") == 0 || strcmp(str, "null") == 0) {
        return NULL;
    }
    char *dstr = calloc(strlen(str) + 1, sizeof(char));
    char *pstr = dstr;
    for (; *str != 0; str++, pstr++) {
        if ((*str >= '0' && *str <= '9') || (*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z')) {
            *pstr = *str;
        } else {
            *pstr = '_';
        }
    }
    *pstr = 0;
    return dstr;
}

static const unsigned char *string_to_label(const char *str) {
    unsigned char *str64 = calloc(strlen(str) + 1, sizeof(unsigned char));
    return sm64_string_convert(str, str64, 0);
}

static const unsigned char *string_to_label2(const char *str) {
    if (str == NULL) return NULL;
    unsigned char *str64 = calloc(strlen(str) + 1, sizeof(char));
    return sm64_string_convert(str, str64, 1);
}

static const char *label_to_name(const unsigned char *str64) {
    char *str = calloc(strlen64(str64) + 1, sizeof(char));
    return sm64_string_invert(str64, str);
}

static const unsigned char *label_decapitalize(const unsigned char *label) {
    unsigned char *nlabel = calloc(strlen64(label) + 1, sizeof(unsigned char));
    unsigned char *plabel = nlabel;
    bool prevWasSpace = true;
    for (; *label != 0xFF; ++label, ++plabel) {
        if (!prevWasSpace && *label >= 10 && *label <= 35) {
            *plabel = *label + 26;
        } else {
            *plabel = *label;
        }
        prevWasSpace = (*plabel >= 62);
    }
    *plabel = 0xFF;
    return nlabel;
}

static int string_to_int(const char *str) {
    int x = 0;
    if (strlen(str) > 2 && str[0] == '0' && str[1] == 'x') {
        sscanf(str + 2, "%X", &x);
    } else {
        sscanf(str, "%d", &x);
    }
    return x;
}

static const unsigned char *int_to_string(const char *fmt, int x) {
    static char str[16];
    static unsigned char str64[16];
    snprintf(str, 16, fmt, x);
    return sm64_string_convert(str, str64, 1);
}

//
// Dynamic Size Array
//

struct Array {
    void *buffer;
    size_t count;
    size_t itemSize;
};

static void array_add(struct Array *array, void *item) {

    // Alloc space
    void *newBuffer = calloc(array->count + 1, array->itemSize);
    if (array->buffer) {
        memcpy(newBuffer, array->buffer, array->count * array->itemSize);
        free(array->buffer);
    }
    array->buffer = newBuffer;

    // Insert item
    memcpy((void *)((size_t) array->buffer + array->itemSize * array->count), item, array->itemSize);
    array->count++;
}

// Doesn't check bounds
static void *array_at(struct Array *array, size_t index) {
    return (void *) ((size_t) array->buffer + array->itemSize * index);
}

#define array_get(array, i, type) (*((type *) array_at(array, i)))

//
// Action list
//

typedef bool (*DynosActionFunction)(const char *);
struct DynosAction {
    const char *str;
    DynosActionFunction action;
};

// "Construct On First Use" aka COFU
static struct Array *dynos_get_action_list() {
    static struct Array sDynosActions = { .buffer = NULL, .count = 0, .itemSize = sizeof(struct DynosAction) };
    return &sDynosActions;
}

static DynosActionFunction dynos_get_action(const char *funcName) {
    struct Array *dynosActions = dynos_get_action_list();
    for (size_t i = 0; i != dynosActions->count; ++i) {
        if (strcmp(array_get(dynosActions, i, struct DynosAction).str, funcName) == 0) {
            return array_get(dynosActions, i, struct DynosAction).action;
        }
    }
    return NULL;
}

void dynos_add_action(const char *funcName, bool (*funcPtr)(const char *), bool overwrite) {
    struct Array *dynosActions = dynos_get_action_list();
    for (size_t i = 0; i != dynosActions->count; ++i) {
        if (strcmp(array_get(dynosActions, i, struct DynosAction).str, funcName) == 0) {
            if (overwrite) {
                array_get(dynosActions, i, struct DynosAction).action = funcPtr;
            }
            return;
        }
    }
    struct DynosAction dynosAction = { string_to_dynamic(funcName), funcPtr };
    array_add(dynosActions, &dynosAction);
}

//
// Utils
//

static bool dynos_is_txt_file(const char *filename, unsigned int length) {
    return
        length >= 4 &&
        filename[length - 4] == '.' &&
        filename[length - 3] == 't' &&
        filename[length - 2] == 'x' &&
        filename[length - 1] == 't';
}

//
// Tokenizer
//

#define DYNOS_STR_TOKENS_MAX_LENGTH 32
#define DYNOS_STR_TOKENS_MAX_COUNT  128
#define DYNOS_LINE_MAX_LENGTH       (DYNOS_STR_TOKENS_MAX_LENGTH * DYNOS_STR_TOKENS_MAX_COUNT)

struct StrTokens {
    char tokens[DYNOS_STR_TOKENS_MAX_COUNT][DYNOS_STR_TOKENS_MAX_LENGTH];
    unsigned int count;
};

static struct StrTokens dynos_split_string(const char *str) {
    struct StrTokens strtk = { .count = 0 };
    bool treatSpacesAsChar = false;
    for (int l = 0;; str++) {
        char c = *str;
        if (c == 0 || (c == ' ' && !treatSpacesAsChar) || c == '\t' || c == '\r' || c == '\n' || c == '#') {
            if (l > 0) {
                strtk.tokens[strtk.count][l] = 0;
                strtk.count++;
                if (strtk.count == DYNOS_STR_TOKENS_MAX_COUNT) {
                    break;
                }
                l = 0;
            }
            if (c == 0 || c == '#') {
                break;
            }
        } else if (c == '\"') {
            treatSpacesAsChar = !treatSpacesAsChar;
        } else if (l < (DYNOS_STR_TOKENS_MAX_LENGTH - 1)) {
            strtk.tokens[strtk.count][l] = c;
            l++;
        }
    }
    return strtk;
}

//
// Types and data
//

enum DynosOptType {
    DOPT_TOGGLE,
    DOPT_CHOICE,
    DOPT_SCROLL,
    DOPT_BIND,
    DOPT_BUTTON,
    DOPT_SUBMENU,
};

struct DynosOption {
    const char *name;
    const char *configName; // Name used in sm64config.txt
    const unsigned char *label;
    const unsigned char *label2; // Full caps label, displayed with colored font
    struct DynosOption *prev;
    struct DynosOption *next;
    struct DynosOption *parent;

    enum DynosOptType type;
    union {

        // TOGGLE
        struct Toggle {
            bool *ptog;
        } toggle;

        // CHOICE
        struct Choice {
            const unsigned char **choices;
            int count;
            int *pindex;
        } choice;

        // SCROLL
        struct Scroll {
            int min;
            int max;
            int step;
            int *pvalue;
        } scroll;

        // BIND
        struct Bind {
            unsigned int mask;
            unsigned int *pbinds;
            bool dynos;
            int index;
        } bind;

        // BUTTON
        struct Button {
            const char *funcName;
        } button;
        
        // SUBMENU
        struct Submenu {
            struct DynosOption *child;
            bool empty;
        } submenu;
    };
};

static struct DynosOption *sPrevOpt = NULL;
static struct DynosOption *sDynosMenu = NULL;
static struct DynosOption *sOptionsMenu = NULL;
static const unsigned char *sDynosTextOptionsMenu;
static const unsigned char *sDynosTextDynosMenu;
static const unsigned char *sDynosTextDisabled;
static const unsigned char *sDynosTextEnabled;
static const unsigned char *sDynosTextNone;
static const unsigned char *sDynosTextDotDotDot;
static const unsigned char *sDynosTextA;
static const unsigned char *sDynosTextOpenLeft;
static const unsigned char *sDynosTextCloseLeft;
static const unsigned char *sDynosTextOpenRight;
static const unsigned char *sDynosTextCloseRight;

//
// Constructors
//

static struct DynosOption *dynos_add_option(const char *name, const char *configName, const char *label, const char *label2) {
    struct DynosOption *opt = calloc(1, sizeof(struct DynosOption));
    opt->name = string_to_dynamic(name);
    opt->configName = string_to_configname(configName);
    opt->label = string_to_label(label);
    opt->label2 = string_to_label2(label2);
    if (sPrevOpt == NULL) { // The very first option
        opt->prev = NULL;
        opt->next = NULL;
        opt->parent = NULL;
        sDynosMenu = opt;
    } else {
        if (sPrevOpt->type == DOPT_SUBMENU && sPrevOpt->submenu.empty) { // First option of a sub-menu
            opt->prev = NULL;
            opt->next = NULL;
            opt->parent = sPrevOpt;
            sPrevOpt->submenu.child = opt;
            sPrevOpt->submenu.empty = false;
        } else {
            opt->prev = sPrevOpt;
            opt->next = NULL;
            opt->parent = sPrevOpt->parent;
            sPrevOpt->next = opt;
        }
    }
    sPrevOpt = opt;
    return opt;
}

static void dynos_end_submenu() {
    if (sPrevOpt) {
        if (sPrevOpt->type == DOPT_SUBMENU && sPrevOpt->submenu.empty) { // ENDMENU command following a SUBMENU command
            sPrevOpt->submenu.empty = false;
        } else {
            sPrevOpt = sPrevOpt->parent;
        }
    }
}

static void dynos_create_submenu(const char *name, const char *label, const char *label2) {
    struct DynosOption *opt = dynos_add_option(name, NULL, label, label2);
    opt->type = DOPT_SUBMENU;
    opt->submenu.child = NULL;
    opt->submenu.empty = true;
}

static void dynos_create_toggle(const char *name, const char *configName, const char *label, int initValue) {
    struct DynosOption *opt = dynos_add_option(name, configName, label, NULL);
    opt->type = DOPT_TOGGLE;
    opt->toggle.ptog = calloc(1, sizeof(bool));
    *opt->toggle.ptog = (unsigned char) initValue;
}

static void dynos_create_scroll(const char *name, const char *configName, const char *label, int min, int max, int step, int initValue) {
    struct DynosOption *opt = dynos_add_option(name, configName, label, NULL);
    opt->type = DOPT_SCROLL;
    opt->scroll.min = min;
    opt->scroll.max = max;
    opt->scroll.step = step;
    opt->scroll.pvalue = calloc(1, sizeof(int));
    *opt->scroll.pvalue = initValue;
}

static void dynos_create_choice(const char *name, const char *configName, const char *label, const char **choices, int count, int initValue) {
    struct DynosOption *opt = dynos_add_option(name, configName, label, NULL);
    opt->type = DOPT_CHOICE;
    opt->choice.choices = calloc(count, sizeof(const unsigned char *));
    for (int i = 0; i != count; ++i) {
    opt->choice.choices[i] = string_to_label(choices[i]);
    }
    opt->choice.count = count;
    opt->choice.pindex = calloc(1, sizeof(int));
    *opt->choice.pindex = initValue;
}

static void dynos_create_button(const char *name, const char *label, const char *funcName) {
    struct DynosOption *opt = dynos_add_option(name, NULL, label, NULL);
    opt->type = DOPT_BUTTON;
    opt->button.funcName = string_to_dynamic(funcName);
}

static void dynos_create_bind(const char *name, const char *configName, const char *label, unsigned int mask, unsigned int *binds) {
    struct DynosOption *opt = dynos_add_option(name, configName, label, NULL);
    opt->type = DOPT_BIND;
    opt->bind.mask = mask;
    opt->bind.pbinds = calloc(MAX_BINDS, sizeof(unsigned int));
    for (int i = 0; i != MAX_BINDS; ++i) {
    opt->bind.pbinds[i] = binds[i];
    }
    opt->bind.dynos = true;
    opt->bind.index = 0;
}

static void dynos_read_file(const char *folder, const char *filename) {

    // Open file
    char fullname[SYS_MAX_PATH];
    snprintf(fullname, SYS_MAX_PATH, "%s/%s", folder, filename);
    FILE *f = fopen(fullname, "rt");
    if (f == NULL) {
        return;
    }

    // Read file and create options
    char buffer[DYNOS_LINE_MAX_LENGTH];
    while (fgets(buffer, DYNOS_LINE_MAX_LENGTH, f) != NULL) {
        struct StrTokens strtk = dynos_split_string(buffer);

        // Empty line
        if (strtk.count == 0) {
            continue;
        }

        // SUBMENU [Name] [Label]
        if (strcmp(strtk.tokens[0], "SUBMENU") == 0 && strtk.count >= 4) {
            dynos_create_submenu(strtk.tokens[1], strtk.tokens[2], strtk.tokens[3]);
            continue;
        }

        // TOGGLE  [Name] [Label] [ConfigName] [InitialValue]
        if (strcmp(strtk.tokens[0], "TOGGLE") == 0 && strtk.count >= 5) {
            dynos_create_toggle(strtk.tokens[1], strtk.tokens[3], strtk.tokens[2], string_to_int(strtk.tokens[4]));
            continue;
        }

        // SCROLL  [Name] [Label] [ConfigName] [InitialValue] [Min] [Max] [Step]
        if (strcmp(strtk.tokens[0], "SCROLL") == 0 && strtk.count >= 8) {
            dynos_create_scroll(strtk.tokens[1], strtk.tokens[3], strtk.tokens[2], string_to_int(strtk.tokens[5]), string_to_int(strtk.tokens[6]), string_to_int(strtk.tokens[7]), string_to_int(strtk.tokens[4]));
            continue;
        }

        // CHOICE  [Name] [Label] [ConfigName] [InitialValue] [ChoiceStrings...]
        if (strcmp(strtk.tokens[0], "CHOICE") == 0 && strtk.count >= 6) {
            const char *choices[DYNOS_STR_TOKENS_MAX_COUNT];
            for (unsigned int i = 0; i != strtk.count - 5; ++i) {
                choices[i] = &strtk.tokens[5 + i][0];
            }
            dynos_create_choice(strtk.tokens[1], strtk.tokens[3], strtk.tokens[2], choices, strtk.count - 5, string_to_int(strtk.tokens[4]));
            continue;
        }

        // BUTTON  [Name] [Label] [FuncName]
        if (strcmp(strtk.tokens[0], "BUTTON") == 0 && strtk.count >= 4) {
            dynos_create_button(strtk.tokens[1], strtk.tokens[2], strtk.tokens[3]);
            continue;
        }

        // BIND    [Name] [Label] [ConfigName] [Mask] [DefaultValues]
        if (strcmp(strtk.tokens[0], "BIND") == 0 && strtk.count >= 5 + MAX_BINDS) {
            unsigned int binds[MAX_BINDS];
            for (int i = 0; i != MAX_BINDS; ++i) {
                binds[i] = string_to_int(strtk.tokens[5 + i]);
            }
            dynos_create_bind(strtk.tokens[1], strtk.tokens[3], strtk.tokens[2], string_to_int(strtk.tokens[4]), binds);
            continue;
        }

        // ENDMENU
        if (strcmp(strtk.tokens[0], "ENDMENU") == 0) {
            dynos_end_submenu();
            continue;
        }
    }
    fclose(f);
}

static void dynos_load_options() {
    char optionsFolder[SYS_MAX_PATH];
    snprintf(optionsFolder, SYS_MAX_PATH, "%s/%s", sys_exe_path(), FS_BASEDIR);
    DIR *dir = opendir(optionsFolder);
    sPrevOpt = NULL;
    if (dir) {
        struct dirent *ent = NULL;
        while ((ent = readdir(dir)) != NULL) {
            if (dynos_is_txt_file(ent->d_name, strlen(ent->d_name))) {
                dynos_read_file(optionsFolder, ent->d_name);
            }
        }
        closedir(dir);
    }
}

//
// Vanilla options menu
//

#define optmenu_toggle optmenu_toggle_unused
#define optmenu_draw optmenu_draw_unused
#define optmenu_draw_prompt optmenu_draw_prompt_unused
#define optmenu_check_buttons optmenu_check_buttons_unused
#define optmenu_open optmenu_open_unused
#define DYNOS_INL
#include "game/options_menu.c"
#undef DYNOS_INL
#undef optmenu_toggle
#undef optmenu_draw
#undef optmenu_draw_prompt
#undef optmenu_check_buttons
#undef optmenu_open

//
// Vanilla actions
//

typedef void (*VanillaActionFunction)(struct Option *, int);
struct VanillaAction {
    const char *str;
    VanillaActionFunction action;
};

// "Construct On First Use" aka COFU
static struct Array *dynos_get_vanilla_action_list() {
    static struct Array sVanillaActions = { .buffer = NULL, .count = 0, .itemSize = sizeof(struct VanillaAction) };
    return &sVanillaActions;
}

static VanillaActionFunction dynos_get_vanilla_action(const char *name) {
    struct Array *vanillaActions = dynos_get_vanilla_action_list();
    for (size_t i = 0; i != vanillaActions->count; ++i) {
        if (strcmp(array_get(vanillaActions, i, struct VanillaAction).str, name) == 0) {
            return array_get(vanillaActions, i, struct VanillaAction).action;
        }
    }
    return NULL;
}

static void dynos_add_vanilla_action(const char *name, VanillaActionFunction func) {
    struct Array *vanillaActions = dynos_get_vanilla_action_list();
    for (size_t i = 0; i != vanillaActions->count; ++i) {
        if (strcmp(array_get(vanillaActions, i, struct VanillaAction).str, name) == 0) {
            return;
        }
    }
    struct VanillaAction vanillaAction = { name, func };
    array_add(vanillaActions, &vanillaAction);
}

static bool dynos_call_vanilla_action(const char *optName) {
    VanillaActionFunction func = dynos_get_vanilla_action(optName);
    if (func) {
        func(NULL, 0);
        return true;
    }
    return false;
}

//
// Convert classic options menu into DynOS menu
//

static struct DynosOption *dynos_convert_option(const unsigned char *label, const unsigned char *label2) {
    struct DynosOption *opt = calloc(1, sizeof(struct DynosOption));
    opt->name = label_to_name(label);
    opt->configName = NULL;
    opt->label = label;
    opt->label2 = label2;
    if (sPrevOpt == NULL) { // The very first option
        opt->prev = NULL;
        opt->next = NULL;
        opt->parent = NULL;
        sOptionsMenu = opt;
    } else {
        if (sPrevOpt->type == DOPT_SUBMENU && sPrevOpt->submenu.empty) { // First option of a sub-menu
            opt->prev = NULL;
            opt->next = NULL;
            opt->parent = sPrevOpt;
            sPrevOpt->submenu.child = opt;
            sPrevOpt->submenu.empty = false;
        } else {
            opt->prev = sPrevOpt;
            opt->next = NULL;
            opt->parent = sPrevOpt->parent;
            sPrevOpt->next = opt;
        }
    }
    sPrevOpt = opt;
    return opt;
}

static void dynos_convert_submenu(const unsigned char *label, const unsigned char *label2) {
    struct DynosOption *opt = dynos_convert_option(label_decapitalize(label), label2);
    opt->type = DOPT_SUBMENU;
    opt->submenu.child = NULL;
    opt->submenu.empty = true;
}

static void dynos_convert_toggle(const unsigned char *label, bool *bval) {
    struct DynosOption *opt = dynos_convert_option(label, NULL);
    opt->type = DOPT_TOGGLE;
    opt->toggle.ptog = (bool *) bval;
}

static void dynos_convert_scroll(const unsigned char *label, int min, int max, int step, unsigned int *uval) {
    struct DynosOption *opt = dynos_convert_option(label, NULL);
    opt->type = DOPT_SCROLL;
    opt->scroll.min = min;
    opt->scroll.max = max;
    opt->scroll.step = step;
    opt->scroll.pvalue = (int *) uval;
}

static void dynos_convert_choice(const unsigned char *label, const unsigned char **choices, int count, unsigned int *uval) {
    struct DynosOption *opt = dynos_convert_option(label, NULL);
    opt->type = DOPT_CHOICE;
    opt->choice.choices = choices;
    opt->choice.count = count;
    opt->choice.pindex = (int *) uval;
}

static void dynos_convert_button(const unsigned char *label, VanillaActionFunction action) {
    struct DynosOption *opt = dynos_convert_option(label_decapitalize(label), NULL);
    opt->type = DOPT_BUTTON;
    opt->button.funcName = string_to_dynamic("dynos_call_vanilla_action");
    dynos_add_vanilla_action(opt->name, action);
}

static void dynos_convert_bind(const unsigned char *label, unsigned int *binds) {
    struct DynosOption *opt = dynos_convert_option(label, NULL);
    opt->type = DOPT_BIND;
    opt->bind.mask = 0;
    opt->bind.pbinds = binds;
    opt->bind.dynos = false;
    opt->bind.index = 0;
}

static void dynos_convert_submenu_options(struct SubMenu *submenu) {
    for (int i = 0; i != submenu->numOpts; ++i) {
        struct Option *opt = &submenu->opts[i];
        switch (opt->type) {
            case OPT_TOGGLE:
                dynos_convert_toggle(opt->label, opt->bval);
                break;

            case OPT_CHOICE:
                dynos_convert_choice(opt->label, opt->choices, opt->numChoices, opt->uval);
                break;

            case OPT_SCROLL:
                dynos_convert_scroll(opt->label, opt->scrMin, opt->scrMax, opt->scrStep, opt->uval);
                break;

            case OPT_SUBMENU:
                dynos_convert_submenu(opt->label, opt->nextMenu->label);
                dynos_convert_submenu_options(opt->nextMenu);
                dynos_end_submenu();
                break;

            case OPT_BIND:
                dynos_convert_bind(opt->label, opt->uval);
                break;

            case OPT_BUTTON:
                dynos_convert_button(opt->label, opt->actionFn);
                break;

            default:
                break;
        }
    }
}

static void dynos_convert_options_menu() {
    sPrevOpt = NULL;
    dynos_convert_submenu_options(&menuMain);
    dynos_add_action("dynos_call_vanilla_action", dynos_call_vanilla_action, true);
}

//
// Loop through DynosOptions
//

typedef bool (*DynosLoopFunc)(struct DynosOption *, void *);
static struct DynosOption *dynos_loop(struct DynosOption *opt, DynosLoopFunc func, void *data) {
    while (opt) {
        if (opt->type == DOPT_SUBMENU) {
            struct DynosOption *res = dynos_loop(opt->submenu.child, func, data);
            if (res) {
                return res;
            }
        } else {
            if (func(opt, data)) {
                return opt;
            }
        }
        opt = opt->next;
    }
    return NULL;
}

//
// Inputs
//

struct KeyState {
    bool pressed;
    bool down;
};

struct DynosKeyboard {
    struct KeyState *state;
    int count;
};

struct DynosJoystick {
    struct KeyState *state;
    int count;
    SDL_Joystick *joystick;
};

static struct DynosKeyboard *sDynosKeyboard = NULL;
static struct DynosJoystick *sDynosJoystick = NULL;

static void dynos_update_controllers_state() {

    // Keyboard
    int kbcount = 0;
    const unsigned char *kbstate = SDL_GetKeyboardState(&kbcount);
    if (sDynosKeyboard) {
        for (int i = 0; i < sDynosKeyboard->count; ++i) {
            if (kbstate[i]) {
                sDynosKeyboard->state[i].pressed = !sDynosKeyboard->state[i].down;
                sDynosKeyboard->state[i].down    = true;
            } else {
                sDynosKeyboard->state[i].pressed = false;
                sDynosKeyboard->state[i].down    = false;
            }
        }
    } else if (kbcount > 0) {
        sDynosKeyboard        = calloc(1, sizeof(struct DynosKeyboard));
        sDynosKeyboard->count = kbcount;
        sDynosKeyboard->state = calloc(kbcount, sizeof(struct KeyState));
    }

    // Joysticks
    if (sDynosJoystick) {
        if (!SDL_JoystickGetAttached(sDynosJoystick->joystick)) { // Failsafe if the joystick is disconnected during game
            free(sDynosJoystick);
            sDynosJoystick = NULL;
        } else {
            for (int i = 0; i < sDynosJoystick->count; ++i) {
                if (SDL_JoystickGetButton(sDynosJoystick->joystick, i)) {
                    sDynosJoystick->state[i].pressed = !sDynosJoystick->state[i].down;
                    sDynosJoystick->state[i].down    = true;
                } else {
                    sDynosJoystick->state[i].pressed = false;
                    sDynosJoystick->state[i].down    = false;
                }
            }
        }
    } else {
        SDL_Joystick *joystick = SDL_JoystickOpen(0);
        if (joystick) {
            sDynosJoystick           = calloc(1, sizeof(struct DynosJoystick));
            sDynosJoystick->count    = SDL_JoystickNumButtons(joystick);
            sDynosJoystick->state    = calloc(sDynosJoystick->count, sizeof(struct KeyState));
            sDynosJoystick->joystick = joystick;
        }
    }
}

static bool dynos_update_controller_from_keyboard(struct DynosOption *opt, void *data) {
    if (opt->type == DOPT_BIND) {
        struct Controller *controller = (struct Controller *) data;
        for (int i = 0; i != MAX_BINDS; ++i) {
            int scancode = (int) opt->bind.pbinds[i];
            if (scancode >= 0 && scancode < sDynosKeyboard->count) {
                controller->buttonPressed |= opt->bind.mask * (sDynosKeyboard->state[scancode].pressed);
                controller->buttonDown    |= opt->bind.mask * (sDynosKeyboard->state[scancode].down);
            }
        }
    }
    return false;
}

static bool dynos_update_controller_from_joystick(struct DynosOption *opt, void *data) {
    if (opt->type == DOPT_BIND) {
        struct Controller *controller = (struct Controller *) data;
        for (int i = 0; i != MAX_BINDS; ++i) {
            int scancode = (int) opt->bind.pbinds[i] - 0x1000;
            if (scancode >= 0 && scancode < sDynosJoystick->count) {
                controller->buttonPressed |= opt->bind.mask * (sDynosJoystick->state[scancode].pressed);
                controller->buttonDown    |= opt->bind.mask * (sDynosJoystick->state[scancode].down);
            }
        }
    }
    return false;
}

static void dynos_update_controller() {
    static clock_t last = 0;
    clock_t curr = clock();

    // Prevent the game from updating inputs twice or more per frame
    // The delay between 2 consecutive updates must be at least 16 ms
    if ((curr - last) >= (CLOCKS_PER_SEC / 60)) {
        dynos_update_controllers_state();
        if (sDynosKeyboard) dynos_loop(sDynosMenu, dynos_update_controller_from_keyboard, (void *) &gControllers[0]);
        if (sDynosJoystick) dynos_loop(sDynosMenu, dynos_update_controller_from_joystick, (void *) &gControllers[0]);
        last = curr;
    }
}

//
// Config
//

static const char *sDynosConfigFilename = "DynOSconfig.cfg";

static bool dynos_get_option_config(struct DynosOption *opt, void *data) {
    unsigned char type = *(unsigned char *) ((void **) data)[0];
    const char *name = (const char *) ((void **) data)[1];
    return (opt->configName != NULL && strcmp(opt->configName, name) == 0 && opt->type == type);
}

static bool dynos_read_config_type_and_name(FILE *f, unsigned char *type, char *name) {
    
    // Read type
    if (fread(type, sizeof(unsigned char), 1, f) != 1) {
        return false;
    }

    // Read string length
    unsigned char len = 0;
    if (fread(&len, sizeof(unsigned char), 1, f) != 1) {
        return false;
    }

    // Read config name
    if (fread(name, sizeof(char), len, f) != len) {
        return false;
    }
    name[len] = 0;
    return true;
}

static void dynos_load_config() {
    char filename[SYS_MAX_PATH];
    snprintf(filename, SYS_MAX_PATH, "%s/%s", sys_user_path(), sDynosConfigFilename);
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return;
    while (true) {
        unsigned char type;
        char name[DYNOS_STR_TOKENS_MAX_LENGTH];
        bzero(name, DYNOS_STR_TOKENS_MAX_LENGTH);
        if (!dynos_read_config_type_and_name(f, &type, name)) {
            break;
        }
    
        struct DynosOption *opt = dynos_loop(sDynosMenu, dynos_get_option_config, (void **) (void *[]){ &type, name });
        if (opt != NULL) {
            switch (opt->type) {
                case DOPT_TOGGLE:
                    fread(opt->toggle.ptog, sizeof(bool), 1, f);
                    break;

                case DOPT_CHOICE:
                    fread(opt->choice.pindex, sizeof(int), 1, f);
                    break;

                case DOPT_SCROLL:
                    fread(opt->scroll.pvalue, sizeof(int), 1, f);
                    break;

                case DOPT_BIND:
                    fread(opt->bind.pbinds, sizeof(unsigned int), MAX_BINDS, f);
                    break;

                default:
                    break;
            }
        }
    }
    fclose(f);
}

static void dynos_write_config_type_and_name(FILE *f, unsigned char type, const char *name) {
    unsigned char len = (unsigned char) strlen(name);
    fwrite(&type, sizeof(unsigned char), 1, f);
    fwrite(&len, sizeof(unsigned char), 1, f);
    fwrite(name, sizeof(char), len, f);
}

static bool dynos_save_option_config(struct DynosOption *opt, void *data) {
    if (opt->configName != NULL) {
        FILE *f = (FILE *) data;
        switch (opt->type) {
            case DOPT_TOGGLE:
                dynos_write_config_type_and_name(f, DOPT_TOGGLE, opt->configName);
                fwrite(opt->toggle.ptog, sizeof(bool), 1, f);
                break;

            case DOPT_CHOICE:
                dynos_write_config_type_and_name(f, DOPT_CHOICE, opt->configName);
                fwrite(opt->choice.pindex, sizeof(int), 1, f);
                break;

            case DOPT_SCROLL:
                dynos_write_config_type_and_name(f, DOPT_SCROLL, opt->configName);
                fwrite(opt->scroll.pvalue, sizeof(int), 1, f);
                break;

            case DOPT_BIND:
                dynos_write_config_type_and_name(f, DOPT_BIND, opt->configName);
                fwrite(opt->bind.pbinds, sizeof(unsigned int), MAX_BINDS, f);
                break;

            default:
                break;
        }
    }
    return 0;
}

static void dynos_save_config() {
    char filename[SYS_MAX_PATH];
    snprintf(filename, SYS_MAX_PATH, "%s/%s", sys_user_path(), sDynosConfigFilename);
    FILE *f = fopen(filename, "wb");
    if (f == NULL) return;
    dynos_loop(sDynosMenu, dynos_save_option_config, (void *) f);
    fclose(f);
}

//
// Get/Set values
//

static bool dynos_get(struct DynosOption *opt, void *data) {
    return (opt->type == DOPT_TOGGLE || opt->type == DOPT_CHOICE || opt->type == DOPT_SCROLL) && (strcmp(opt->name, (const char *) data) == 0);
}

int dynos_get_value(const char *name) {
    struct DynosOption *opt = dynos_loop(sDynosMenu, dynos_get, (void *) name);
    if (opt) {
        if (opt->type == DOPT_TOGGLE) {
            return (int) *opt->toggle.ptog;
        }
        if (opt->type == DOPT_CHOICE) {
            return *opt->choice.pindex;
        }
        if (opt->type == DOPT_SCROLL) {
            return *opt->scroll.pvalue;
        }
    }
    return 0;
}

void dynos_set_value(const char *name, int value) {
    struct DynosOption *opt = dynos_loop(sDynosMenu, dynos_get, (void *) name);
    if (opt) {
        if (opt->type == DOPT_CHOICE) {
            *opt->toggle.ptog = (bool) value;
        }
        if (opt->type == DOPT_CHOICE) {
            *opt->choice.pindex = value;
        }
        if (opt->type == DOPT_SCROLL) {
            *opt->scroll.pvalue = value;
        }
    }
}

//
// Render
//

static bool sIsBinding = 0;
static struct DynosOption *sCurrentMenu = NULL;
static struct DynosOption *sCurrentOpt = NULL;

static const unsigned char sDialogCharWidths[256] = {
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,
    6,  6,  5,  6,  6,  5,  8,  8,  6,  6,  6,  6,  6,  5,  6,  6,
    8,  7,  6,  6,  6,  5,  5,  6,  5,  5,  6,  5,  4,  5,  5,  3,
    7,  5,  5,  5,  6,  5,  5,  5,  5,  5,  7,  7,  5,  5,  4,  4,
    8,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    8,  8,  8,  8,  7,  7,  6,  7,  7,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  6,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    7,  5, 10,  5,  9,  8,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  7,  7,  6,  6,  8,  0,  8, 10,  6,  4,  5,  0,  0
};

static int dynos_get_render_string_length(const unsigned char *str64) {
    int length = 0;
    for (; *str64 != DIALOG_CHAR_TERMINATOR; ++str64) {
        length += (int) sDialogCharWidths[*str64];
    }
    return length;
}

static void dynos_render_string(const unsigned char *str64, int x, int y) {
    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0);
    for (; *str64 != DIALOG_CHAR_TERMINATOR; ++str64) {
        if (*str64 != DIALOG_CHAR_SPACE) {
            if (*str64 == 253) { // underscore
                create_dl_translation_matrix(MENU_MTX_NOPUSH, -1, -5, 0);
                void **fontLUT = (void **) segmented_to_virtual(main_font_lut);
                void *packedTexture = segmented_to_virtual(fontLUT[159]);
                gDPPipeSync(gDisplayListHead++);
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
                gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
                create_dl_translation_matrix(MENU_MTX_NOPUSH, 0, +5, 0);
            } else {
                void **fontLUT = (void **) segmented_to_virtual(main_font_lut);
                void *packedTexture = segmented_to_virtual(fontLUT[*str64]);
                gDPPipeSync(gDisplayListHead++);
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
                gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
            }
        }
        create_dl_translation_matrix(MENU_MTX_NOPUSH, sDialogCharWidths[*str64], 0, 0);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

static void dynos_print_string(const unsigned char *str64, int x, int y, unsigned int rgbaFront, unsigned int rgbaBack, bool alignLeft) {
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    if ((rgbaBack & 0xFF) != 0) {
        gDPSetEnvColor(gDisplayListHead++, ((rgbaBack >> 24) & 0xFF), ((rgbaBack >> 16) & 0xFF), ((rgbaBack >> 8) & 0xFF), ((rgbaBack >> 0) & 0xFF));
        if (alignLeft) {
            dynos_render_string(str64, GFX_DIMENSIONS_FROM_LEFT_EDGE(x) + 1, y - 1);
        } else {
            dynos_render_string(str64, GFX_DIMENSIONS_FROM_RIGHT_EDGE(x + dynos_get_render_string_length(str64) - 1), y - 1);
        }
    }
    if ((rgbaFront & 0xFF) != 0) {
        gDPSetEnvColor(gDisplayListHead++, ((rgbaFront >> 24) & 0xFF), ((rgbaFront >> 16) & 0xFF), ((rgbaFront >> 8) & 0xFF), ((rgbaFront >> 0) & 0xFF));
        if (alignLeft) {
            dynos_render_string(str64, GFX_DIMENSIONS_FROM_LEFT_EDGE(x), y);
        } else {
            dynos_render_string(str64, GFX_DIMENSIONS_FROM_RIGHT_EDGE(x + dynos_get_render_string_length(str64)), y);
        }
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
}

static void dynos_print_box(int x, int y, int w, int h, unsigned int rgbaColor, bool alignLeft) {
    if ((rgbaColor && 0xFF) != 0) {
        Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));
        if (!matrix) return;
        if (alignLeft) {
            create_dl_translation_matrix(MENU_MTX_PUSH, GFX_DIMENSIONS_FROM_LEFT_EDGE(x), y + h, 0);
        } else {
            create_dl_translation_matrix(MENU_MTX_PUSH, GFX_DIMENSIONS_FROM_RIGHT_EDGE(x + w), y + h, 0);
        }
        guScale(matrix, (float) w / 130.f, (float) h / 80.f, 1.f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gDPSetEnvColor(gDisplayListHead++, ((rgbaColor >> 24) & 0xFF), ((rgbaColor >> 16) & 0xFF), ((rgbaColor >> 8) & 0xFF), ((rgbaColor >> 0) & 0xFF));
        gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    }
}

static inline int dynos_get_label2_x(int x, const unsigned char *str) {
    int len = 0;
    while (*str != GLOBAR_CHAR_TERMINATOR) ++str, ++len;
    return x - len * 6; // stride is 12
}

//
// Options menu
//

static int dynos_get_option_count() {
    struct DynosOption *opt = sCurrentOpt;
    while (opt->prev) {
        opt = opt->prev;
    }
    int count = 0;
    while (opt) {
        opt = opt->next;
        count++;
    }
    return count;
}

static int dynos_get_current_option_index() {
    struct DynosOption *opt = sCurrentOpt;
    int index = 0;
    while (opt->prev) {
        opt = opt->prev;
        index++;
    }
    return index;
}

#define PREV(opt) (opt == NULL ? NULL : opt->prev)
#define NEXT(opt) (opt == NULL ? NULL : opt->next)
static struct DynosOption **dynos_get_options_to_draw() {
    static struct DynosOption *sOptionList[13];
    bzero(sOptionList, 13 * sizeof(struct DynosOption *));

    sOptionList[6]  = sCurrentOpt;
    sOptionList[5]  = PREV(sOptionList[6]);
    sOptionList[4]  = PREV(sOptionList[5]);
    sOptionList[3]  = PREV(sOptionList[4]);
    sOptionList[2]  = PREV(sOptionList[3]);
    sOptionList[1]  = PREV(sOptionList[2]);
    sOptionList[0]  = PREV(sOptionList[1]);
    sOptionList[7]  = NEXT(sOptionList[6]);
    sOptionList[8]  = NEXT(sOptionList[7]);
    sOptionList[9]  = NEXT(sOptionList[8]);
    sOptionList[10] = NEXT(sOptionList[9]);
    sOptionList[11] = NEXT(sOptionList[10]);
    sOptionList[12] = NEXT(sOptionList[11]);

    int start = 12, end = 0;
    for (int i = 0; i != 13; ++i) {
        if (sOptionList[i] != NULL) {
            start = MIN(start, i);
            end = MAX(end, i);
        }
    }

    if (end - start < 7) {
        return &sOptionList[start];
    }
    if (end <= 9) {
        return &sOptionList[end - 6];
    }
    if (start >= 3) {
        return &sOptionList[start];
    }
    return &sOptionList[3];
}
#undef PREV
#undef NEXT

#define COLOR_WHITE             0xFFFFFFFF
#define COLOR_BLACK             0x000000FF
#define COLOR_GRAY              0xA0A0A0FF
#define COLOR_DARK_GRAY         0x808080FF
#define COLOR_SELECT            0x80E0FFFF
#define COLOR_SELECT_BOX        0x00FFFF20
#define COLOR_ENABLED           0x20E020FF
#define COLOR_DISABLED          0xFF2020FF
#define OFFSET_FROM_LEFT_EDGE   64
#define OFFSET_FROM_RIGHT_EDGE  64

static void dynos_draw_option(struct DynosOption *opt, int y) {
    if (opt == NULL) {
        return;
    }

    // Selected box
    if (opt == sCurrentOpt) {
        unsigned char a = (unsigned char) ((coss(gGlobalTimer * 0x800) + 1.f) * 0x20);
        dynos_print_box(OFFSET_FROM_LEFT_EDGE - 4, y - 2, GFX_DIMENSIONS_FROM_RIGHT_EDGE(OFFSET_FROM_RIGHT_EDGE) - GFX_DIMENSIONS_FROM_LEFT_EDGE(OFFSET_FROM_LEFT_EDGE) + 8, 20, COLOR_SELECT_BOX + a, 1);
    }

    // Label
    if (opt == sCurrentOpt) {
        dynos_print_string(opt->label, OFFSET_FROM_LEFT_EDGE, y, COLOR_SELECT, COLOR_BLACK, 1);
    } else {
        dynos_print_string(opt->label, OFFSET_FROM_LEFT_EDGE, y, COLOR_WHITE, COLOR_BLACK, 1);
    }

    // Values
    int w;
    switch (opt->type) {
        case DOPT_TOGGLE:
            if (*opt->toggle.ptog) {
                dynos_print_string(sDynosTextEnabled, OFFSET_FROM_RIGHT_EDGE, y, COLOR_ENABLED, COLOR_BLACK, 0);
            } else {
                dynos_print_string(sDynosTextDisabled, OFFSET_FROM_RIGHT_EDGE, y, COLOR_DISABLED, COLOR_BLACK, 0);
            }
            break;

        case DOPT_CHOICE:
            dynos_print_string(opt->choice.choices[*opt->choice.pindex], OFFSET_FROM_RIGHT_EDGE, y, opt == sCurrentOpt ? COLOR_SELECT : COLOR_WHITE, COLOR_BLACK, 0);
            break;

        case DOPT_SCROLL:
            w = 80 * (float)(*opt->scroll.pvalue - opt->scroll.min) / (float)(opt->scroll.max - opt->scroll.min);
            dynos_print_string(int_to_string("%d", *opt->scroll.pvalue), OFFSET_FROM_RIGHT_EDGE, y, opt == sCurrentOpt ? COLOR_SELECT : COLOR_WHITE, COLOR_BLACK, 0);
            dynos_print_box(OFFSET_FROM_RIGHT_EDGE + 28, y + 4, 82, 8, COLOR_DARK_GRAY, 0);
            dynos_print_box(OFFSET_FROM_RIGHT_EDGE + 29 + 80 - w, y + 5, w, 6, opt == sCurrentOpt ? COLOR_SELECT : COLOR_WHITE, 0);
            break;

        case DOPT_BIND:
            for (int i = 0; i != MAX_BINDS; ++i) {
                unsigned int bind = opt->bind.pbinds[i];
                if (opt == sCurrentOpt && i == opt->bind.index) {
                    if (sIsBinding) {
                        dynos_print_string(sDynosTextDotDotDot, OFFSET_FROM_RIGHT_EDGE + (2 - i) * 36, y, COLOR_SELECT, COLOR_BLACK, 0);
                    } else if (bind == VK_INVALID) {
                        dynos_print_string(sDynosTextNone, OFFSET_FROM_RIGHT_EDGE + (2 - i) * 36, y, COLOR_SELECT, COLOR_BLACK, 0);
                    } else {
                        dynos_print_string(int_to_string("%04X", bind), OFFSET_FROM_RIGHT_EDGE + (2 - i) * 36, y, COLOR_SELECT, COLOR_BLACK, 0);
                    }
                } else {
                    if (bind == VK_INVALID) {
                        dynos_print_string(sDynosTextNone, OFFSET_FROM_RIGHT_EDGE + (2 - i) * 36, y, COLOR_GRAY, COLOR_BLACK, 0);
                    } else {
                        dynos_print_string(int_to_string("%04X", bind), OFFSET_FROM_RIGHT_EDGE + (2 - i) * 36, y, COLOR_WHITE, COLOR_BLACK, 0);
                    }
                }
            }
            break;

        case DOPT_BUTTON:
            break;

        case DOPT_SUBMENU:
            if (opt == sCurrentOpt) {
                dynos_print_string(sDynosTextA, OFFSET_FROM_RIGHT_EDGE, y, COLOR_SELECT, COLOR_BLACK, 0);
            }
            break;
    }
}

static void dynos_draw_menu() {
    if (sCurrentMenu == NULL) {
        return;
    }

    // Colorful label
    const unsigned char *label2 = (sCurrentOpt->parent ? sCurrentOpt->parent->label2 : (sCurrentMenu == sDynosMenu ? sDynosTextDynosMenu : sDynosTextOptionsMenu));
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    print_hud_lut_string(HUD_LUT_GLOBAL, dynos_get_label2_x(SCREEN_WIDTH / 2, label2), 40, label2);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    // Display options
    struct DynosOption **optionsToDraw = dynos_get_options_to_draw();
    for (int i = 0; i != 7; ++i) {
        dynos_draw_option(optionsToDraw[i], 156 - 20 * i);
    }

    // Scroll bar
    int optCount = dynos_get_option_count();
    int optIndex = dynos_get_current_option_index();
    if (optCount > 7) {
        int h = (int) (134.f * sqrtf(1.f / (optCount - 6)));
        int y = 37 + (134 - h) * (1.f - MAX(0.f, MIN(1.f, (float)(optIndex - 3) / (float)(optCount - 6))));
        dynos_print_box(OFFSET_FROM_RIGHT_EDGE - 16, 36, 8, 136, COLOR_DARK_GRAY, 0);
        dynos_print_box(OFFSET_FROM_RIGHT_EDGE - 15, y, 6, h, COLOR_WHITE, 0);
    }
}

//
// Processing
//

#define SOUND_DYNOS_SAVED   (SOUND_MENU_MARIO_CASTLE_WARP2  | (0xFF << 8))
#define SOUND_DYNOS_SELECT  (SOUND_MENU_CHANGE_SELECT       | (0xF8 << 8))
#define SOUND_DYNOS_OK      (SOUND_MENU_CHANGE_SELECT       | (0xF8 << 8))
#define SOUND_DYNOS_CANCEL  (SOUND_MENU_CAMERA_BUZZ         | (0xFC << 8))

enum {
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_A,
    INPUT_Z
};

enum {
    RESULT_NONE,
    RESULT_OK,
    RESULT_CANCEL
};

static int dynos_get_keyboard_key() {
    if (sDynosKeyboard) {
        for (int i = 0; i < sDynosKeyboard->count; ++i) {
            if (sDynosKeyboard->state[i].pressed) {
                return i;
            }
        }
    }
    return VK_INVALID;
}

static int dynos_get_joystick_key() {
    if (sDynosJoystick) {
        for (int i = 0; i < sDynosJoystick->count; ++i) {
            if (sDynosJoystick->state[i].pressed) {
                return i + 0x1000;
            }
        }
    }
    return VK_INVALID;
}

static int dynos_get_raw_key() {
    int key = dynos_get_joystick_key();
    if (key == VK_INVALID) key = dynos_get_keyboard_key();
    return key;
}

static int dynos_opt_process_input(struct DynosOption *opt, int input) {
    switch (opt->type) {
        case DOPT_TOGGLE:
            if (input == INPUT_LEFT) {
                *opt->toggle.ptog = false;
                return RESULT_OK;
            }
            if (input == INPUT_RIGHT) {
                *opt->toggle.ptog = true;
                return RESULT_OK;
            }
            if (input == INPUT_A) {
                *opt->toggle.ptog = !(*opt->toggle.ptog);
                return RESULT_OK;
            }
            break;

        case DOPT_CHOICE:
            if (input == INPUT_LEFT) {
                *opt->choice.pindex = (*opt->choice.pindex + opt->choice.count - 1) % (opt->choice.count);
                return RESULT_OK;
            }
            if (input == INPUT_RIGHT || input == INPUT_A) {
                *opt->choice.pindex = (*opt->choice.pindex + 1) % (opt->choice.count);
                return RESULT_OK;
            }
            break;

        case DOPT_SCROLL:
            if (input == INPUT_LEFT) {
                *opt->scroll.pvalue = MAX(opt->scroll.min, *opt->scroll.pvalue - opt->scroll.step * (gPlayer1Controller->buttonDown & A_BUTTON ? 5 : 1));
                return RESULT_OK;
            }
            if (input == INPUT_RIGHT) {
                *opt->scroll.pvalue = MIN(opt->scroll.max, *opt->scroll.pvalue + opt->scroll.step * (gPlayer1Controller->buttonDown & A_BUTTON ? 5 : 1));
                return RESULT_OK;
            }
            break;

        case DOPT_BIND:
            if (input == INPUT_LEFT) {
                opt->bind.index = MAX(0, opt->bind.index - 1);
                return RESULT_OK;
            }
            if (input == INPUT_RIGHT) {
                opt->bind.index = MIN(MAX_BINDS - 1, opt->bind.index + 1);
                return RESULT_OK;
            }
            if (input == INPUT_Z) {
                opt->bind.pbinds[opt->bind.index] = VK_INVALID;
                return RESULT_OK;
            }
            if (input == INPUT_A) {
                opt->bind.pbinds[opt->bind.index] = VK_INVALID;
                sIsBinding = true;
                controller_get_raw_key();
                return RESULT_OK;
            }
            break;

        case DOPT_BUTTON:
            if (input == INPUT_A && opt->button.funcName != NULL) {
                DynosActionFunction action = dynos_get_action(opt->button.funcName);
                if (action != NULL && action(opt->name)) {
                    return RESULT_OK;
                }
                return RESULT_CANCEL;
            }
            break;

        case DOPT_SUBMENU:
            if (input == INPUT_A) {
                if (opt->submenu.child != NULL) {
                    sCurrentOpt = opt->submenu.child;
                    return RESULT_OK;
                }
                return RESULT_CANCEL;
            }
            break;
    }
    return RESULT_NONE;
}

static void dynos_open(struct DynosOption *menu) {
    play_sound(SOUND_DYNOS_SELECT, gDefaultSoundArgs);
    sCurrentMenu = menu;
    sCurrentOpt = menu;
}

static void dynos_close() {
    if (sCurrentMenu != NULL) {
        play_sound(SOUND_DYNOS_SAVED, gDefaultSoundArgs);
        controller_reconfigure();
        configfile_save(configfile_name());
        dynos_save_config();
        sCurrentMenu = NULL;
    }
}

static void dynos_process_inputs() {
    static int sStickTimer = 0;
    static bool sPrevStick = 0;

    // Stick values
    float stickX = gPlayer1Controller->stickX;
    float stickY = gPlayer1Controller->stickY;
    if (absx(stickX) > 60 || absx(stickY) > 60) {
        if (sStickTimer == 0) {
            sStickTimer = (sPrevStick ? 2 : 9);
        } else {
            stickX = 0;
            stickY = 0;
            sStickTimer--;
        }
        sPrevStick = true;
    } else {
        sStickTimer = 0;
        sPrevStick = false;
    }

    // Key binding
    if (sIsBinding) {
        unsigned int key = (sCurrentOpt->bind.dynos ? (unsigned int) dynos_get_raw_key() : controller_get_raw_key());
        if (key != VK_INVALID) {
            play_sound(SOUND_DYNOS_SELECT, gDefaultSoundArgs);
            sCurrentOpt->bind.pbinds[sCurrentOpt->bind.index] = key;
            sIsBinding = false;
        }
        return;
    }

    if (sCurrentMenu != NULL) {

        // Up
        if (stickY > +60) {
            if (sCurrentOpt->prev != NULL) {
                sCurrentOpt = sCurrentOpt->prev;
            } else {
                while (sCurrentOpt->next) sCurrentOpt = sCurrentOpt->next;
            }
            play_sound(SOUND_DYNOS_SELECT, gDefaultSoundArgs);
            return;
        }

        // Down
        if (stickY < -60) {
            if (sCurrentOpt->next != NULL) {
                sCurrentOpt = sCurrentOpt->next;
            } else {
                while (sCurrentOpt->prev) sCurrentOpt = sCurrentOpt->prev;
            }
            play_sound(SOUND_DYNOS_SELECT, gDefaultSoundArgs);
            return;
        }

        // Left
        if (stickX < -60) {
            switch (dynos_opt_process_input(sCurrentOpt, INPUT_LEFT)) {
                case RESULT_OK:     play_sound(SOUND_DYNOS_OK, gDefaultSoundArgs); break;
                case RESULT_CANCEL: play_sound(SOUND_DYNOS_CANCEL, gDefaultSoundArgs); break;
                case RESULT_NONE:   break;
            }
            return;
        }

        // Right
        if (stickX > +60) {
            switch (dynos_opt_process_input(sCurrentOpt, INPUT_RIGHT)) {
                case RESULT_OK:     play_sound(SOUND_DYNOS_OK, gDefaultSoundArgs); break;
                case RESULT_CANCEL: play_sound(SOUND_DYNOS_CANCEL, gDefaultSoundArgs); break;
                case RESULT_NONE:   break;
            }
            return;
        }

        // A
        if (gPlayer1Controller->buttonPressed & A_BUTTON) {
            switch (dynos_opt_process_input(sCurrentOpt, INPUT_A)) {
                case RESULT_OK:     play_sound(SOUND_DYNOS_OK, gDefaultSoundArgs); break;
                case RESULT_CANCEL: play_sound(SOUND_DYNOS_CANCEL, gDefaultSoundArgs); break;
                case RESULT_NONE:   break;
            }
            return;
        }

        // B
        if (gPlayer1Controller->buttonPressed & B_BUTTON) {
            if (sCurrentOpt->parent != NULL) {
                sCurrentOpt = sCurrentOpt->parent;
                play_sound(SOUND_DYNOS_SELECT, gDefaultSoundArgs);
            } else {
                dynos_close();
            }
            return;
        }

        // Z
        if (gPlayer1Controller->buttonPressed & Z_TRIG) {
            switch (dynos_opt_process_input(sCurrentOpt, INPUT_Z)) {
                case RESULT_OK:     play_sound(SOUND_DYNOS_OK, gDefaultSoundArgs); break;
                case RESULT_CANCEL: play_sound(SOUND_DYNOS_CANCEL, gDefaultSoundArgs); break;
                case RESULT_NONE:
                    if (sCurrentMenu == sDynosMenu) {
                        dynos_close();
                    } else {
                        dynos_open(sDynosMenu);
                    } break;
            }
            return;
        }

        // R
        if (gPlayer1Controller->buttonPressed & R_TRIG) {
            if (sCurrentMenu == sOptionsMenu) {
                dynos_close();
            } else {
                dynos_open(sOptionsMenu);
            }
            return;
        }

        // Start
        if (gPlayer1Controller->buttonPressed & START_BUTTON) {
            dynos_close();
            return;
        }
    } else if (gPlayer1Controller->buttonPressed & R_TRIG) {
        dynos_open(sOptionsMenu);
    } else if (gPlayer1Controller->buttonPressed & Z_TRIG) {
        dynos_open(sDynosMenu);
    }
}

static void dynos_draw_prompt() {
    if (sCurrentMenu == sOptionsMenu) {
        dynos_print_string(sDynosTextOpenLeft, 100, 212, COLOR_WHITE, COLOR_BLACK, 1);
        dynos_print_string(sDynosTextCloseRight, 100, 212, COLOR_WHITE, COLOR_BLACK, 0);
    } else if (sCurrentMenu == sDynosMenu) {
        dynos_print_string(sDynosTextCloseLeft, 100, 212, COLOR_WHITE, COLOR_BLACK, 1);
        dynos_print_string(sDynosTextOpenRight, 100, 212, COLOR_WHITE, COLOR_BLACK, 0);
    } else {
        dynos_print_string(sDynosTextOpenLeft, 100, 212, COLOR_WHITE, COLOR_BLACK, 1);
        dynos_print_string(sDynosTextOpenRight, 100, 212, COLOR_WHITE, COLOR_BLACK, 0);
    }
    dynos_process_inputs();
}

//
// Init
//

static const BehaviorScript bhvDynosUpdateController[] = {
    0x000B0000,
    0x08000000,
    0x0C000000, (uintptr_t) dynos_update_controller,
    0x09000000,
};

static void dynos_spawn_object() {
    if (gObjectLists) {
        struct Object *head = (struct Object *) &gObjectLists[OBJ_LIST_SPAWNER];
        struct Object *next = (struct Object *) head->header.next;
        while (true) {
            if (!next || next == head) { create_object(bhvDynosUpdateController); break; }
            if (next->behavior == bhvDynosUpdateController && next->activeFlags != 0) { break; }
            next = (struct Object *) next->header.next;
        }
    }
}

static void (*sControllerKeyboardRead)(OSContPad *) = NULL;
static void dynos_controller_keyboard_read(OSContPad *pad) {
    dynos_spawn_object();
    if (sControllerKeyboardRead) {
        sControllerKeyboardRead(pad);
    }
}

static void (*sControllerJoystickRead)(OSContPad *) = NULL;
static void dynos_controller_joystick_read(OSContPad *pad) {
    dynos_spawn_object();
    if (sControllerJoystickRead) {
        sControllerJoystickRead(pad);
    }
}

void dynos_init() {

    // Convert options menu
    dynos_convert_options_menu();

    // Create DynOS menu
    dynos_load_options();

    // Warp to level
    const char *levelNames[64];
    int levelCount = level_get_count(true);
    const enum LevelNum *levelList = level_get_list(true, true);
    for (int i = 0; i < levelCount; ++i) levelNames[i] = string_to_dynamic(level_get_name_decapitalized(levelList[i]));
    dynos_create_submenu("dynos_warp_submenu", "Warp to Level", "WARP TO LEUEL");
    dynos_create_choice("dynos_warp_level", NULL, "Level Select", (const char **) levelNames, levelCount, 0);
    dynos_create_choice("dynos_warp_act", NULL, "Star Select", (const char **) (const char *[]) { "Star 1", "Star 2", "Star 3", "Star 4", "Star 5", "Star 6" }, 6, 0);
    dynos_create_button("dynos_warp_to_level", "Warp", "dynos_warp_to_level");
    dynos_end_submenu();

    // Restart level
    dynos_create_button("dynos_restart_level", "Restart Level", "dynos_restart_level");

    // Return to main menu
    dynos_create_button("dynos_return_to_main_menu", "Return to Main Menu", "dynos_return_to_main_menu");

    // Init config
    dynos_load_config();

    // Init the inputs update routine
    sControllerKeyboardRead  = controller_keyboard.read;
    sControllerJoystickRead  = controller_sdl.read;
    controller_keyboard.read = dynos_controller_keyboard_read;
    controller_sdl.read      = dynos_controller_joystick_read;

    // Init text
    sDynosTextOptionsMenu = string_to_label2("OPTIONS");
    sDynosTextDynosMenu   = string_to_label2("DYNOS MENU");
    sDynosTextDisabled    = string_to_label("Disabled");
    sDynosTextEnabled     = string_to_label("Enabled");
    sDynosTextNone        = string_to_label("NONE");
    sDynosTextDotDotDot   = string_to_label("...");
    sDynosTextA           = string_to_label("(A)  "); ((unsigned char *) sDynosTextA)[1] = 84; ((unsigned char *) sDynosTextA)[4] = 83;
    sDynosTextOpenLeft    = string_to_label("Z DynOS"); ((unsigned char *) sDynosTextOpenLeft)[0] = 87;
    sDynosTextCloseLeft   = string_to_label("Z Return"); ((unsigned char *) sDynosTextCloseLeft)[0] = 87;
    sDynosTextOpenRight   = string_to_label("R Options"); ((unsigned char *) sDynosTextOpenRight)[0] = 88;
    sDynosTextCloseRight  = string_to_label("R Return"); ((unsigned char *) sDynosTextCloseRight)[0] = 88;
}

//
// Hijack
//

unsigned char optmenu_open = 0;

void optmenu_toggle(void) {
    dynos_close();
}

void optmenu_draw(void) {
    dynos_draw_menu();
}

void optmenu_draw_prompt(void) {
    dynos_draw_prompt();
    optmenu_open = (sCurrentMenu != NULL);
}

void optmenu_check_buttons(void) {
    dynos_update_controller();
}

//
// Return to Main Menu
//

extern char gDialogBoxState;
extern short gMenuMode;
void dynos_unpause_game() {
    level_set_transition(0, 0);
    play_sound(SOUND_MENU_PAUSE_2, gDefaultSoundArgs);
    gDialogBoxState = 0;
    gMenuMode = -1;
}

bool dynos_return_to_main_menu(UNUSED const char *optName) {
    optmenu_toggle();
    dynos_unpause_game();
    fade_into_special_warp(-2, 0);
    return true;
}

//
// Warp to Level
//

bool dynos_perform_warp(enum LevelNum levelNum, int actNum) {
    enum CourseNum courseNum = level_get_course(levelNum);
    if (courseNum == COURSE_NONE) {
        return false;
    }

    // Free everything from the current level
    optmenu_toggle();
    dynos_unpause_game();
    set_sound_disabled(FALSE);
    play_shell_music();
    stop_shell_music();
    stop_cap_music();
    clear_objects();
    clear_area_graph_nodes();
    clear_areas();
    main_pool_pop_state();

    // Reset Mario's state
    gMarioState->healCounter = 0;
    gMarioState->hurtCounter = 0;
    gMarioState->numCoins = 0;
    gMarioState->input = 0;
    gMarioState->controller->buttonPressed = 0;
    gHudDisplay.coins = 0;

    // Load the new level
    gCurrLevelNum = levelNum;
    gCurrCourseNum = courseNum;
    gCurrActNum = (courseNum <= COURSE_STAGES_MAX ? actNum : 0);
    gDialogCourseActNum = gCurrActNum;
    gCurrAreaIndex = 1;
    level_script_execute((struct LevelCommand *) level_get_script(levelNum));
    sWarpDest.type = 2;
    sWarpDest.levelNum = gCurrLevelNum;
    sWarpDest.areaIdx = 1;
    sWarpDest.nodeId = 0x0A;
    sWarpDest.arg = 0;
    gSavedCourseNum = gCurrCourseNum;
    return true;
}

bool dynos_warp_to_level(UNUSED const char *optName) {
    enum LevelNum levelNum = level_get_list(true, true)[dynos_get_value("dynos_warp_level")];
    return dynos_perform_warp(levelNum, dynos_get_value("dynos_warp_act") + 1);
}

//
// Restart Level
//

bool dynos_restart_level(UNUSED const char *optName) {
    enum LevelNum levelNum = (enum LevelNum) gCurrLevelNum;
    if (levelNum == LEVEL_BOWSER_1) levelNum = LEVEL_BITDW;
    if (levelNum == LEVEL_BOWSER_2) levelNum = LEVEL_BITFS;
    if (levelNum == LEVEL_BOWSER_3) levelNum = LEVEL_BITS;
    return dynos_perform_warp(levelNum, gCurrActNum);
}
