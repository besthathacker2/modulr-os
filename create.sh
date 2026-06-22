# 1. Standard compilation pass of all kernel system elements
nasm -f elf64 loader.s -o loader.o
x86_64-elf-g++ -c kernel.cpp -o kernel.o -ffreestanding -O3 -fno-exceptions -fno-rtti -std=c++17
x86_64-elf-g++ -c paging.cpp -o paging.o -ffreestanding -O3 -fno-exceptions -fno-rtti
x86_64-elf-g++ -c wine.cpp -o wine.o -ffreestanding -O3 -fno-exceptions -fno-rtti -std=c++17

# 2. Link objects to generate final unified OS binary
x86_64-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O3 -nostdlib loader.o kernel.o paging.o wine.o -lgcc

# 3. Build a raw empty 20 Megabyte storage disk image
dd if=/dev/zero of=production_disk.img bs=1M count=20

# 4. Inject a real FAT Directory Entry into Sector 2 pointing to our program payload
# Filename: "NOTEPAD EXE", Attribute: 0x20 (Archive), Start Sector: 5, File Size: 64 Bytes
printf 'NOTEPAD EXE\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x40\x00\x00\x00' | dd of=production_disk.img bs=512 seek=2 conv=notrunc

# 5. Inject a real functional 64-bit PE Windows header signature directly into Sector 5
# Offset 0: 'MZ', Offset 0x28: Pointer to PE, Offset 0x28: 'PE\0\0', Offset 0x30: 64-bit Magic 0x20B
printf '\x4D\x5A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x28\x00\x00\x00\x50\x45\x00\x00\x64\x86\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x02\x0B' | dd of=production_disk.img bs=512 seek=5 conv=notrunc

# 6. Execute our graphics-enabled OS inside the emulator with our custom hard drive 
