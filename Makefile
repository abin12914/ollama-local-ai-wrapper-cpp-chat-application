CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra $(shell pkg-config --cflags gtk+-3.0)
LDLIBS := $(shell pkg-config --libs gtk+-3.0) -lcurl

TARGET := ai-chat-local
SOURCES := main.cpp app_state.cpp AI-Chat-Local.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all clean run install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDLIBS)

%.o: %.cpp app_state.h app.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

install: $(TARGET)
	mkdir -p $(HOME)/.local/bin
	mkdir -p $(HOME)/.local/share/applications
	mkdir -p $(HOME)/.local/share/icons/hicolor/256x256/apps
	cp $(TARGET) $(HOME)/.local/bin/$(TARGET)
	cp app-icon.png $(HOME)/.local/bin/app-icon.png
	cp app-icon.png $(HOME)/.local/share/icons/hicolor/256x256/apps/$(TARGET).png
	printf '[Desktop Entry]\nName=AI Chat Local\nComment=Local AI chat client\nExec=%s/.local/bin/%s\nIcon=%s\nTerminal=false\nType=Application\nCategories=Utility;\n' "$(HOME)" "$(TARGET)" "$(TARGET)" > $(HOME)/.local/share/applications/$(TARGET).desktop

clean:
	rm -f $(OBJECTS) $(TARGET)
