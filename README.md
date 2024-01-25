
# Associative arrays implemented using tries

## Name: Toaha Ayman Shams
## ID: 1210216

This `README.md` file describes an implementation of associative
arrays using *tries*, and evaluates when these are superior to
the use of a *hash table* based implementation.

# External resources used:

No external resources needed

# State of the current implementation

Works properly

# Hash tables:

## Strengths:

* Hash tables provide fast average-case performance for operations like insert, delete, and search, typically O(1).
* Also since they use an array based structure it results in better memory management

## Weaknesses

* Hash tables do not maintain any order of elements.
* Hash tables can cause poor performance if too much cluthering occurs


# Tries:

## Strengths:

* Tries can retrieve elements in lexicographical order easily.

## Weaknesses

* They are mainly suited for strings or data types that can be represented as strings.



# Summary
In summary if we are dealing with various types of data or if the data needed to be stored is not that big then Hash tables are better suited.
On the other hand if we are dealing datas which are mostly strings or large amounts of data and we want it to be ordered, Tries would do a better jobb over Hash tables.


## Conditions that would make a hash table appropriate:

Choose a hash table when *ALL* of the conditions in the **required**
list are true and any of the conditions in the **preferred** list
are true.

### Hash table **required** conditions
* Various data types

### Hash table **preferred** conditions
* No ordering required
* Small Dataset



## Conditions that would make a trie appropriate:

Choose a trie when *ALL* of the conditions in the **required**
list are true and any of the conditions in the **preferred** list
are true.

### Trie **required** conditions
* String data types

### Trie **preferred** conditions
* Lexicographical Ordering
* Large Dataset


