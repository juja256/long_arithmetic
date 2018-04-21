import random
import sys


def generateNumbers():

    if(len(sys.argv)<5):
        print('Usage: python <file_name.py> <pair_count_of_numbers> <base>=[10,16] '
        '<[-b ~ bits, -kb ~ kbits]> <numbers_lenght> <file_name>')
        print(" * This program writes <count_of_numbers> numbers to file * ")
    else:
        if(str(sys.argv[3])=="-b"):
            numbers_len = int(sys.argv[4])
        elif(str(sys.argv[3])=="-kb"):
            numbers_len = 1000*int(sys.argv[4])
    
        with open(str(sys.argv[5]),"w") as file:
            for i in range(0,2*int(sys.argv[1])):
                if(int(sys.argv[2])==16):
                    n_i = hex(random.getrandbits(numbers_len)).upper()
                    if(len(n_i[2:len(n_i)-1])%4!=0):
                        file.write("0" + n_i[2:len(n_i)-1] + "\n")
                    else:
                        file.write(n_i[2:len(n_i)-1] + "\n")
                elif(int(sys.argv[2])==10):
                    n_i = random.getrandbits(numbers_len)
                    file.write(str(n_i) + "\n")
                   
        file.close()

if __name__ == '__main__':
    generateNumbers()