cmake_minimum_required(VERSION 2.8)

project(thequick_light)

###############
### Sources ###
###############

set(SOURCES 
        iniparser_light.cpp
        stringtools_light.cpp)

#################################
### Adding final build target ###
#################################

add_library(${PROJECT_NAME} SHARED ${SOURCES})

############################################
### Adding library into the testing list ###
############################################

set(TESTED_LIBRARIES ${TESTED_LIBRARIES} ${PROJECT_NAME} CACHE STRING "" FORCE)

########################################
### Adding headers into include list ###
########################################

#set(INCLUDE_HEADER_FILES ${INCLUDE_HEADER_FILES}
#        ${CMAKE_CURRENT_SOURCE_DIR}/iniparser_light.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/stringtools_light.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/libtools_light.h
# CACHE STRING "" FORCE)
