# LightSpeedLinearAlgebra

`LSLA` provides a vector implementation optimized for large workloads, while `LSLASimple` remains available as a straightforward sequential reference. This is so that I have something to compare the optimizations against.

## Version 1
### Using threads and parallel optimization

For vectors with at least 32,768 elements (random large number), the main library partitions
independent element-wise work and dot-product input into contiguous ranges
across up to the number of available workers using: `std::thread::hardware_concurrency()`. Smaller vectors
remain sequential to avoid thread scheduling overhead. Parallel dot products
use deterministic per-range partial sums, so they can differ slightly from
sequential floating-point accumulation due to rounding.

### Results:

The testing file was setup to generate vectors of size = 100,000 and both implementations were tasked with running an operation on the large vector and computing a dot product. Results were unexpected in that the simple implementation was approximately 7.5x faster than the parallel version in completing an operation, and approximately 8.51x faster in computing a dot product. The overhead of allocating threads, splitting the work amongst them, then adding the result together appears to be much heavier than just using the simple implementation.
