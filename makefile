
APP=/usr/local/bin/comma.impl
APPLY=/usr/local/bin/apply
FLDMRG=/usr/local/bin/fldmrg

all : $(APPLY) $(FLDMRG) $(APP)
	cp -p ./comma /usr/local/bin/

$(APPLY) : apply.o vector_int32.o vector_int8.o vector_int64.o
	gcc -std=c99 apply.o vector_int32.o vector_int8.o vector_int64.o -o $(APPLY)

$(FLDMRG) : fldmrg.o vector_int32.o vector_int8.o
	gcc -std=c99 fldmrg.o vector_int32.o vector_int8.o -o $(FLDMRG)

$(APP) : comma.impl.o vector_int8.o
	gcc -std=c99 comma.impl.o vector_int8.o -o $(APP)

apply.o : apply.c
	gcc -std=c99 -c apply.c -o apply.o

fldmrg.o : fldmrg.c
	gcc -std=c99 -c fldmrg.c -o fldmrg.o

comma.impl.o : comma.impl.c
	gcc -std=c99 -c comma.impl.c -o comma.impl.o

vector_int32.o : vector_int32.c vector_int32.h
	gcc -std=c99 -c vector_int32.c -o vector_int32.o

vector_int8.o : vector_int8.c vector_int8.h
	gcc -std=c99 -c vector_int8.c -o vector_int8.o

vector_int64.o : vector_int64.c vector_int64.h
	gcc -std=c99 -c vector_int64.c -o vector_int64.o

clean :
	rm -f $(APP) $(APPLY) $(FLDMRG)
	rm -f *.o

