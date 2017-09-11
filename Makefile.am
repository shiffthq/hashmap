AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libhashtable.la
libhashtable_la_SOURCES = hashtable.c murmur2.c hashtable.h

noinst_PROGRAMS = hashtable_test
hashtable_test_SOURCES = hashtable_test.c
hashtable_test_LDADD = ./libhashtable.la

TESTS = $(noinst_PROGRAMS)
