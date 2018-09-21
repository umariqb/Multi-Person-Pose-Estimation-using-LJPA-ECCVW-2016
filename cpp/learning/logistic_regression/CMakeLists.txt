#This is a re-implementation of the LR algorithm idea in C++.

# You are free to use, change, or redistribute the code in any way you wish for
# non-commercial purposes, but please maintain the name of the original author.
# This code comes with no warranty of any kind.

# LR ALGORITHM

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(LR)


############################## Eigen ##############################
# Locations to search for Eigen
SET(EIGEN_SEARCH_HEADERS ${SEARCH_HEADERS}
						/usr/include/eigen3)
LIST(APPEND EIGEN_SEARCH_HEADERS /usr/include/eigen3) 
# Ubuntu default location.
LIST(APPEND EIGEN_SEARCH_HEADERS /usr/local/include/eigen3) 

# Eigen installed
MESSAGE("-- Check for Eigen 3.0")
FIND_PATH(EIGEN_INCLUDE NAMES Eigen/Core PATHS ${EIGEN_SEARCH_HEADERS})
IF (NOT EXISTS ${EIGEN_INCLUDE})
  MESSAGE(FATAL_ERROR "Can't find Eigen. Try passing -DEIGEN_INCLUDE=...")
ENDIF (NOT EXISTS ${EIGEN_INCLUDE})
MESSAGE("-- Found Eigen 3.0: ${EIGEN_INCLUDE}")

############ OpenCV is installed ###################
#SET(OpenCV_DIR /path/to/your/opencv/installation) # probably needs to be set
FIND_PACKAGE(OpenCV REQUIRED)

############ NOW LR ##################

INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/include
					${EIGEN_INCLUDE})
ADD_EXECUTABLE(LR src/Main.cpp src/LogisticRegression.cpp)
TARGET_LINK_LIBRARIES(LR ${OpenCV_LIBS})
