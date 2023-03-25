CPPFLAGS=-ggdb -Wall -Wextra -fno-rtti -fno-exceptions -std=c++11 -O3 -static

all: md5 md5_bmi1 md5_avx512

md5: md5.cpp md5-x86-asm.h
	$(CXX) $(CPPFLAGS) -o $@ md5.cpp
md5_bmi1: md5.cpp md5-x86-asm.h
	$(CXX) $(CPPFLAGS) -mbmi -o $@ md5.cpp
md5_avx512: md5.cpp md5-x86-asm.h
	$(CXX) $(CPPFLAGS) -mbmi -mavx512vl -o $@ md5.cpp
