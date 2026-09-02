# LightSpeedLinearAlgebra

`LSLA` provides a vector implementation optimized for large workloads, while
`LSLASimple` remains available as a straightforward sequential reference.

For vectors with at least 32,768 elements, the main library partitions
independent element-wise work and dot-product input into contiguous ranges
across up to `std::thread::hardware_concurrency()` workers. Smaller vectors
remain sequential to avoid thread scheduling overhead. Parallel dot products
use deterministic per-range partial sums, so they can differ slightly from
sequential floating-point accumulation due to rounding.
