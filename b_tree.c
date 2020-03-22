#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct temp{
	int n;
	char name[32];	
};

struct page{
	char page_type = '1';
	char table_id = '1';
	
	char extra[18];
	
	struct temp t[3];
	
};

struct level{
	//char a[4];
	char pagetype = '2';
	char table_id;
	char unused[2];
	int block[16] ;
	int id[15];
	
};
struct Table_page{
	char table_type = '3';
	char table_id = '0';
	char unused[6];
	int root_page_id[10];
	char table_name[10][8];
};
void levelup(FILE *, FILE *,int*,int,int);
void levelup2(FILE *, FILE *, int*,int,int);
void index(FILE *diff,FILE *start, struct page *p,int *indexno,int no_of_records,int t,int p1)
{
	
	struct level levels;
	levels.table_id = p->table_id;
	for (int i = 0; i < no_of_records; i++)
	{
		levels.block[i] = i + ((*indexno));
		if (i != 0)
		{
			levels.id[i-1]=p[i].t[0].n;
			printf("\n%d  %d %d %d %d", (*indexno + no_of_records), (*indexno), levels.block[i - 1], levels.id[i - 1], levels.block[i]);
		}

	}
	
	fwrite(&levels, sizeof(levels), 1, start);
	
	*indexno += no_of_records;
	levelup(diff, start,indexno,t,p1);


}
void levelup(FILE * start, FILE *end,int *index_no,int tableno,int pageno)
{
	struct page *p = (struct page*)calloc(16, sizeof(struct page));
	
	int i = 0;
	int worst = 0;
	if (feof(start) != 0)
	{
		return;
	}
	while (feof(start) == 0)
	{
		fread(&p[i], sizeof(p[i]), 1, start);
		if (p[i].table_id-'0' == tableno && p[i].page_type-'0'==pageno)
			i++;
		if (i == 16)
			break;
	}
	
	
	
	index(start,end, p,index_no,i,tableno,pageno);
	
	i = 0;
	free(p);
}

void index2(FILE *diff, FILE *start, struct level *p, int *indexno,int no_of_records, int t, int p1)
{

	struct level levels;
	levels.table_id = p->table_id;
	for (int i = 0; i < no_of_records; i++)
	{
		levels.block[i] = i + ((*indexno) );
		if (i != 0)
		{
			levels.id[i - 1] = p[i].id[0];

			printf("\n%d %d %d %d %d",(*indexno+no_of_records), (*indexno) , levels.block[i - 1], levels.id[i - 1], levels.block[i]);
		}

	}

	fwrite(&levels, sizeof(levels), 1, start);
	*indexno += no_of_records;

	levelup2(diff, start, indexno ,t,p1);


}
void levelup2(FILE * start, FILE *end, int *index_no,int tableno,int pageno)
{
	struct level *p = (struct level*)calloc(16, sizeof(struct level));

	int i = 0;
	int worst = 0;
	if (feof(start) != 0)
	{
		return;
	}
	while (feof(start) == 0)
	{
		fread(&p[i], sizeof(p[i]), 1, start);

		if (p[i].table_id-'0' == tableno && p[i].pagetype-'0' == pageno)
			i++;
		if (i == 16)
			break;
	}
	

	

	index2(start, end, p, index_no,i,tableno,pageno);

	i = 0;
	free(p);
}

void search(struct Table_page table, int id)
{
	struct level levels;
	struct page p;
	FILE *fp = fopen("sample.dat", "rb");
	int block = table.root_page_id[0];
	X:
	int offset = block * 128;
	
	fseek(fp, offset, 0);
	char type;
	fread(&type, sizeof(type), 1, fp);
	fseek(fp, offset, 0);
	if (type - '0' == 2)
	{
		fread(&levels, sizeof(levels), 1, fp);
		for (int i = 0; i < 15; i++)
		{
			if (id < levels.id[i])
			{
				block = levels.block[i];
				goto X;
			}
			if (levels.id[i] < 0 || i==14)
			{
				block = levels.block[i + 1];
				goto X;
			}
		}

	}
	else if (type - '0' == 1)
	{
		fread(&p, sizeof(p), 1, fp);
		for (int i = 0; i < 3; i++)
		{
			if (p.t[i].n == id)
			{
				printf("\n%d %s", p.t[i].n, p.t[i].name);
				return;
			}
		}

		return;
	}
}

void search_between(struct Table_page table, int id, int id2)
{

	struct level levels;
	struct page p;
	FILE *fp = fopen("sample.dat", "rb");
	int block = table.root_page_id[0];
X:
	int offset = block * 128;

	fseek(fp, offset, 0);
	char type;
	fread(&type, sizeof(type), 1, fp);
	fseek(fp, offset, 0);
	if (type - '0' == 2)
	{
		fread(&levels, sizeof(levels), 1, fp);
		for (int i = 0; i < 15; i++)
		{
			if (id < levels.id[i])
			{
				block = levels.block[i];
				goto X;
			}
			if (levels.id[i] < 0 || i == 14)
			{
				block = levels.block[i + 1];
				goto X;
			}
		}

	}
	else if (type - '0' == 1)
	{
		
		fread(&p, sizeof(p), 1, fp);
		for (int i = 0; i < 3; i++)
		{
			
			if (p.t[i].n == id)
			{
				printf("\n%d %s", p.t[i].n, p.t[i].name);
				i++;
				while (p.t[i].n < id2 && i<3)
				{
					printf("\n%d %s", p.t[i].n, p.t[i].name);
					i++;
				}
				if (i == 3)
					i--;
				if (p.t[i].n == id2)
					return;
				Y:
				fread(&p, sizeof(p), 1, fp);
				i = 0;
				while (p.t[i].n < id2 && i<3)
				{
					printf("\n%d %s", p.t[i].n, p.t[i].name);

					i++;
				}
				if (i==3)
					i--;
				if (p.t[i].n == id2)
					return;
				goto Y;
			}
			
		}

		return;
	}
}
void search_between_sub(struct Table_page table, int id, int id2,char *sub)
{

	struct level levels;
	struct page p;
	FILE *fp = fopen("sample.dat", "rb");
	int block = table.root_page_id[0];
X:
	int offset = block * 128;

	fseek(fp, offset, 0);
	char type;
	fread(&type, sizeof(type), 1, fp);
	fseek(fp, offset, 0);
	if (type - '0' == 2)
	{
		fread(&levels, sizeof(levels), 1, fp);
		for (int i = 0; i < 15; i++)
		{
			if (id < levels.id[i])
			{
				block = levels.block[i];
				goto X;
			}
			if (levels.id[i] < 0 || i == 14)
			{
				block = levels.block[i + 1];
				goto X;
			}
		}

	}
	else if (type - '0' == 1)
	{

		fread(&p, sizeof(p), 1, fp);
		for (int i = 0; i < 3; i++)
		{

			if (p.t[i].n == id)
			{
				if (strstr(p.t[i].name,sub)!=NULL)
				 printf("\n%d %s", p.t[i].n, p.t[i].name);
				i++;
				while (p.t[i].n < id2 && i<3)
				{
					if (strstr(p.t[i].name, sub)!=NULL)
						printf("\n%d %s", p.t[i].n, p.t[i].name);
					i++;
				}
				if (i == 3)
					i--;
				if (p.t[i].n == id2)
					return;
			Y:
				fread(&p, sizeof(p), 1, fp);
				i = 0;
				while (p.t[i].n < id2 && i<3)
				{
					if (strstr(p.t[i].name, sub) != NULL)
						printf("\n%d %s", p.t[i].n, p.t[i].name);

					i++;
				}
				if (i == 3)
					i--;
				if (p.t[i].n == id2)
					return;
				goto Y;
			}

		}

		return;
	}
}
int main()
{
	struct Table_page table;
	struct temp samp;
	char record[128];
	FILE *fp, *tfp;
	struct page p;
	fp = fopen("sample.dat", "wb");
	if ((tfp = fopen("randomuserdata.csv.csv", "r")) == NULL)
	{
		printf("not exists\n");
		return 0;
	}int i = 0; int j = 0, k = 0;
	while (fscanf(tfp, "%s", record)>0)
	{
		i++;
		samp.n = atoi(strtok(record, ","));
		strcpy(samp.name, strtok(NULL, ","));
		//strcpy(samp.dist, strtok(NULL, ","));
		p.t[k].n = samp.n;
		//strcpy(p[j].t[k].dist, samp.dist);
		strcpy(p.t[k].name, samp.name);
		
		
		
		k++;
		
		if (k == 3)
		{
			fwrite(&p, sizeof(p), 1, fp);
			//printf(" %d  %d %d     ", sizeof(p[j]), j, k);
			k = 0;
			j++;
		}
		
		
	}
	fwrite(&p, sizeof(p), 1, fp);
	fclose(fp);
	fclose(tfp);
	k = 0;
	j = 0;
	i = 0;
	int worst = 0;
	fp = fopen("sample.dat", "rb");
	fread(&p, sizeof(p), 1, fp);
	while (feof(fp)==0)
	{
		for (int k = 0; k < 3; k++)
		{
			printf("%d\t%s\n", p.t[k].n, p.t[k].name);
			i++;
		}
		fread(&p, sizeof(p), 1, fp);
	}
	fclose(fp);
	
	fp = fopen("sample.dat", "rb");
	ftell(fp);
	FILE *fp_for_append = fopen("sample.dat", "ab");
	
	int block = 0;
	 levelup(fp, fp_for_append,&block,1,1);


	 while (true)
	 {
		 long int offset = ftell(fp);
		 long int offset1 = ftell(fp_for_append);
		 if (offset1-offset==128)
		 {
			 table.root_page_id[0] = block;
			 table.table_name[0][1] = 's';
			 table.table_name[0][2] = '\0';

			 break;
		 }

		 fclose(fp);
		 fclose(fp_for_append);
		 fp = fopen("sample.dat", "rb");
		 fseek(fp, offset, 0);

		 
		 fp_for_append = fopen("sample.dat", "ab");
		 levelup2(fp, fp_for_append, &block,1,2);
		 
	 }
	 fclose(fp);
	 fclose(fp_for_append);
	 int id = 0;
	 printf("\n \n enter a id:");
	 scanf("%d", &id);
	 search(table, id);
	 printf("\n\n enter two ids:");
	 int id1;
	 scanf("%d", &id);
	 scanf("%d", &id1);
	 search_between(table, id,id1);
	 char sub[20];
	 printf("\n\n enter 2 id's and substring: ");
	 scanf("%d", &id);
	 scanf("%d", &id1);
	 scanf("%s", sub);
	 search_between_sub(table, id, id1,sub);
	getchar();
	getchar();
	return 0;
}