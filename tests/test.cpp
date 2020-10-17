// Copyright 2020 Dmitry Kalinin <x.shreddered.x@gmail.com>

#include <gtest/gtest.h>

#include <shared_ptr.hpp>

#include <string>

struct Student {
    std::string name;
    std::string group;
    explicit Student(const std::string& _name, const std::string& _group)
        : name(_name), group(_group) {}
};

TEST(SharedPtr, CopyMove) {
    SharedPtr<Student> ptr = new Student("Dmitry", "IU8-31");
    // copy ctor
    auto ptr1 = ptr;
    SharedPtr<Student> ptr3;
    // copy assignment
    ptr3 = ptr;
    // move ctor
    auto ptr4 = std::move(ptr);
    // move assignment
    ptr3 = std::move(ptr4);
}

TEST(SharedPtr, use_count) {
    SharedPtr<Student> ptr = new Student("Dmitry", "IU8-31"),
                       ptr1 = ptr;
    // two smart pointers
    EXPECT_EQ(ptr.use_count(), 2);
    ptr.reset();
    // should be only one
    EXPECT_EQ(ptr1.use_count(), 1);
    auto tmp = std::move(ptr1);
    // nothing ever changed because of move
    EXPECT_EQ(tmp.use_count(), 1);
}

TEST(SharedPtr, Operators) {
    SharedPtr<Student> ptr = new Student("Dmitry", "IU8-31");
    EXPECT_TRUE(ptr);
    Student s(*ptr);
    EXPECT_EQ(s.name, "Dmitry");
    EXPECT_EQ(s.group, "IU8-31");

    EXPECT_EQ(ptr->name, "Dmitry");
    EXPECT_EQ(ptr->group, "IU8-31");

    Student* raw = ptr.get();
    EXPECT_EQ(raw->name, "Dmitry");
    EXPECT_EQ(raw->group, "IU8-31");
}
