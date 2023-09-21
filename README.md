# flops-gauge

These are the utility tool I used to measure the peak flops.
Currently it only supports x86. Pull requests for other ISAs
are welcome.

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