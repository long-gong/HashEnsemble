#ifndef ENSEMBLEHASH_H_
#define ENSEMBLEHASH_H_

#if USE_XXH
#include "xxh3.h"
#elif USE_WYH
#include "wyhash.h"
#else
#include "MurmurHash3.h"
#include "Spooky.h"
#include "wyhash.h"
#include "xxh3.h"
#endif 

#include <string>
#include <type_traits>

namespace nonstd {

enum class HashFamily : int {
  XXH = 1,    /* xxHash https://github.com/Cyan4973/xxHash */
  WYH,    /* wyhash https://github.com/wangyi-fudan/wyhash */
  MURMUR, /* MurmurHash https://github.com/aappleby/smhasher */
  SPOOKY  /* SpookyHash https://github.com/andikleen/spooky-c */
};

/* 64-bit hash (except for MurmurHash3) */
class HashEnsemble {
 public:
#if USE_XXH
  explicit HashEnsemble(unsigned seed = 142857u)
        : family_(HashFamily::XXH), seed_(seed) {}
#elif USE_WYH
  explicit HashEnsemble(unsigned seed = 142857u)
      : family_(HashFamily::WYH), seed_(seed) {}
#else
  explicit HashEnsemble(HashFamily family, unsigned seed = 142857u)
      : family_(family), seed_(seed) {}
  HashEnsemble() : HashEnsemble(HashFamily::WYH) {}
#endif

  inline std::string name() const {
#if USE_XXH
    return "xxhash";
#elif USE_WYH
    return "wyhash";
#else
    switch (family_) {
      case HashFamily::XXH: return "xxhash";
      case HashFamily::WYH: return "wyhash";
      case HashFamily::MURMUR: return "MurmurHash3";
      case HashFamily::SPOOKY: return "Spooky";
      default:return "Unknown";
    }
#endif
  }

  inline HashFamily type() const {
    return family_;
  }

  template<typename Key>
  inline uint64_t operator()(Key key) const {
#if USE_XXH
    return XXH3_64bits_withSeed((void *) &key, sizeof(key), seed_);
#elif USE_WYH
    return wyhash((void *) &key, sizeof(key), seed_, _wyp);
#else
    // USE runtime choice
    static_assert(std::is_integral<Key>::value, "Only Integer Type");
    switch (family_) {
      case HashFamily::XXH:return XXH3_64bits_withSeed((void *) &key, sizeof(key), seed_);
      case HashFamily::WYH:return wyhash((void *) &key, sizeof(key), seed_, _wyp);
      case HashFamily::MURMUR: {
        uint32_t out;
        MurmurHash3_x86_32((void *) &key, sizeof(key), seed_, (void *) &out);
        return out;
      }
      case HashFamily::SPOOKY:return SpookyHash::Hash64((void *) &key, sizeof(key), seed_);
      default:return 0;
    }
#endif
  }

  template<typename RandIt>
  inline uint64_t operator()(RandIt first, RandIt last) const {
    return operator()(reinterpret_cast<unsigned char *>(&*first),
                      std::distance(first, last) * sizeof(*first));
  }

  inline uint64_t operator()(const std::string &key) const {
    return operator()(key.c_str(), key.size());
  }

  inline uint64_t operator()(const char *key_start, size_t key_len) const {
#if USE_XXH
    return XXH3_64bits_withSeed((void *) key_start, key_len, seed_);
#elif USE_WYH
    return wyhash((void *) key_start, key_len, seed_, _wyp);
#else
    switch (family_) {
      case HashFamily::XXH:return XXH3_64bits_withSeed((void *) key_start, key_len, seed_);
      case HashFamily::WYH:return wyhash((void *) key_start, key_len, seed_, _wyp);
      case HashFamily::MURMUR: {
        uint32_t out;
        MurmurHash3_x86_32((void *) key_start, key_len, seed_, (void *) &out);
        return out;
      }
      case HashFamily::SPOOKY:return SpookyHash::Hash64((void *) key_start, key_len, seed_);
      default:return 0;
    }
#endif
  }

  inline uint64_t operator()(const unsigned char *key_start, size_t key_len) const {
#if USE_XXH
    return XXH3_64bits_withSeed((void *) key_start, key_len, seed_);
#elif USE_WYH
    return wyhash((void *) key_start, key_len, seed_, _wyp);
#else
    switch (family_) {
      case HashFamily::XXH:return XXH3_64bits_withSeed((void *) key_start, key_len, seed_);
      case HashFamily::WYH:return wyhash((void *) key_start, key_len, seed_, _wyp);
      case HashFamily::MURMUR: {
        uint32_t out;
        MurmurHash3_x86_32((void *) key_start, key_len, seed_, (void *) &out);
        return out;
      }
      case HashFamily::SPOOKY:return SpookyHash::Hash64((void *) key_start, key_len, seed_);
      default:return 0;
    }
#endif
  }

 private:
  const HashFamily family_;
  const unsigned seed_;
};

}  // namespace nonstd
#endif  // ENSEMBLEHASH_H_
