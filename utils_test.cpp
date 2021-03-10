//
// Created by ctoast on 2021/3/9.
//

#include <iostream>

#include "utils.h"
#include "gtest/gtest.h"

TEST(TESTCASE,test1){
    char path[]="./test2.txt";
    writeFile(path,"abc");
    char* data=readFile(path,3);
    EXPECT_STREQ(data,"abc");
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
