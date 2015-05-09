######################################################################
# Automatically generated by qmake (2.01a) Sa. Mai 10 07:29:55 2014
######################################################################

TEMPLATE = subdirs

# Directories

INCLUDEPATH += src/include/ \
	rtosc/include

# Input

HEADERS += src/include/plugin.h \
	src/include/project.h \
	src/include/instrument.h \
	src/core/loaded_project.h \
	src/include/utils.h \
	src/include/tuple_helpers.h \
	src/include/daw.h \
	src/include/util.h \
	src/include/lfo.h \
	src/include/types.h \
	src/include/zynaddsubfx.h \
	src/include/osc_string.h \
	src/include/lo_port.h \
	src/include/command.h \
	src/include/effect.h \
	src/include/work_queue.h \
	src/include/daw_visit.h \
	src/include/note_line.h \
	src/include/ports.h \
	src/include/recorder.h \
	src/include/soundfile.h \
	src/include/jack.h \
	src/include/audio.h \
	src/include/audio_instrument.h \
	src/include/pid.h \
	src/include/io.h \
	src/include/jack_player.h \
	src/include/sample.h \
	src/core/engine.h \
	src/core/jack_engine.h \
	src/include/fraction.h \
	src/include/debug.h

SOURCES += src/core/main.cpp \
	src/include/plugin.cpp \
	src/include/project.cpp \
	src/songs/demo.cpp \
	src/include/instrument.cpp \
	src/core/loaded_project.cpp \
	src/core/lfo.cpp \
	src/test/test_str.cpp \
	src/include/zynaddsubfx.cpp \
	src/include/osc_string.cpp \
	src/include/lo_port.cpp \
	src/include/daw_visit.cpp \
	src/include/note_line.cpp \
	src/include/command.cpp \
	src/include/recorder.cpp \
	src/include/soundfile.cpp \
	src/include/jack.cpp \
	src/include/audio_instrument.cpp \
	src/include/pid.cpp \
	src/include/io.cpp \
	src/include/effect.cpp \
	src/include/jack_player.cpp \
	src/test/commands.cpp \
	src/test/maths.cpp \
	src/core/engine.cpp \
	src/core/jack_engine.cpp \
	src/include/fraction.cpp \
	src/songs/debug_proj.cpp

OTHER_FILES += INSTALL.md \
	CMakeLists.txt \
	src/CMakeLists.txt \
	src/core/CMakeLists.txt \
	src/songs/CMakeLists.txt \
	src/include/CMakeLists.txt \
	src/test/CMakeLists.txt \
	src/config.h.in \
	README.md \
	RULES.md


