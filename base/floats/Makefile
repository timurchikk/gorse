SOURCES = src/munit.c src/simd_test.c

ifeq ($(shell uname -m),x86_64)
	SOURCES += src/simd_amd64.c
	CFLAGS = -mavx -mfma -mavx512f -mavx512dq
else ifeq ($(shell uname -m),aarch64)
	SOURCES += src/simd_arm64.c
endif

OBJECTS		= $(SOURCES:.c=.o)
DEPENDENCES	= $(SOURCES:.c=.d)
EXECUTE		= src/simd_test

$(EXECUTE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $(EXECUTE)

test: $(EXECUTE)
	./${EXECUTE}

clean:
	rm $(OBJECTS) $(DEPENDENCES) $(EXECUTE)

-include $(DEPENDENCES)

%.d: %.c
	@set -e; \
	rm -f $@; \
	$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' $@.$$$$ > $@; \
	rm -f $@.$$$$
