/************************************************************************
 *
 * CSE130 Assignment 5
 *
 * Copyright (C) 2021-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 * 
 ************************************************************************/

/*
* Sources:
* https://www.geeksforgeeks.org/program-for-least-recently-used-lru-page-replacement-algorithm/
* https://www.geeksforgeeks.org/page-faults-in-lfu-implementation/
* http://www.mathcs.emory.edu/~cheung/Courses/355/Syllabus/9-virtual-mem/SC-replace.html
*/

#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct pages {
    unsigned int page_ref;
    int time;
    int count;
    int second_chance;
}pages;

static void *page_queue;
enum algorithm algo;
int frame;

static bool inner_equalitor(void *outer, void *inner) 
{
    return ((pages*)outer)->page_ref == *(unsigned int*)inner;
}

static int inner_comparator(void *a, void *b) 
{      
    return ((pages*)a)->time - ((pages*)b)->time;
}

static int inner_comparator_count(void *a, void *b) 
{      
    return ((pages*)a)->count - ((pages*)b)->count;
}

/**
 * Initialise your ALGORITHM pager with FRAMES frames (physical pages).
 */
void pager_init(enum algorithm algorithm, unsigned int frames) {
    page_queue = queue_create();
    algo = algorithm;
    frame = frames;
}

/**
 * Free any dynamically allocated resources.
 */
void pager_destroy() {
}

/**
 * A request has been made for virtual page PAGE. If your pager does
 * not currently have PAGE in physical memory, request it from the 
 * simulator.
 */
void pager_request(unsigned int page) {
    //FIRST_IN_FIRST_OUT
    if(algo == FIRST_IN_FIRST_OUT)
    {
        //if there are no pages in our queue
        if(queue_find(page_queue, inner_equalitor, &page) == NULL)
        {
            //check if size is less than given, if yes, add
            if(queue_size(page_queue) < frame)
            {
                sim_get_page(page);
                //allocate space for the page and add to queue
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
            }
            else //if not, remove page and put current one in
            {
                sim_get_page(page);
                free(queue_dequeue(page_queue));
                //allocate space for the page and add to queue
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
            }
        }
        else  //if there are pages in queue
        {
           pages *found = queue_find(page_queue, inner_equalitor, &page);
           found->time = sim_time();
        }
    }

    //LEAST_RECENTLY_USED
    if(algo == LEAST_RECENTLY_USED)
    {
        //if there are no pages in our queue
        if(queue_find(page_queue, inner_equalitor, &page) == NULL)
        {
            //check if size is less than given, if yes, add
            if(queue_size(page_queue) < frame)
            {
                sim_get_page(page);
                //allocate space for the page and add to queue
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
            }
            else //if not, remove page and put current one in
            {
                sim_get_page(page);
                free(queue_dequeue(page_queue));
                //allocate space for the page and add to queue
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
                queue_sort(page_queue, inner_comparator);
            }
        }
        else  //if there are pages in queue
        {
           pages *found = queue_find(page_queue, inner_equalitor, &page);
           found->time = sim_time();
           queue_sort(page_queue, inner_comparator);
        }
    }

    //LEAST_FREQUENTLY_USED
    if(algo == LEAST_FREQUENTLY_USED)
    {
        //if there are no pages in our queue
        if(queue_find(page_queue, inner_equalitor, &page) == NULL)
        {
            //check if size is less than given, if yes, add
            if(queue_size(page_queue) < frame)
            {
                sim_get_page(page);
                //allocate space for the page and add to queue
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
                new_page->count++;
                queue_sort(page_queue, inner_comparator_count);
            }
            else //if not, remove page and put current one in
            {
                sim_get_page(page);
                free(queue_dequeue(page_queue));
                //allocate space for the page and add to queue
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
                new_page->count++;
                queue_sort(page_queue, inner_comparator_count);
            }
        }
        else  //if there are pages in queue
        {
           pages *found = queue_find(page_queue, inner_equalitor, &page);
           found->time = sim_time();
           found->count++;
           queue_sort(page_queue, inner_comparator_count);
        }
    }

    //SECOND_CHANCE
    if(algo == SECOND_CHANCE)
    {
        //check if full, if yes, check is page is already present
        if(queue_size(page_queue) == frame)
        {
            // check if page already in queue
            pages *found = queue_find(page_queue, inner_equalitor, &page);
            if(found != NULL)
            {
                found->second_chance = 1;
            }
            else  //if page not present,set second chance to 0, put first element of queue in the back,set head to new head
            {
                pages *head = queue_head(page_queue);
                while(head->second_chance == 1)
                {
                    head->second_chance = 0;
                    queue_enqueue(page_queue, queue_dequeue(page_queue));
                    head = queue_head(page_queue);
                }
                sim_get_page(page);
                free(queue_dequeue(page_queue));
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
            }
        }    
        else if(queue_size(page_queue) < frame) //if we have space, check if page already present
        {
            pages *found = queue_find(page_queue, inner_equalitor, &page);
            if(found != NULL)
            {
                found->second_chance = 1;
            }
            else //if not, add
            {
                sim_get_page(page);
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 0;
                queue_enqueue(page_queue, new_page);
            }
        }
        else //if empty, add
        {
            sim_get_page(page);
            pages *new_page = malloc(sizeof(pages));
            new_page->page_ref = page;
            new_page->time = sim_time();
            new_page->count = 0;
            new_page->second_chance = 0;
            queue_enqueue(page_queue, new_page);
        }
    }

    //CIRCULAR_QUEUE
    if(algo == CIRCULAR_QUEUE)
    {
        //check if full, if yes, check is page is already present
        if(queue_size(page_queue) == frame)
        {
            // check if page already in queue
            pages *found = queue_find(page_queue, inner_equalitor, &page);
            if(found != NULL)
            {
                found->second_chance = 1;
            }
            else   //if page not present,set second chance to 0, put first element of queue in the back,set head to new head
            {
                pages *head = queue_head(page_queue);
                while(head->second_chance == 1)
                {
                    head->second_chance = 0;
                    queue_enqueue(page_queue, queue_dequeue(page_queue));  
                    head = queue_head(page_queue);
                }
                sim_get_page(page);
                free(queue_dequeue(page_queue));
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 1;
                queue_enqueue(page_queue, new_page);
            }
        }    
        else if(queue_size(page_queue) < frame)  //if we have space, check if page already present
        {
            pages *found = queue_find(page_queue, inner_equalitor, &page);
            if(found != NULL)
            {
                found->second_chance = 1;
            }
            else  //if not, add
            {
                sim_get_page(page);
                pages *new_page = malloc(sizeof(pages));
                new_page->page_ref = page;
                new_page->time = sim_time();
                new_page->count = 0;
                new_page->second_chance = 1;
                queue_enqueue(page_queue, new_page);
            }
        }
        else  //if empty, add
        {
            sim_get_page(page);
            pages *new_page = malloc(sizeof(pages));
            new_page->page_ref = page;
            new_page->time = sim_time();
            new_page->count = 0;
            new_page->second_chance = 1;
            queue_enqueue(page_queue, new_page);
        }
    }
}