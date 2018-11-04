all: gaussian_elim.c
	gcc gaussian_elim.c -o gelim -lpthread

clean: gelim
	rm gelim
