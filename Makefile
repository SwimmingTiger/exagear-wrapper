release:
	gcc -static -O2 -o ubt_x32a64_al ubt_x32a64_al_wrapper.c && strip ubt_x32a64_al
debug:
	gcc -static -O2 -DDEBUG_OUTPUT -o ubt_x32a64_al ubt_x32a64_al_wrapper.c
