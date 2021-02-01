#ifndef _SMART_HEAP_HPP
#define _SMART_HEAP_HPP

#include <Windows.h>
#include <iostream>
#include <vector>
#include <random>

class SmartHeap
{
private:
	struct SmartHeapAllocation {
		unsigned char* HeapAllocation;
		uint32_t TotalHeapSize;

		uint32_t RVAToData;
		uint32_t SizeOfData;
	};

	// Vector which stores all heap allocations
	std::vector<SmartHeapAllocation> HeapAllocations;
	
	uint32_t RandomInteger(uint32_t Min, uint32_t Max)
	{
		std::random_device RandomDevice;
		std::mt19937 MTDevice(RandomDevice());
		std::uniform_real_distribution<float> Distance(Max, Min);

		return (uint32_t)Distance(MTDevice);
	}
	unsigned char RandomByte()
	{
		return (unsigned char)this->RandomInteger(0, 255);
	}

	void PopulateHeapWithFake(void* Heap, uint32_t HeapSize)
	{
		// Heap allocation to store fake bytes:
		// These could be written directly into the vector but this is neater
		// and requires just one memcpy
		unsigned char* FakeBytesHeap = new unsigned char[HeapSize];

		printf("Generated fake bytes: ");
		for (uint32_t i = 0; i < HeapSize; i++)
		{
			FakeBytesHeap[i] = this->RandomByte();
			printf("%02x ", FakeBytesHeap[i]);
		}
		printf("\n");

		// Copy the new heap full of fake values into the 'real fake'
		memcpy(Heap, FakeBytesHeap, HeapSize);

		delete[] FakeBytesHeap;
	}

public:
	bool DestroyHeapAllocations()
	{
		for (const auto& SegmentHeap : this->HeapAllocations)
			delete[] SegmentHeap.HeapAllocation;
		return true;
	}

	std::vector<unsigned char> ReconstructHeapAllocations()
	{
		std::vector<unsigned char> ReturnVector;

		for (const auto& SegmentHeap : this->HeapAllocations)
		{
			// Vector to hold real data (stack variable)
			std::vector<unsigned char> ExtractedData(SegmentHeap.SizeOfData);

			// Copy the data from the heap allocation into our vector
			memcpy(&ExtractedData[0], SegmentHeap.HeapAllocation + SegmentHeap.RVAToData, SegmentHeap.SizeOfData);

			// Push this back into the return vector
			// Consider using a faster way of copying this data - this is slow.
			for (const auto& Byte : ExtractedData)
				ReturnVector.push_back(Byte);
		}

		return ReturnVector;
	}

	SmartHeap(std::vector<unsigned char> Bytes, uint32_t StoreSegments)
	{
		// Separate the vector into individual segments - these are 
		// to be hidden later on inside larger heaps with fake values surrrounding
		// They are hidden in the following form:
		// {0, 1, 2, 0, 1, 2, 0, 1, 2} - assuming that there are 3 segments
		// They are not taken directly from {Bytes.Size()/StoreSegments} purely to
		// make this more complex and harder to find the original set of bytes in
		// heap memory
		
		std::vector< std::vector<unsigned char> > Segments;
		for (uint32_t StartIter = 0; StartIter < StoreSegments; StartIter++)
		{
			std::vector<unsigned char> CurrentSegment;

			uint32_t ObjectiveStart = Segments.size();
			
			for (uint32_t i = ObjectiveStart; i < Bytes.size(); i += StoreSegments)
				CurrentSegment.push_back(Bytes[i]);

			Segments.push_back(CurrentSegment);
		}

		printf("\nSegments size: %d\n", Segments.size());
		
		// Now that all of these are split, we need to allocate a heap larger
		// than the true size of them and fill in the zeromemory with random
		// bytes to isolate each segment from eachother. These will later be
		// rebuilt by doing the exact opposite calculation

		for (const auto& Segment : Segments)
		{
			// Total size of the heap allocation
			// This is double the size because we're going to
			// later pick a random point between 0 and Segment.size()
			// to allocate the real data into
			uint32_t TotalHeapSize = Segment.size() * 2;

			// Allocate the segment heap with double the size of the data
			unsigned char* SegmentHeap = new unsigned char[TotalHeapSize];

			// Zeromemory the whole heap allocation
			ZeroMemory(SegmentHeap, TotalHeapSize);

			// Populate the segment with random bytes
			this->PopulateHeapWithFake(SegmentHeap, TotalHeapSize);

			// Bounds for the real data's RVA inside the heap allocation
			uint32_t SmallestBound = 0, LargestBound = Segment.size();

			// Randomized RVA for the real data
			uint32_t DataRVA = this->RandomInteger(SmallestBound, LargestBound);

			// Copy the real data into the heap
			memcpy(SegmentHeap + DataRVA, &Segment[0], Segment.size());
			
			printf("Allocated segment heap:\n\tAddress:%llx\n\tSize:%d\n\tDataRVA:%llx\n\n", SegmentHeap, TotalHeapSize, DataRVA);

			// Store this segment
			this->HeapAllocations.push_back({
				SegmentHeap,
				TotalHeapSize,
				DataRVA,
				(uint32_t)Segment.size()
			});
		}
	}
};

#endif
