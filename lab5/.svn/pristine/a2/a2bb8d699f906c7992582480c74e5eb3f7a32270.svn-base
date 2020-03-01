# name of the program (Minix service)
PROG=lab5 

# source code files to be compiled
SRCS = lab5.c vc_macros.h vc.c vc.h keyboard.h keyboard.c i8042.h utils.h utils.c timer.c i8254.h

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CPPFLAGS += -pedantic -D LAB5

# list of library dependencies (for Lab 5, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
