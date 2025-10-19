# Pack files

## pack.*pack format
1. 4 byte signature
2. version number
3. 4-byte number of objects contained in pack
4. Number of entries

## Entry formats:
1. Undeltified: 3 bits for type, (n-1)*7+4-bit length data
2. n-byte type and length (3-bit type, (n-1)*7+4-bit length)
   base object name if OBJ_REF_DELTA or a negative relative
offset from the delta object's position in the pack if this
is an OBJ_OFS_DELTA object
   compressed delta data

# *idx format

1. Fanout table
- it contains 256 entries
- For each entry (from 00-FF), it says how many objects starts with bytes less than or equal that value.
2. Sorted SHA-1 list
3. Offsets
- A byte offset in the .pack file where each object's data starts


## How to find an object from a SHA in a pack file
1. Look in .idx fanout table at position 0x9d
2. Binary search the sorted SHA-1 list from position 45-47
3. In the offset table, the object starts at byte 327.
4. Seek to byte 327, decompress and read the object

