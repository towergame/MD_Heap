all:
	gcc main.c -o heap

test: all
	./heap -c chunks1.txt -s requests1.txt
	./heap -c chunks2.txt -s requests2.txt
	./heap -c chunks3.txt -s requests3.txt
