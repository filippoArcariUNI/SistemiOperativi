

# Da rimpiazzare con PostOffice.c
$(EXE): postOffice.c clean 
	@if [ -f "./$(FILE)" ]; then \
		mkdir $(DIR) ; \
		mv $(FILE) $(DIR); \
		gcc $< -o $(EXE) ; \
		mv $(EXE) $(DIR) ; \
	else \
		echo "Error: file not exist" ; \
		exit 1 ; \
	fi

EXEC = $(wildcard *.out)
clean:
	@echo Removing $(EXEC)
	@rm -f $(EXEC)
	