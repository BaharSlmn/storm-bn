# Create storm-bn.
add_executable(storm-bn-cli ${PROJECT_SOURCE_DIR}/src/storm-bn-cli/storm-bn.cpp)
target_link_libraries(storm-bn-cli storm-bn storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-bn-cli PROPERTIES OUTPUT_NAME "storm-bn")

add_dependencies(binaries storm-bn-cli)

# installation
install(TARGETS storm-bn-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
