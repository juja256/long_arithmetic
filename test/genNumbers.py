import random
import sys
import math


def generateNumbers():

    if(len(sys.argv)<5):
        print('Usage: python <file_name.py> <pair_count_of_numbers> <base>=[10,16] '
        '<[-b ~ bits, -kb ~ kbits]> <numbers_length> <file_name_for_ext_libs> <file_for_long_ar>')
        print(" * This program writes <pair_count_of_numbers> numbers to file * ")
    else:
        if(str(sys.argv[3])=="-b"):
            numbers_len = int(sys.argv[4])
        elif(str(sys.argv[3])=="-kb"):
            numbers_len = 1000*int(sys.argv[4])
    
        with open(str(sys.argv[5]),"w") as file_for_ext_libs, open(str(sys.argv[6]),"w") as file_for_long_ar:
            for i in range(0, 2*int(sys.argv[1])):
                if(int(sys.argv[2])==16):
                    n_i = hex(random.getrandbits(numbers_len)).upper()
                    if(len(n_i[2:len(n_i)-1])%4!=0):
                        file_for_ext_libs.write("0" + n_i[2:len(n_i)-1] + "\n")
                        file_for_long_ar.write("0x0" + n_i[2:len(n_i)-1] + "\n")
                    else:
                        file_for_ext_libs.write(n_i[2:len(n_i)-1] + "\n")
                        file_for_long_ar.write("0x" + n_i[2:len(n_i)-1] + "\n")
                elif(int(sys.argv[2])==10):
                    n_i = random.getrandbits(numbers_len)
                    file_for_ext_libs.write(str(n_i) + "\n")
                    n_i_hex = hex(n_i).upper()
                    if(len(n_i_hex[2:len(n_i_hex)-1])%4!=0):
                        file_for_long_ar.write("0x0" + n_i_hex[2:len(n_i_hex)-1] + "\n")
                    else:
                        file_for_long_ar.write("0x" + n_i_hex[2:len(n_i_hex)-1] + "\n")
                
        file_for_long_ar.close()
        file_for_ext_libs.close()

if __name__ == '__main__':
    generateNumbers()