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


# How to implement in log (draft)
1. I get a SHA from a commit object
2. I want to find the commit object of that SHA, but it's in a packfile
3. I will read in .idx fanout table to find objects starting from the first two digits.
4. Then do binary search on sorted SHA-1 list.
5. Look up offset for entry #46
