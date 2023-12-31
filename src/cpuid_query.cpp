#include <asm/prctl.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <map>

struct CPUIdResult {
  int eax;
  int ebx;
  int edx;
  int ecx;
};

std::map<int, std::map<int, CPUIdResult>> allResults;

CPUIdResult query(int i, int j) {

  int eax, ebx, ecx, edx;

  __asm__("mov %4, %%eax\n\t"
          "mov %5, %%ecx\n\t"
          "cpuid\n\t"
          "mov %%eax, %0\n\t"
          "mov %%ebx, %1\n\t"
          "mov %%edx, %2\n\t"
          "mov %%ecx, %3\n\t"
          : "=r"(eax), "=r"(ebx), "=r"(edx), "=r"(ecx)
          : "r"(i), "r"(j)
          : "%eax", "%ebx", "ecx", "edx");

  CPUIdResult result = {
      .eax = eax,
      .ebx = ebx,
      .edx = edx,
      .ecx = ecx,
  };

  printf("Query %#x eax %#x ebx %#x edx %#x ecx %#x.\n", i, eax, ebx, edx, ecx);

  // Update all results.
  allResults
      .emplace(std::piecewise_construct, std::forward_as_tuple(i),
               std::forward_as_tuple())
      .first->second.emplace(j, result);

  return result;
}

CPUIdResult xgetbv(int i) {

  int eax, ebx, ecx, edx;

  __asm__("mov %2, %%ecx\n\t"
          "xgetbv\n\t"
          "mov %%eax, %0\n\t"
          "mov %%ecx, %1\n\t"
          : "=r"(eax), "=r"(edx)
          : "r"(i)
          : "%eax", "%ebx", "ecx", "edx");

  CPUIdResult result = {
      .eax = eax,
      .edx = edx,
  };

  printf("XGETBV %#x eax %#x edx %#x.\n", i, eax, edx);

  return result;
}

void query_xcomp() {

  long long result = 0;

#define ARCH_GET_XCOMP_PERM 0x1022
#define ARCH_REQ_XCOMP_PERM 0x1023
#define XFEATURE_XTILECFG 17
#define XFEATURE_XTILEDATA 18

  if (syscall(SYS_arch_prctl, ARCH_GET_XCOMP_PERM, &result) == 0) {
    printf("Succ ARCH_GET_XCOMP_PERM %d, %#llx\n", ARCH_GET_XCOMP_PERM, result);
  } else {
    printf("Fail query xcomp %d\n", ARCH_GET_XCOMP_PERM);
  }
  if (syscall(SYS_arch_prctl, ARCH_REQ_XCOMP_PERM, XFEATURE_XTILEDATA)) {
    printf("\n Fail to do XFEATURE_XTILEDATA \n\n");
  } else {
    printf("\n TILE DATA USE SET - OK \n\n");
  }
  if (syscall(SYS_arch_prctl, ARCH_GET_XCOMP_PERM, &result) == 0) {
    printf("Succ ARCH_GET_XCOMP_PERM %d, %#llx\n", ARCH_GET_XCOMP_PERM, result);
  } else {
    printf("Fail query xcomp %d\n", ARCH_GET_XCOMP_PERM);
  }
}

int main() {

  auto result = query(0, 0);

  printf("%s\n", (char *)&result.ebx);

  int max_func = result.eax;
  for (int i = 1; i <= max_func; ++i) {
    query(i, 0);
  }

  result = query(0x80000000, 0);
  int max_ext_func = result.eax;
  for (int i = 0x80000001; i <= max_ext_func; ++i) {
    query(i, 0);
  }

  // Deterministic cache parameters. Invalid eax = ebx = ecx = edx = 0
  for (int i = 0; true; ++i) {
    result = query(0x4, i);
    if (result.eax == 0 && result.ebx == 0 && result.ecx == 0 &&
        result.edx == 0) {
      break;
    }
  }

  // Core hierarchy. Invald: eax = ebx = 0
  for (int i = 0; true; ++i) {
    result = query(0xb, i);
    if (result.eax == 0 && result.ebx == 0) {
      break;
    }
  }

  // CPU extended features.
  query(0x7, 0x1);
  query(0x7, 0x2);

  query(0xd, 0x1);
  query(0xd, 0x2);
  query(0xd, 0x3);
  query(0xd, 0x4);
  query(0xd, 0x5);
  query(0xd, 0x6);
  query(0xd, 0x7);

  // for (int i = 0; i <= 0x14; ++i) {
  //   query(0x80000001, i);
  //   query(0x80000008, i);
  // }

  xgetbv(0);

  query_xcomp();

  printf("All results.\n");
  printf("{\n");
  for (const auto &f : allResults) {
    auto i = f.first;
    printf("  { %#x, {", i);
    for (const auto &g : f.second) {
      const auto &j = g.first;
      const auto &result = g.second;
      printf("{%#x, {%#x, %#x, %#x, %#x}}, ", j, result.eax, result.ebx,
             result.edx, result.ecx);
    }
    printf("}},\n");
  }
  printf("}\n");

  return 0;
}