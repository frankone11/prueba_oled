#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := prueba-oled

ifndef IDF_PATH
IDF_PATH := /home/paco/esp/ESP8266_RTOS_SDK
endif

include $(IDF_PATH)/make/project.mk

