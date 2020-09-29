// ddd_sub.c.inc

void bhv_bowsers_sub_loop(void) {
    if (gCurrActNum > 1)
        obj_mark_for_deletion(o);
}
