.PHONY = all clean cliente gestor verificador

CC = gcc

LINKERFLAG = -lm

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: ${BINS}
	cp ${BINS} ~/TP

%: %.o
	@echo "Checking.."
	${CC} ${LINKERFLAG} $< -o $@

%.o: %.c
	@echo "Creating object.."
	${CC} -c $<

cliente: client.o

gestor: server.o

verificador: verificador.o

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}
	