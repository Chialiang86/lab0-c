#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    while (q->head) {
        list_ele_t *del;
        del = q->head;
        q->head = q->head->next;
        free(del->value);
        free(del);
    }
    free(q);
}

/*
 * initialize an element
 */
list_ele_t *add_ele(const char *str)
{
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    // issue : memory leak
    if (!newh)
        return NULL;

    char *s = malloc(sizeof(char) * (strlen(str) + 1));
    if (!s) {
        free(newh);
        newh = NULL;
        return NULL;
    }

    strncpy(s, str, strlen(str) + 1);
    newh->next = NULL;
    newh->value = s;
    return newh;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh = add_ele(s);

    if (!newh)
        return false;

    newh->next = q->head;
    q->head = newh;
    q->tail = q->size == 0 ? newh : q->tail;
    q->size++;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh = add_ele(s);

    if (!newh)
        return false;

    if (!q->size) {
        q->head = newh;
        q->tail = newh;
    } else {
        q->tail->next = newh;
        q->tail = newh;
    }
    q->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    list_ele_t *del_node = q->head;

    if (!del_node->value) {
        sp = NULL;
    } else {
        if (sp) {
            strncpy(sp, del_node->value, bufsize);
            sp[bufsize - 1] = '\0';
        }
        q->head = del_node->next;
        free(del_node->value);
    }
    free(del_node);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->size <= 1)
        return;

    list_ele_t *pre = NULL, *pivot = q->head, *post = q->head->next;
    q->tail = pivot;
    while (post) {
        pivot->next = pre;
        pre = pivot;
        pivot = post;
        post = post->next;
    }
    pivot->next = pre;
    q->head = pivot;
}

// void print(queue_t *q)
// {
//     for (list_ele_t * ele = q->head; ele; ele = ele->next)
//         printf("%s ", ele->value);
//     printf("\n\n");
// }

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head)
        return;

    list_ele_t *lpart, *rpart;
    list_ele_t *tmp;
    for (unsigned long len = 1; len < q->size; len *= 2) {
        list_ele_t *last_tail = NULL, *tail;
        list_ele_t *last_tmp = tmp;
        tmp = q->head;
        while (tmp) {
            // add element to left queue
            lpart = tmp;
            for (unsigned long i = 0; i < len && tmp; i++, tmp = tmp->next)
                last_tmp = tmp;
            last_tmp->next = NULL;

            // add element to right queue
            rpart = tmp;
            for (unsigned long i = 0; i < len && tmp; i++, tmp = tmp->next)
                last_tmp = tmp;
            last_tmp->next = NULL;

            list_ele_t *res = merge(lpart, rpart, &tail);
            if (!last_tail)
                q->head = res;
            else
                last_tail->next = res;

            last_tail = tail;
        }
        q->tail = tail;
    }
}

list_ele_t *merge(list_ele_t *lpart, list_ele_t *rpart, list_ele_t **end)
{
    if (!lpart || !rpart)
        return lpart ? lpart : NULL;

    list_ele_t *res_head = NULL, *res_tail;

    if (strcasecmp(lpart->value, rpart->value) > 0) {
        res_head = rpart;
        rpart = rpart->next;
    } else {
        res_head = lpart;
        lpart = lpart->next;
    }

    res_tail = res_head;
    while (lpart || rpart) {
        if (lpart && rpart) {
            if (strcasecmp(lpart->value, rpart->value) > 0) {
                res_tail->next = rpart;
                res_tail = res_tail->next;
                rpart = rpart->next;
            } else {
                res_tail->next = lpart;
                res_tail = res_tail->next;
                lpart = lpart->next;
            }
        } else {
            while (lpart) {
                res_tail->next = lpart;
                res_tail = res_tail->next;
                lpart = lpart->next;
            }
            while (rpart) {
                res_tail->next = rpart;
                res_tail = res_tail->next;
                rpart = rpart->next;
            }
        }
    }
    *end = res_tail;
    return res_head;
}