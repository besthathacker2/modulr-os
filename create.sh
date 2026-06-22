sudo apt update
sudo apt install \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    wget \
    nasm

mkdir -p $HOME/cross
export PREFIX="$HOME/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

wget https://ftp.gnu.org/gnu/binutils/binutils-2.45.tar.xz
tar -xf binutils-2.45.tar.xz

mkdir build-binutils
cd build-binutils

../binutils-2.45/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --with-sysroot \
    --disable-nls \
    --disable-werror

make -j$(nproc)
make install

cd ..

wget https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.xz
tar -xf gcc-15.1.0.tar.xz

mkdir build-gcc
cd build-gcc

../gcc-15.1.0/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --enable-languages=c,c++ \
    --without-headers

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)

make install-gcc
make install-target-libgcc

nasm -f elf64 loader.s -o loader.o
x86_64-elf-g++ -c src/kernel.cpp -o kernel.o -ffreestanding -O3 -fno-exceptions -fno-rtti -std=c++17
x86_64-elf-g++ -c src/paging.cpp -o paging.o -ffreestanding -O3 -fno-exceptions -fno-rtti
x86_64-elf-g++ -c src/wine.cpp -o wine.o -ffreestanding -O3 -fno-exceptions -fno-rtti -std=c++17

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
