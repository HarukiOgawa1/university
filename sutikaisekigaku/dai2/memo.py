characters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
counter = [0] * 26
sum = 0
book = open('HEIDI.txt', 'r', encoding = 'UTF-8')
datas = book.readlines()


for data in datas:
	counter[0] += data.count('a') + data.count('A')
	counter[1] += data.count('b') + data.count('B')
	counter[2] += data.count('c') + data.count('C')
	counter[3] += data.count('d') + data.count('D')
	counter[4] += data.count('e') + data.count('E')
	counter[5] += data.count('f') + data.count('F')
	counter[6] += data.count('g') + data.count('G')
	counter[7] += data.count('h') + data.count('H')
	counter[8] += data.count('i') + data.count('I')
	counter[9] += data.count('j') + data.count('J')
	counter[10] += data.count('k') + data.count('K')
	counter[11] += data.count('l') + data.count('L')
	counter[12] += data.count('m') + data.count('M')
	counter[13] += data.count('n') + data.count('N')
	counter[14] += data.count('o') + data.count('O')
	counter[15] += data.count('p') + data.count('P')
	counter[16] += data.count('q') + data.count('Q')
	counter[17] += data.count('r') + data.count('R')
	counter[18] += data.count('s') + data.count('S')
	counter[19] += data.count('t') + data.count('T')
	counter[20] += data.count('u') + data.count('U')
	counter[21] += data.count('v') + data.count('V')
	counter[22] += data.count('w') + data.count('W')
	counter[23] += data.count('x') + data.count('X')
	counter[24] += data.count('y') + data.count('Y')
	counter[25] += data.count('z') + data.count('Z')
	
book.close()

f = open('output.txt', 'w', encoding= 'UTF-8')

characters_list = ['sa']
for i in characters:
	sum += 1
	#characters_list.append(str(counter[sum-1])+ ' : ' + i)
	characters_list.append(str(counter[sum-1]))
	print(counter[sum-1])

print(characters_list)

characters_list.sort()
print(characters_list)

for i in counter:
	sum += i

f.write("合計文字数=" + str(sum) + "\n")


f.close()

