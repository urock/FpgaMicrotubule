	
all: libRostaPCIeAPI.a libRC47.a



clean:
	-rm -f *.a



libRostaPCIeAPI.a: 
	@echo
	@echo Compiling $@....
	@echo
	(cd ./RostaPCIeAPI;$(MAKE))	
	
	
libRC47.a: 
	@echo
	@echo Compiling $@....
	@echo
	(cd ./rc47;$(MAKE))
