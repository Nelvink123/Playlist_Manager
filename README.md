# Music Playlist Manager

A menu-driven C program that manages a music playlist using four data structures, one from each module of the Data Structures syllabus.

| Module | Concept | Used for |
|---|---|---|
| 1 | **Stack** | Undo the last removed song |
| 2 | **Circular Doubly Linked List** | The playlist itself |
| 3 | **Binary Search Tree** (keyed by rating) | Songs sorted by rating |
| 4 | **Hashing** (separate chaining) | Fast search by title |

## Features

1. Add a song (title, artist, rating 1-5)
2. Remove a song
3. Undo the last removal
4. Search a song by title
5. Display the playlist (circular order)
6. Display songs sorted by rating

## How to compile and run

```
gcc -Wall -o playlist_manager playlist_manager.c
./playlist_manager
```

---

## Introduction and problem statement

"Good morning. Our mini project is a **Music Playlist Manager** written in C.

Think about what a real music app has to do. It has to keep songs in an order, loop back to the first song after the last one, let you undo a mistaken delete, show songs ranked by rating, and find a song by name instantly.

No single data structure does all of this well. So instead of forcing everything into one array, we gave each job to the structure that is best at it. We used one structure from each module of our syllabus: a **Stack** from Module 1, a **Circular Doubly Linked List** from Module 2, a **Binary Search Tree** from Module 3, and **Hashing** from Module 4."

## The big idea: one song, three indexes

"Before going structure by structure, here is the key design idea.

There is exactly **one `Song` object in memory per song**. It holds the title, artist, rating, and two pointers, `next` and `prev`.

The BST, the hash table, and the undo stack do **not** store copies of songs. They only store **pointers** to that same `Song`. So the playlist, the tree, and the hash table are three different ways of organising the same data.

The benefit is that there is no duplicated data, no risk of the copies going out of sync, and each structure can be optimised for its own job."

## Structure 1: Circular Doubly Linked List (Module 2)

**What it is:** every `Song` node has a `next` and a `prev` pointer, and the last node points back to the first.

**Why we need it:**

- "A playlist grows and shrinks all the time. With an **array**, deleting a song from the middle means shifting every later element. With a linked list, we just change two pointers, so insertion and deletion at a known node are **O(1)**."
- "It is **doubly** linked, so we can go to the next song or the previous song. A singly linked list can only go forward."
- "It is **circular**, so after the last song, `next` takes us back to the first. That is repeat mode with no extra code. There is no `NULL` to check for."

**How it works in the code:**

- "When the first song is added, it points to itself: `s->next = s; s->prev = s`. It is circular from the very first insert."
- "For every later song, we find the tail using `playHead->prev`. That is only possible because the list is circular and doubly linked. Then we link the new node between the tail and the head. Adding at the end is **O(1)** with no traversal."
- "Because there is no `NULL`, we traverse with a `do...while` loop that stops when we come back to `playHead`."

## Structure 2: Stack (Module 1)

**What it is:** an array `undoStack[50]` and an index `undoTop`. Push adds at the top, pop removes from the top.

**Why we need it:**

- "Undo means restoring the **most recent** removal first. That is exactly **Last-In-First-Out**, which is what a stack is."
- "If we removed A, then B, then C, undo must bring back C, then B, then A. Any structure that does not give LIFO would need extra logic."
- "Push and pop are both **O(1)**."

**How it works in the code:**

- "When we remove a song, we unlink it from the playlist but we do **not** free it. We push its pointer onto the stack."
- "Undo pops the pointer and relinks that exact node at the end of the playlist."
- "This is the same idea as the undo button in a text editor."

## Structure 3: Binary Search Tree (Module 3)

**What it is:** each tree node holds a pointer to a `Song`. Songs with a lower rating go to the left, and songs with an equal or higher rating go to the right.

**Why we need it:**

- "We want to show songs sorted by rating. The playlist is in the order songs were added, so we would need to sort it every time."
- "A BST keeps songs in sorted position **as they are inserted**. An **in-order traversal** (left, node, right) always visits nodes in ascending order. So displaying sorted songs is just a traversal. We never call a sorting function."

**How it works in the code:**

- "`bstInsert` is recursive. If the current node is `NULL`, that is where the new node goes. Otherwise we compare ratings and recurse left or right."
- "`bstInorder` is also recursive: go left, print, go right."
- "Insertion is **O(log n)** on average for a balanced tree."

**Honest note (good to mention):** "Ratings only go from 1 to 5, so many songs share a rating and go to the right. With many songs the tree becomes lopsided and insertion approaches O(n). The sorted output is still correct. For a bigger system we would use a self-balancing tree, or key the tree on something more varied."

## Structure 4: Hashing with separate chaining (Module 4)

**What it is:** an array of 101 buckets. A hash function turns a title into a bucket number. Each bucket is a linked list of nodes.

**Why we need it:**

- "To find a song by title in the playlist, we would walk every node, which is **O(n)**. With hashing, we compute the bucket directly and check only that bucket. That is **O(1) on average**."

**How it works in the code:**

- "The hash function is `sum = sum * 31 + character` for every character, then `sum % 101`. Multiplying by 31 makes the position of each character matter, so titles with the same letters in a different order usually land in different buckets."
- "Two titles can still hash to the same bucket. That is a **collision**. We handle it with **separate chaining**: each bucket holds a linked list, and new nodes are added at the front. This is the 'open hashing' technique in our syllabus."
- "We also use the hash table to reject duplicate titles when adding a song."

## Conclusion

"To summarise:

- **Circular Doubly Linked List** stores the playlist, with O(1) insert and delete and built-in repeat.
- **Stack** gives undo, because undo is LIFO.
- **BST** gives sorted-by-rating output without a sort step.
- **Hashing** gives near-instant search by title.

Each structure solves a different problem, and together they make one working system. Thank you. We are happy to take questions."

---

## Time complexity summary

| Operation | Structure(s) | Complexity |
|---|---|---|
| Add song | List (tail insert) + BST + Hash | O(1) + O(log n) avg + O(1) avg |
| Remove song | List (search, then unlink) | O(n) search, O(1) unlink |
| Undo | Stack + List | O(1) |
| Search by title | Hash table | O(1) average |
| Display playlist | List traversal | O(n) |
| Display sorted by rating | BST in-order | O(n) |

## Known limitations and future scope

- Removing a song only unlinks it from the playlist. The song stays in the BST and hash table, so it still appears in the sorted view and in search results. Its title also stays in the hash table, so it cannot be added again until it is restored with undo.
- Fix: delete the song from the BST and hash table on removal, and reinsert on undo.
- The BST can become lopsided because ratings only range from 1 to 5. A balanced tree (AVL) would fix this.
- `removeSong` walks the list to find the song. Using the hash table to jump straight to the node would make it O(1).
- The undo stack holds at most 50 removals.
- Songs are not saved to a file, so data is lost on exit.

## Quick answers for questions

- **Why not just use an array?** Deleting or inserting in the middle needs shifting (O(n)), an array has a fixed size, and it gives no repeat, undo, or sorted view by itself.
- **Why a doubly linked list and not singly?** Going to the previous song, and finding the tail through `playHead->prev`, need the `prev` pointer.
- **Why a stack for undo?** Undo restores the latest removal first, which is LIFO.
- **Why a BST and not sorting each time?** The BST keeps songs in order as they are inserted, so the sorted view is a plain traversal.
- **What is a collision, and how did you handle it?** Two titles mapping to the same bucket. We used separate chaining, a linked list per bucket.
- **Why is the hash table size 101?** It is a prime number, which spreads keys more evenly across buckets.
