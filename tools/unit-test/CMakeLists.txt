Include(FetchContent)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG		   v3.8.1
)
FetchContent_MakeAvailable(Catch2)

#testing bin
add_executable( tile_engine_test
	${CMAKE_CURRENT_SOURCE_DIR}/engine_test.cpp
)

target_link_libraries( tile_engine_test PRIVATE
	tile_engine
	Catch2::Catch2WithMain
)

add_test(
	NAME unit_tester
	COMMAND $<TARGET_FILE:tile_engine_test> --success
)
