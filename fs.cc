#include "fs.h"
#include <cmath>

int INE5412_FS::fs_format()
{
	// Check if the file system is already mounted
    if (mounted) {
        cout << "Error: File system is already mounted. Format operation aborted.\n";
        return 0; // Return failure
    }

	// Calculate block distribution 
	int total_blocks = disk->size();
	int inode_blocks = static_cast<int>(std::ceil(total_blocks * 0.1));

	// Reserving ten percent of blocks to inodes
	superblock.ninodeblocks = inode_blocks;
	superblock.ninodes = inode_blocks * INODES_PER_BLOCK;

	// Freeing the inode table
	for (int i = 1; i <= inode_blocks; ++i) {
		fs_block inode_block;
		for (int j = 0; j < INODES_PER_BLOCK; ++j) {
			inode_block.inode[j].isvalid = false;
		}
		disk->write(i, inode_block.data);
	}

	// Writing the superblock
	superblock.magic = FS_MAGIC;
	superblock.nblocks = total_blocks;
	fs_block new_superblock;
	new_superblock.super = superblock;
	disk->write(0, new_superblock.data);

	return 1; // Return success
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
	if (mounted) {
        cout << "Error: File system is already mounted.\n";
        return 0; // Return failure
    }

	// Read the superblock from disk
	fs_block superblock_block;
	disk->read(0, superblock_block.data);
	superblock = superblock_block.super;

	// Check if the magic number is valid
	if (superblock.magic != FS_MAGIC) {
		cout << " Error: Invalid magic number. Not a valid file system.\n";
		return 0; // Return failure
	}

	// Build a bitmap of free blocks
	vector<bool> free_blocks(superblock.nblocks, true); // Assume all blocks are ionitially free

	// Mark inode blocks as used
	for (int i = 1; i <= superblock.ninodeblocks; ++i) {
		free_blocks[i] = false;
	}

	// Mark data blocks used by valid inodes
	for (int i = 1; i <= superblock.ninodeblocks; ++i) {
		fs_block inode_block = read_block(i);
		for (int j = 0; j < INODES_PER_BLOCK; ++j) {
			fs_inode inode = inode_block.inode[j];
			if (inode.isvalid) {
				for (int k = 0; k < POINTERS_PER_INODE; ++k) {
					if (inode.direct[k]) {
						free_blocks[inode.direct[k]] = false;
					}
				}
				if (inode.indirect) {
					fs_block indirect_block = read_block(inode.indirect);
					for (int k = 0; k < POINTERS_PER_BLOCK; ++k) {
						if (indirect_block.pointers[k]) {
							free_blocks[indirect_block.pointers[k]] = false;
						}
					}
				}
			}
		} 
	}
	mounted = true;
	return 1; // Return success
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
