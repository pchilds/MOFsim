## Process this file with automake to produce Makefile.in
AM_CFLAGS=$(GTK_CFLAGS)
AM_CPPFLAGS= -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
bin_PROGRAMS=MOFsim
if USE_II
MOFsim_SOURCES=		\
	main.c main.h	\
	drawcirc.c drawcirc.h	\
	util.c util.h	\
	data.c data.h	\
	disp.c disp.h	\
	pprc.c pprc.h
else
MOFsim_SOURCES=		\
	main.c main.h	\
	drawcirc.c drawcirc.h	\
	util.c util.h	\
	data.c data.h	\
	disp.c disp.h	\
	proc.c proc.h
endif
MOFsim_LDADD=$(GTK_LIBS)
