#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>
#include <boost/container/flat_map.hpp>


struct Pod {
  uint32_t item1{0};
  uint32_t item2{0};
  time_t timestamp{0};
};

using pod_vec = std::vector<Pod>;
class PodVectorContainer {
 public:
  PodVectorContainer() {
    for (uint16_t i = 0;i < 3; i++) {
      my_vec_.emplace_back(Pod());
    }
  }

 private:
  pod_vec my_vec_;
};

template <class T>
class PodMapContainer {
 public:
  PodMapContainer() {
    for (uint16_t i = 0; i < 3; i++) {
      my_map_[i] = Pod();
    }
  }

 private:
  T my_map_;
};



// conveniences
using pod_map = std::map<uint8_t, Pod>;
using pod_umap = std::unordered_map<uint8_t, Pod>;
using pod_fmap = boost::container::flat_map<uint8_t,Pod>;
using PMContainer = PodMapContainer<pod_map>;
using PUMContainer = PodMapContainer<pod_umap>;
using PFMContainer = PodMapContainer<pod_fmap>;
using pod_map_container_vec = std::vector<PMContainer>;
using pod_umap_container_vec = std::vector<PUMContainer>;
using pod_fmap_container_vec = std::vector<PFMContainer>;
using pod_vec_container_vec = std::vector<PodVectorContainer>;

static int i;
static void sigact(int sig, siginfo_t *siginfo, void *context) {
  std::cout << "it is now: " << i << "\n";
}

enum class RunType { vector, map, unordered_map, flat_map };

int main(int argc, char **argv) {
  struct sigaction act;

  memset(&act, '\0', sizeof(act));
  act.sa_sigaction = &sigact;
  /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not
   * sa_handler. */
  act.sa_flags = SA_SIGINFO;

  if (sigaction(SIGINT, &act, NULL) < 0) {
    std::cerr << "Sailed sigaction!\n";
  }

  pod_vec_container_vec pvvec;
  pod_map_container_vec pmvec;
  pod_umap_container_vec  pumvec;
  pod_fmap_container_vec  pfmvec;

  std::cout << "sizeof(PodVectorContainer): " << sizeof(PodVectorContainer)
            << " size of 1,000,000: " << sizeof(PodVectorContainer) * 10000000
            << "\n";
  std::cout << "sizeof(PMContainer): " << sizeof(PMContainer)
            << " size of 1,000,000: " << sizeof(PMContainer) * 10000000
            << "\n";
  std::cout << "sizeof(PUMContainer): " << sizeof(PUMContainer)
            << " size of 1,000,000: " << sizeof(PUMContainer) * 10000000
            << "\n";
  std::cout << "sizeof(PFMContainer): " << sizeof(PFMContainer)
            << " size of 1,000,000: " << sizeof(PFMContainer) * 10000000
            << "\n";
  std::cout << "size of Pod: " << sizeof(Pod)
            << " size of 1,000,000 * 3: " << sizeof(Pod) * 10000000 * 3 << "\n";

  RunType type = RunType::vector;
  if (argc > 1) {
    if (strcmp(argv[1], "map") == 0) {
      type = RunType::map;
    } else if (strcmp(argv[1], "unordered_map") == 0) {
      type = RunType::unordered_map;
    } else if (strcmp(argv[1], "flat_map") == 0) {
      type = RunType::flat_map;
  }

  if (type == RunType::vector) {
    std::cout << "vector mode!\n";
    for (i = 0; i < 10000000; i++) {
      if (i % 10000 == 0)
        std::cout << ".";
      pvvec.emplace_back(PodVectorContainer());
    }
  } else if (type == RunType::map) {
    std::cout << "map mode!\n";
    for (i = 0; i < 10000000; i++) {
      if (i % 10000 == 0)
        std::cout << ".";
      pmvec.emplace_back(PMContainer());
    }
  } else if (type == RunType::unordered_map) {
    std::cout << "unordered map mode!\n";
   for (i = 0; i < 10000000; i++) {
      if (i % 10000 == 0)
        std::cout << ".";
      pumvec.emplace_back(PUMContainer());
  } 
   }else if (type == RunType::flat_map) {
    std::cout << "flat map mode!\n";
   for (i = 0; i < 10000000; i++) {
      if (i % 10000 == 0)
        std::cout << ".";
      pfmvec.emplace_back(PFMContainer());
    }
  }

  std::cout << "done!\n";
  sleep(120);
}

}
