//
// Created by Christian Haase on 29/08/2023.
//

#include <ScopedAllocationDetector.h>
#include <memory>

static size_t antiOptimizationVariable = 0;

inline void dontOptimizePointer (const void* ptr)
{
    antiOptimizationVariable += reinterpret_cast<size_t> (ptr);
}

int main()
{
    struct SomeObj
    {
        int a, b, c;
    };

    bool allocationDetected = false;

    // the start of the section you want to examine
    {
        ntlab::ScopedAllocationDetector allocationDetector;
        allocationDetector.onAllocation = [&] (size_t bytes, const std::string* optionalFileAndLine)
        {
            allocationDetected = true;
            std::cout << "Allocation detected: " << bytes << " bytes at " << optionalFileAndLine
                      << std::endl;
        };

        // this will trigger the detection
        std::unique_ptr<SomeObj> someObj (new SomeObj);
        dontOptimizePointer (someObj.get());
    }

    if (! allocationDetected)
        return 1;

    // this won't trigger the detection
    std::unique_ptr<SomeObj> someOtherObj (new SomeObj);
    dontOptimizePointer (someOtherObj.get());

    return 0;
}