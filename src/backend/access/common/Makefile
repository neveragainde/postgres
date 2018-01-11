#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for access/common
#
# IDENTIFICATION
#    src/backend/access/common/Makefile
#
#-------------------------------------------------------------------------

subdir = src/backend/access/common
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global

OBJS = bufmask.o heaptuple.o indextuple.o printsimple.o \
	reloptions.o scankey.o tupconvert.o tupdesc.o

ifneq ($(PORTNAME), cloudabi)
OBJS += printtup.o
endif

include $(top_srcdir)/src/backend/common.mk
