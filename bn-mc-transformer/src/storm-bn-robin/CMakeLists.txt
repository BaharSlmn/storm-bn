file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-bn-robin/*.h ${PROJECT_SOURCE_DIR}/src/storm-bn-robin/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-bn-robin)



file(GLOB_RECURSE STORM_BN_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-bn-robin/*/*/*.cpp)
file(GLOB_RECURSE STORM_BN_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-bn-robin/*/*/*.h)


# Create storm-bn-robin.
add_library(storm-bn-robin SHARED ${STORM_BN_ROBIN_SOURCES} ${STORM_BN_ROBIN_HEADERS} src/transformer/BayesianNetworkTransformer.h src/transformer/BayesianNetworkTransformer.cpp src/transformer/Graph.cpp src/transformer/Graph.h src/transformer/TopologicalOrderingFinder.h src/transformer/TopologicalOrderingFinder.cpp src/transformer/TopologicalOrderingReader.h src/transformer/TopologicalOrderingReader.cpp
        src/parser/BIFFORMAT.h src/parser/BNNetwork.h src/parser/BNNetwork.cpp src/parser/BNNode.h src/parser/BNNode.cpp src/parser/Evidence.h src/parser/Evidence.cpp src/parser/Hypothesis.h src/parser/Hypothesis.cpp src/parser/Parameter.h src/parser/Parameter.cpp src/parser/ProbabilityRow.h src/parser/ProbabilityRow.cpp src/parser/ProbabilityTable.h src/parser/ProbabilityTable.cpp src/parser/Utils.h src/parser/Utils.cpp src/jani/literals.h src/jani/JaniFileCreator.h src/jani/JaniFileCreator.cpp
        src/QueriesCreator.h src/QueriesCreator.cpp src/VariablesFileCreator.h src/VariablesFileCreator.cpp config.h )

# Remove define symbol for shared libstorm.
set_target_properties(storm-bn-robin PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-bn-robin)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-bn-robin PUBLIC storm storm-parsers ${STORM_BN_ROBIN_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_BN_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            DEPENDS ${HEADER}
    )
    list(APPEND STORM_BN_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_bn_robin_headers DEPENDS ${STORM_BN_ROBIN_OUTPUT_HEADERS} ${STORM_BN_ROBIN_HEADERS})
add_dependencies(storm-bn-robin copy_storm_bn_headers)

# installation
install(TARGETS storm-bn-robin EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

