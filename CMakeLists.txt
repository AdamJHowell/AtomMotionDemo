cmake_minimum_required( VERSION 3.23 )
project( AtomMotionDemo )

set( CMAKE_CXX_STANDARD 14 )

include_directories( .
		C:/Users/Adam/AppData/Local/Arduino15/packages/arduino/hardware/avr/1.8.5/cores/arduino/
		C:/Code/Arduino/libraries/
		C:/Code/Arduino/libraries/Servo/src/
		"C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/"
		"C:/Program Files (x86)/Arduino/hardware/tools/avr/lib/gcc/avr/7.3.0/include/"
)

add_executable( AtomMotionDemo
		AtomMotionDemo.h
		AtomMotionDemo.ino
		AtomMotion.h
		AtomMotion.cpp
)

set_target_properties( AtomMotionDemo PROPERTIES LINKER_LANGUAGE CXX )
