cmake_minimum_required(VERSION 3.27)
project(DD_optimization_program)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.15.2
)

find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

FetchContent_MakeAvailable(googletest)

# ── Gurobi detection ──────────────────────────────────────────────────────────

# 1. Prioridad: variable de entorno
if(NOT DEFINED GUROBI_HOME AND DEFINED ENV{GUROBI_HOME})
    set(GUROBI_HOME $ENV{GUROBI_HOME})
    message(STATUS "Gurobi from environment: ${GUROBI_HOME}")
endif()

# 2. Auto-detect si aún no está definido
if(NOT DEFINED GUROBI_HOME)

    file(GLOB GUROBI_CANDIDATES
        "/Library/gurobi*/macos_universal2"
        "/Library/gurobi*/mac64"
        "/opt/gurobi*/linux64"
        "$ENV{HOME}/gurobi*/linux64"
        "$ENV{HOME}/gurobi/linux64"
    )

    if(GUROBI_CANDIDATES)
        list(SORT GUROBI_CANDIDATES ORDER DESCENDING)
        list(GET GUROBI_CANDIDATES 0 GUROBI_HOME)
        message(STATUS "Gurobi auto-detected at: ${GUROBI_HOME}")
    else()
        message(FATAL_ERROR
            "Gurobi not found.\n"
            "Set GUROBI_HOME or install Gurobi in a standard path."
        )
    endif()

else()
    message(STATUS "Gurobi home (user-provided): ${GUROBI_HOME}")
endif()

# 3. Validación (MUY importante)
if(NOT EXISTS "${GUROBI_HOME}/include/gurobi_c++.h")
    message(FATAL_ERROR "Invalid GUROBI_HOME: ${GUROBI_HOME}")
endif()

set(GUROBI_INCLUDE_DIRS "${GUROBI_HOME}/include")
set(GUROBI_LIB_DIRS    "${GUROBI_HOME}/lib")

include_directories(${GUROBI_INCLUDE_DIRS})
link_directories(${GUROBI_LIB_DIRS})

# Derive the versioned library name from the installation folder name.
# /Library/gurobi1102/... → folder token "gurobi1102" → lib name "gurobi110"
# /Library/gurobi1302/... → folder token "gurobi1302" → lib name "gurobi130"
get_filename_component(_GUROBI_PARENT "${GUROBI_HOME}" DIRECTORY)
get_filename_component(_GUROBI_FOLDER "${_GUROBI_PARENT}" NAME)  # e.g. "gurobi1102"
string(REGEX MATCH "gurobi([0-9]+)" _MATCH "${_GUROBI_FOLDER}")
if(_MATCH)
    set(_GUROBI_VER "${CMAKE_MATCH_1}")          # e.g. "1102"
    string(LENGTH "${_GUROBI_VER}" _VER_LEN)
    if(_VER_LEN GREATER 3)
        # Drop the patch digit: 1102 → 110, 1300 → 130
        string(SUBSTRING "${_GUROBI_VER}" 0 3 GUROBI_LIB_VERSION)
    else()
        set(GUROBI_LIB_VERSION "${_GUROBI_VER}")
    endif()
else()
    set(GUROBI_LIB_VERSION "130")  # safe fallback
endif()
message(STATUS "Gurobi link library: gurobi${GUROBI_LIB_VERSION}")
# e.g. gurobi110  or  gurobi130


set(EXCEPTIONS_SOURCE
        Exceptions/MyExceptions.h
        Exceptions/MyExceptions.cpp)

set(PROBLEMS_SOURCE
        Examples/KnapsackInstance/KnapsackProblem.h
        Examples/KnapsackInstance/KnapsackProblem.cpp
        Examples/IndependentSetInstance/IndependentSetProblem.cpp
        Examples/IndependentSetInstance/IndependentSetProblem.h
        Examples/SetCoverInstance/SetCoverProblem.cpp
        Examples/SetCoverInstance/SetCoverProblem.h
        Examples/KnapsackInstance/KnapsackProblemState.h
        Examples/KnapsackInstance/KnapsackProblemState.cpp
        Examples/SetCoverInstance/SetCoverInstance.h
        Examples/SetCoverInstance/SetCoverInstance.cpp
        Examples/IndependentSetInstance/IndependentSetInstance.h
        Examples/IndependentSetInstance/IndependentSetInstance.cpp
        Examples/KnapsackInstance/KnapsackInstance.h
        Examples/KnapsackInstance/KnapsackInstance.cpp
        Examples/SOCKnapsackInstance/SOCKnapsackInstance.h
        Examples/SOCKnapsackInstance/SOCKnapsackInstance.cpp
        Examples/SOCKnapsackInstance/SOCKnapsackProblem.h
        Examples/SOCKnapsackInstance/SOCKnapsackProblem.cpp
        Examples/SchedulerInstance/SchedulerState.h
        Examples/SchedulerInstance/SchedulerInstance.h
        Examples/SchedulerInstance/SchedulerInstance.cpp
        Examples/SchedulerInstance/SchedulerProblem.h
        Examples/SchedulerInstance/SchedulerProblem.cpp
        Examples/SchedulerInstance/SchedulerPathSolver.h
        Examples/SchedulerInstance/SchedulerPathSolver.cpp
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.h
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.cpp)

set(GUROBI_PROBLEMS_SOURCE
        Examples/KnapsackInstance/KnapsackGurobiClass.h
        Examples/KnapsackInstance/KnapsackGurobiClass.cpp
        Examples/IndependentSetInstance/IndependentSetGurobiClass.h
        Examples/IndependentSetInstance/IndependentSetGurobiClass.cpp
        Examples/SetCoverInstance/SetCoverGurobiClass.h
        Examples/SetCoverInstance/SetCoverGurobiClass.cpp
        Examples/SOCKnapsackInstance/SOCKnapsackGurobiClass.h
        Examples/SOCKnapsackInstance/SOCKnapsackGurobiClass.cpp
        Examples/SchedulerInstance/SchedulerGurobiClass.h
        Examples/SchedulerInstance/SchedulerGurobiClass.cpp)

add_library(Exceptions ${EXCEPTIONS_SOURCE})
add_library(Problems ${PROBLEMS_SOURCE})
add_library(GurobiProblems ${GUROBI_PROBLEMS_SOURCE})
add_library(SourceCode INTERFACE)

target_include_directories(SourceCode
        INTERFACE
        SourceCode
        SourceCode/AnswerStructure
        SourceCode/DDBuilder
        SourceCode/DDStructure
        SourceCode/DDCutGenerators
        SourceCode/GraphVisualization
        SourceCode/ObjectiveFunction
        SourceCode/Problems
        SourceCode/DDBuilder/RestrictedDDBuilder
        SourceCode/DDBuilder/RelaxedDDBuilder
        SourceCode/GraphAlgorithms/DDReducer
)
target_include_directories(Exceptions PUBLIC Exceptions)
target_include_directories(Problems PUBLIC Problems)
target_include_directories(GurobiProblems PUBLIC GurobiProblems)
target_link_libraries(GurobiProblems PRIVATE SourceCode Exceptions)

add_executable(Main main.cpp
        Examples/GuiHandler.h
        Examples/GuiHandler.cpp
        Examples/GuiStructure.h
        Examples/GuiStructure.cpp)

add_executable(MainCuts main_cuts.cpp
        Examples/GuiHandler.h
        Examples/GuiHandler.cpp
        Examples/GuiStructure.h
        Examples/GuiStructure.cpp
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.h
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.cpp)

add_executable(MainGurobi main_gurobi.cpp
        Examples/GuiHandler.h
        Examples/GuiHandler.cpp
        Examples/GuiStructure.h
        Examples/GuiStructure.cpp
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.h
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.cpp)

add_executable(Main_knapsack main_knapsack.cpp
        Examples/GuiHandler.h
        Examples/GuiHandler.cpp
        Examples/GuiStructure.h
        Examples/GuiStructure.cpp
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.h
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.cpp)

add_executable(Main_socknapsack main_socknapsack.cpp
        Examples/GuiHandler.h
        Examples/GuiHandler.cpp
        Examples/GuiStructure.h
        Examples/GuiStructure.cpp
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.h
        Examples/AbstractGurobiProblems/AbstractProblemGurobiClass.cpp)

target_link_libraries(Problems PRIVATE SourceCode Exceptions)
target_link_libraries(Main PRIVATE
        SourceCode
        Exceptions
        Problems
        GurobiProblems
        gurobi_c++
        gurobi${GUROBI_LIB_VERSION})
target_link_libraries(MainCuts PRIVATE
        SourceCode
        Exceptions
        Problems
        GurobiProblems
        gurobi_c++
        gurobi${GUROBI_LIB_VERSION})
target_link_libraries(MainGurobi PRIVATE
        SourceCode
        Exceptions
        Problems
        GurobiProblems
        gurobi_c++
        gurobi${GUROBI_LIB_VERSION})
target_link_libraries(Main_knapsack PRIVATE
        SourceCode
        Exceptions
        Problems
        GurobiProblems
        gurobi_c++
        gurobi${GUROBI_LIB_VERSION})

target_link_libraries(Main_socknapsack PRIVATE
        SourceCode
        Exceptions
        Problems
        GurobiProblems
        gurobi_c++
        gurobi${GUROBI_LIB_VERSION})


add_executable(DD_optimization_program_tests
        Test/test_KnapsackProblem.cpp
        Test/test_IndependentSetProblem.cpp
        Test/test_SetCoverProblem.cpp
        Test/test_KnapsackStateProblem.cpp
        Test/test_Cuts.cpp
        Test/test_TargetCut.cpp
        Test/test_GraphEqualOperation.cpp
        Test/test_IndependentSetProblemSort.cpp
        Test/test_SetCoverProblemSort.cpp
        Test/test_SchedulerProblem.cpp
)

target_link_libraries(DD_optimization_program_tests PRIVATE
        SourceCode
        Exceptions
        gtest_main
        Problems
        GurobiProblems
        gurobi_c++
        gurobi${GUROBI_LIB_VERSION})

include(GoogleTest)
gtest_discover_tests(DD_optimization_program_tests)