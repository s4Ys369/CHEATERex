#include "types.h"
#include "fs/fs.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#ifdef _WIN32
#include <direct.h>
#endif

#define STR_TOKENS_MAX_LENGTH   32
#define STR_TOKENS_MAX_COUNT    32

//
// Dynamic Size Array
//

struct Array {
    void **buffer;
    u32 count;
};
static const struct Array sEmptyArray = { NULL, 0 };

void array_add(struct Array *array, void *ptr, u32 size) {

    // Creates an item
    void *item = calloc(1, size);
    memcpy(item, ptr, size);

    // Alloc space
    void **newBuffer = calloc(array->count + 1, sizeof(void *));
    if (array->buffer) {
        memcpy(newBuffer, array->buffer, array->count * sizeof(void *));
        free(array->buffer);
    }
    array->buffer = newBuffer;

    // Insert item
    array->buffer[array->count] = item;
    array->count++;
}

// Doesn't check bounds
void *array_get(struct Array *array, u32 index) {
    return array->buffer[index];
}

#define ARRAY_ADD(array, x)         array_add(&array, &x, sizeof(x))
#define ARRAY_GET(array, i, type)   (*((type *) array_get(&array, i)))

//
// Structs
//

struct DynosValue {
    u8 type;
    union {
        u32 uval;
        struct Bind {
            const char *key;
            u32 mask;
            u32 binds[MAX_BINDS];
        } bind;
    };
};

struct DynosValues {
    const char *id;
    struct Array values;
};

struct DynosAction {
    const char *str;
    void (*actionFn)(struct Option *, s32);
};

#ifndef OPTIONS_MENU_H

enum OptType {
    OPT_INVALID = 0,
    OPT_TOGGLE,
    OPT_CHOICE,
    OPT_SCROLL,
    OPT_SUBMENU,
    OPT_BIND,
    OPT_BUTTON,
};

struct SubMenu;

struct Option {
    enum OptType type;
    const u8 *label;
    union {
        u32 *uval;
        bool *bval;
    };
    union {
        struct {
            const u8 **choices;
            u32 numChoices;
        };
        struct {
            u32 scrMin;
            u32 scrMax;
            u32 scrStep;
        };
        struct SubMenu *nextMenu;
        void (*actionFn)(struct Option *, s32);
    };
};

struct SubMenu {
    struct SubMenu *prev; // this is set at runtime to avoid needless complication
    const u8 *label;
    struct Option *opts;
    s32 numOpts;
    s32 select;
    s32 scroll;
};

#endif

//
// Utils
//

static s32 dynos_is_txt_file(const char *filename, u32 length) {
    if (length < 4) {
        return FALSE;
    }
    return
        filename[length - 4] == '.' &&
        filename[length - 3] == 't' &&
        filename[length - 2] == 'x' &&
        filename[length - 1] == 't';
}

struct StrTokens { char tokens[STR_TOKENS_MAX_COUNT][STR_TOKENS_MAX_LENGTH]; u32 count; };
static struct StrTokens dynos_split_string(const char *str) {
    struct StrTokens strtk = { .count = 0 };
    s32 l = 0;
    s32 treatSpacesAsChar = FALSE;
    for (; *str != 0; str++) {
        char c = *str;
        if ((c == ' ' && !treatSpacesAsChar) || c == '\t' || c == '\r' || c == '\n') {
            if (l > 0) {
                strtk.tokens[strtk.count][l] = 0;
                strtk.count++;
                if (strtk.count == STR_TOKENS_MAX_COUNT) {
                    return strtk;
                }
                l = 0;
            }
        } else if (c == '\"') {
            treatSpacesAsChar = !treatSpacesAsChar;
        } else if (l < (STR_TOKENS_MAX_LENGTH - 1)) {
            strtk.tokens[strtk.count][l] = c;
            l++;
        }
    }
    return strtk;
}

static u32 dynos_string_to_int(const char *str) {
    u32 x = 0;
    if (strlen(str) > 2 && str[0] == '0' && str[1] == 'x') {
        sscanf(str + 2, "%X", &x);
    } else {
        sscanf(str, "%u", &x);
    }
    return x;
}

static const char *dynos_alloc_string(const char *str) {
    char *s = calloc(strlen(str) + 1, sizeof(char));
    memcpy(s, str, strlen(str) * sizeof(char));
    return s;
}

static const u8 *dynos_alloc_sm64_string(const char *str) {
    u8 *str64 = calloc(strlen(str) + 1, sizeof(u8));
    u32 i = 0;
    for (; *str != 0; str++) {
        char c = *str;
        if (c >= '0' && c <= '9') {
            str64[i++] = (u8)(c - '0');
        } else if (c >= 'A' && c <= 'Z') {
            str64[i++] = (u8)(c - 'A' + 10);
        } else if (c >= 'a' && c <= 'z') {
            str64[i++] = (u8)(c - 'a' + 36);
        } else if (c == '[') {
            char c1 = *(str + 1);
            char c2 = *(str + 2);
            if (c2 == ']') {
                switch (c1) {
                    case 'A': str64[i++] = 84; str += 2; break;
                    case 'B': str64[i++] = 85; str += 2; break;
                    case 'C': str64[i++] = 86; str += 2; break;
                    case 'Z': str64[i++] = 87; str += 2; break;
                    case 'R': str64[i++] = 88; str += 2; break;
                }
            }
        } else {
            switch (c) {
                case ' ':  str64[i++] = 158; break;
                case '\'': str64[i++] = 62; break;
                case '\"': str64[i++] = 246; break;
                case '.':  str64[i++] = 63; break;
                case ':':  str64[i++] = 230; break;
                case ',':  str64[i++] = 111; break;
                case '-':  str64[i++] = 159; break;
                case '?':  str64[i++] = 244; break;
                case '!':  str64[i++] = 242; break;
                case '/':  str64[i++] = 208; break;
                case '^':  str64[i++] = 80; break;
                case '|':  str64[i++] = 81; break;
                case '<':  str64[i++] = 82; break;
                case '>':  str64[i++] = 83; break;
                case '*':  str64[i++] = 251; break;
                case '@':  str64[i++] = 249; break;
                case '+':  str64[i++] = 250; break;
                case '_':  str64[i++] = 253; break;
                default:   str64[i++] = 158; break;
            }
        }
    }
    str64[i] = 0xFF;
    return (const u8 *) str64;
}

static const u8 **dynos_alloc_choice_list(u32 count) {
    const u8 **choiceList = calloc(count, sizeof(u8 *));
    return choiceList;
}

// "Construct On First Use" aka COFU
static struct Array *dynos_get_action_list() {
    static struct Array sDynosActions = { NULL, 0 };
    return &sDynosActions;
}

static void (*dynos_get_action(const char *funcName))(struct Option *, s32) {
    struct Array dynosActions = *dynos_get_action_list();
    for (u32 i = 0; i != dynosActions.count; ++i) {
        if (strcmp(ARRAY_GET(dynosActions, i, struct DynosAction).str, funcName) == 0) {
            return ARRAY_GET(dynosActions, i, struct DynosAction).actionFn;
        }
    }
    return NULL;
}

//
// Data
//

static struct Array sDynosValues;
static struct SubMenu *sDynosMenu = NULL;

//
// Init
//

static struct DynosValue *dynos_create_slot(struct DynosValues *dynosValues) {
    struct DynosValue dvalue;
    ARRAY_ADD(dynosValues->values, dvalue);
    return array_get(&dynosValues->values, dynosValues->values.count - 1);
}

static struct SubMenu *dynos_read_file(const char *folder, const char *filename) {

    // Open file
    char fullname[SYS_MAX_PATH];
    snprintf(fullname, SYS_MAX_PATH, "%s/%s", folder, filename);
    FILE *f = fopen(fullname, "rt");
    if (f == NULL) {
        return NULL;
    }

    // Init structures
    struct DynosValues *dynosValues = NULL;
    struct Array optionList = sEmptyArray;
    struct SubMenu *subMenu = calloc(1, sizeof(struct SubMenu));
    subMenu->label = NULL;
    
    // Read file and create options
    char buffer[1024];
    while (fgets(buffer, 1024, f) != NULL) {
        struct StrTokens strtk = dynos_split_string(buffer);
        struct Option option;

        // Empty line or comment
        if (strtk.count == 0 || strtk.tokens[0][0] == '#') {
            continue;
        }

        // ID [Id] - Mandatory, must be placed in first
        if (strcmp(strtk.tokens[0], "ID") == 0 && strtk.count >= 2) {
            const char *id = dynos_alloc_string(strtk.tokens[1]);

            // Check if Id is not already registered
            for (u32 i = 0; i != sDynosValues.count; ++i) {
                if (strcmp(ARRAY_GET(sDynosValues, i, struct DynosValues).id, id) == 0) {
                    return NULL;
                }
            }
            struct DynosValues dv = { id, sEmptyArray };
            ARRAY_ADD(sDynosValues, dv);
            dynosValues = array_get(&sDynosValues, sDynosValues.count - 1);
            continue;
        }

        // TITLE [Name] - Mandatory
        else if (strcmp(strtk.tokens[0], "TITLE") == 0 && strtk.count >= 2) {
            subMenu->label = dynos_alloc_sm64_string(strtk.tokens[1]);
            continue;
        }

        // TOGGLE [Name] [InitialValue]
        else if (strcmp(strtk.tokens[0], "TOGGLE") == 0 && strtk.count >= 3) {
            if (dynosValues == NULL) {
                return NULL;
            }
            struct DynosValue *dvalue = dynos_create_slot(dynosValues);
            dvalue->type        = OPT_CHOICE;
            dvalue->uval        = dynos_string_to_int(strtk.tokens[2]);
            option.type         = OPT_CHOICE;
            option.label        = dynos_alloc_sm64_string(strtk.tokens[1]);
            option.uval         = &dvalue->uval;
            option.numChoices   = 2;
            option.choices      = dynos_alloc_choice_list(option.numChoices);
            option.choices[0]   = dynos_alloc_sm64_string("Disabled");
            option.choices[1]   = dynos_alloc_sm64_string("Enabled");
        }

        // SCROLL [Name] [InitialValue] [Min] [Max] [Step]
        else if (strcmp(strtk.tokens[0], "SCROLL") == 0 && strtk.count >= 6) {
            if (dynosValues == NULL) {
                return NULL;
            }
            struct DynosValue *dvalue = dynos_create_slot(dynosValues);
            dvalue->type        = OPT_SCROLL;
            dvalue->uval        = dynos_string_to_int(strtk.tokens[2]);
            option.type         = OPT_SCROLL;
            option.label        = dynos_alloc_sm64_string(strtk.tokens[1]);
            option.uval         = &dvalue->uval;
            option.scrMin       = dynos_string_to_int(strtk.tokens[3]);
            option.scrMax       = dynos_string_to_int(strtk.tokens[4]);
            option.scrStep      = dynos_string_to_int(strtk.tokens[5]);
        }

        // CHOICE [Name] [InitialValue] [ChoiceStrings...]
        else if (strcmp(strtk.tokens[0], "CHOICE") == 0 && strtk.count >= 4) {
            if (dynosValues == NULL) {
                return NULL;
            }
            struct DynosValue *dvalue = dynos_create_slot(dynosValues);
            dvalue->type        = OPT_CHOICE;
            dvalue->uval        = dynos_string_to_int(strtk.tokens[2]);
            option.type         = OPT_CHOICE;
            option.label        = dynos_alloc_sm64_string(strtk.tokens[1]);
            option.uval         = &dvalue->uval;
            option.numChoices   = strtk.count - 3;
            option.choices      = dynos_alloc_choice_list(option.numChoices);
            for  (u32 i = 0; i != option.numChoices; ++i) {
            option.choices[i]   = dynos_alloc_sm64_string(strtk.tokens[3 + i]);
            }
        }

        // BUTTON [Name] [FuncName]
        else if (strcmp(strtk.tokens[0], "BUTTON") == 0 && strtk.count >= 3) {
            if (dynosValues == NULL) {
                return NULL;
            }
            struct DynosValue *dvalue = dynos_create_slot(dynosValues);
            dvalue->type        = OPT_BUTTON;
            option.type         = OPT_BUTTON;
            option.label        = dynos_alloc_sm64_string(strtk.tokens[1]);
            option.actionFn     = dynos_get_action(strtk.tokens[2]);
        }

        // BIND [Name] [Key] [Mask] [DefaultValues]
        else if (strcmp(strtk.tokens[0], "BIND") == 0 && strtk.count >= 4 + MAX_BINDS) {
            if (dynosValues == NULL) {
                return NULL;
            }
            struct DynosValue *dvalue = dynos_create_slot(dynosValues);
            dvalue->type        = OPT_BIND;
            dvalue->bind.key    = dynos_alloc_string(strtk.tokens[2]);
            dvalue->bind.mask   = dynos_string_to_int(strtk.tokens[3]);
            for  (u32 i = 0; i != MAX_BINDS; ++i) {
            dvalue->bind.binds[i] = dynos_string_to_int(strtk.tokens[4 + i]);
            }
            option.type         = OPT_BIND;
            option.label        = dynos_alloc_sm64_string(strtk.tokens[1]);
            option.uval         = dvalue->bind.binds;
        }

        // Unknown command
        else {
            continue;
        }

        // Add option to optionList
        ARRAY_ADD(optionList, option);
    }
    fclose(f);

    // Check subMenu
    if (subMenu->label == NULL || optionList.count == 0) {
        return NULL;
    }

    // Add options to subMenu
    struct Option *options = calloc(optionList.count, sizeof(struct Option));
    for (u32 i = 0; i != optionList.count; ++i) {
        options[i] = ARRAY_GET(optionList, i, struct Option);
    }
    subMenu->opts = options;
    subMenu->numOpts = optionList.count;
    return subMenu;
}

void dynos_init() {
    sDynosValues = sEmptyArray;
    char optionsFolder[SYS_MAX_PATH];
    snprintf(optionsFolder, SYS_MAX_PATH, "%s/%s", sys_exe_path(), FS_BASEDIR);
    DIR *dir = opendir(optionsFolder);
    struct Array optionList = sEmptyArray;

    // Look for every .txt files inside the "res" folder
    if (dir) {
        struct dirent *ent = NULL;
        while ((ent = readdir(dir)) != NULL) {
            if (dynos_is_txt_file(ent->d_name, strlen(ent->d_name))) {
                struct SubMenu *subMenu = dynos_read_file(optionsFolder, ent->d_name);
                if (subMenu != NULL) {
                    struct Option option;
                    option.type = OPT_SUBMENU;
                    option.label = subMenu->label;
                    option.nextMenu = subMenu;
                    ARRAY_ADD(optionList, option);
                }
            }
        }
        closedir(dir);
    }

    // Create DynOS Menu
    if (optionList.count != 0) {
        sDynosMenu = calloc(1, sizeof(struct SubMenu));
        sDynosMenu->label = dynos_alloc_sm64_string("DYNOS MENU");
        sDynosMenu->numOpts = optionList.count;
        sDynosMenu->opts = calloc(optionList.count, sizeof(struct Option));
        for (u32 i = 0; i != optionList.count; ++i) {
            sDynosMenu->opts[i] = ARRAY_GET(optionList, i, struct Option);
        }
    }
}

void dynos_add_button_action(const char *funcName, void (*funcPtr)(struct Option *, s32)) {
    struct Array *dynosActions = dynos_get_action_list();
    for (u32 i = 0; i != dynosActions->count; ++i) {
        if (strcmp(ARRAY_GET(*dynosActions, i, struct DynosAction).str, funcName) == 0) {
            return;
        }
    }
    struct DynosAction dynosAction = { dynos_alloc_string(funcName), funcPtr };
    ARRAY_ADD(*dynosActions, dynosAction);
}

void dynos_add_binds(void (*func)(u32, u32 *)) {
    for (u32 i = 0; i != sDynosValues.count; ++i) {
        struct DynosValues *dynosValues = array_get(&sDynosValues, i);
        for (u32 j = 0; j != dynosValues->values.count; ++j) {
            struct DynosValue *dynosValue = array_get(&dynosValues->values, j);
            if (dynosValue->type == OPT_BIND) {
                (*func)(dynosValue->bind.mask, dynosValue->bind.binds);
            }
        }
    }
}

void dynos_load_bind(const char *key, const char **values) {
    for (u32 i = 0; i != sDynosValues.count; ++i) {
        struct DynosValues *dynosValues = array_get(&sDynosValues, i);
        for (u32 j = 0; j != dynosValues->values.count; ++j) {
            struct DynosValue *dynosValue = array_get(&dynosValues->values, j);
            if (dynosValue->type == OPT_BIND && strcmp(dynosValue->bind.key, key) == 0) {
                for (u32 k = 0; k != MAX_BINDS; ++k) {
                    sscanf(values[k], "%x", &dynosValue->bind.binds[k]);
                }
            }
        }
    }
}

void dynos_save_binds(FILE *f) {
    for (u32 i = 0; i != sDynosValues.count; ++i) {
        struct DynosValues *dynosValues = array_get(&sDynosValues, i);
        for (u32 j = 0; j != dynosValues->values.count; ++j) {
            struct DynosValue *dynosValue = array_get(&dynosValues->values, j);
            if (dynosValue->type == OPT_BIND) {
                fprintf(f, "%s ", dynosValue->bind.key);
                for (u32 k = 0; k != MAX_BINDS; ++k) {
                    fprintf(f, "%04x ", dynosValue->bind.binds[k]);
                }
                fprintf(f, "\n");
            }
        }
    }
}

//
// Get/Set values
//

u32 dynos_get_value(const char *id, u8 slot) {
    for (u32 i = 0; i != sDynosValues.count; ++i) {
        struct DynosValues *dynosValues = array_get(&sDynosValues, i);
        if (strcmp(dynosValues->id, id) == 0 && slot < dynosValues->values.count) {
            struct DynosValue *dynosValue = array_get(&dynosValues->values, slot);
            if (dynosValue->type == OPT_TOGGLE || dynosValue->type == OPT_SCROLL || dynosValue->type == OPT_CHOICE) {
                return dynosValue->uval;
            }
            return 0;
        }
    }
    return 0;
}

void dynos_set_value(const char *id, u8 slot, u32 value) {
    for (u32 i = 0; i != sDynosValues.count; ++i) {
        struct DynosValues *dynosValues = array_get(&sDynosValues, i);
        if (strcmp(dynosValues->id, id) == 0 && slot < dynosValues->values.count) {
            struct DynosValue *dynosValue = array_get(&dynosValues->values, slot);
            if (dynosValue->type == OPT_TOGGLE || dynosValue->type == OPT_SCROLL || dynosValue->type == OPT_CHOICE) {
                dynosValue->uval = value;
            }
            return;
        }
    }
}

//
// Option Menu
//

static const u8 *sDynosOpenText = NULL;
static const u8 *sDynosCloseText = NULL;

static void optmenu_draw_text(s16 x, s16 y, const u8 *str, u8 col);

void optmenu_draw_prompt_dynos() {
    if (sDynosMenu != NULL) {
        if (!optmenu_open) {
            if (sDynosOpenText == NULL) {
                sDynosOpenText = dynos_alloc_sm64_string("[Z] DynOS");
            }
            gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
            optmenu_draw_text(56, 212, sDynosOpenText, 0);
            gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
        } else {
            if (sDynosCloseText == NULL) {
                sDynosCloseText = dynos_alloc_sm64_string("[Z] Return");
            }
            gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
            optmenu_draw_text(56, 212, sDynosCloseText, 0);
            gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
        }
    }
}

void optmenu_toggle_dynos() {
    if (sDynosMenu != NULL) {
        if (!optmenu_open) {
#ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
#endif
            currentMenu = sDynosMenu;
            optmenu_open = 1;
        } else {
            optmenu_toggle();
        }
    }
}