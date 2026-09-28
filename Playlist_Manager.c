/*
 * MUSIC PLAYLIST MANAGER
 * Mini Project - Data Structures & Algorithms
 *
 * Data structures used:
 *  1. Stack                    -> undo removed song                 (Module 1)
 *  2. Circular Doubly Linked List -> the playlist itself             (Module 2)
 *  3. Binary Search Tree (by rating) -> sorted view of songs         (Module 3)
 *  4. Hashing (chaining)       -> fast search by title               (Module 4)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_UNDO  50
#define HASH_SIZE 101

/* ---------------- Song node - lives in the circular doubly linked list (Module 2) ---------------- */
typedef struct Song {
    char title[50];
    char artist[50];
    int rating;              /* 1 to 5 */
    struct Song *next;
    struct Song *prev;
} Song;

Song *playHead = NULL;       /* head of the circular doubly linked playlist */

/* ---------------- Stack for Undo (Module 1) ---------------- */
Song *undoStack[MAX_UNDO];
int undoTop = -1;

void pushUndo(Song *s) {
    if (undoTop < MAX_UNDO - 1) undoStack[++undoTop] = s;
    else printf("Undo stack full - oldest removal can no longer be undone.\n");
}
Song *popUndo(void) {
    if (undoTop == -1) return NULL;
    return undoStack[undoTop--];
}

/* ---------------- Binary Search Tree keyed by rating (Module 3) ---------------- */
typedef struct BSTNode {
    Song *song;
    struct BSTNode *left, *right;
} BSTNode;

BSTNode *bstRoot = NULL;

BSTNode *bstInsert(BSTNode *node, Song *s) {
    if (node == NULL) {
        BSTNode *n = (BSTNode *) malloc(sizeof(BSTNode));
        n->song = s;
        n->left = n->right = NULL;
        return n;
    }
    if (s->rating < node->song->rating) node->left = bstInsert(node->left, s);
    else node->right = bstInsert(node->right, s);
    return node;
}

void bstInorder(BSTNode *node) {
    if (!node) return;
    bstInorder(node->left);
    printf("%-20s by %-15s Rating:%d\n", node->song->title, node->song->artist, node->song->rating);
    bstInorder(node->right);
}

/* ---------------- Hash Table keyed by title (Module 4 - separate chaining) ---------------- */
typedef struct HashNode {
    Song *song;
    struct HashNode *next;
} HashNode;

HashNode *hashTable[HASH_SIZE];

int hashFunc(const char *title) {
    unsigned int sum = 0;
    for (int i = 0; title[i]; i++) sum = sum * 31 + title[i];
    return sum % HASH_SIZE;
}

void hashInsert(Song *s) {
    int idx = hashFunc(s->title);
    HashNode *node = (HashNode *) malloc(sizeof(HashNode));
    node->song = s;
    node->next = hashTable[idx];
    hashTable[idx] = node;
}

Song *hashSearch(const char *title) {
    int idx = hashFunc(title);
    HashNode *cur = hashTable[idx];
    while (cur) {
        if (strcmp(cur->song->title, title) == 0) return cur->song;
        cur = cur->next;
    }
    return NULL;
}

/* ---------------- Playlist operations ---------------- */
void addSong(const char *title, const char *artist, int rating) {
    if (hashSearch(title)) { printf("Song '%s' already in playlist.\n", title); return; }

    Song *s = (Song *) malloc(sizeof(Song));
    strncpy(s->title, title, sizeof(s->title) - 1);   s->title[sizeof(s->title) - 1] = '\0';
    strncpy(s->artist, artist, sizeof(s->artist) - 1); s->artist[sizeof(s->artist) - 1] = '\0';
    s->rating = rating;

    if (playHead == NULL) {
        s->next = s; s->prev = s;
        playHead = s;
    } else {
        Song *tail = playHead->prev;
        tail->next = s;  s->prev = tail;
        s->next = playHead; playHead->prev = s;
    }

    bstRoot = bstInsert(bstRoot, s);
    hashInsert(s);
    printf("Added '%s' by %s (rating %d).\n", title, artist, rating);
}

void removeSong(const char *title) {
    if (playHead == NULL) { printf("Playlist is empty.\n"); return; }
    Song *cur = playHead;
    do {
        if (strcmp(cur->title, title) == 0) {
            if (cur->next == cur) {
                playHead = NULL;
            } else {
                cur->prev->next = cur->next;
                cur->next->prev = cur->prev;
                if (cur == playHead) playHead = cur->next;
            }
            pushUndo(cur);   /* song stays in BST/hash so it can still be found/undone */
            printf("Removed '%s' from playlist. (undo available)\n", title);
            return;
        }
        cur = cur->next;
    } while (cur != playHead);
    printf("Song '%s' not found in playlist.\n", title);
}

void undoRemove(void) {
    Song *s = popUndo();
    if (!s) { printf("Nothing to undo.\n"); return; }

    if (playHead == NULL) {
        s->next = s; s->prev = s;
        playHead = s;
    } else {
        Song *tail = playHead->prev;
        tail->next = s; s->prev = tail;
        s->next = playHead; playHead->prev = s;
    }
    printf("Restored '%s' to playlist.\n", s->title);
}

void displayPlaylist(void) {
    if (!playHead) { printf("Playlist is empty.\n"); return; }
    Song *cur = playHead;
    printf("--- Playlist (circular order) ---\n");
    do {
        printf("%-20s by %-15s Rating:%d\n", cur->title, cur->artist, cur->rating);
        cur = cur->next;
    } while (cur != playHead);
}

void displaySortedByRating(void) {
    if (!bstRoot) { printf("No songs added yet.\n"); return; }
    printf("--- Songs sorted by rating (ascending) ---\n");
    bstInorder(bstRoot);
}

void searchSong(const char *title) {
    Song *s = hashSearch(title);
    if (s) printf("Found -> %s by %s, Rating %d\n", s->title, s->artist, s->rating);
    else printf("Song '%s' not found.\n", title);
}

/* ---------------- Menu-driven main ---------------- */
void flushInput(void) { int c; while ((c = getchar()) != '\n' && c != EOF); }

int main(void) {
    int choice, rating;
    char title[50], artist[50];

    for (int i = 0; i < HASH_SIZE; i++) hashTable[i] = NULL;

    while (1) {
        printf("\n===== MUSIC PLAYLIST MANAGER =====\n");
        printf("1. Add Song\n");
        printf("2. Remove Song\n");
        printf("3. Undo Last Removal\n");
        printf("4. Search Song by Title\n");
        printf("5. Display Playlist\n");
        printf("6. Display Songs Sorted by Rating\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { flushInput(); continue; }
        flushInput();

        switch (choice) {
            case 1:
                printf("Enter Title: "); fgets(title, sizeof(title), stdin); title[strcspn(title, "\n")] = 0;
                printf("Enter Artist: "); fgets(artist, sizeof(artist), stdin); artist[strcspn(artist, "\n")] = 0;
                printf("Enter Rating (1-5): "); scanf("%d", &rating); flushInput();
                addSong(title, artist, rating);
                break;
            case 2:
                printf("Enter Title to remove: "); fgets(title, sizeof(title), stdin); title[strcspn(title, "\n")] = 0;
                removeSong(title);
                break;
            case 3: undoRemove(); break;
            case 4:
                printf("Enter Title to search: "); fgets(title, sizeof(title), stdin); title[strcspn(title, "\n")] = 0;
                searchSong(title);
                break;
            case 5: displayPlaylist(); break;
            case 6: displaySortedByRating(); break;
            case 0: printf("Exiting. Goodbye!\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
