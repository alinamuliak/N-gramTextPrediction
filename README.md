# N-gram Text Prediction

## About the project
The aim of the project is to develop an application
for extracting N-grams from the text, which can work with unlimited text size
and quickly and efficiently use available resources.

The application works in two modes: **training** and **prediction**.
The user sets the set of texts for learning, and also selects certain settings
of the analysis (1-gram, 2-gram, 3-gram, parallelization settings). After the training is completed,
user can enter the prediction mode and test it.

### Prerequisites
- `CMake`
- `boost`

## Usage
1. Clone the repository.
2. Set the desired attributes in `.cfg` file.
3. Train the model:
    - compile the program with `./compile.sh -O`
    - run `команда`
4. Test model in the prediction mode.
    - run `команда2`
    - after the preprocessing for prediction is done, the input from user will be requested

### `.cfg` file settings
```
indir = "../data"                       # source directory for files

ngram_par = 3                           # parameter n
option = 0                              # 0 for counting and 1 for word prediction
word_num = 3                            # number of words to predict

out_prob = "../results/out_prob.txt"    # file to save result probabilities
out_ngram = "../results/out_words.txt"  # file to save result n-gram and following words

index_threads = 2                       # threads used for indexing
merge_threads = 2                       # threads used for merging
prediction_threads = 4                  # threads for prediction

files_queue_s = 1000000                 # amount of max file paths in files_queue
strings_queue_s = 1000000000            # max size of queue of strings in bytes
merge_queue_s = 10000                   # max amount of elements in merging queue

allowed_ext = .txt .zip                 # extensions to index (supported only .zip and .txt)
```


## Roadmap
- [x] Implement послідовний training algorithm
- [x] Implement послідовний prediction algorithm
- [x] Implement parallel training
- [x] Implement parallel processing for prediction
- [ ] Add processing of unknown words
- [ ] Build comparing plots for different settings

# Contributors:
- [Alina Muliak](https://github.com/alinamuliak)
- [Oleksandra Stasiuk](https://github.com/oleksadobush)
- [Anastasiia Tkachyshyn](https://github.com/tkachyshyn)



