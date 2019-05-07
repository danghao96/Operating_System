#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.

#define ROOTINO 1 // root i-number
#define BSIZE 512 // block size

// File system super block
struct superblock
{
    uint size;    // Size of file system image (blocks)
    uint nblocks; // Number of data blocks
    uint ninodes; // Number of inodes.
};

#define NDIRECT (12)
// On-disk inode structure
struct dinode
{
    short type;              // File type
    short major;             // Major device number (T_DEV only)
    short minor;             // Minor device number (T_DEV only)
    short nlink;             // Number of links to inode in file system
    uint size;               // Size of file (bytes)
    uint addrs[NDIRECT + 1]; // Data block addresses
};

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent
{
    ushort inum;
    char name[DIRSIZ];
};

int main(int argc, char *argv[])
{
    int fd;
    if (argc == 2)
    {
        fd = open(argv[1], O_RDONLY);
        if (fd == -1)
        {
            fprintf(stderr, "image not found.\n");
            exit(1);
        }
    }
    else if (argc == 3)
    {
        //TO DO for extra credit.
    }
    else
    {
        fprintf(stderr, "Usage: xv6_fsck <file_system_image>");
        exit(1);
    }

    int rc;
    struct stat sbuf;
    rc = fstat(fd, &sbuf);
    assert(rc == 0);

    void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(img_ptr != MAP_FAILED);

    struct superblock *sb;
    sb = (struct superblock *)(img_ptr + BSIZE);
    struct dinode *dip = (struct dinode *)(img_ptr + 2 * BSIZE);
    int addrscount[sb->nblocks];
    for(int i = 0; i < sb->nblocks; i++){
        addrscount[i] = 0;
    }
    for (int i = 0; i < sb->ninodes; i++)
    {
        //valid
        if (dip->type < 0 || dip->type > 3)
        {
            fprintf(stderr, "ERROR: bad inode.\n");
            exit(1);
        }
        //in-use
        if (dip->type > 0 && dip->type < 4)
        {
            //printf("%d, %d\n", NDIRECT, BSIZE / sizeof(uint));
            for (int j = 0; j < NDIRECT; j++)
            {
                if (dip->addrs[j] < 0 || dip->addrs[j] > sb->size)
                {
                    fprintf(stderr, "ERROR: bad direct address in inode.\n");
                    exit(1);
                }
            }
            uint *bptr;
            bptr = (uint *)(img_ptr + dip->addrs[NDIRECT] * BSIZE);
            for (int j = 0; j < BSIZE / sizeof(uint); j++)
            {
                if (bptr[j] >= sb->size)
                {
                    fprintf(stderr, "ERROR: bad indirect address in inode.\n");
                    exit(1);
                }
            }
        }

        //dir
        struct dirent *dptr = (struct dirent *)(img_ptr + dip->addrs[0] * BSIZE);
        if (dip->type == 1)
        {
            if (strcmp(dptr[0].name, ".") != 0 || strcmp(dptr[1].name, "..") != 0)
            {
                fprintf(stderr, "ERROR: directory not properly formatted.\n");
                exit(1);
            }
        }

        //more than once
        for (int j = 0; j < NDIRECT; j++)
        {
            addrscount[dip->addrs[j]]++;
            if (dip->addrs[j] != 0 && addrscount[dip->addrs[j]] > 1)
            {
                fprintf(stderr, "ERROR: direct address used more than once.\n");
                exit(1);
            }
        }

        uint *bptr = (uint *)(img_ptr + dip->addrs[NDIRECT] * BSIZE);
        for (int j = 0; j < BSIZE / sizeof(uint); j++){
            addrscount[bptr[j]]++;
            if (bptr[j] != 0 && addrscount[bptr[j]] > 1)
            {
                fprintf(stderr, "ERROR: indirect address used more than once.\n");
                exit(1);
            }
        }

        //root
        if (i == 1)
        {
            if (dip->type != 1)
            {
                fprintf(stderr, "ERROR: root directory does not exist.\n");
                exit(1);
            }
            struct dirent *rtdptr = (struct dirent *)(img_ptr + dip->addrs[0] * BSIZE);
            if (rtdptr[0].inum != 1 || rtdptr[1].inum != 1)
            {
                fprintf(stderr, "ERROR: root directory does not exist.\n");
                exit(1);
            }
        }
        dip++;
    }

    return 0;
}