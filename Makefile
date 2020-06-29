TARGET = wav2mp3.exe
SRCSDIR = ./src
SRCS = ./main.cpp \
	./$(SRCSDIR)/argsparser/ArgsParser.cpp \
	./$(SRCSDIR)/filemgr/FileManager.cpp \
	./$(SRCSDIR)/encodemgr/EncodeManager.cpp \
	./$(SRCSDIR)/file/File.cpp \
	./$(SRCSDIR)/file/FileReader.cpp \
	./$(SRCSDIR)/file/FileWriter.cpp \
	./$(SRCSDIR)/file/WavFile.cpp \
	./$(SRCSDIR)/file/Mp3File.cpp \
	./$(SRCSDIR)/logger/ConsoleLogger.cpp \
	./$(SRCSDIR)/logger/Log.cpp \
	./$(SRCSDIR)/lamelib/LameLib.cpp \
	./$(SRCSDIR)/threadmgr/ThreadMgr.cpp \

INCLUDES = -I./ \
	   -I./include \
	   -I./$(SRCSDIR)/argsparser \
	   -I./$(SRCSDIR)/encodemgr \
	   -I./$(SRCSDIR)/filemgr \
	   -I./$(SRCSDIR)/file \
	   -I./$(SRCSDIR)/logger \
	   -I./$(SRCSDIR)/lamelib \
	   -I./$(SRCSDIR)/threadmgr \
	   -I./$(SRCSDIR)/stream \
	   -I./$(SRCSDIR)/utils \

LIBDIR = ./lib \

LIBS = ./externallibs/libmp3lame.a -lpthread
OBJS = main.o \
	./$(SRCSDIR)/argsparser/ArgsParser.o \
	./$(SRCSDIR)/filemgr/FileManager.o \
	./$(SRCSDIR)/encodemgr/EncodeManager.o \
	./$(SRCSDIR)/file/File.o \
	./$(SRCSDIR)/file/FileWriter.o \
	./$(SRCSDIR)/file/FileReader.o \
	./$(SRCSDIR)/file/Mp3File.o \
	./$(SRCSDIR)/file/WavFile.o \
	./$(SRCSDIR)/logger/ConsoleLogger.o \
	./$(SRCSDIR)/logger/Log.o \
	./$(SRCSDIR)/lamelib/LameLib.o \
	./$(SRCSDIR)/threadmgr/ThreadManager.o \

OBJDIR = objs

all: $(OBJDIR) $(OBJS)
	mv $(OBJS) $(OBJDIR)/ 
	g++ $(OBJDIR)/*.o -o $(TARGET) $(LIBS)   

objs:
	mkdir $(OBJDIR) 


%.o:%.cpp
	g++ -c $(INCLUDES) -o $@ $<

clean:
	rm -fr $(OBJS) $(TARGET) $(OBJDIR)
