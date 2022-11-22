################################################################
#   CMake file for using KSpecStream as submodule              #
# 															   #
################################################################

list(APPEND SRC_Qt_MOC
	${CMAKE_CURRENT_LIST_DIR}/KSpecStream.h
)

list(APPEND SRC_Qt
	${CMAKE_CURRENT_LIST_DIR}/KSpecStream.cpp
)

list(APPEND INCL
	${CMAKE_CURRENT_LIST_DIR}
)
