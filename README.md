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

## Version 1.0.2 (AI-Assisted)
### Build configuration

All prior results were measured from a `Debug` build (`-g`, no optimization), because a fresh `cmake -S . -B build` leaves `CMAKE_BUILD_TYPE` empty on single-config generators. That made every earlier comparison invalid — the timings above reflect unoptimized code, not the "optimized" library's actual design. `CMakeLists.txt` now:

- Defaults `CMAKE_BUILD_TYPE` to `Release` when the caller doesn't set one, so `-O3 -DNDEBUG` is applied unless a different config is explicitly requested (`-DCMAKE_BUILD_TYPE=Debug` still works as before).
- Added `-march=x86-64-v3`, raising the vectorization floor from the SSE2-only x86_64 baseline to AVX2/FMA (supported by essentially every x86_64 CPU since ~2013). This is opt-out rather than `-march=native`, so built binaries stay portable to other machines rather than only the one that built them. Disable with `-DLSLA_ENABLE_ARCH_TUNING=OFF` for maximally portable (SSE2-only) binaries.
- Hides symbol visibility by default (`-fvisibility=hidden`/`-fvisibility-inlines-hidden`), exporting only the public `Vector`/`HelloWorld` API (via CMake's `generate_export_header`) from `libLSLA.so`/`libLSLASimple.so`. This lets the compiler optimize more aggressively within each library and avoids treating every symbol as interposable.

### Results:

With an actual `Release` build, the gap between the two implementations narrows dramatically from the `Debug` numbers previously reported (7.5x/8.51x slower), but the parallel version remains slower overall (roughly 2x for the elementwise operation, 2-3x for the dot product) — these build-configuration changes only affect code generation, not the per-call `std::thread` spawn/join overhead, which is the dominant cost identified for the parallel path. That overhead is unrelated to build configuration and is left for a follow-up change (e.g. a persistent thread pool).