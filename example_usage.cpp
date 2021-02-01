#include "smart_heap.hpp"

void PrintByteVector(const char* InitialMessage, std::vector<unsigned char> Bytes)
{
	printf(InitialMessage);
	for (const auto& Byte : Bytes)
		printf(" %02x", Byte);
	printf("\n");
}

int main()
{
	std::vector<unsigned char> WantToHideBytes = { ... };

	PrintByteVector("Attempting to hide:", WantToHideBytes);

	// The default constructor of our class will automatically create the segments and heap allocations
	SmartHeap* SmartHeapInstance = new SmartHeap(WantToHideBytes, 5);

	std::vector<unsigned char> ReconstructedData = SmartHeapInstance->ReconstructHeapAllocations();

	PrintByteVector("Reconstructed:", ReconstructedData);

	SmartHeapInstance->DestroyHeapAllocations();

	std::getchar();
	return 0;
}
