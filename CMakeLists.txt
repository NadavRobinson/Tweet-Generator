cmake_minimum_required(VERSION 3.28)
project(ex1 C)

set(CMAKE_C_STANDARD 17)

add_executable(ex1
        markov_chain.c
        linked_list.c
        tweets_generator.c
)
