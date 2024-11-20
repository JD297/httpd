PREFIX		= /usr/local
BINDIR		= $(PREFIX)/bin

TARGET		= httpd
TARGETDIR	= bin
SRCDIR		= src
SRC_FILES	= $(wildcard $(SRCDIR)/*.c)

CC		= gcc
CCFLAGS		= -Wall -Wextra -Wpedantic
#CCFLAGSDEBUG	= -g
#CCLIBSSTATIC	= -static

$(TARGET): $(SRC_FILES)
	$(CC) $(CCFLAGS) $(CCFLAGSDEBUG) $(CCLIBSSTATIC) $(SRC_FILES) -DTARGET=\"$(TARGET)\" -o $(TARGETDIR)/$(TARGET)

clean:
	rm -f $(TARGETDIR)/$(TARGET)

install: $(TARGET)
	cp $(TARGETDIR)/$(TARGET) $(BINDIR)

uninstall:
	rm -f $(BINDIR)/$(TARGET)
