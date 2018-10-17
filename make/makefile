objs = hello.o print.o

hello: $(objs)
	# $@ ターゲット名
	$(CC) -Wall -O2 -o $@ $(objs)

hello.o: hello.c
	# $< 依存ファイルの先頭
	$(CC) -c $<

print.o: print.c
	$(CC) -c $<

.PHONY: clean
clean:
	$(RM) hello $(objs)