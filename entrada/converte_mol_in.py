import sys

fin = open(sys.argv[1])
fout = open(sys.argv[2], 'w')

for i in fin:
    line = i.split()
    newline = ' '.join([line[-1]] + line[:-1])
    fout.write(newline + '\n')

fin.close()
fout.close()





