CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
TARGET := coup_game
SOURCES := main.cpp Card.cpp Deck.cpp Player.cpp Prtstr.cpp Run.cpp randoms.cpp
HEADERS := Input.h Card.h Deck.h Player.h Prtstr.h Run.h randoms.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
