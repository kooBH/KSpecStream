################################################################
#   CMake file for using KSpecStream as submodule              #
# 															   #
################################################################

list(APPEND SRC_Qt_MOC
	${CMAKE_CURRENT_LIST_DIR}/KSpecStream.h
	${CMAKE_CURRENT_LIST_DIR}/KWavStream.h
)

list(APPEND SRC_Qt
	${CMAKE_CURRENT_LIST_DIR}/KSpecStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/KWavStream.cpp
)

list(APPEND INCL
	${CMAKE_CURRENT_LIST_DIR}
)
