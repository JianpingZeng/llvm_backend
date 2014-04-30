#! /usr/bin/env python

fi = open('corei7','r')

blockcount = 1

doc = ""  
fo = open('corei7'+str(blockcount),'w')
for line in fi:
  if line[0] == '\n':
    fo.write(doc)
    fo.close()
    blockcount = blockcount + 1
    fo = open('corei7'+str(blockcount),'w')
    doc = ""  
  else:
    doc += line
      
fi.close()


fi = open('cse502','r')

blockcount = 1

doc = ""  
fo = open('cse502'+str(blockcount),'w')
for line in fi:
  if line[0] == '\n':
    fo.write(doc)
    fo.close()
    blockcount = blockcount + 1
    fo = open('cse502'+str(blockcount),'w')
    doc = ""  
  else:
    doc += line
      
fi.close()
