add_executable(lirs_vs_ideal_benchmark run_experiment.cc)

target_include_directories(lirs_vs_ideal_benchmark PRIVATE
    ${CMAKE_SOURCE_DIR}/LIRS
    ${CMAKE_SOURCE_DIR}/ideal
)

target_link_libraries(lirs_vs_ideal_benchmark PRIVATE)
