# Tweet Generator Using Markov Chains

Generate unique tweets by analyzing an existing dataset using **Markov Chains**, a powerful technique in Natural Language Processing (NLP). This project demonstrates how to use probabilistic models to produce synthetic text while preserving the tone and structure of the original data.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [Compilation](#compilation)
  - [Running](#running-the-program)
- [How It Works](#how-it-works)

## Introduction
This project showcases the use of Markov Chains to generate new tweets based on a dataset of existing tweets. It's designed to explore the basics of text generation and understand how sequential probabilistic models work in NLP.

## Features
- Create new sentences that mimic the style of the input dataset.
- Simple and intuitive implementation of Markov Chains.
- Easily customizable to work with any text dataset.
- Learn the foundations of probabilistic modeling in NLP.

## Getting Started

### Prerequisites
- C Compiler (e.g., GCC).
- A text file containing tweets or any dataset in plain text format.

### Installation
- Clone the repository:
```
git clone https://github.com/your-username/tweet-generator-markov.git  
cd tweet-generator-markov
```
-  Add your dataset file (e.g., tweets.txt) to the directory.

## Usage

### Compilation
- Compile the project using the following command:
```
gcc tweets_generator.c markov_chain.c linked_list.c -o tweets_generator
```

### Running the Program
- Run the executable:
```
/tweets_generator <seed> <number of tweets to generate> <path to input file> <amount of words to read>
```
Example:
```
./tweets_generator 454545 10 tweets.txt 150
```

## How It Works

- The input dataset is preprocessed to remove unnecessary characters and split into tokens.
- A Markov Chain is constructed, where each word serves as a state, and transitions between states are based on probabilities derived from the dataset.
- The model generates new sentences by traversing the Markov Chain probabilistically.
  
