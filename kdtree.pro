TEMPLATE	 = app

TARGET		 = kdTree

MOC_DIR		 = mocs
OBJECTS_DIR	 = objs

QT		+= opengl

CONFIG		+= warn_on release

INCLUDEPATH	+= .

HEADERS		+= glwinobj.h gltexobj.h
SOURCES		+= glwinobj.cpp gltexobj.cpp main.cpp
LIBS += -lglut -lGLU
