//
// Created by Christian Haase on 29/08/2023.
//

#include <ScopedAllocationDetector.h>

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

        // this ensures that the object is not optimized away
        std::cout << "Address of allocated object: " << someObj.get() << std::endl;
    }

    if (! allocationDetected)
        return 1;

    // this won't trigger the detection
    std::unique_ptr<SomeObj> someOtherObj (new SomeObj);

    // this ensures that the object is not optimized away
    std::cout << "Address of allocated other object: " << someOtherObj.get() << std::endl;

    return 0;
}