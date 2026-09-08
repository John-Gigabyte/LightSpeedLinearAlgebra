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

## Version 1.0.1
### Increase the `largeSize` value by 10x

The overhead of the original version was proven to be too large for vectors with 100,000 items or less. The simple version of the library was outperforming the "optimized" version. The `largeSize` variable, which defines the number of elements the Vector will contain, was increase by 10x (from 100 to 1 mil).

### Results:

The optimized version now performs approximately 1.22x faster (17.86% faster) than the simple version for completing the operation, and is 1.41x faster (29.72% faster) in computing the dot product. Noting how small these percentages are , it is safe to assume that the "optimized" library will not show any benefit unless the number of elements is at or above 1 million.