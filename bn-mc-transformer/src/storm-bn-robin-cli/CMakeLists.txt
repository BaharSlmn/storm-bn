# Create storm-bn-robin
add_executable(storm-bn-robin-cli ${PROJECT_SOURCE_DIR}/src/storm-bn-robin-cli/storm-bn-robin.cpp)
target_link_libraries(storm-bn-robin-cli storm-bn-robin storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-bn-robin-cli PROPERTIES OUTPUT_NAME "storm-bn-robin")

add_dependencies(binaries storm-bn-robin-cli)

# installation
install(TARGETS storm-bn-robin-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
