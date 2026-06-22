# 1. Standard compilation pass of all kernel system elements
nasm -f elf64 loader.s -o loader.o
x86_64-elf-g++ -c kernel.cpp -o kernel.o -ffreestanding -O3 -fno-exceptions -fno-rtti -std=c++17
x86_64-elf-g++ -c paging.cpp -o paging.o -ffreestanding -O3 -fno-exceptions -fno-rtti
x86_64-elf-g++ -c wine.cpp -o wine.o -ffreestanding -O3 -fno-exceptions -fno-rtti -std=c++17

# 2. Link objects to generate final unified OS binary
x86_64-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O3 -nostdlib loader.o kernel.o paging.o wine.o -lgcc

# 3. Build a raw empty 20 Megabyte storage disk image
# Create image
dd if=/dev/zero of=production_disk.img bs=1M count=20480

# Format as FAT
mkfs.fat production_disk.img

# Mount
mkdir -p mnt
sudo mount -o loop production_disk.img mnt

# Copy an existing executable
cp *.exe mnt/NOTEPAD.EXE

# Unmount
sudo umount mnt
