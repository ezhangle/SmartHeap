# SmartHeap
A small single header class to hide stack variables in segmented heaps with randomized padding

What use does this serve?
This is purely a proof of concept of splitting a stack variable into multiple heap allocations and later reconstructing it. Each segment of the stack variable lies in a separate heap allocation which is double the size of the memory we hide within it. The heap allocation is double the size of the segment so that we can write the real data to a random point inside the allocation and fill the rest with fake memory, making it harder for manual reconstruction. This may be used in scenarios such as manualmapping to split the data into sections (in the form of segments, see diagram below), later directly writing the section from its allocation into the target process without reconstructing the image. This would mean that the data is never reconstructed and a reverser will have a tough time trying to find each individual section.

This is purely proof of concept, there is currently no security benefit with using this inside a protected application.

![alt text](https://cdn.discordapp.com/attachments/794994954857349141/805937874758926365/unknown.png)
