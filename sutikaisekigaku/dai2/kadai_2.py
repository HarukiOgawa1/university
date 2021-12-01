characters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
counter = [0] * 26
sum = 0
book = open('task_1.txt', 'r', encoding = 'UTF-8')
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

f.write("a ="+ str(counter[0])+ "\n")
f.write("b ="+ str(counter[1])+ "\n")
f.write("c ="+ str(counter[2])+ "\n")
f.write("d ="+ str(counter[3])+ "\n")
f.write("e ="+ str(counter[4])+ "\n")
f.write("f ="+ str(counter[5])+ "\n")
f.write("g ="+ str(counter[6])+ "\n")
f.write("h ="+ str(counter[7])+ "\n")
f.write("i ="+ str(counter[8])+ "\n")
f.write("j ="+ str(counter[9])+ "\n")
f.write("k ="+ str(counter[10])+ "\n")
f.write("l ="+ str(counter[11])+ "\n")
f.write("m ="+ str(counter[12])+ "\n")
f.write("n ="+ str(counter[13])+ "\n")
f.write("o ="+ str(counter[14])+ "\n")
f.write("p ="+ str(counter[15])+ "\n")
f.write("q ="+ str(counter[16])+ "\n")
f.write("r ="+ str(counter[17])+ "\n")
f.write("s ="+ str(counter[18])+ "\n")
f.write("t ="+ str(counter[19])+ "\n")
f.write("u ="+ str(counter[20])+ "\n")
f.write("v ="+ str(counter[21])+ "\n")
f.write("w ="+ str(counter[22])+ "\n")
f.write("x ="+ str(counter[23])+ "\n")
f.write("y ="+ str(counter[24])+ "\n")
f.write("z ="+ str(counter[25])+ "\n")

for i in counter:
	sum += i

f.write("合計文字数=" + str(sum) + "\n")


f.close()

