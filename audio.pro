QT += charts multimedia

HEADERS += \
    audioPlayer.h \
    audioRecorder.h \
    chrip.h \
    sonar.h \
    widget.h

SOURCES += \
    audioPlayer.cpp \
    audioRecorder.cpp \
    chrip.cpp \
    main.cpp\
    sonar.cpp \
    widget.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/charts/audio
INSTALLS += target

DISTFILES += \
    Data/Chirp.csv

# using shell_path() to correct path depending on platform
# escaping quotes and backslashes for file paths
copydata.commands = $(COPY_DIR) \"$$shell_path($$PWD\\Data)\" \"$$shell_path($$OUT_PWD)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
