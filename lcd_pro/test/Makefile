src = $(wildcard *.c)
obj = $(patsubst %.c, obj/%.o, $(src))

cc=arm-linux-gcc

ALL : main

main : $(obj)
	$(cc) -o $@ $^ -pthread

obj/%.o : %.c
	$(cc) -o $@ -c $< -Wall 

clean :
	-rm -rf $(obj)

.PHONY : ALL clean




