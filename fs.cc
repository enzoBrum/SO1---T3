#include "fs.h"

int INE5412_FS::fs_format()
{
	return 0;
}

void INE5412_FS::fs_debug()
{
	union fs_block block;

	disk->read(0, block.data);

	const string spaces = "    ";
	cout << "superblock:\n";
	cout << spaces << (block.super.magic == FS_MAGIC ? "magic number is valid\n" : "magic number is invalid!\n");
 	cout << spaces << block.super.nblocks << " blocks\n";
	cout << spaces << block.super.ninodeblocks << " inode blocks\n";
	cout << spaces << block.super.ninodes << " inodes\n";

	for (int i = 1; i <= this->superblock.ninodeblocks; ++i) {
		fs_block block = this->read_block(i);

		for (int j = 0; j < this->INODES_PER_BLOCK; ++j) {
			fs_inode inode = block.inode[j];
			
			if (!inode.isvalid) continue;

			cout << "inode " << (i-1)*INODES_PER_BLOCK + j + 1 << ":\n" << spaces << "size: " << inode.size << " bytes\n" << spaces << "direct blocks: ";

			for (int k = 0; k < this->POINTERS_PER_INODE; ++k)
				if (inode.direct[k])
					cout << inode.direct[k] << ' ';
			
			cout << "\n" << spaces << "indirect block: " << ((inode.indirect) ? inode.indirect : '-') << "\n" << spaces << "indirect data blocks: ";

			if (!inode.indirect) {
				cout << "-\n";
				continue;
			}

			fs_block indirect_block = this->read_block(inode.indirect);

			for (int k = 0; k < this->POINTERS_PER_BLOCK; ++k)
				if (indirect_block.pointers[k])
					cout << indirect_block.pointers[k] << ' ';
			cout << '\n';
		}
	}
}

int INE5412_FS::fs_mount()
{
	return 0;
}

int INE5412_FS::fs_create()
{
	return 0;
}

int INE5412_FS::fs_delete(int inumber)
{
	return 0;
}

int INE5412_FS::fs_getsize(int inumber)
{
	return -1;
}

int INE5412_FS::fs_read(int inumber, char *data, int length, int offset)
{
	return 0;
}

int INE5412_FS::fs_write(int inumber, const char *data, int length, int offset)
{
	return 0;
}
