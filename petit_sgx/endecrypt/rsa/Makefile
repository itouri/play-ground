App_Name = main
App_Files = $(wildcard *.c)
App_Objects = $(App_Files:.c=.o)
App_LinkFlags = -lssl -lcrypto
App_IncludePath = 
App_C_Flags = -g $(App_IncludePath)

CC = gcc

%.o: %.c
	$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC => $<"

$(App_Name): $(App_Objects)
	$(CC) $^ -o $@ $(App_LinkFlags)
	@echo "LINK => $@"

.PHONY: clean
clean:
	rm -rf $(App_Name) *.o