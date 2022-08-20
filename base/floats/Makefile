SOURCES = src/munit.c src/simd_test.c

ifeq ($(shell uname -m),x86_64)
	SOURCES += src/simd_amd64.c
	CFLAGS = -mavx -mfma
endif

OBJECTS	= $(SOURCES:.c=.o)
EXECUTE	= src/simd_test

$(EXECUTE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $(EXECUTE)

test: $(EXECUTE)
	./${EXECUTE}

clean:
	rm $(OBJECTS) $(EXECUTE)
