idf_component_register(
    SRCS
        "config.c"
        "main.c"
        "on_entry_state_transitions.c"
        "on_exit_state_transitions.c"
        "state_buttons.c"
        "state_tasks.c"
        "statemachine.c"
        "wrapper_tm1637.cpp"
        "TM1637Display.cpp"
        # "wrapper_7seg.c"
    INCLUDE_DIRS
        "."
        "include"
)
