#include <gtest/gtest.h>
#include <vector>
#include "../mymemorypool.h"

class PoolTestEnvironment : public testing::Environment {
 public:
  void SetUp() override {}
  void TearDown() override {
    system("ipcrm -M 0x2345");
    system("ipcrm -M 0x2346");
    system("ipcrm -M 0x2347");
    system("ipcrm -M 0x2348");
    system("ipcrm -M 0x2349");
    system("ipcrm -M 0x234A");
    system("ipcrm -M 0x234B");
    system("ipcrm -M 0x234C");
    system("ipcrm -M 0x234D");
  }
};

TEST(UnInitializedTest, HandlesUnInitializedInput) {
  MyMemoryPool<20> pool;
  ASSERT_EQ(NULL, pool.apply());
  ASSERT_EQ(NULL, pool.get(0));
  ASSERT_EQ(-1, pool.getstatus(0));
  ASSERT_EQ(-1, pool.empty());
  ASSERT_EQ(-1, pool.size());
  ASSERT_EQ(-1, pool.full());
}

//test function apply & release
TEST(BasicTest, HandlesBasic) {
  MyMemoryPool<20> pool;
  pool.init(0x2345, 20);
  std::vector<void*> v;
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(20, v.size());
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(20, v.size());
  for (int i = 0; i < 10; ++i) {
    pool.release(v.back());
    v.pop_back();
  }
  ASSERT_EQ(10, v.size());
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(20, v.size());
}

TEST(GetTest, HandlesGetValid) {
  MyMemoryPool<20> pool;
  pool.init(0x2346, 20);
  std::vector<void*> v;
  v.push_back(pool.apply());
  ASSERT_EQ(v.back(), pool.get(0));
  ASSERT_NE(nullptr, pool.get(1));
  ASSERT_EQ(reinterpret_cast<uint8_t*>(pool.get(10)) + 40, reinterpret_cast<uint8_t*>(pool.get(12)));
}

TEST(GetTest, HandlesGetInvalid) {
  MyMemoryPool<20> pool;
  pool.init(0x2347, 20);
  ASSERT_EQ(NULL, pool.get(-1));
  ASSERT_EQ(NULL, pool.get(20));
  ASSERT_EQ(NULL, pool.get(21));
}

TEST(GetStatusTest, HandlesGetValid) {
  MyMemoryPool<20> pool;
  pool.init(0x2348, 20);
  std::vector<void*> v;
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(20, v.size());
  for (int i = 0; i < 20; ++i) {
    ASSERT_EQ(1, pool.getstatus(i)) << i;
  }
  for (int i = 0; i < 20; ++i) {
    pool.release(v.back());
    v.pop_back();
  }
  for (int i = 0; i < 20; ++i) {
    ASSERT_EQ(0, pool.getstatus(i));
  }
  v.push_back(pool.apply());
  ASSERT_EQ(1, pool.getstatus(0));
}

TEST(GetStatusTest, HandlesGetInvalid) {
  MyMemoryPool<20> pool;
  pool.init(0x2349, 20);
  ASSERT_EQ(-1, pool.getstatus(-1));
  ASSERT_EQ(-1, pool.getstatus(20));
  ASSERT_EQ(-1, pool.getstatus(21));
}

TEST(EmptyTest, HandlesEmpty) {
  MyMemoryPool<20> pool;
  pool.init(0x234A, 20);
  ASSERT_EQ(1, pool.empty());
  void* pos = pool.apply();
  ASSERT_EQ(0, pool.empty());
  pool.release(pos);
  ASSERT_EQ(1, pool.empty());
}

TEST(SizeTest, HandlesSize) {
  MyMemoryPool<20> pool;
  pool.init(0x234B, 20);
  ASSERT_EQ(0, pool.size());
  std::vector<void*> v;
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
    ASSERT_EQ(i + 1, pool.size());
  }
}

TEST(FullTest, HandlesFull) {
  MyMemoryPool<20> pool;
  pool.init(0x234C, 5);
  ASSERT_EQ(0, pool.full());
  std::vector<void*> v;
  for (int i = 0; i < 5; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(1, pool.full());
  pool.release(v.back());
  ASSERT_EQ(0, pool.full());
}

TEST(ClearTest, HandlesClear) {
  MyMemoryPool<20> pool;
  pool.init(0x234D, 20);
  std::vector<void*> v;
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(0, pool.empty());
  ASSERT_EQ(1, pool.full());
  ASSERT_EQ(20, pool.size());
  v.clear();
  pool.clear();
  ASSERT_EQ(1, pool.empty());
  ASSERT_EQ(0, pool.full());
  ASSERT_EQ(0, pool.size());
  for (int i = 0; i < 20; ++i) {
    ASSERT_EQ(0, pool.getstatus(i));
  }
  for (int i = 0; i < 20; ++i) {
    void* pos = pool.apply();
    if (pos != NULL)
      v.push_back(pos);
  }
  ASSERT_EQ(20, v.size());
  ASSERT_EQ(0, pool.empty());
  ASSERT_EQ(1, pool.full());
  ASSERT_EQ(20, pool.size());
}


int main(int argc, char **argv) {
  ::testing::AddGlobalTestEnvironment(new PoolTestEnvironment);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}