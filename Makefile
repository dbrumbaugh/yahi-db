sm_test: storagemanager.o sm_test.c
	gcc sm_test.c storagemanager.o -o sm_test

storagemanager.o: storagemanager.c
	gcc -c storagemanager.c

clean:
	rm *.o sm_test testdb/persons.tbl
	rmdir testdb
