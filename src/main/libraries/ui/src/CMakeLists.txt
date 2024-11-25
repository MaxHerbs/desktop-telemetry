SET(SOURCES screens/ui_commuteScreen.c
    screens/ui_weatherScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c)

add_library(ui ${SOURCES})
