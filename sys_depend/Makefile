

all: $(patsubst %.cpp, %.o, $(wildcard *.cpp))

clean:
	rm *.o *.d; exit 0

%.o: %.cpp
	$(CXX) $(patsubst %.o, %.cpp, $@) -MMD -c $(CFLAGS)

%.d: %.cpp # without cpp target! need to process _before_ cpp targets
	$(CXX) -MM -MF $@ $^

include $(wildcard *.d)
