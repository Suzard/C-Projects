//Edward Chen ID #88277651
#include<stdio.h>
#include <string.h>
#define FSIZE		4
#define VSIZE		8
#define PAGE_SIZE	2
#define WR 		1
#define RD		0
#define MAX_PPN		15

struct page_s {
    unsigned int valid;
    unsigned int dirty;
    unsigned int pn;		/* physical page number */
    int value[PAGE_SIZE];
};

struct page_s frame[FSIZE];
int ctr[FSIZE] = { 0 };

struct page_s disk[VSIZE];
struct page_s vpage[VSIZE];
int grepptr;

int full = 0;			//To check whether all frames are filled
int a[60], n;			//To take the input

static int time = 0;		//This is counter that keeps track of current time
int repptr;
int pf = 0;			/* page fault */

void init_disk(struct page_s *d)
{
    int i, j, p;
    p = 0;
    memset(d, 0, sizeof(disk));
    for (i = 0; i < (sizeof(disk) / sizeof(struct page_s)); i++) {
	for (j=0; j< PAGE_SIZE; j++) {
	    d->value[j] = -1;
	}
	d->pn = p;
	p++;
	d++;
    }
}

void init_vpage(struct page_s *d)
{
    int i, j, p;
    p = 0;
    memset(d, 0, sizeof(vpage));
    for (i = 0; i < (sizeof(vpage) / sizeof(struct page_s)); i++) {
	for (j=0; j< PAGE_SIZE; j++) {
	    d->value[j] = -1;
	}
	d->pn = p;
	p++;
	d++;
    }
}

int display()
{
    int i;

    printf("Memory Frame are:\n");
    for (i = 0; i < full; i++) {
	printf("%d\t v0=%d v1=%d\tctr=%d\n", frame[i].pn,
	       frame[i].value[0], frame[i].value[1], ctr[i]);
    }
}


//Fucntion for find the LRU page using their corresponding counter values
int Longestopt()
{
    int i, min;
    min = 0;
    for (i = 0; i < FSIZE; i++)
	if (ctr[min] > ctr[i])
	    min = i;

    repptr = min;
    return repptr;
}

//int Pagerep(int ele)
struct page_s Pagerep(struct page_s ele, int fr)
{
    //int temp;
    struct page_s temp;
    repptr = Longestopt();	//Gets the LRU page
    temp = frame[repptr];
    vpage[temp.pn].pn = 0xff; 
    vpage[temp.pn].dirty = 0;
    vpage[temp.pn].valid = 0;

    //???? temp.pn = repptr;
    //grepptr = repptr;
    vpage[fr].pn = repptr;
    frame[repptr] = ele;
    ctr[repptr] = time;		//page is brought in it's counter is kept as per the current time of use

    return temp;
}

int Pagefault(struct page_s ele, int fr)
{
    if (full != FSIZE) {
	ctr[full] = time;	//Setting the counter as per current time of use
        vpage[fr].pn = full;
	frame[full++] = ele;
	return -1;
    } else {
	struct page_s temp = Pagerep(ele, fr);
	int pn = temp.pn;//???? temp.pn;
	/* Save from frame(main) to disk */
        temp.pn = temp.pn; //????
	temp.dirty = 0;
	temp.valid = 0;
	disk[pn] = temp; 
        //vpage[pn] = temp; /* vpage update when save from frame to disk */
	//vpage[pn].pn = 0xff;
        //printf("%s fr=%d pn=%d scrap\n", __func__, fr, pn);
	return pn;
    }
}

int Search(int addr, int *found)
{
    int i, flag, fr;

    fr = addr / PAGE_SIZE;
    flag = 0;
    if (full != 0) {
	for (i = 0; i < full; i++)
	    if (fr == frame[i].pn) {
		flag = 1;
		ctr[i] = time;	//If page fault doesn't occur, but the element is referenced, it's counter is set to the current time of use
		*found = i;
		break;
	    }
    }
    return flag;
}


void frame_search(int addr, int write, int val, int *rval)
{
    int found;
    int fr = addr / PAGE_SIZE;
    int offset = addr % PAGE_SIZE;

    time++;			//Time of use is incremented
    if (Search(addr, &found) != 1) {	/* page fault; load from disk */
	struct page_s temp;
	temp = disk[fr];
	temp.valid = 1;
	disk[fr].valid = 1; /* mark disk is valid in frame */
	vpage[fr].valid = 1; /* ???? */
	if (write) {
	    temp.value[offset] = val;
	    temp.dirty = 1;
	    vpage[fr].dirty = 1; /* ???? */
	} else {
	    *rval = temp.value[offset];
	}
	Pagefault(temp, fr); /* add fr, so we can update vpage[fr].pn */



	pf++;
    } else {
	if (write) {
	    frame[found].value[offset] = val;
	    frame[found].dirty = 1;
	    vpage[fr].dirty = 1; /* ???? */
	} else {
	    *rval = frame[found].value[offset];
	}
    }
}

#ifndef READ_FILE
char *getToken(char **sp)
{
    static const char *sep = " \t\n";
    static char vb[] = "|", vbf;
    char *p, *s;
    if (vbf) {
	vbf = 0;
	return vb;
    }
    if (sp == NULL || *sp == NULL || **sp == '\0')
	return (NULL);
    s = *sp;
    if (*s == '"')
	p = strchr(++s, '"');
    else if (*s == '\'')
	p = strchr(++s, '\'');
    else
	p = s + strcspn(s, "| \t\n");
    if (*p != '\0') {
	if (*p == '|') {
	    *vb = vbf = '|';
	}
	*p++ = '\0';
	p += strspn(p, sep);
    }
    *sp = p;
    if (!*s) {
	vbf = 0;
	return vb;
    }
    return s;
}

int p_read(char *addr, int *rdv)
{
    int address = atoi(addr);
    int rval;

    if (address < 0 || address > MAX_PPN)
	return -1;
    frame_search(address, 0, RD, &rval);
    *rdv = rval;
    return 0;
}

int p_write(char *addr, char *wval)
{
    int address = atoi(addr);
    int val = atoi(wval);

    if (address < 0 || address > MAX_PPN)
	return -1;
    frame_search(address, WR, val, 0);
    return 0;
}

int p_showmain(char *ppn)
{
    int pn = atoi(ppn);
    int i, j, found = 0;;

    if (pn < 0 || pn > FSIZE) {
	printf("Command argument '%s' out of bound\n", ppn);
	return -1;
    }
#if 1 /* pn is just index */
int lpn;
i = pn;

lpn = frame[i].pn;
printf("Address\tContents\n");
for (j = 0; j < PAGE_SIZE; j++) {
    printf("%d\t", (i*PAGE_SIZE + j)); /*address lpn*PAGE_SIZE */
    if ((frame[i].value[j]) >= 0) {
	printf("%d\n", frame[i].value[j]);
    } else {
	printf("-1\n");
    }
}
found = 1;
#else /* logical pn */
    for (i = 0; i < full; i++) {
	if (frame[i].pn == pn) { /* this is logical */
	    printf("Address\tContents\n");
	    for (j = 0; j < PAGE_SIZE; j++) {
		printf("%d\t", (pn*PAGE_SIZE + j)); /* Address */
		if ((frame[i].value[j]) >= 0) {
		    printf("%d\n", frame[i].value[j]);
		} else {
		    printf("%c\n", '-');
		}
	    }
            found = 1;
	    break;
	}
    }
#endif
    return 0;
}
int p_showptable(void)
{
    int i;

    printf("VPageNum\tValid\tDirty\tPN\n");
    for (i = 0; i < (sizeof(vpage) / sizeof(struct page_s)); i++) {
	printf("%d\t\t%d\t%d\t", i, vpage[i].valid, vpage[i].dirty);
        if (vpage[i].pn > (sizeof(vpage) / sizeof(struct page_s)))
		printf("%d\n", i);
	else 
		printf("%d\n", vpage[i].pn);
    }
}

int p_showdisk(char *ppn)
{
    int pn = atoi(ppn);
    int i, j, found = 0;

    if (pn < 0 || pn > VSIZE) {
	printf("Command argument '%s' out of bound\n", ppn);
	return -1;
    }
    i = pn;
    printf("Address\tContents\n");
    for (j = 0; j < PAGE_SIZE; j++) {
	printf("%d\t", (pn*PAGE_SIZE + j));
	if ((disk[i].value[j]) >= 0) {
	    printf("%d\n", disk[i].value[j]);
	} else {
	    printf("-1\n");
	}
    }
    found = 1;
    return 0;
    /* Search main memory and find the index to frame  */
#if 0
    found = 0;
    for (i = 0; i < full; i++) {
	if (frame[i].pn == pn) {
	    printf("Main[entry]\tAddress\tContents\n");
	    for (j = 0; j < PAGE_SIZE; j++) {
		printf("%d\t\t%d\t", i, (pn*PAGE_SIZE + j));
		if ((frame[i].value[j]) >= 0) {
		    printf("%d\n", frame[i].value[j]);
		} else {
		    printf("-1\n");
		}
	    }
            found = 1;
	    break;
	}
    }
    if (!found)
 	printf("Main %s entry has no valid memory\n", ppn);
    /* Search main memory and find the index to frame  */
    return 0;

#endif
}
#endif

int main()
{

#ifdef READ_FILE
    int i, rval;
    FILE *fp;
    int found;

    init_disk(disk);
    init_vpage(vpage);
    fp = fopen("Input.txt", "r");
    printf("The number of elements in the reference string are :");
    fscanf(fp, "%d", &n);
    printf("%d", n);
    for (i = 0; i < n; i++)
	fscanf(fp, "%d", &a[i]);
    printf("\nThe elements present in the string are\n");
    for (i = 0; i < n; i++)
	printf("%d  ", a[i]);
    printf("\n\n");
    for (i = 0; i < n; i++)
	frame_search(a[i], WR, i, &rval);

    display();
    printf("\nThe number of page faults are %d\n", pf);
#else
    char text[100];
    char *t, *p = text, *cmd[3];
    int i;

    init_disk(disk);
    memset(frame, -1, sizeof (frame));
    init_vpage(vpage);
    printf("$ ");
    fgets(text, sizeof(text), stdin);
    while (1) {
	t = getToken(&p);
	cmd[0] = t;
	if (!strcasecmp(t, "quit"))
	    break;
	for (i = 1; (t != NULL && i <= 2); i++) {
	    t = getToken(&p);
	    cmd[i] = t;
	}

	if (!strcasecmp(cmd[0], "read")) {
	    int rd;
	    //printf("read %s\n", cmd[1]);
	    if (!cmd[1] || p_read(cmd[1], &rd))
		goto error;
	    else {
		if (rd >= 0) {
		    printf("%d\n", rd);
		} else {
		    printf("-1\n");
		}
	    }
	}

	else if (!strcasecmp(cmd[0], "write")) {
	    if (!cmd[1] || !cmd[2] || p_write(cmd[1], cmd[2])) {
		goto error;
	    }
	} else if (!strcasecmp(cmd[0], "showmain")) {
            if (!cmd[1] || p_showmain(cmd[1]))
		goto error;
	}

	else if (!strcasecmp(cmd[0], "showdisk")) {
            if (!cmd[1] || p_showdisk(cmd[1]))
		goto error;
	}

	else if (!strcasecmp(cmd[0], "showptable")) {
            p_showptable();
	} else {
error:
	    printf("*** Command error ***\n");
	}
	printf("$ ");
	fgets(text, sizeof(text), stdin);
	p = text;
    }

#endif
    return 0;
}
