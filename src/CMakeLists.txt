set(UTIL_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/utils.cc"
        "${FUNNEL_INC_DIR}/funnels/utils.hh"
        PARENT_SCOPE)

set(TRANSITION_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/transitions.cc"
        "${FUNNEL_INC_DIR}/funnels/transitions.hh"
        PARENT_SCOPE)

set(FUNNEL_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/lyapunov.cc"
        "${FUNNEL_INC_DIR}/funnels/lyapunov.hh"
        "${CMAKE_CURRENT_SOURCE_DIR}/funnels.cc"
        "${FUNNEL_INC_DIR}/funnels/funnels.hh"
        "${FUNNEL_INC_DIR}/funnels/funnel_computations.hh"
        "${FUNNEL_INC_DIR}/funnels/distances.hh"
        "${FUNNEL_INC_DIR}/funnels/dynamics.hh"
        PARENT_SCOPE)