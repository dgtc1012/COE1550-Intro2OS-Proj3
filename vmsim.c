/*
 * Daniel McGrath
 * CS 1550: Project 3
 * Main program for virtual memory simulation
 *
 * 11/2/2016
 */

#include "vmsim.h"

int main(int argc, char ** argv)
{
    // command line args
    int    c = 0;
    int    num_frames = 0;
    char * algo = "";
    int    refresh_rate = 0;
    int    tau = 0;
    char * trace_file_name = "";

    FILE * trace_file = NULL;

    char * usage = "usage: ./vmsim -n <numframes> -a <opt|clock|aging|work> [-r refresh] [-t <tau>] <tracefile>\n";

    // fifo page replacement current index
    int current_index = -1;

    unsigned int  num_accesses = 0;
    unsigned int  addr = 0;
    unsigned char mode = 0;

    int page_faults = 0;
    int disk_writes = 0;

    // flags for parsing command line arguments
    int n, a, r, t;
    n = a = r = t = 0;

    // use getopt to parse command line args
    while( (c = getopt(argc, argv, "n:a:r:t:")) != -1)
    {
        switch(c)
        {
            case 'n':
                n = 1;
                num_frames = atoi(optarg);
                break;
            case 'a':
                a = 1;
                algo = optarg;
                break;
            case 'r':
                r = 1;
                refresh_rate = atoi(optarg);
                break;
            case 't':
                t = 1;
                tau = atoi(optarg);
                break;
            case '?':
                printf("%s", usage);
                exit(0);
                break;
            case ':':
                printf("%s", usage);
                exit(0);
                break;
        } // switch

    } //while

    if(!n || !a || !r || !t)
    {
        printf("%s", usage);
        exit(0);
    }

    printf("num frames: %d algo: %s refresh rate: %d tau: %d\n", num_frames, algo, refresh_rate, tau);
    printf("argv[%d]: %s\n", optind, argv[optind]);


    trace_file = fopen(argv[optind], "r");

    if(trace_file == NULL)
    {
        printf("error opening %s\n", trace_file_name);
        exit(0);
    }

    
    if(strcmp(algo, "opt"))
    {
        //opt algorithm code
        //pre-processing
        while(fscanf(trace_file, "%x %c", &addr, &mode) == 2)
            num_accesses++;

        unsigned int address_array[num_accesses];
        unsigned char mode_array[num_accesses];
        unsigned int i = 0;
        unsigned int line_count = 0;

        rewind(trace_file);

        while(fscanf(trace_file, "%x %c", &address_array[line_count], &mode_array[line_count]) == 2)
            line_count++;

        fclose(trace_file);

        struct PTE_32 * page_table = calloc(sizeof(struct PTE_32), PT_SIZE_1MB);
        struct future_node ** future  = calloc(sizeof(struct future_node *), PT_SIZE_1MB);
        struct future_node ** end_future  = calloc(sizeof(struct future_node *), PT_SIZE_1MB);

        if(!page_table)
        {
            fprintf(stderr, "Error on mallocing page table\n");
            exit(0);
        }

        // page_table = malloc(PT_SIZE_1MB * PTE_SIZE_BYTES);
        int page_frames[num_frames];

        for(i = 0; i < num_frames; i++)
            page_frames[i] = -1;

        for(i = 0; i < PT_SIZE_1MB; i++)
        {
            page_table[i].valid      = 0;
            page_table[i].dirty      = 0;
            page_table[i].referenced = 0;
            page_table[i].index      = 0;
            page_table[i].frame      = 0;
            future[i]                = NULL;
            end_future[i]            = NULL;
        }

        
        for(i = 0; i < line_count; i++)
        {
            int page_num = PTE32_INDEX(address_array[i]);
            //int f_index = page_num % PT_SIZE_1MB;
            if(future[page_num] == NULL)
            {
                future[page_num] = malloc(sizeof(struct future_node));
                future[page_num]->access_time = i;
                future[page_num]->next = NULL;
                
                end_future[page_num] = future[page_num];
            }
            else
            {
                struct future_node * curr = future[page_num];
                
                end_future[page_num]->next = malloc(sizeof(struct future_node));
                end_future[page_num]->next->access_time = i;
                end_future[page_num]->next->next = NULL;
                end_future[page_num] = end_future[page_num]->next;
            }
        }
        
        //actual algorithm
        int j = 0;
        int frame_index = 0;
        for(i = 0; i < line_count; i++){
            int page_num = PTE32_INDEX(address_array[i]);
            if(page_table[page_num].valid == 1){
                int mode = mode_array[i];
                if(mode == 'W'){
                    page_table[page_num].dirty = 1;
                }
            }
            else{
                
                if(frame_index < num_frames){
                    //there is an empty 
                    page_frames[frame_index] = page_num;
                    frame_index++;
                    page_faults++;
                }
                else{
                    //evict
                    int max_index = frames[0];
                    int max = future[frames[0]]->access_time;
                    for(j = 1; j<num_frames; j++){
                        if(future[page_frames[j]] > max){
                            max = future[page_frames[j]];
                            max_index = j;
                        }
                    }
                    if(page_table[page_frames[max_index]].dirty == 1){
                        disk_writes++;
                    }
                    page_faults++;
                    
                }
            }
        }
    }
    else if(strcmp(algo, "clock"))
    {
        //clock algorithm code
    }
    else if(strcmp(algo, "aging"))
    {
        //aging algorithm code
    }
    else
    {
        //working set algorithm code
    }
    
    // opt
    
    /*memset(page_table, 0, PT_SIZE_1MB * PTE_SIZE_BYTES);*/

    /*// Create the first frame of the frames linked list*/
    /*struct frame_struct *frame = malloc(sizeof(struct frame_struct));*/
    /*if(!frame)*/
    /*{*/
        /*fprintf(stderr, "Error on mallocing frame struct\n");*/
        /*exit(1);*/
    /*}*/
    /*memset(frame, 0, sizeof(struct frame_struct));*/

    return 0;
}

void addToFront(struct future_node ** head, int t)
{
    struct future_node * newNode;
    newNode = malloc(sizeof(struct future_node));
    newNode->access_time = t;
    newNode->next = *head;
    *head = newNode;
}

