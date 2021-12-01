import re
import random

d = 0

f = open('task_1.txt', 'r')
txtfile= f.read()

txtfile, n = re.subn("[,!,?,`,.,`,0,1,2,3,4,5,6,7,8,9,-,(,),',:,;,*,]", ' ', txtfile)
txtfile, n = re.subn('[",-]', ' ', txtfile)

for i in range(65, 91):
    n = txtfile.count(chr(i)) + txtfile.count(chr(i + 32))
    d = d + n

for j in range(100):
    random_number = random.randint(0, (d + txtfile.count(' ')))

    print(txtfile[random_number], end = '')

f.close
