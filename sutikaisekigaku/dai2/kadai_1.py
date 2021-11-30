import re

f = open('HEIDI.txt', 'r')
txtfile = f.read()

characters_1 = "[,!,?,`,.,`,0,1,2,3,4,5,6,7,8,9,-,(,),',:,;,*]"
characters_2 = '[",-]'

for x in range(len(characters_1)):
    txtfile = txtfile.replace(characters_1[x]," ")
for x in range(len(characters_2)):
    txtfile = txtfile.replace(characters_2[x]," ")

w = open('task_1.txt', 'w')
w.write(txtfile)

f.close
w.close
