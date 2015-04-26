CC = c++
LIBS = -lboost_system -lfolly
LLIB = -L/usr/local/lib -rdynamic -Wl,-rpath,/usr/local/lib
CFLAGS  = -Wall -g -O2 -std=c++11 -Wno-mismatched-tags  -L/usr/local/lib -rdynamic -Wl,-rpath,/usr/local/lib

# the build target executable:
TARGET = async_client

all: $(TARGET)

$(TARGET): $(TARGET).cc
	$(CC) $(CFLAGS) -o $(TARGET).o $(TARGET).cc  $(LLIBS) $(LIBS)
#$(CC) -Wall -g -O2 -std=c++11 -Wno-mismatched-tags  -L/usr/local/lib -rdynamic $(LIBS) -Wl,-rpath,/usr/local/lib


clean:
	$(RM) $(TARGET)

