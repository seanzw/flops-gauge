#######################################################################
#                Single Core Peak Flops Gauge
#######################################################################

peak_flops_%.exe: src/peak_flops.c src/%.h
	gcc $^ -Dkernel_config=$* -O3 -std=gnu99 -march=native -o $@
	# ./$@

omp_peak_flops_%.exe: src/peak_flops.c src/%.h
	gcc $^ -Dkernel_config=$* -DUSE_OPENMP -fopenmp -O3 -std=gnu99 -march=native -o $@
	# ./$@


# AOCL_BLIS_INSTALL_PATH=/data/zhengrong/aocl/blis-install
# AOCL_LIB_PATH   := $(AOCL_BLIS_INSTALL_PATH)/lib
# AOCL_INC_PATH   := $(AOCL_BLIS_INSTALL_PATH)/include/blis
# AOCL_SHARE_PATH := $(AOCL_BLIS_INSTALL_PATH)/share/blis

# AOCL_INSTALL_PATH=/data/zhengrong/aocl/4.1.0/aocc

#######################################################################
#                 GEMM Gauge
#######################################################################

ifndef AOCL_INSTALL_PATH
$(warning Using default AOCL_INSTALL_PATH)
AOCL_INSTALL_PATH=/usr/local/lib
endif

AOCL_LIB_PATH   := $(AOCL_INSTALL_PATH)/lib_ILP64
AOCL_INC_PATH   := $(AOCL_INSTALL_PATH)/include_ILP64
AOCL_SHARE_PATH := $(AOCL_INSTALL_PATH)/lib_ILP64

gemm_cblas_aocl.exe: gemm_cblas.c *.h
	gcc $^ -O3 -I$(AOCL_INC_PATH) -L$(AOCL_LIB_PATH) -lblis-mt -lm -fopenmp -o $@

gemm_cblas_aocl_single.exe: gemm_cblas.c *.h
	gcc $^ -O3 -DSINGLE_THREAD -I$(AOCL_INC_PATH) -L$(AOCL_LIB_PATH) -lblis -lm -o $@

gemm_cblas_aocl_prof.exe: gemm_cblas_prof.c *.h
	gcc $^ -O3 -I$(AOCL_INC_PATH) -L$(AOCL_LIB_PATH) -lblis-mt -lm -fopenmp -o $@

gemm_cblas_aocl_single_prof.exe: gemm_cblas_prof.c *.h
	gcc $^ -O3 -I$(AOCL_INC_PATH) -L$(AOCL_LIB_PATH) -lblis -lm -o $@

#######################################################################
#                 GEMM MKL Gauge
#######################################################################

ifndef MKL_INSTALL_PATH
$(warning Using default MKL_INSTALL_PATH)
MKL_INSTALL_PATH=/usr/local/lib
endif

MKL_LIB_PATH   := $(MKL_INSTALL_PATH)/lib
MKL_INC_PATH   := $(MKL_INSTALL_PATH)/include
MKL_SHARE_PATH := $(MKL_INSTALL_PATH)/lib

gemm_cblas_mkl.exe: src/gemm_cblas.c src/*.h
	gcc $< -O3 -DUSE_MKL -I$(MKL_INC_PATH) -L$(MKL_LIB_PATH) -lmkl_rt -lpthread -lm -ldl -o $@

gemm_cblas_mkl_single.exe: gemm_cblas.c *.h
	gcc $^ -O3 -DSINGLE_THREAD -DUSE_MKL -I$(MKL_INC_PATH) -L$(MKL_LIB_PATH) -lmkl_rt -lpthread -lm -ldl -o $@

gemm_cblas_measure: gemm_cblas_aocl.exe gemm_cblas_mkl.exe
	LD_LIBRARY_PATH=$(AOCL_LIB_PATH) ./gemm_cblas_aocl.exe
	mv dense_mm.csv ../dense_mm_aocl.csv
	LD_LIBRARY_PATH=$(MKL_LIB_PATH) ./gemm_cblas_mkl.exe
	mv dense_mm.csv ../dense_mm_mkl.csv

clean:
	rm *.exe
