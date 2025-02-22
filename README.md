# TURNITIN-LIKE PLATFORM
**SUMMARY**: This project attempts to find similar documents and alert the user to check them for plagiarism. \
1. Each document is located in a 3d space of strings and is stored in a kd-tree or R-tree, through which it can also be accessed. \
2. Examining the document while using Locality Sensitive Hashing (LSH), maximizes the probability of similar documents ending up in the same bucket of the hash table. \
3. The candidate pairs need to be checked for plagiarism.

**DEPENDECIES**: eigen-3.4.0 Library  

**IMPORTANT NOTICE**: known bugs and test file format can be found in te documentation file.
