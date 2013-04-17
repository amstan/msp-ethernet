#!/usr/bin/env python2
import sys

for filename in sys.argv[1:]:
	data=open(filename).read()
	data+="\0"
	
	filename=filename.split("/")[-1].replace(".","_")
	
	print "unsigned int %s_len = %d;" % (filename,len(data))
	print "char %s[] = {" % (filename)
	print ", ".join(hex(ord(char)) for char in data)
	print "};"
	print