#include "hashensemble.h"
#if defined(USE_XXH) || defined(USE_WYH)
#include "MurmurHash3.h"
#include "Spooky.h"
#include "wyhash.h"
#include "xxh3.h"
#endif 

#include "simple_timer.h"
#include "random.h"
#include <bitset>
#include <iostream>
#include <boost/dynamic_bitset.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>

#include <fmt/format.h>
#include "utils.h"

using namespace fmt::literals;
using namespace nonstd;

// global variables
namespace g {
#ifdef VERBOSE_LEVEL
constexpr int verbose_level = VERBOSE_LEVEL;
#else
constexpr int verbose_level = 1;
#endif
}

double collision_test(const std::vector<uint64_t> &input, size_t n_bins, HashEnsemble &h) {
  print_if(fmt::format("Collision Test (hash family: {hash}, key type = {key_t}) ... ",
                       "hash"_a = h.name(),
                       "key_t"_a = "64-bit integer"), g::verbose_level > 0);
  boost::dynamic_bitset<> bitmap(n_bins); // all 0's by default
  for (const auto key: input) bitmap[h(key) % n_bins] = true;
  println_if("Done!", g::verbose_level > 0);
  return static_cast<double>(input.size() - bitmap.count()) / n_bins;
}

inline std::pair<double, double> speed_test(const std::vector<uint64_t> &input, HashEnsemble &h) {
  print_if(fmt::format("Speed Test (hash family: {hash}, key type = {key_t}) ... ",
                       "hash"_a = h.name(),
                       "key_t"_a = "64-bit integer"), g::verbose_level > 0);

  constexpr unsigned seed = 142857u;
  const size_t count = input.size();
  SimpleTimer timer;
  double exec_time, base_time, raw_exec_time = 0;
  static uint64_t dummy __attribute__((used)) = 0; // prevent from removing by compiler

  std::random_device rd;
  dummy = rd();

  timer.restart();
  for (const auto key: input) dummy ^= key;
  base_time = timer.elapsed();

  print_if(fmt::format("base time: {:.2f} ... ", base_time), g::verbose_level > 1);

  timer.restart();
  for (const auto key: input) dummy ^= h(key);
  exec_time = timer.elapsed();

  print_if(fmt::format("exec time: {:.2f} ... ", exec_time), g::verbose_level > 1);

  exec_time -= base_time;

  switch (h.type()) {

    case HashFamily::XXH: {
      timer.restart();
      for (const auto key: input) dummy ^= XXH3_64bits_withSeed((void *) &key, sizeof(key), seed);
      raw_exec_time = timer.elapsed();
      break;
    }
    case HashFamily::WYH: {
      timer.restart();
      for (const auto key: input) dummy ^= wyhash((void *) &key, sizeof(key), seed, _wyp);
      raw_exec_time = timer.elapsed();
      break;
    }
    case HashFamily::MURMUR: {
      uint64_t out;
      timer.restart();
      for (const auto key: input) {
        MurmurHash3_x86_32((void *) &key, sizeof(key), seed, (void *) &out);
        dummy ^= out;
      }
      raw_exec_time = timer.elapsed();
      break;
    }
    case HashFamily::SPOOKY: {
      timer.restart();
      for (const auto key: input) dummy ^= SpookyHash::Hash64((void *) &key, sizeof(key), seed);
      raw_exec_time = timer.elapsed();
      break;
    }
  }
  print_if(fmt::format("raw exec time: {:.2f} ... ", raw_exec_time), g::verbose_level > 1);

  raw_exec_time -= base_time;
  println_if("Done!", g::verbose_level > 0);
  return {static_cast<double>(count) / exec_time, static_cast<double>(count) / raw_exec_time};
}

std::vector<size_t> get_test_ratios() {
  static std::vector<size_t> _ratios = {1, 2, 5, 10};
  return _ratios;
}

#if USE_XXH || USE_WYH
void test_all_in_one(const std::vector<uint64_t> &input, std::ostream &os = std::cout) {
  println_if(" \t\t-----> !!Start Test!! <------", g::verbose_level > 0);
  HashEnsemble h{};
  auto speed = speed_test(input, h);
  std::vector<std::string> collisions;
  for (const auto &ratio: get_test_ratios()) {
    auto col = collision_test(input, input.size() * ratio, h);
    collisions.emplace_back(fmt::format("{:.2f}", col * 100));
  }
  os << fmt::format("{name},{key_t},{speed:.2f},{raw_speed:.2f},{degradation:.2f},{collisions}\n",
                    "name"_a = h.name() + "_ct",
                    "key_t"_a = "64-bit integer",
                    "speed"_a = speed.first,
                    "raw_speed"_a = speed.second,
                    "degradation"_a= (speed.second - speed.first) / speed.second * 100.0,
                    "collisions"_a = fmt::join(collisions, ","));
  println_if(" \t\t-----> !!Done!! <------", g::verbose_level > 0);
}
#else
void test_all_in_one(const std::vector<uint64_t> &input, std::ostream &os = std::cout) {
  println_if(" \t\t-----> !!Start Test!! <------", g::verbose_level > 0);

  for (auto tid = static_cast<int>(HashFamily::XXH); tid <= static_cast<int>(HashFamily::SPOOKY); tid++) {
    auto t = HashFamily(tid);
    HashEnsemble h(t);
    auto speed = speed_test(input, h);
    std::vector<std::string> collisions;
    for (const auto &ratio: get_test_ratios()) {
      auto col = collision_test(input, input.size() * ratio, h);
      collisions.emplace_back(fmt::format("{:.2f}", col * 100));
    }
    os << fmt::format("{name},{key_t},{speed:.2f},{raw_speed:.2f},{degradation:.2f},{collisions}\n",
                      "name"_a = h.name(),
                      "key_t"_a = "64-bit integer",
                      "speed"_a = speed.first,
                      "raw_speed"_a = speed.second,
                      "degradation"_a= (speed.second - speed.first) / speed.second * 100.0,
                      "collisions"_a = fmt::join(collisions, ","));
  }

  println_if(" \t\t-----> !!Done!! <------", g::verbose_level > 0);
}
#endif

template<typename T>
void archive_data(const std::vector<T> &data, const char *ofilename = "test_data.dat") {
  std::ofstream f(ofilename, std::ofstream::binary);
  boost::archive::binary_oarchive ar(f);
  ar & data;
}

template<typename T>
void load_data(std::istream &inf, std::vector<T> &data) {
  boost::archive::binary_iarchive ar(inf);
  ar & data;
}

int main(int argc, char **argv) {
  auto count = size_t(1e6);
  if (argc == 2) {
    std::string s{argv[1]};
    try {
      if (s.find('e') != std::string::npos)
        count = size_t(std::stof(s));
      else
        count = std::stoul(s);
    } catch (const std::invalid_argument &e) {
      std::cerr << fmt::format("InvalidArgumentError: {}", e.what()) << std::endl;
      exit(1);
    } catch (const std::out_of_range &e) {
      std::cerr << fmt::format("OutOfRangeError: {}", e.what()) << std::endl;
      exit(1);
    } catch (const std::exception &e) {
      std::cerr << fmt::format("UnknownError: {}", e.what()) << std::endl;
      exit(1);
    }
  } else if (argc > 2) {
    std::cerr
        << fmt::format("Usage: {0} [NUM_KEYS_TO_TEST]\n\t\t\twhere NUM_KEYS_TO_TEST is a positive integer", argv[0])
        << std::endl;
    exit(1);
  }
  println_if(fmt::format("Number tests : {}", count), g::verbose_level > 0);
  std::vector<uint64_t> data;
  const char *data_filename = "test_data.dat";
  const char *result_filename = "bench_results.csv";
  std::ifstream inf(data_filename);
  if (inf.is_open()) {
    print_if(fmt::format("Loading test data from {} ... ", data_filename), g::verbose_level > 0);
    load_data(inf, data);
    inf.close();
    println_if("Done!", g::verbose_level > 0);
  } else {
    print_if(fmt::format("Generating random test data and save to file {} ... ", data_filename), g::verbose_level > 0);
    data = GenerateRandomSet64(count);
    archive_data(data);
    println_if("Done!", g::verbose_level > 0);
  }

  std::ofstream os(result_filename, std::ios::app);
  std::vector<std::string> collision_cases;
  for (const auto r: get_test_ratios()) {
    collision_cases.emplace_back(fmt::format("collision_{} (%)", r));
  }
  long pos = os.tellp();
  if (pos == 0)
    os
        << fmt::format(
            "name,key type,speed (million keys per second),raw_speed (million keys per second),degradation (%),{collisions}\n",
            "collisions"_a = fmt::join(collision_cases, ","));
  test_all_in_one(data, os);
  os.close();

  return 0;
}
