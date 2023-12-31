# flops-gauge

These are the utility tool I used to measure the peak flops.
Currently it only supports x86. Pull requests for other ISAs
are welcome.

### Monitor CPU Frequency

I found this command to monitor the CPU frequency very helpful
for me to relate GFlops back to CPU cycles and latency.

```bash
watch -n 1 'cat /proc/cpuinfo | grep Hz'
```

### Single Core Peak Flops

I use inline assembly to measurethe peak single core flops.
The configuration is specified via `[isa_feature]_[data_type]`.
For example, to test the peak avx2 flops for float type:

```bash
make peak_flops_avx2_float.exe
```

Which will build the binary and run it. Check the `src` folder
for available configuration. Notice that if your CPU does not
support the ISA feature, the compilation will fail.

### Multiple Core Peak Flops

We also support measure multi-core peak flops, parallelized
with OpenMP. Simply add `omp_` prefix to the single core target.
The binary takes one more argument to specify the number of
threads you want to measure. For example:

```bash
make omp_peak_flops_avx512_float.exe
./omp_peak_flops_avx512_float.exe [threads]
```

### GEMM with MKL

We use anaconda to manage the MKL installation. First create
a new environment and install mkl.

```bash
conda create --name mkl python=3
conda activate mkl
conda install -c intel mkl mkl-include mkl-static
```

Now set the `MKL_INSTALL_PATH` to the conda environment folder.

```bash
export MKL_INSTALL_PATH=/home/{user}/anaconda3/envs/mkl
```

Now we can create the gemm profiling binary.

```bash
make gemm_cblas_mkl.exe
```

To run the binary, you need to set the LD_LIBRARY_PATH

```bash
LD_LIBRARY_PATH=$MKL_INSTALL_PATH/lib ./gemm_cblas_mkl.exe
```

This will run a variety of matrix sizes (M, N, K)
multiple times and pick the best throughput. It will
dump the final results in `dense_mm.csv`.
